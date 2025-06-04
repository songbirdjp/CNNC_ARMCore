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

typedef struct{
    double Kp;
    double Ki;
    double Kd;
    double Setpoint;
    double Integral;
    double PreviousError;
    double IntegralLimit;
    double OutputLimit;
} PID_TypeDef;

typedef enum {
    MotorFSM_Idle,
    MotorFSM_Init,
    MotorFSM_Forward2FindZero,
    MotorFSM_Backward2FindZero,
    MotorFSM_POSITION_ADJUST,
    MotorFSM_ZERO_CONFIRMED,
    MotorFSM_StayAtPresetPos,
    MotorFSM_ManualControl,
    MotorFSM_AutoControl,
    MotorFSM_ERROR_STATE
} MotorFindingZeroFSM_t;

typedef struct{
    uint8_t AFCHandShakeOK;
    uint8_t AFCControlmode;
    uint8_t AFCSampleMode;
    uint16_t AFCSampleDelay;
} AFCConfigParam_TypeDef;

typedef struct{
    int8_t pwm;
    uint16_t encoderValCurrent;
    uint16_t encoderValTarget;
    uint16_t presetPos;
    PID_TypeDef *pid;
    uint8_t motorInitEnable;
    uint8_t motorFindZeroOK;
    AFTBrakeTypeDef motorBrakeStatus;
} MotorCtrlParam_TypeDef;

extern MotorFindingZeroFSM_t MagMotorState;

void motorCtrlByPWM(motorTypeDef motorType,float dutyCycle);
MotorCtrlParam_TypeDef *MAG_motorParam_get(void);
MotorCtrlParam_TypeDef *AFT_motorParam_get(void);
MotorFindingZeroFSM_t *MagMotorState_get(void);
#endif
