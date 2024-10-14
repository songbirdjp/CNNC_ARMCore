#include "uart_port.h"
#include "drv_uart.h"
#include "ulog.h"

static DEVICE_UART dose_uart = {0};

static DEVICE_UART *dose_uart_get(void)
{
    return &dose_uart;
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
  /* USER CODE BEGIN UART5_IRQn 0 */

  /* USER CODE END UART5_IRQn 0 */
  HAL_UART_IRQHandler((UART_HandleTypeDef *)dose_uart_get());
  /* USER CODE BEGIN UART5_IRQn 1 */

  /* USER CODE END UART5_IRQn 1 */
}

#undef USING_UART_OPTION_FUNCTION
#ifdef USING_UART_OPTION_FUNCTION
static DEVICE_UART_OPT dose_uart_opt = {0};
static int8_t dose_uart_opt_before_write(DEVICE_UART *uart)
{
    // printf("before write\r\n");
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    return 0;
}
static int8_t dose_uart_opt_after_write(DEVICE_UART *uart)
{
    // printf("after write\r\n");

    return 0;
}
static int8_t dose_uart_opt_complete_write(DEVICE_UART *uart)
{
    // printf("complete write\r\n");
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    return 0;
}
static int8_t dose_uart_opt_before_read(DEVICE_UART *uart)
{
    // printf("before read\r\n");
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    return 0;
}
static int8_t dose_uart_opt_after_read(DEVICE_UART *uart)
{
    // printf("after read\r\n");

    return 0;
}
static int8_t dose_uart_opt_complete_read(DEVICE_UART *uart)
{
    // printf("complete read\r\n");
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    return 0;
}
static int8_t device_dose_uart_opt_init(DEVICE_UART *dose_uart, DEVICE_UART_OPT *dose_uart_opt)
{
    dose_uart_opt->before_write = dose_uart_opt_before_write;
    dose_uart_opt->after_write = dose_uart_opt_after_write;
    dose_uart_opt->complete_write = dose_uart_opt_complete_write;
    dose_uart_opt->before_read = dose_uart_opt_before_read;
    dose_uart_opt->after_read = dose_uart_opt_after_read;
    dose_uart_opt->complete_read = dose_uart_opt_complete_read;

    return dose_uart->ioctl(dose_uart, UART_CMD_SET_OPT_FUNC, (void *)dose_uart_opt);
}
#endif

int8_t device_dose_uart_init(uint8_t *device_name)
{
    int8_t ret = uart_init(dose_uart_get(), device_name);
    if (ret != 0)
    {
        return ret;
    }

#ifdef USING_UART_OPTION_FUNCTION
    ret = device_dose_uart_opt_init(dose_uart_get(), &dose_uart_opt);
    if (ret != 0)
    {
        return ret;
    }
#endif

    osMessageQueueAttr_t dose_uart_rx_queue_attributes = {
    .name = "uart_rx_queue"
    };
    osMessageQueueId_t dose_uart_rx_queue = osMessageQueueNew (5, sizeof(struct dose_uart), &dose_uart_rx_queue_attributes);
    if (dose_uart_rx_queue == NULL)
    {
        return -1;
    }

    ret = dose_uart_get()->ioctl(dose_uart_get(), UART_CMD_SET_DMA_RX_QUEUE, (void *)dose_uart_rx_queue);
    if (ret != 0)
    {
        return ret;
    }

    uint8_t *rx_buf = (uint8_t *)pvPortMalloc(sizeof(struct dose_uart));  /* here should check when use dma mode */
    if (rx_buf == NULL)
    {
        return -2;
    }

    uint16_t rx_buf_len = sizeof(struct dose_uart) - sizeof(uint16_t); /* indicate rx buf max len */
    struct dma_rx_buf_info
    {
        uint8_t *buf;
        uint16_t buf_len;
    }info = {rx_buf, rx_buf_len};

    return dose_uart_get()->ioctl(dose_uart_get(), UART_CMD_SET_DMA_RX_BUF, (void *)&info);
}

int8_t device_dose_uart_open(void)
{
    return dose_uart_get()->open(dose_uart_get());
}

#include "frame_statistics.h"
#include "frame_format.h"
static struct frame_statistics dose_uart_frame_stats = {0};
static struct frame_statistics *dose_uart_frame_stats_get(void)
{
    return &dose_uart_frame_stats;
}

int8_t device_dose_uart_data_read(struct dose_uart *buf, uint32_t timeout)
{
    int8_t ret = dose_uart_get()->read(dose_uart_get(), buf, timeout);
    if (ret != 0)
    {
        return ret;
    }

#if 0
    LOG_I("recv original: %d bytes\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        LOG_I("%.2x ", buf->buf[i]);
    }
    LOG_I("\r\n");
#endif

    uint16_t offset = 0, length = 0;

    ret = frame_format_parse(dose_uart_frame_stats_get(), buf->buf, buf->len, &offset, &length);
    if (ret != 0)
    {
        return ret;
    }

    if (length > 0)
    {
        memcpy(buf->buf, &buf->buf[offset], length);
    }

    buf->len = length;

#if 0
    printf("recv: %d bytes\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        printf("%.2x ", buf->buf[i]);
    }
    printf("\r\n");
#endif

    return 0;
}

static int8_t dose_uart_data_write_callback(uint8_t *buf, uint16_t size, uint32_t timeout)
{
    return dose_uart_get()->write(dose_uart_get(), buf, size, timeout);
}

int8_t device_dose_uart_data_write(struct dose_uart *buf, uint16_t size, uint32_t timeout)
{
    return frame_format_pack_and_send(dose_uart_frame_stats_get(), buf, size, dose_uart_data_write_callback, timeout);
}
