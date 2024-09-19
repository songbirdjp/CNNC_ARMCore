#ifndef __INTERLOCK_APP_H__
#define __INTERLOCK_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum interlock_status_bits
{
    INTERLOCK_BOARD_POWER_FAULT = 0,
    INTERLOCK_HV_LIMIT,
    INTERLOCK_COMM_TIMEOUT,
    INTERLOCK_WDT_FAULT,
    INTERLOCK_ADCS7476_1_LIMIT_HIGH,
    INTERLOCK_ADCS7476_1_LIMIT_LOW,
    INTERLOCK_ADCS7476_2_LIMIT_HIGH,
    INTERLOCK_ADCS7476_2_LIMIT_LOW,
    INTERLOCK_ILLEGAL_WRITE,
    INTERLOCK_DOSE_RATE_LOW,
    INTERLOCK_DOSE_RATE_HIGH,
    INTERLOCK_DOSE_TOTAL_LOW,
    INTERLOCK_DOSE_TOTAL_HIGH,
    INTERLOCK_DOSE_SYMMETRY_FAULT,
    INTERLOCK_DOSE_DUMMY_TIMEOUT,
    INTERLOCK_MAX
};

int8_t interlock_status_set(enum interlock_status_bits bit, uint8_t value);
uint16_t interlock_status_get(void);
int8_t interlock_status_cleanup(void);
int8_t interlock_fault_register_callback(int8_t (*cb)(void));

#ifdef __cplusplus
}
#endif

#endif /* __INTERLOCK_APP_H__ */