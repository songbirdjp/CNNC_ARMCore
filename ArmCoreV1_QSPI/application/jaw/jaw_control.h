#ifndef __JAW_CONTROL_H__
#define __JAW_CONTROL_H__

#include "kenimatic.h"
#include "init_call.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PREPARE_INPOS  (0.1*ENCODER_CNT_PER_MM + 0.5)   //precision: 0.1mm
#define MAX_SERVO_INPOS  (0.3*ENCODER_CNT_PER_MM + 0.5)   //precision: 0.3mm
typedef enum {
    INIT_MOVE_FORWARD = 100,
    INIT_MOVE_BACKWARD,//101
    LIMSWITCH_FALLING,//102
    LIMSWITCH_RISING,//103
    INIT_END,//104
    IDLE,//105
    PARK_START,//106
    PARK_END,//107
    PREPARE_START,//108
    PREPARE_END,//109
    SERVO,//110
    MANUAL_START,//111
    MANUAL_END,//112
    POWER_SAVE,//113
    SHUTDOWN,//114
    ERROR_STATE//115
} JawCtlFsm;

typedef enum {
    CMD_POS_KP_MODE,       
    CMD_POS_KI_MODE,       
    CMD_POS_KD_MODE,        
    CMD_SPD_KP_MODE,       
    CMD_SPD_KI_MODE,        
    CMD_SPD_KD_MODE,        
    CMD_PWM_MODE,        
    CMD_SPEED_MODE,      
    CMD_POSITION_MODE,   
    CMD_POWER_MODE,  
    CMD_DISPLAY_MODE,  
    CMD_ENCODE_MODE,
    CMD_FSM_STATUS,
    CMD_SECOND_POS,
    CMD_SWITCH_LEVEL,
    CMD_SET_PARAM
} UARTCmd;

typedef enum {
    COMMAND,    //state or pos changed
    LIMIT_SWITCH,   //limit switch is triggered
    ENC_Z,  //encoder Z signal
    PLAN_DATA
} JawSignalType;

typedef enum {
    SWITCH_PRESSED,    
    SWITCH_RELEASED  
} LimitSwitchStatus;

typedef struct
{
    JawCtlFsm MotorState;
    bool startMovingFlag;
    uint16_t encoderLast16;
    uint32_t encoderLast32;
    int16_t encoderDelta32;
    uint32_t encoderTotalCnt;
    uint16_t preparePos;
  //  uint16_t posInPlan;
    uint16_t crtPlanPos;
    int8_t newSign;
    int8_t lastSign;
    int8_t dirChangeFlag;
    uint16_t manualPos;
    //uint16_t realPlanCmd;
    SVG_Type fSVG;
    PIDAdjType uartPIDCmd;
    uint32_t location_timer;
   // uint16_t LastEncoderFindZero;
}JawControlInfo;

struct JawFlagType
{
    uint8_t axes;
    uint16_t JawEncZ[2];
    uint16_t JawLimit[2];
    uint16_t stateCmd[2];
    uint16_t cmdPos[2];
    uint16_t cmdTime; // <--- 新增：用于存放 RI 的运动时间 (ms)
};

typedef struct
{
    uint16_t jawMinADSetting;
    uint16_t jawMaxADSetting;
    uint16_t jawMaxVelocity;
    uint16_t jawTimeConst;
    uint16_t jawAcceleration;
    uint16_t jawHomeVelocity;
    uint16_t jawParkPos;
    uint16_t jaw2ndEncCalibrationPK;
    uint16_t jaw2ndEncCalibrationPB;
    uint16_t jawDualChTolerance;
    uint16_t limitPos;
    uint16_t limitNeg;
    uint16_t homeEncodeValue;
    uint16_t pkp;
    uint16_t pki;
    uint16_t pkd;
    uint16_t skp;
    uint16_t ski;
    uint16_t skd;
} JAW_SET_PARAM;

extern osMessageQueueId_t motor_signal_queueHandle;
extern JAW_SET_PARAM jawParameterByAxes[2];

void plcSetJawParam(uint8_t *pData);
void messageToJawTask(struct JawFlagType source, uint16_t signalType, uint8_t axes, uint16_t* value);
int8_t planJawPosCheck(uint16_t planPos, uint16_t actPos, uint8_t axes, uint16_t precision);

#ifdef __cplusplus
}
#endif

#endif