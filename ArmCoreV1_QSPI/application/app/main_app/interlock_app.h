#ifndef __INTERLOCK_APP_H__
#define __INTERLOCK_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MUTEX_TIMEOUT_MS    (10)

enum interlock_fault_info
{
    INTERLOCK_FAULT_COM_TIMEOUT = 0,
    INTERLOCK_FAULT_ADCS7476_1_OFFSET_LIMIT_LOW,
    INTERLOCK_FAULT_ADCS7476_1_OFFSET_LIMIT_HIGH,
    INTERLOCK_FAULT_ADCS7476_2_OFFSET_LIMIT_LOW,
    INTERLOCK_FAULT_ADCS7476_2_OFFSET_LIMIT_HIGH,
    INTERLOCK_FAULT_DOSE_RATE_LOW,
    INTERLOCK_FAULT_DOSE_RATE_HIGH,
    INTERLOCK_FAULT_DOSE_CP_LOW,
    INTERLOCK_FAULT_DOSE_CP_HIGH,
    INTERLOCK_FAULT_DOSE_SYMMETRY_FAULT,
    INTERLOCK_FAULT_DOSE_DUMMY_END_TIME,
    INTERLOCK_FAULT_DOSE_REACH_UPPER_LIMIT,
    INTERLOCK_FAULT_MAX
};

int8_t interlock_fault_info_set(enum interlock_fault_info type, uint32_t value);
int8_t interlock_fault_info_clear(void);

int8_t interlock_status_value_locked_set(uint32_t value);
uint32_t interlock_status_get(void);
int8_t interlock_status_cleanup(void);
int8_t interlock_fault_register_callback(int8_t (*cb)(uint32_t interlock));

#ifdef __cplusplus
}
#endif

#endif /* __INTERLOCK_APP_H__ */