#include "uart_port.h"
#include "init_call.h"
#include "ulog.h"

static uart_protocol_t uart9_protocal = {0};
uart_protocol_t *uart_protocal_get(void)
{
    return &uart9_protocal;
}
static int8_t uart_init(uint8_t *device_name)
{
    int8_t ret = 0;

    ret = uart_protocol_init(uart_protocal_get(),
                             UART_DEV_NAME_UART9,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        LOG_E("uart_protocol_init error: %d\r\n", ret);
        return -1;
    }

    return 0;
}
INIT_COMPONENT_EXPORT(uart_init);

int8_t device_afc_uart_open(void)
{
    return uart_protocol_open(uart_protocal_get());
}

int8_t device_afc_uart_data_recv_with_block(uint8_t *buf, uint16_t size, uint32_t timeout)
{
    return uart_protocol_recv(uart_protocal_get(), buf, size, timeout);
}

int8_t device_afc_uart_data_read(struct uart_data *cmd, uint32_t timeout)
{
    return uart_protocol_get(uart_protocal_get(), cmd->id, cmd->cmd, cmd->data, cmd->len, timeout);
}

int8_t device_afc_uart_data_write(struct uart_data *cmd, uint32_t timeout)
{
    return uart_protocol_set(uart_protocal_get(), cmd->id, cmd->cmd, cmd->data, cmd->len, timeout);
}
