#ifndef __UART_PORT_H__
#define __UART_PORT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_AFC_UART_NAME_DEFAULT    "uart9"

#define AFC_UART_MAX_LENGTH      128
struct AFC_uart
{
    uint8_t buf[AFC_UART_MAX_LENGTH];
    uint16_t len;
};

struct cmd_object
{
    union
    {
        uint8_t byte;
        struct
        {
            uint8_t cmd_id : 7;
            uint8_t cmd_ack : 1;
        }bits;
    }id;    /* device rs422 id */

    uint8_t type;   /* cmd type */
    uint16_t len;   /* data length */
    uint8_t *data;  /* data pointer */
};

int8_t device_AFC_uart_init(uint8_t *device_name);
int8_t device_AFC_uart_open(void);
int8_t device_AFC_uart_data_read(struct AFC_uart *buf, uint32_t timeout);
int8_t device_AFC_uart_data_write(struct AFC_uart *buf, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif


#endif /* __UART_PORT_H__ */