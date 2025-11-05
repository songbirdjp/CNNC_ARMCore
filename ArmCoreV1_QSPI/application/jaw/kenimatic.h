#ifndef __KENIMATIC_H__
#define __KENIMATIC_H__

#include "jaw_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENCODER_PULSES_PER_TURN 1024
#define ENCODER_CNT_PER_MM  204.8

#define sign(a) ( (a<0)?-1:1 )

#define STATE_STANDSTILL 0
#define STATE_MOVING 1
#define STATE_STOPPING 2
#define STATE_ESTOPPING 3
#define STATE_ERROR 4

#define ERROR_DISABLED 99
#define STATUS_OK 0
#define ERROR_SVG_CYCLETIME 101
#define ERROR_SVG_OVERRIDE 102
#define ERROR_SVG_LIMITS_REACHED 103
#define ERROR_SVG_DYNCALC 104
#define ERROR_SVG_LIMIT_POS 105
#define ERROR_SVG_LIMIT_VEL 106
#define ERROR_SVG_LIMIT_ACC 107
#define ERROR_SVG_LIMIT_JERK 108
#define ERROR_SVG_LIMIT_POS_NEG 109
#define ERROR_SVG_LIMIT_POS_POS 110

typedef enum {
    SPEED_ADJ,
    POS_ADJ,
    DOUBLE_ADJ
} PIDAdjType;

typedef struct
{
    double Kp;
    double Ki;
    double Kd;
    double Setpoint;
    double Integral;
    double PreviousError;
    double IntegralLimit;
    double OutputLimit;
    double deadZone;
    double integralStartErr;
} PID_TypeDef;

typedef struct Axis_Parameter_Limits_Type
{
    double PositionPos;
    double PositionNeg;
    double VelocityPos;
    double VelocityNeg;
    double AccelerationPos;
    double AccelerationNeg;
    double JerkPos;
    double JerkNeg;
} Axis_Parameter_Limits_Type;
/* Declaration of datatype and function for Set-Value Generation */

typedef struct SVG_Type
{
    double TargetPosition;
    double StartPosition;
    struct Axis_Parameter_Limits_Type DynamicLimits;
    struct Axis_Parameter_Limits_Type DynamicValues;
    double Cycletime;
    double Override;
    double Position;
    double Speed;
    double Acceleration;
    unsigned short Status;
    unsigned char State;
    unsigned char Phase;
    signed char moveDirection;
    double beginPosition;
    double beginSpeed;
    double beginAcc;
    double endPosition;
    double elapsedTime;
    double ds;
    double dt[7];
    double delta;
    double epsilon;
    double v;
    double a;
    double j;
    double debug1;
    double debug2;
    unsigned short Enable;
    unsigned short Start;
    unsigned short Stop;
    unsigned short EStop;
    unsigned short Done;
    unsigned short endLimits;
} SVG_Type;

typedef struct SVG_Motion_Param
{
    double v_max; 
    double a_start_max;
    double a_end_max;
    double v_start;
    double v_end;
    double T1;
    double T2;
    double T3;
    double T4;
    double T5;
    double T6;
    double T7;
}SVG_Motion_Param;

void initSVG(struct SVG_Type* inst, uint8_t axes);
int8_t SVG(struct SVG_Type* inst, uint8_t axes);
float PositionPIDCtrl(uint32_t current_position, uint32_t _setPosition, PID_TypeDef *PID_parameters);
float SpeedPIDCtrl(float actualSpeed, float _setSpeed, PID_TypeDef *PID_parameters);

#ifdef __cplusplus
}
#endif

#endif