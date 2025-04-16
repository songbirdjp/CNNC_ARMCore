/**
 * @file drv_pin.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _DRV_PIN_H_
#define _DRV_PIN_H_

#include "dev_pin.h"
#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME_PIN_DO_PULSE_INHIBIT "doPulseInhibit"
#define DEVICE_NAME_PIN_DO_EMERGENCY "doEmergency"
#define DEVICE_NAME_PIN_DO_KV_TREATMENT_EN "doKvTreatment"
#define DEVICE_NAME_PIN_DO_MV_TREATMENT_EN "doMvTreatment"

#define DEVICE_NAME_PIN_DI_INT "diInt"
#define DEVICE_NAME_PIN_DI_GATING "diGating"

#define DEVICE_NAME_PIN_RUN_LED_1 "RUN_LED1"
#define DEVICE_NAME_PIN_RUN_LED_2 "RUN_LED2"
#define DEVICE_NAME_PIN_RUN_LED_3 "RUN_LED3"
#define DEVICE_NAME_PIN_RUN_LED_4 "RUN_LED4"
#define DEVICE_NAME_PIN_RUN_LED_5 "RUN_LED5"
#define DEVICE_NAME_PIN_RUN_LED_6 "RUN_LED6"

void driver_pin_init(void);
int8_t pin_test(void);

#ifdef __cplusplus
}
#endif

#endif
