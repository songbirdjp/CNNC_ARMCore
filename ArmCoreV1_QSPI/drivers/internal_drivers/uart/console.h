#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "stdint.h"
#include "drv_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_NAME_DEFAULT        DEVICE_NAME_UART1
#define CONSOLE_CMD_MAX_LENGTH      128


struct CmdMessage
{
    uint8_t buf[CONSOLE_CMD_MAX_LENGTH];
    uint16_t len;
};


int8_t device_console_init(uint8_t *device_name);
int8_t console_cmd_process(void);

#ifdef __cplusplus
}
#endif

#endif