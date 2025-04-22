#ifndef __ADC_APP_H__
#define __ADC_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum mcu_adc_channel
{
    MCU_ADC_CHANNEL_SF6 = 0,
};

float mcu_adc_value_get(enum mcu_adc_channel channel);


#ifdef __cplusplus
}
#endif

#endif /* __ADC_APP_H__ */