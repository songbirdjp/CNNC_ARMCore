#ifndef __SYS_CFG_H__
#define __SYS_CFG_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FW_VERSION  "00.00.01"

struct sys_info
{
    uint8_t fw_version[20];
    uint8_t compile_time[30];
};

struct sys_info *system_info_get(void);



#ifdef __cplusplus
}
#endif

#endif
