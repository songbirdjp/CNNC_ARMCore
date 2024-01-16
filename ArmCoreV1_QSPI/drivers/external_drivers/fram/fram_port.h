#ifndef __FRAM_PORT_H__
#define __FRAM_PORT_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USING_FRAM
#ifdef USING_FRAM

#define DEVICE_FRAM_NAME_DEFAULT    "spi6"

#define FRAM_SIZE               0x8000
#define FRAM_ADDR_END           FRAM_SIZE
#define BYTE_LEN_PER_LINE       64  /* char num + '\0' must less than BYTE_LEN_PER_LINE */

int8_t device_fram_init(uint8_t *device_name);
#endif


#ifdef __cplusplus
}
#endif

#endif