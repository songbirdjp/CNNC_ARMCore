#ifndef __UART_PORT_H__
#define __UART_PORT_H__

#include <stdint.h>
#include "uart_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

struct uart_data
{
    uint32_t id;
    uint8_t cmd;
    uint16_t len;
    uint8_t *data;
};

int8_t device_dose_uart_open(void);
int8_t device_dose_uart_data_recv_with_block(uint8_t *buf, uint16_t size, uint32_t timeout);
int8_t device_dose_uart_data_read(struct uart_data *cmd, uint32_t timeout);
int8_t device_dose_uart_data_write(struct uart_data *cmd, uint32_t timeout);

#ifdef __cplusplus
}
#endif


#endif /* __UART_PORT_H__ */