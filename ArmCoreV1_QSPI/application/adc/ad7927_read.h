#ifndef __ADC_GET_H__
#define __ADC_GET_H__
#include "cmsis_os2.h"
#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define     AD7927_CFGREG_11       1// Enable
#define     AD7927_CFGREG_10       0// SEQ
#define     AD7927_CFGREG_9        1// DON’T CARE
#define     AD7927_CFGREG_8        1// ADD2
#define     AD7927_CFGREG_7        0// ADD1
#define     AD7927_CFGREG_6        1// ADD0
#define     AD7927_CFGREG_5        1// PM1  Power Manager
#define     AD7927_CFGREG_4        1// PM0  Power Manager
#define     AD7927_CFGREG_3        0// Shadow Reg
#define     AD7927_CFGREG_2        1// DON’T CARE
#define     AD7927_CFGREG_1        0// Range is 0 to V_ref
#define     AD7927_CFGREG_0        1// twos complement or straight binary
#define     BUFFER                 0

HAL_StatusTypeDef AD7927_Init(SPI_HandleTypeDef *hspi);
uint16_t ADCgetValue(SPI_HandleTypeDef* hspi, uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif