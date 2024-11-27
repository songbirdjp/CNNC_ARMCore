#ifndef __JAW_IO_H__
#define __JAW_IO_H__

#include "main.h"
#include <stdint.h>
#include "cmsis_os2.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

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

void motorPowerCtrl(uint8_t _powerCtrl,uint8_t axesType);
void BrakeCtrl(uint8_t _brakeCtrl, uint8_t axesType);
void setEncodeValue(uint16_t setValue, uint8_t axesType);
uint16_t getEncodeValue(uint8_t axesType);
void motorCtrlByPWM(double dutyCycle, uint8_t axesType);
void startPWMOutput(uint8_t axesType);
void stopPWMOutput(uint8_t axesType);
void motorEnable(uint8_t axesType);
void motorDisable(uint8_t axesType);
void startEncodeTim(uint8_t axesType);
void stopEncodeTim(uint8_t axesType);
void yjaw_nfault_callback(void);
void xjaw_nfault_callback(void);

#ifdef __cplusplus
}
#endif

#endif