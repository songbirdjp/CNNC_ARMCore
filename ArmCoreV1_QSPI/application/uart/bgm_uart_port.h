#ifndef __BGM_UART_PORT_H__
#define __BGM_UART_PORT_H__

#include "uart_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_PROTOCOL_NUM      3
#define UART_FRAME_SIZE_MAX    128

enum uart_protocol_id
{
    UART_PROTOCOL_AFC = 0,
    UART_PROTOCOL_DOSE1,
    UART_PROTOCOL_DOSE2,
    UART_PROTOCOL_MAX,
};
enum uart_dev_id
{
    UART_DEV_EPS = 0,
    UART_DEV_VPS,
    UART_DEV_MAX,
};

enum uart_id
{
    BGM_UART_AFC = 0,
    BGM_UART_DOSE1,
    BGM_UART_DOSE2,
    BGM_UART_EPS,
    BGM_UART_VPS,
    BGM_UART_MAX
};

struct uart_data
{
    uint32_t id;
    uint8_t cmd;
    uint16_t len;
    uint8_t *data;
};

uart_protocol_t *uart_protocal_get(enum uart_protocol_id id);
int8_t uart_open(enum uart_id id);
int8_t uart_data_recv_with_block(enum uart_id id, uint8_t *buf, uint16_t size, uint32_t timeout);
int8_t uart_data_write(enum uart_id id, struct uart_data *cmd, uint16_t size, uint32_t timeout);
int8_t uart_data_read(enum uart_id id, struct uart_data *cmd, uint32_t timeout);

#ifdef __cplusplus
}
#endif


#endif /* __BGM_UART_PORT_H__ */