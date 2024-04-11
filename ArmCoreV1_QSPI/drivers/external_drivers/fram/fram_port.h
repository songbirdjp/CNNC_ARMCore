#ifndef __FRAM_PORT_H__
#define __FRAM_PORT_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DEVICE_FRAM_NAME_DEFAULT    "spi6"

#define FRAM_SIZE               0x8000
#define FRAM_ADDR_END           FRAM_SIZE
#define BYTE_LEN_PER_LINE       80  /* char num + '\0' must less than BYTE_LEN_PER_LINE */
#define LOG_INFO_UPDATE_LINES   1   /* log info update every line */


#ifdef __cplusplus
}
#endif

#endif