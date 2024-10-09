#ifndef __BGM_UART_PORT_H__
#define __BGM_UART_PORT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BGM_UART_MAX_LENGTH      128
struct bgm_uart
{
    uint8_t buf[BGM_UART_MAX_LENGTH];
    uint16_t len;
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

int8_t device_uart_init(enum uart_id id);
int8_t device_uart_open(enum uart_id id);
int8_t device_uart_data_read(enum uart_id id, struct bgm_uart *buf, uint32_t timeout);
int8_t device_uart_data_write(enum uart_id id, struct bgm_uart *buf, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif


#endif /* __BGM_UART_PORT_H__ */