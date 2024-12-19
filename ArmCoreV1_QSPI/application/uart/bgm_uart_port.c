#include "bgm_uart_port.h"
#include "drv_uart.h"

/*
 * AFC: UART7
 * DOSE1: UART5
 * DOSE2: UART2
 * EPS: UART3
 * VPS: UART4
 */
#define DEVICE_AFC_UART_NAME_DEFAULT    "uart7"
#define DEVICE_DOSE1_UART_NAME_DEFAULT  "uart5"
#define DEVICE_DOSE2_UART_NAME_DEFAULT  "uart2"
#define DEVICE_EPS_UART_NAME_DEFAULT    "uart3"
#define DEVICE_VPS_UART_NAME_DEFAULT    "uart4"

static DEVICE_UART bgm_uart[BGM_UART_MAX] = {0};

static DEVICE_UART *bgm_uart_get(enum uart_id id)
{
    if (id >= BGM_UART_MAX)
    {
        return NULL;
    }

    return &bgm_uart[id];
}

/**
  * @brief This function handles UART7 global interrupt.
  */
void UART7_IRQHandler(void)
{
  /* USER CODE BEGIN UART7_IRQn 0 */

  /* USER CODE END UART7_IRQn 0 */
  HAL_UART_IRQHandler((UART_HandleTypeDef *)bgm_uart_get(BGM_UART_AFC));
  /* USER CODE BEGIN UART7_IRQn 1 */

  /* USER CODE END UART7_IRQn 1 */
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
  /* USER CODE BEGIN UART5_IRQn 0 */

  /* USER CODE END UART5_IRQn 0 */
  HAL_UART_IRQHandler((UART_HandleTypeDef *)bgm_uart_get(BGM_UART_DOSE1));
  /* USER CODE BEGIN UART5_IRQn 1 */

  /* USER CODE END UART5_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler((UART_HandleTypeDef *)bgm_uart_get(BGM_UART_DOSE2));
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler((UART_HandleTypeDef *)bgm_uart_get(BGM_UART_EPS));
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */

  /* USER CODE END UART4_IRQn 0 */
  HAL_UART_IRQHandler((UART_HandleTypeDef *)bgm_uart_get(BGM_UART_VPS));
  /* USER CODE BEGIN UART4_IRQn 1 */

  /* USER CODE END UART4_IRQn 1 */
}

static uint8_t *device_uart_name_find(enum uart_id id)
{
    switch (id)
    {
    case BGM_UART_AFC:
        return DEVICE_AFC_UART_NAME_DEFAULT;
        break;
    case BGM_UART_DOSE1:
        return DEVICE_DOSE1_UART_NAME_DEFAULT;
        break;
    case BGM_UART_DOSE2:
        return DEVICE_DOSE2_UART_NAME_DEFAULT;
        break;
    case BGM_UART_EPS:
        return DEVICE_EPS_UART_NAME_DEFAULT;
        break;
    case BGM_UART_VPS:
        return DEVICE_VPS_UART_NAME_DEFAULT;
        break;
    default:
        break;
    }

    return NULL;
}

// #undef USING_UART_OPTION_FUNCTION
#ifdef USING_UART_OPTION_FUNCTION
static DEVICE_UART_OPT uart_opt = {0};
static int8_t uart_opt_before_write(DEVICE_UART *uart)
{
    // printf("before write\r\n");

    if (!memcmp(uart->name, DEVICE_EPS_UART_NAME_DEFAULT, strlen(DEVICE_EPS_UART_NAME_DEFAULT)))
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    }
    else if (!memcmp(uart->name, DEVICE_VPS_UART_NAME_DEFAULT, strlen(DEVICE_VPS_UART_NAME_DEFAULT)))
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
    }
    
    return 0;
}
static int8_t uart_opt_after_write(DEVICE_UART *uart)
{
    // printf("after write\r\n");

    return 0;
}
static int8_t uart_opt_complete_write(DEVICE_UART *uart)
{
    // printf("complete write\r\n");
    if (!memcmp(uart->name, DEVICE_EPS_UART_NAME_DEFAULT, strlen(DEVICE_EPS_UART_NAME_DEFAULT)))
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    }
    else if (!memcmp(uart->name, DEVICE_VPS_UART_NAME_DEFAULT, strlen(DEVICE_VPS_UART_NAME_DEFAULT)))
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
    }

    return 0;
}
static int8_t uart_opt_before_read(DEVICE_UART *uart)
{
    // printf("before read\r\n");
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    return 0;
}
static int8_t uart_opt_after_read(DEVICE_UART *uart)
{
    // printf("after read\r\n");

    return 0;
}
static int8_t uart_opt_complete_read(DEVICE_UART *uart)
{
    // printf("complete read\r\n");
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    return 0;
}
static int8_t device_uart_opt_init(DEVICE_UART *uart, DEVICE_UART_OPT *uart_opt)
{
    uart_opt->before_write = uart_opt_before_write;
    uart_opt->after_write = uart_opt_after_write;
    uart_opt->complete_write = uart_opt_complete_write;
    uart_opt->before_read = uart_opt_before_read;
    uart_opt->after_read = uart_opt_after_read;
    uart_opt->complete_read = uart_opt_complete_read;

    return uart->ioctl(uart, UART_CMD_SET_OPT_FUNC, (void *)uart_opt);
}
#endif


int8_t device_uart_init(enum uart_id id)
{
    if (id >= BGM_UART_MAX)
    {
        return -1;
    }

    uint8_t *device_name = device_uart_name_find(id);
    DEVICE_UART *obj = bgm_uart_get(id);

    int8_t ret = uart_init(obj, device_name);
    if (ret != 0)
    {
        return ret;
    }

#ifdef USING_UART_OPTION_FUNCTION
    ret = device_uart_opt_init(obj, &uart_opt);
    if (ret != 0)
    {
        return ret;
    }
#endif

    osMessageQueueAttr_t uart_rx_queue_attributes = {
    .name = "uart_rx_queue"
    };
    osMessageQueueId_t uart_rx_queue = osMessageQueueNew (5, sizeof(struct bgm_uart), &uart_rx_queue_attributes);
    if (uart_rx_queue == NULL)
    {
        return -2;
    }

    ret = obj->ioctl(obj, UART_CMD_SET_DMA_RX_QUEUE, (void *)uart_rx_queue);
    if (ret != 0)
    {
        return ret;
    }

    uint8_t *rx_buf = (uint8_t *)pvPortMalloc(sizeof(struct bgm_uart));  /* here should check when use dma mode */
    if (rx_buf == NULL)
    {
        return -3;
    }

    uint16_t rx_buf_len = sizeof(struct bgm_uart) - sizeof(uint16_t); /* indicate rx buf max len */
    struct dma_rx_buf_info
    {
        uint8_t *buf;
        uint16_t buf_len;
    }info = {rx_buf, rx_buf_len};

    return obj->ioctl(obj, UART_CMD_SET_DMA_RX_BUF, (void *)&info);
}

int8_t device_uart_open(enum uart_id id)
{
    if (id >= BGM_UART_MAX)
    {
        return -1;
    }

    DEVICE_UART *obj = bgm_uart_get(id);

    return obj->open(obj);
}

#include "frame_statistics.h"
#include "frame_format.h"
static struct frame_statistics uart_frame_stats[BGM_UART_MAX] = {0};
static struct frame_statistics *uart_frame_stats_get(enum uart_id id)
{
    if (id >= BGM_UART_MAX)
    {
        return NULL;
    }

    return &uart_frame_stats[id];
}

int8_t device_uart_data_read(enum uart_id id, struct bgm_uart *buf, uint32_t timeout)
{
    if (id >= BGM_UART_MAX)
    {
        return -1;
    }

    DEVICE_UART *obj = bgm_uart_get(id);

    int8_t ret = obj->read(obj, buf, timeout);
    if (ret != 0)
    {
        return ret;
    }

#if 0
    printf("recv original: %d bytes\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        printf("%.2x ", buf->buf[i]);
    }
    printf("\r\n");
#endif


    if (id == BGM_UART_EPS || id == BGM_UART_VPS)
    {
        /* EPS and VPS are not need to parse frame format */
    }
    else
    {
        uint16_t offset = 0, length = 0;

        ret = frame_format_parse(uart_frame_stats_get(id), buf->buf, buf->len, &offset, &length);
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
    }

    return 0;
}

static int8_t uart_afc_write_callback(uint8_t *buf, uint16_t size, uint32_t timeout)
{
    DEVICE_UART *obj = bgm_uart_get(BGM_UART_AFC);

    return obj->write(obj, buf, size, timeout);
}

static int8_t uart_dose1_write_callback(uint8_t *buf, uint16_t size, uint32_t timeout)
{
    DEVICE_UART *obj = bgm_uart_get(BGM_UART_DOSE1);

    return obj->write(obj, buf, size, timeout);
}

static int8_t uart_dose2_write_callback(uint8_t *buf, uint16_t size, uint32_t timeout)
{
    DEVICE_UART *obj = bgm_uart_get(BGM_UART_DOSE2);

    return obj->write(obj, buf, size, timeout);
}

static int8_t (*uart_data_write_callback[BGM_UART_MAX])(uint8_t *buf, uint16_t size, uint32_t timeout) =
{
    [BGM_UART_AFC] = uart_afc_write_callback,
    [BGM_UART_DOSE1] = uart_dose1_write_callback,
    [BGM_UART_DOSE2] = uart_dose2_write_callback
};

int8_t device_uart_data_write(enum uart_id id, struct bgm_uart *buf, uint16_t size, uint32_t timeout)
{
    if (id == BGM_UART_EPS || id == BGM_UART_VPS)
    {
        return bgm_uart_get(id)->write(bgm_uart_get(id), buf, size, timeout);
    }
    else
    {
        return frame_format_pack_and_send(uart_frame_stats_get(id), buf, size, uart_data_write_callback[id], timeout);
    }

    return 0;
}
