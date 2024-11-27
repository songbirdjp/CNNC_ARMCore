#ifndef __ADC_APP_H__
#define __ADC_APP_H__
#include "ad7927_read.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ADC_VALUE   4096 //12 bits ADC
#define ADC_WORK_LENGTH   125 //unit£ºmm
#define ADC_RESOLUTION   ADC_WORK_LENGTH/MAX_ADC_VALUE //unit£ºmm

#ifdef __cplusplus
}
#endif

#endif