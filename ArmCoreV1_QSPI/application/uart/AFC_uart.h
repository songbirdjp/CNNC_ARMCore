#ifndef __AFC_UART_H__
#define __AFC_UART_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AFC_UART_ID    0
#define AFC_UART_FRAME_SIZE_MAX    128

struct afc_object
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

#ifdef __cplusplus
}
#endif

#endif /* __AFC_UART_H__ */