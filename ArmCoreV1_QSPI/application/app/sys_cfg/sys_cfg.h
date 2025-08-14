#ifndef __SYS_CFG_H__
#define __SYS_CFG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FW_VERSION  "00.00.01"

struct sys_info
{
    uint8_t fw_version[32];
    uint8_t compile_time[32];
    uint8_t uid_cryptogram[64];
    uint32_t uid_cryptogram_valid;
    uint32_t reserved[7]    /* flash word == 32bytes */
};

struct sys_info *system_info_get(void);
void system_info_print(void);
int8_t system_encrypt_init(void);

#ifdef __cplusplus
}
#endif

#endif
