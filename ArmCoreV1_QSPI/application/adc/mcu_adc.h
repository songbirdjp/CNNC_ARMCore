#ifndef __MCU_ADC_H__
#define __MCU_ADC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POWER_DIFF_TOLERANCE    0.05

enum mcu_adc_channel
{
    MCU_ADC_CHANNEL_P5V = 0,
    MCU_ADC_CHANNEL_REF,
};

float mcu_adc_value_get(enum mcu_adc_channel channel);
int8_t board_power_limit_fault_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __MCU_ADC_H__ */