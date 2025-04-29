#ifndef __BGM_UART_H__
#define __BGM_UART_H__

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
    uint16_t *len;   /* data length */
    uint8_t *data;  /* data pointer */
};

int8_t uart_cmd_write(enum uart_id id, struct cmd_object *cmd);
int8_t uart_cmd_parse_callback_register(enum uart_id id, int8_t (*callback)(enum uart_id id, struct cmd_object *cmd));
int8_t uart_init_callback_register(enum uart_id id, int8_t (*callback)(void));

/************************modbus cmd define**************************************/
enum modbus_cmd
{
    READ_HOLDING_REGISTERS = 0x03,  /* read/write register */
    READ_INPUT_REGISTERS = 0x04,    /* read only register */
    WRITE_SINGLE_REGISTER = 0x06,
    WRITE_MULTIPLE_REGISTERS = 0x10,
    // READ_DEVICE_IDENTIFICATION = 0x2B,
};

struct modbus_cmd_object
{
    uint8_t addr;
    uint8_t type;
    uint16_t len;
    uint8_t *data;
};

uint16_t modbus_crc16_cal(const uint8_t *data, uint16_t length);
int8_t uart_modbus_cmd_write(enum uart_id id, struct modbus_cmd_object *obj);

#ifdef __cplusplus
}
#endif

#endif /* __BGM_UART_H__ */