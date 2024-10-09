#ifndef __BGM_UART_H__
#define __BGM_UART_H__

#include <stdint.h>
#include "bgm_uart_port.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BGM_UART_ID    0

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

int8_t uart_cmd_write(enum uart_id id, struct cmd_object *cmd);


/************************modbus cmd define**************************************/
#define DEVICE_ADDRESS_EPS  0x01
#define DEVICE_ADDRESS_VPS  0x01

enum modbus_cmd
{
    READ_HOLDING_REGISTERS = 0x03,
    READ_INPUT_REGISTERS = 0x04,
    WRITE_SINGLE_REGISTER = 0x06,
    WRITE_MULTIPLE_REGISTERS = 0x10,
    // READ_DEVICE_IDENTIFICATION = 0x2B,
};


#ifdef __cplusplus
}
#endif

#endif /* __BGM_UART_H__ */