#ifndef __DOSE_UART_H__
#define __DOSE_UART_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DOSE_UART_ID    0

struct dose_object
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

int8_t dose_uart_cmd_write(struct dose_object *cmd);


#ifdef __cplusplus
}
#endif

#endif /* __DOSE_UART_H__ */