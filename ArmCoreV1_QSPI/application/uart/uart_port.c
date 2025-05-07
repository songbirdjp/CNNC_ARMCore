#include "uart_port.h"
#include "dev_uart.h"
#include "init_call.h"
#include "ulog.h"

/*
 * AFC: UART7
 * DOSE1: UART5
 * DOSE2: UART2
 * RTM: UART3
 * EPS/VPS: UART4
 */

static uart_protocol_t uart_protocal[UART_PROTOCOL_MAX] = {0};
uart_protocol_t *uart_protocal_get(enum uart_protocol_id id)
{
    return &uart_protocal[id];
}
static int8_t bgm_uart_protocol_init(void)
{
    int8_t ret = 0;

    ret = uart_protocol_init(uart_protocal_get(UART_PROTOCOL_AFC),
                             UART_DEV_NAME_UART7,
                             1000,
                             10000,
                             10000);

    ret |= uart_protocol_init(uart_protocal_get(UART_PROTOCOL_DOSE1),
                             UART_DEV_NAME_UART5,
                             1000,
                             10000,
                             10000);

    ret |= uart_protocol_init(uart_protocal_get(UART_PROTOCOL_DOSE2),
                             UART_DEV_NAME_USART2,
                             1000,
                             10000,
                             10000);

    ret |= uart_protocol_init(uart_protocal_get(UART_PROTOCOL_RTM),
                             UART_DEV_NAME_USART3,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        LOG_E("uart_protocol_init error: %d\r\n", ret);
    }

    return ret;
}
INIT_COMPONENT_EXPORT(bgm_uart_protocol_init);

static uart_dev_t *uart_dev[UART_DEV_MAX] = {NULL};
static uart_dev_t *uart_dev_get(enum uart_dev_id id)
{
    return uart_dev[id];
}
static int8_t bgm_uart_dev_init(void)
{
    int8_t ret = 0;

    uart_dev[UART_DEV_EPS_VPS] = device_uart_find(UART_DEV_NAME_UART4);
    if (uart_dev[UART_DEV_EPS_VPS] == NULL)
    {
        LOG_E("device_uart_find err: %s\r\n", UART_DEV_NAME_UART4);
        return -1;
    }

    ret = dev_uart_init(uart_dev[UART_DEV_EPS_VPS],
                        DEV_UART_IOCTL_USE_DMA,
                        5,
                        UART_FRAME_SIZE_MAX);
    if (ret != 0)
    {
        LOG_E("dev_uart_init err: %d\r\n", ret);
        return -2;
    }

    return ret;
}
INIT_COMPONENT_EXPORT(bgm_uart_dev_init);


int8_t uart_open(enum uart_id id)
{
    if (id < UART_PROTOCOL_MAX)
    {
        return uart_protocol_open(uart_protocal_get(id));
    }
    else if (id - UART_PROTOCOL_MAX < UART_DEV_MAX)
    {
        return dev_uart_open(uart_dev_get(id - UART_PROTOCOL_MAX));
    }
    else
    {
        return -1;
    }

    return 0;
}

int8_t uart_data_recv_with_block(enum uart_id id, uint8_t *buf, uint16_t size, uint32_t timeout)
{
    if (id < UART_PROTOCOL_MAX)
    {
        return uart_protocol_recv(uart_protocal_get(id), buf, size, timeout);
    }
    else if (id - UART_PROTOCOL_MAX < UART_DEV_MAX)
    {
        return dev_uart_recv(uart_dev_get(id - UART_PROTOCOL_MAX), buf, size, timeout);
    }
    else
    {
        return -1;
    }

    return 0;
}

int8_t uart_data_write(enum uart_id id, struct uart_data *cmd, uint16_t size, uint32_t timeout)
{
    if (id < UART_PROTOCOL_MAX)
    {
        return uart_protocol_set(uart_protocal_get(id), cmd->id, cmd->cmd, cmd->data, cmd->len, timeout);
    }
    else if (id - UART_PROTOCOL_MAX < UART_DEV_MAX)
    {
        return dev_uart_send(uart_dev_get(id - UART_PROTOCOL_MAX), cmd->data, size, timeout);
    }
    else
    {
        return -1;
    }

    return 0;
}

int8_t uart_data_read(enum uart_id id, struct uart_data *cmd, uint32_t timeout)
{
    if (id < UART_PROTOCOL_MAX)
    {
        return uart_protocol_get(uart_protocal_get(id), cmd->id, cmd->cmd, cmd->data, cmd->len, timeout);
    }
    else if (id - UART_PROTOCOL_MAX < UART_DEV_MAX)
    {
        return dev_uart_send(uart_dev_get(id - UART_PROTOCOL_MAX), cmd->data, cmd->len, timeout);
    }
    else
    {
        return -1;
    }

    return 0;
}
