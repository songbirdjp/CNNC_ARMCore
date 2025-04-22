#ifndef __ADC_PORT_H__
#define __ADC_PORT_H__

#include <stdint.h>
#include "adc.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME_ADC1_DEFAULT    "ADC1"
#define DEVICE_NAME_ADC3_DEFAULT    "ADC3"

#define ADC1_CHANNEL_NUM            1
#define ADC3_CHANNEL_NUM            3

#define ADC_CHANNEL_NUM_MAX         (ADC1_CHANNEL_NUM > ADC3_CHANNEL_NUM ? ADC1_CHANNEL_NUM : ADC3_CHANNEL_NUM)

struct adc_object
{
    ADC_HandleTypeDef *hadc;
    osMutexId_t mutex;
    osEventFlagsId_t event;
    uint32_t event_flag;
    uint16_t data[ADC_CHANNEL_NUM_MAX];

    void (*init)(void);
    uint8_t channel_num;
    uint16_t *result;
};

struct adc_object *adc_object_get(uint8_t *device_name);
int8_t adc_init(uint8_t *device_name, osEventFlagsId_t event);
int8_t adc_sample_interval_set(uint16_t sample_interval_10ns);
int8_t adc_sample_start(void);
int8_t adc_sample_data_amend(uint16_t *data, uint8_t channel_num);


#ifdef __cplusplus
}
#endif

#endif /* __ADC_PORT_H__ */
