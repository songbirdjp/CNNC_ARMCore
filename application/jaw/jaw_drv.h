#ifndef __JAW_IO_H__
#define __JAW_IO_H__

#include "main.h"
#include <stdint.h>
#include "cmsis_os2.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "ulog.h"

#ifdef __cplusplus
extern "C" {
#endif
enum jawAxes {X,Y,XY};

typedef struct
{
    uint16_t MotorMoveEn;
    uint16_t MotorDir;
    uint8_t EnableTriggernFault;
} JawMotorSignals;

int8_t motor_callback_register(uint8_t type, void (*callback)(uint8_t axesType));
void setEncodeValue(uint16_t setValue, uint8_t axesType);
uint16_t getEncodeValue(uint8_t axesType);
void motorCtrlByPWM(double dutyCycle, uint8_t axesType);
void motorEnable(uint8_t axesType);
void motorDisable(uint8_t axesType);
void yjaw_nfault_callback(void);
void xjaw_nfault_callback(void);
void motor_servo_start(uint8_t axesType);

#ifdef __cplusplus
}
#endif

#endif