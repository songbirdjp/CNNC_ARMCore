#ifndef __AFC_CMD_H__
#define __AFC_CMD_H__

#include <stdint.h>

typedef enum
{
    UARTCmdType_HandshakeDown = 0x01,
    UARTCmdType_CommandDown,
    UARTCmdType_RtDataDown
} UARTCmdType_t;

typedef struct{
    uint8_t AFCHandShakeOK;
    uint8_t AFCControlmode;
    uint8_t AFCSampleMode;
    uint16_t AFCSampleDelay;
} AFCConfigParam_TypeDef;
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

typedef struct{
    //int8_t pwm;
    uint16_t encoderValCurrent;
    uint16_t encoderValTarget;
    uint16_t presetPos;
    uint16_t deadZone;
    //PID_TypeDef *pid;
    uint8_t motorStatus;
    uint8_t motorFindZeroOK;
    AFTBrakeTypeDef motorBrakeStatus;
} MotorCtrlParam_TypeDef;

#define CMD_MagMotorCtrl                0x40
#define SubCmd_MagMotorEn               0x00
#define SubCmd_MagMotorFindZero         0x01
#define SubCmd_MagMotorPosCtrl          0x02
#define SubCmd_MagMotorFreeMove         0x03
#define SubCmd_MagMotorMoveByStep       0x04

#define CMD_AFTMotorCtrl                0x42
#define SubCmd_AFTBrakeCtrl             0x00
#define SubCmd_AFTMotorEn               0x01
#define SubCmd_AFTMotorFindZero         0x02
#define SubCmd_AFTMotorPosCtrl          0x03
#define SubCmd_AFTMotorFreeMove         0x04
#define SubCmd_AFTMotorMoveByStep       0x05

#define AFC_SHELL_CMD
int BGM2AFC_Handshake(void);
void AFC_SetAFCControlMode(uint8_t mode);
// void AFC_SetADCSampleMode(uint8_t mode);
void AFC_SetADCSampleDelay(uint16_t delay);
void AFC_DeleteADCData(void);
void AFC_GetADCValueByFrame(void);

void AFC_IS_MagMotorFindZeroOK(void);
void AFC_MagMotorSetPos(uint16_t pos);
void AFC_MagMotorRunByStep(uint8_t dir,uint16_t _stepVal);
void AFC_MagMotorGetEncValue(void);
void AFC_MagMotorSetPresetPos(uint16_t pos);
void AFC_MagMotorSetDeadZone(uint16_t deadZone);

void AFC_IS_AFTMotorFindZeroOK(void);
void AFC_AFTMotorSetPos(uint16_t pos);
void AFC_AFTMotorRunByStep(uint8_t dir,uint16_t _stepVal);
void AFC_AFTMotorGetEncValue(void);
void AFC_AFTMotorSetPresetPos(uint16_t pos);
void AFC_SetAFTBrakeStatus(uint8_t status);
void AFC_SetAFTMotorStatus(uint8_t status); 
void AFC_AFTMotorSetDeadZone(uint16_t deadZone);
#endif