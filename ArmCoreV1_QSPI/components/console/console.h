#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_NAME_DEFAULT        "uart1"


int8_t device_console_init(uint8_t *device_name);
int8_t device_console_write(uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif