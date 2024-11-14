#ifndef __JAW_CONTROL_H__
#define __JAW_CONTROL_H__

#include "kenimatic.h"
#include "init_call.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    INIT_MOVE_FORWARD = 10,
    INIT_MOVE_BACKWARD,
    INIT_END,
    IDLE,
    PREPARE_START,
    PREPARE_END,
    SERVO,
    LIMSWITCH_FALLING,
    LIMSWITCH_RISING,
    ZERO_CONFIRMED,
    ERROR_STATE
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
    CMD_CLEAR_STATUS
} UARTCmd;

typedef struct
{
    uint16_t jawCurrentPos;
    uint16_t jawStatusInfo;
}JawFeedbackInfo;

typedef struct
{
    JawCtlFsm MotorState;
    bool startMovingFlag;
    uint16_t encoderLast;
    uint16_t homeEncodeValue;
    uint16_t oldPlanCmd;
    uint16_t posInPlan;
    SVG_Type fSVG;
    PIDAdjType uartPIDCmd;
    uint32_t location_timer;
    uint16_t LastEncoderFindZero;
}JawControlInfo;

struct JawFlagType
{
    uint8_t axes;
    uint8_t JawEncZ[2];
    uint8_t JawLimit[2];
    uint16_t masterCmd[2];
  //  float tmp;
};

typedef struct
{
    uint16_t jawMinADSetting;
    uint16_t jawMaxADSetting;
    uint16_t jaw2ndEncCalibrationPK;
    uint16_t jaw2ndEncCalibrationPB;
    uint16_t jawDualChTolerance;
} JAW_SET_PARAM;

extern osMessageQueueId_t motor_signal_queueHandle;
extern JawFeedbackInfo jawFeedbackByAxes[2];
extern JAW_SET_PARAM jawParameterByAxes[2];

void setJawParam(uint8_t *pData);

#ifdef __cplusplus
}
#endif

#endif