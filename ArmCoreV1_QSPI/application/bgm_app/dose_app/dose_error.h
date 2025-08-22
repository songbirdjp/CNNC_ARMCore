#ifndef __DOSE_ERROR_H__
#define __DOSE_ERROR_H__

#include <stdint.h>
#include "bgm_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

enum dose_para_type
{
    DOSE_PARA_ADC = 0,
    DOSE_PARA_DAC,
    DOSE_PARA_TRIGGER_INTERVAL_MIN,
    DOSE_PARA_DOSE_MODE,
    DOSE_PARA_PULSE_MODE,
    DOSE_PARA_PRF,
    DOSE_PARA_DOSE_METER,
    DOSE_PARA_CP_NUM,
    DOSE_PARA_RI_NUM,
    DOSE_PARA_CP_TOLERATE,
    DOSE_PARA_CP_RI_MAP,
    DOSE_PARA_RI_INFO,
    DOSE_PARA_BEAM_INFO,
    DOSE_PARA_INTERLOCK_OVERRIDE,
    DOSE_PARA_NOT_READY_OVERRIDE,
    DOSE_PARA_TIMESTAMP,
    DOSE_PARA_MAX,
};

int8_t dose_err_info_clear(void);
uint32_t dose_err_info_get(enum uart_id id);
int8_t dose_para_check(enum dose_para_type para_type, enum uart_id id, uint16_t idx, void *value);

#ifdef __cplusplus
}
#endif
#endif /* __DOSE_ERROR_H__ */
