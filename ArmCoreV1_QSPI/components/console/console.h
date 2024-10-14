#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_NAME_DEFAULT        "uart1"


int8_t device_console_init(uint8_t *device_name);


#define USING_COM1_FOR_YMODEM
#ifdef USING_COM1_FOR_YMODEM
enum com1_mode
{
    CONSOLE_MODE,
    YMODEM_MODE
};

enum com1_mode *com_mode_get(void);
int8_t ymodem_data_read(uint8_t *buf, uint16_t *len, uint32_t timeout);
int8_t ymodem_data_write(uint8_t *buf, uint16_t len, uint32_t timeout);
#endif

#ifdef __cplusplus
}
#endif

#endif