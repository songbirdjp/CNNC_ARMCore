#ifndef __MOTORCTRL_H__ 
#define __MOTORCTRL_H__

#include "stdint.h"

typedef enum 
{
    MOTOR_MAG,
    MOTOR_AFT
}motorTypeDef;

typedef enum 
{
    AFT_BRAKE_OFF,
    AFT_BRAKE_ON
}AFTBrakeTypeDef;

typedef struct
{
    uint16_t MotorMoveEn;
    uint16_t MotorDir;
    uint8_t EnableTriggernFault;
    
} MotorCtrlSignalDef_t;


#endif
