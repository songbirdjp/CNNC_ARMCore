#ifndef __MCU_ADC_H__
#define __MCU_ADC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POWER_DIFF_TOLERANCE    0.05


int8_t board_power_limit_fault_get(void);
int8_t mcu_adc_sample_start(uint16_t sample_interval_10ns);

#ifdef __cplusplus
}
#endif

#endif /* __MCU_ADC_H__ */