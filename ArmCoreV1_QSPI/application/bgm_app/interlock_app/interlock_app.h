#ifndef __INTERLOCK_APP_H__
#define __INTERLOCK_APP_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

enum locked_type
{
    LOCKED_TYPE_IO = 0,
    LOCKED_TYPE_BGM_ERR,
    LOCKED_TYPE_DOSE_ERR,
    LOCKED_TYPE_AFC_ERR,
};

struct interlock
{
    uint32_t io;
    uint32_t bgm_err;
    uint32_t dose_err[2];
    uint32_t dose_interlock[2];
    uint32_t afc_err;
    uint32_t afc_interlock;
};

struct interlock *interlock_status_get(void);
int8_t interlock_locked_set(enum locked_type type, uint32_t value, uint32_t value_1);
int8_t interlock_fault_clear(void);

#ifdef __cplusplus
}
#endif

#endif /* __INTERLOCK_APP_H__ */