#include "uart_port.h"
#include "drv_uart.h"
#include "ulog.h"


static DEVICE_UART AFC_uart = {0};
static DEVICE_UART *AFC_uart_get(void)
{
    return &AFC_uart;
}

void UART9_IRQHandler(void)
{
  HAL_UART_IRQHandler((UART_HandleTypeDef *)AFC_uart_get());
}


int8_t device_AFC_uart_init(uint8_t *device_name)
{
    int8_t ret = uart_init(AFC_uart_get(), device_name);
    if (ret != 0)
    {
        return ret;
    }

    osMessageQueueAttr_t AFC_uart_rx_queue_attributes = {
    .name = "uart_rx_queue"
    };
    osMessageQueueId_t AFC_uart_rx_queue = osMessageQueueNew (5, sizeof(struct AFC_uart), &AFC_uart_rx_queue_attributes);
    if (AFC_uart_rx_queue == NULL)
    {
        return -1;
    }

    ret = AFC_uart_get()->ioctl(AFC_uart_get(), UART_CMD_SET_DMA_RX_QUEUE, (void *)AFC_uart_rx_queue);
    if (ret != 0)
    {
        return ret;
    }

    uint8_t *rx_buf = (uint8_t *)pvPortMalloc(sizeof(struct AFC_uart));  /* here should check when use dma mode */
    if (rx_buf == NULL)
    {
        return -2;
    }

    uint16_t rx_buf_len = sizeof(struct AFC_uart) - sizeof(uint16_t); /* indicate rx buf max len */
    struct dma_rx_buf_info
    {
        uint8_t *buf;
        uint16_t buf_len;
    }info = {rx_buf, rx_buf_len};

    return AFC_uart_get()->ioctl(AFC_uart_get(), UART_CMD_SET_DMA_RX_BUF, (void *)&info);
}

int8_t device_AFC_uart_open(void)
{
    return AFC_uart_get()->open(AFC_uart_get());
}

#include "frame_statistics.h"
#include "frame_format.h"
static struct frame_statistics AFC_uart_frame_stats = {0};
static struct frame_statistics *AFC_uart_frame_stats_get(void)
{
    return &AFC_uart_frame_stats;
}

int8_t device_AFC_uart_data_read(struct AFC_uart *buf, uint32_t timeout)
{
    int8_t ret = AFC_uart_get()->read(AFC_uart_get(), buf, timeout);
    if (ret != 0)
    {
        return ret;
    }

#if 1
    LOG_E("recv original: %d bytes\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        LOG_E("%.2x ", buf->buf[i]);
    }
    LOG_E("\r\n");
#endif

    uint16_t offset = 0, length = 0;

    ret = frame_format_parse(AFC_uart_frame_stats_get(), buf->buf, buf->len, &offset, &length);
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

static int8_t AFC_uart_data_write_callback(uint8_t *buf, uint16_t size, uint32_t timeout)
{
    return AFC_uart_get()->write(AFC_uart_get(), buf, size, timeout);
}

int8_t device_AFC_uart_data_write(struct AFC_uart *buf, uint16_t size, uint32_t timeout)
{
    return frame_format_pack_and_send(AFC_uart_frame_stats_get(), buf, size, AFC_uart_data_write_callback, timeout);
}
