#include "jaw_control.h"
#include "main_app.h"
#include "shell.h"
#include "planData.h"
#include <stdlib.h>
#include "ad7927_app.h"

#define PERMIT_FOLLOWING_ERR  250  //when PWM = 100, speed is 10count/4ms
#define MAX_ENCODE_VAL  65535
#define INIT_SPEED  80
#define MAX_TWO_CHANNEL_DIFF ENCODER_PULSES_PER_TURN //5mm

#define JAWY_NFAULT_NAME "GPIOD_6"
#define JAWX_NFAULT_NAME "GPIOA_15"
#define JAWX_ENCZ_NAME "GPIOA_8"
#define JAWY_ENCZ_NAME "GPIOB_10"
#define JAWX_LIMIT_NAME "GPIOG_14"
#define JAWY_LIMIT_NAME "GPIOB_11"

#define Y_LIMIT_POS 117// the distance from limit switch to @ISO 120mm = 107.94mm, Y INIT pos = 6.68mm
#define X_LIMIT_POS 86// the distance from limit switch to @ISO 50mm = 71.45mm, X INIT pos = 12.49mm

#define SAMP_BUF_SIZE    1
#define MAX_PREPARE_INPOS  (0.1*ENCODER_CNT_PER_MM + 0.5)   //precision: 0.1mm

//JawFeedbackInfo jawFeedbackByAxes[2];
JAW_SET_PARAM jawParameterByAxes[2];
osMessageQueueId_t motor_signal_queueHandle = NULL;
static JawControlInfo jawControlByAxes[2];
static float prtBufCmd[SAMP_BUF_SIZE]; // sample command position
static float prtBufAct[SAMP_BUF_SIZE]; // sample actual value

PID_TypeDef motor_pid_pos[2];
PID_TypeDef motor_pid_spd[2];

void yjaw_EncZ_callback(void);
void xjaw_EncZ_callback(void);
void yjaw_limitSwitch_callback(void);
void xjaw_limitSwitch_callback(void);

//#define SECOND_PERMIT_ERROR    8 // 0.244mm
static uint16_t jawHomePos[2] = {2558,1368};
static uint16_t avoidCollisionDist[2] = {31796,31387};//= Cx1/Cy1+Cx2/Cy2-204.8, 2 jaw minimum distance is 1mm

#ifdef BANKA
   // static uint16_t jawEncZero[2] = {2258,1845};
   // static uint16_t initSecondPosCheck[2] = {873,848};
    double paraK[2] = {6.5571, 6.5726};
    double paraB[2] = {-3453.33, -2897.54};
#else
    //static uint16_t jawEncZero[2] = {2254,1843};
   // static uint16_t initSecondPosCheck[2] = {908,754};
    double paraK[2] = {6.5138, 6.5028};
    double paraB[2] = {-3207.74, -2885.02};
#endif

static void motorParamInit(uint8_t axes)
{
    memset(&motor_pid_pos[axes], 0, sizeof(PID_TypeDef));
    memset(&motor_pid_spd[axes], 0, sizeof(PID_TypeDef));

    motor_pid_pos[axes].Kp = (double)jawParameterByAxes[axes].pkp/100;//2.0;
    motor_pid_pos[axes].Ki = (double)jawParameterByAxes[axes].pki/100;//0.2;
    motor_pid_pos[axes].Kd = (double)jawParameterByAxes[axes].pkd/100;//0;
    motor_pid_pos[axes].Setpoint = 15000;
    motor_pid_pos[axes].IntegralLimit =50;
    motor_pid_pos[axes].OutputLimit = 999;
    motor_pid_pos[axes].deadZone = 5;
    motor_pid_pos[axes].integralStartErr = 200;

    motor_pid_spd[axes].Kp = (double)jawParameterByAxes[axes].skp/100;//0.8;
    motor_pid_spd[axes].Ki = (double)jawParameterByAxes[axes].ski/100;//0.8;
    motor_pid_spd[axes].Kd = (double)jawParameterByAxes[axes].skd/100;//0;
    motor_pid_spd[axes].Setpoint = 0;
    motor_pid_spd[axes].IntegralLimit =50;
    motor_pid_spd[axes].OutputLimit = 100;
    motor_pid_spd[axes].deadZone = 3;
    motor_pid_spd[axes].integralStartErr = 20;

    initSVG(&jawControlByAxes[axes].fSVG, axes);
}

static void jawDeviceInit(void)
{
    memset(&jawControlByAxes, 0, sizeof(JawControlInfo) * 2);
  //  memset(&jawFeedbackByAxes, 0, sizeof(JawFeedbackInfo) * 2);
    memset(prtBufAct, 0, SAMP_BUF_SIZE * sizeof(float));
    memset(prtBufCmd, 0, SAMP_BUF_SIZE * sizeof(float));

    gpio_pin_irq_callback_register(JAWX_NFAULT_NAME, xjaw_nfault_callback);
    gpio_pin_irq_callback_register(JAWY_NFAULT_NAME, yjaw_nfault_callback);
    gpio_pin_irq_callback_register(JAWX_ENCZ_NAME, xjaw_EncZ_callback);
    gpio_pin_irq_callback_register(JAWY_ENCZ_NAME, yjaw_EncZ_callback);
    gpio_pin_irq_callback_register(JAWX_LIMIT_NAME, xjaw_limitSwitch_callback);
    gpio_pin_irq_callback_register(JAWY_LIMIT_NAME, yjaw_limitSwitch_callback);

    jawParameterByAxes[X].homeEncodeValue = jawHomePos[X];
    jawParameterByAxes[Y].homeEncodeValue = jawHomePos[Y];
    jawParameterByAxes[X].limitPos = (uint16_t)(X_LIMIT_POS * ENCODER_CNT_PER_MM);//17613
    jawParameterByAxes[Y].limitPos = (uint16_t)(Y_LIMIT_POS * ENCODER_CNT_PER_MM);//23962
    jawParameterByAxes[X].limitNeg = jawHomePos[X];
    jawParameterByAxes[Y].limitNeg = jawHomePos[Y];
    jawParameterByAxes[X].pkp = jawParameterByAxes[Y].pkp = 200;
    jawParameterByAxes[X].pki = jawParameterByAxes[Y].pki = 100;
    jawParameterByAxes[X].pkd = jawParameterByAxes[Y].pkd = 0;
    jawParameterByAxes[X].skp = jawParameterByAxes[Y].skp = 80;
    jawParameterByAxes[X].ski = jawParameterByAxes[Y].ski = 80;
    jawParameterByAxes[X].skd = jawParameterByAxes[Y].skd = 0;

    motorParamInit(X);
    motorParamInit(Y);
}

void messageToJawTask(struct JawFlagType source, uint16_t signalType, uint8_t axes, uint16_t* value)
{
    memset(&source, 0 , sizeof(struct JawFlagType));
    source.axes = axes;
   
    switch(signalType)
    {
        case COMMAND:
            if(axes == XY)
            {
                source.stateCmd[X] = value[X];
                source.stateCmd[Y] = value[Y];
            }
            else source.stateCmd[axes] = *value;
        break;
        case PLAN_DATA:
            if(axes == XY)
            {
                source.cmdPos[X] = value[X];
                source.cmdPos[Y] = value[Y];
            }
            else source.cmdPos[axes] = *value;
        break;
        default: break;
    }  
    osMessageQueuePut(motor_signal_queueHandle, &source, 0, 0);
}

void getEncodeTotalValue(uint8_t axes)
{
    __IO uint16_t crtEncoder = 0;
    int16_t encoderDelta16 = 0;

    if(jawControlByAxes[axes].encoderTotalCnt > 0)
    {
        crtEncoder = getEncodeValue(axes);
        encoderDelta16 = crtEncoder - jawControlByAxes[axes].encoderLast16;
       // if(encoderDelta16 == 0) return;
        if(abs(encoderDelta16) > ENCODER_PULSES_PER_TURN) 
        {
            if(encoderDelta16 < 0)    jawControlByAxes[axes].encoderTotalCnt += 0x10000;//overflow
            else    jawControlByAxes[axes].encoderTotalCnt -= 0x10000;//underflow
        }
        jawControlByAxes[axes].encoderTotalCnt += encoderDelta16;
        jawControlByAxes[axes].encoderLast16 = crtEncoder;
        jawControlByAxes[axes].encoderDelta32 = (jawControlByAxes[axes].encoderTotalCnt - jawControlByAxes[axes].encoderLast32)/4;//use for speed cal
        jawControlByAxes[axes].encoderLast32 = jawControlByAxes[axes].encoderTotalCnt;
        rtFeedback.jawRTPos[axes] = jawControlByAxes[axes].encoderTotalCnt/4;

        //printf("enc32 %d\r\n", jawControlByAxes[axes].encoderDelta32);
    } 
}

uint8_t setEncodeTotalValue(uint16_t setValue, uint8_t axes)
{
    jawControlByAxes[axes].encoderTotalCnt = setValue*4;
    if(jawControlByAxes[axes].encoderTotalCnt > MAX_ENCODE_VAL) return 0;

    jawControlByAxes[axes].encoderLast32 = jawControlByAxes[axes].encoderTotalCnt;
    jawControlByAxes[axes].encoderLast16 = jawControlByAxes[axes].encoderTotalCnt;
    rtFeedback.jawRTPos[axes] = setValue;
    setEncodeValue(jawControlByAxes[axes].encoderTotalCnt, axes);

    return 1;
}

void yjaw_EncZ_callback(void)
{
    struct JawFlagType Jawflag;
    uint16_t value = 0;
    uint8_t axes = Y;

    if(jawControlByAxes[axes].MotorState == INIT_END)
    {
      //  printf("%d\r\n",secondPosFeedback.jawSecondPos[axes]);
        setEncodeTotalValue(jawParameterByAxes[axes].homeEncodeValue, axes);
        value = INIT_END;
        messageToJawTask(Jawflag, COMMAND, axes, &value);
    }
    else if(jawControlByAxes[axes].MotorState == UART_DEBUG){
        value = getEncodeValue(axes)/4;
      //  printf("%d,%d\r\n",value,secondPosFeedback.jawSecondPos[axes]);
        printf("%d,",value);
      //  printf("%d,",secondPosFeedback.jawSecondPos[axes]);
    }
}

void xjaw_EncZ_callback(void)
{
    struct JawFlagType Jawflag;
    uint16_t value = 0;
    uint8_t axes = X;
    static uint8_t cnt = 0;

    if(jawControlByAxes[axes].MotorState == INIT_END)
    {
     //   printf("%d\r\n",secondPosFeedback.jawSecondPos[axes]);
        setEncodeTotalValue(jawParameterByAxes[axes].homeEncodeValue, axes);
        value = INIT_END;
        messageToJawTask(Jawflag, COMMAND, axes, &value);   
    }
    else if(jawControlByAxes[axes].MotorState == UART_DEBUG){
        value = getEncodeValue(axes)/4;
     //   printf("%d,%d\r\n",value,secondPosFeedback.jawSecondPos[axes]);
       printf("%d,",value);
      //  printf("%d,",secondPosFeedback.jawSecondPos[axes]);
    }
}

int8_t checkSwitchLevel(uint8_t axes)
{
    if(axes == X)
    {
        if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_14) == GPIO_PIN_RESET)   return 0;
        else    return 1;
    }
    else if(axes == Y)
    {
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET)   return 0;
        else    return 1;
    }

    return -1;
}

void yjaw_limitSwitch_callback(void)
{
    struct JawFlagType Jawflag;
    uint16_t value = 0;
    uint8_t axes = Y;
   
    if(checkSwitchLevel(axes) == SWITCH_PRESSED)
    {
        if(jawControlByAxes[axes].MotorState == LIMSWITCH_FALLING){
            value = LIMSWITCH_FALLING;
            messageToJawTask(Jawflag, COMMAND, axes, &value);
        } 
        else if(jawControlByAxes[axes].MotorState > INIT_END){
            interlockFeedback.jawInterlock[axes] |= 0x200;//bit 9 
            value = ERROR_STATE;
            messageToJawTask(Jawflag, COMMAND, axes, &value);
        }        
    } 
    // else 
    // {
    //     if(jawControlByAxes[axes].MotorState > INIT_END)   interlockFeedback.jawInterlock[axes] &= ~0x200;//bit 9           
    // } 
}

void xjaw_limitSwitch_callback(void)
{
    struct JawFlagType Jawflag;
    uint16_t value = 0;
    uint8_t axes = X;

    if(checkSwitchLevel(axes) == SWITCH_PRESSED)   
    {
        if(jawControlByAxes[axes].MotorState == LIMSWITCH_FALLING){
            value = LIMSWITCH_FALLING;
            messageToJawTask(Jawflag, COMMAND, axes, &value);
        } 
        else if(jawControlByAxes[axes].MotorState > INIT_END){
            interlockFeedback.jawInterlock[axes] |= 0x200;//bit 9 
            value = ERROR_STATE;
            messageToJawTask(Jawflag, COMMAND, axes, &value);
        }        
    } 
    // else 
    // {
    //     if(jawControlByAxes[axes].MotorState > INIT_END)   interlockFeedback.jawInterlock[axes] &= ~0x200;//bit 9           
    // } 
}

void plcSetJawParam(uint8_t *pData)
{
    uint16_t value = 0;

    jawParameterByAxes[X].jawMinADSetting = (pData[91] << 8) + pData[90];//fetch jaw parameter
    jawParameterByAxes[X].jawMaxADSetting = (pData[93] << 8) + pData[92];
    value = (pData[95] << 8) + pData[94];
    if(value > 0)   jawParameterByAxes[X].homeEncodeValue = value;
    value = (pData[97] << 8) + pData[96];
    if(value > 0)   jawParameterByAxes[X].pkp = value;
    value = (pData[99] << 8) + pData[98];
    jawParameterByAxes[X].pki = value;
    value = (pData[101] << 8) + pData[100];
    jawParameterByAxes[X].pkd = value;
    value = (pData[103] << 8) + pData[102];
    if(value > 0)   jawParameterByAxes[X].skp = value;
    value = (pData[105] << 8) + pData[104];
    jawParameterByAxes[X].ski = value;
    value = (pData[107] << 8) + pData[106];
    jawParameterByAxes[X].skd = value;
    jawParameterByAxes[X].jawMaxVelocity = (pData[109] << 8) + pData[108];
    jawParameterByAxes[X].jawTimeConst = (pData[111] << 8) + pData[110];
    jawParameterByAxes[X].jawAcceleration = (pData[113] << 8) + pData[112];
    jawParameterByAxes[X].jawHomeVelocity = (pData[115] << 8) + pData[114];
    jawParameterByAxes[X].jawParkPos = (pData[117] << 8) + pData[116];
   // printf("param %x %x %x %x %x %x", pData[114],pData[115],pData[116],pData[117],pData[118],pData[119]);
    jawParameterByAxes[X].jaw2ndEncCalibrationPK = (pData[119] << 8) + pData[118];
    jawParameterByAxes[X].jaw2ndEncCalibrationPB = (pData[121] << 8) + pData[120];
    value = (pData[123] << 8) + pData[122];
    if(value > 0)   jawParameterByAxes[X].limitNeg = value;
    value = (pData[125] << 8) + pData[124];
    if(value > 0)   jawParameterByAxes[X].limitPos = value;
    jawParameterByAxes[Y].jawMinADSetting = (pData[129] << 8) + pData[128];
    jawParameterByAxes[Y].jawMaxADSetting = (pData[131] << 8) + pData[130];
    value = (pData[133] << 8) + pData[132];
    if(value > 0)   jawParameterByAxes[Y].homeEncodeValue = value;
    value = (pData[135] << 8) + pData[134];
    if(value > 0)   jawParameterByAxes[Y].pkp = value;
    value = (pData[137] << 8) + pData[136];
    jawParameterByAxes[Y].pki = value;
    value = (pData[139] << 8) + pData[138];
    jawParameterByAxes[Y].pkd = value;
    value = (pData[141] << 8) + pData[140];
    if(value > 0)   jawParameterByAxes[Y].skp = value;
    value = (pData[143] << 8) + pData[142];
    jawParameterByAxes[Y].ski = value;
    value = (pData[145] << 8) + pData[144];
    jawParameterByAxes[Y].skd = value;
    jawParameterByAxes[Y].jawMaxVelocity = (pData[147] << 8) + pData[146];
    jawParameterByAxes[Y].jawTimeConst = (pData[149] << 8) + pData[148];
    jawParameterByAxes[Y].jawAcceleration = (pData[151] << 8) + pData[150];
    jawParameterByAxes[Y].jawHomeVelocity = (pData[153] << 8) + pData[152];
    jawParameterByAxes[Y].jawParkPos = (pData[155] << 8) + pData[154];
    jawParameterByAxes[Y].jaw2ndEncCalibrationPK = (pData[157] << 8) + pData[156];
    jawParameterByAxes[Y].jaw2ndEncCalibrationPB = (pData[159] << 8) + pData[158];
    value = (pData[161] << 8) + pData[160];
    if(value > 0)   jawParameterByAxes[Y].limitNeg = value;
    value = (pData[163] << 8) + pData[162];
    if(value > 0)   jawParameterByAxes[Y].limitPos = value;

    printf("JawX home:%d\r\n",jawParameterByAxes[X].homeEncodeValue);
    printf("JawY home:%d\r\n",jawParameterByAxes[Y].homeEncodeValue);
    printf("JawX park:%d\r\n",jawParameterByAxes[X].jawParkPos);
    printf("JawY park:%d\r\n",jawParameterByAxes[Y].jawParkPos);
    printf("JawX limit pos:%d - %d\r\n",jawParameterByAxes[X].limitNeg, jawParameterByAxes[X].limitPos);
    printf("JawY limit pos:%d - %d\r\n",jawParameterByAxes[Y].limitNeg, jawParameterByAxes[Y].limitPos);
}

int8_t planJawPosCheck(uint16_t planPos, uint16_t actPos, uint8_t axes)
{
   // if((planPos < jawControlByAxes[axes].homeEncodeValue) || (planPos >= jawControlByAxes[axes].limitPos))   return -1; //pos error
    if(planPos < jawParameterByAxes[axes].limitNeg)
    {
        interlockFeedback.jawInterlock[axes] |= 0x4;
        printf("%d jaw plan too small %d!\r\n",axes,planPos);
       // rtFeedback.jawInfo[axes] |= 0x80;
        return -1;
    }
    else if(planPos > jawParameterByAxes[axes].limitPos)
    {
        interlockFeedback.jawInterlock[axes] |= 0x8;
        printf("%d jaw plan too big %d!\r\n",axes,planPos);
       // rtFeedback.jawInfo[axes] |= 0x80;
        return -1;
    }
    if((planPos <= (actPos + motor_pid_pos[axes].deadZone)) && (planPos >= (actPos - motor_pid_pos[axes].deadZone))) return 0;

    return 1;//normal
}
#if 0
static int8_t prepareCheck(uint16_t planPos, uint16_t actPos, uint8_t axes)
{
    uint16_t actualPlanPos = (uint16_t)(planPos/2.5);

    //if(planPos == jawControlByAxes[axes].oldPlanCmd) return 0;//plan pos no change 
    if((actualPlanPos < jawControlByAxes[axes].homeEncodeValue) || (actualPlanPos > MAX_ENCODE_VAL))   return -1; //pos error
    if((actualPlanPos < (actPos + motor_pid_pos[axes].deadZone)) && (actualPlanPos > (actPos - motor_pid_pos[axes].deadZone))) return 0;

    return 1;//normal
}

static int8_t planCheck(uint16_t planPos, uint8_t axes)
{
    uint16_t actualPlanPos = (uint16_t)(planPos/2.5);

    if((actualPlanPos < jawControlByAxes[axes].homeEncodeValue) || (actualPlanPos > MAX_ENCODE_VAL))   return -1; //pos error
    if(planPos == jawControlByAxes[axes].oldPlanCmd) return 0;//plan pos no change 

    return 1;//normal
}

static bool initSecPosCheck(uint8_t axes)
{
    if((secondPosFeedback.jawSecondPos[axes] < (initSecondPosCheck[axes]+SECOND_PERMIT_ERROR)) &&
        (secondPosFeedback.jawSecondPos[axes] > (initSecondPosCheck[axes]-SECOND_PERMIT_ERROR)))
        return 1;
    else  return 0;
}
#endif
void JawCtrlLoop(struct JawFlagType *pFlag, uint8_t axes)
{
    int8_t checkRet = 0;
   // uint16_t jawEncInitVal = 0;
    bool findZero = 0;
    uint16_t enc = 0;

    switch (pFlag->stateCmd[axes])
    {
    case FSM_INIT:
        jawControlByAxes[axes].MotorState = INIT_MOVE_BACKWARD;
           // printf("Jaw INIT\r\n");
        break;
    case FSM_IDLE:
        jawControlByAxes[axes].MotorState = IDLE;
        //  printf("jaw idle\r\n");
        break;
    case FSM_PARK:
        jawControlByAxes[axes].MotorState = PARK_START;
       // printf("jaw%d  PARK\r\n", axesType);
        break;
    case FSM_PREPARE:
        jawControlByAxes[axes].MotorState = PREPARE_START;
        break;
    case FSM_SERVO:
        jawControlByAxes[axes].MotorState = SERVO;
        break;
    case FSM_SHUTDOWN:
        jawControlByAxes[axes].MotorState = SHUTDOWN;
        break;
    case FSM_POWERSAVE:
        jawControlByAxes[axes].MotorState = POWER_SAVE;
        break;
    case ERROR_STATE:
        jawControlByAxes[axes].MotorState = ERROR_STATE;
        break;
    case UART_DEBUG:
        jawControlByAxes[axes].MotorState = UART_DEBUG;
        break;
    default:    break;
    }

    // printf("state %d\r\n",MotorState[axesType]);
    switch (jawControlByAxes[axes].MotorState)
    {
    case INIT_MOVE_BACKWARD:
        motorParamInit(axes);
        int8_t swState = checkSwitchLevel(axes);
       // jawFeedbackByAxes[axesType].jawStatusInfo |= 0x01;
        if(swState == SWITCH_PRESSED){
            jawControlByAxes[axes].MotorState = INIT_MOVE_FORWARD;
        }
        else if(swState == SWITCH_RELEASED)
        {
            setEncodeTotalValue(1000, axes);
            motor_pid_spd[axes].Setpoint = -INIT_SPEED;
            jawControlByAxes[axes].uartPIDCmd = SPEED_ADJ;
            jawControlByAxes[axes].MotorState = LIMSWITCH_FALLING;

            if(!jawControlByAxes[axes].startMovingFlag)
            {
                jawControlByAxes[axes].location_timer = 0;
                jawControlByAxes[axes].startMovingFlag = 1;
                motorEnable(axes);
            } 
            printf("jaw%d init move backward speed %lf rpm\r\n", axes, motor_pid_spd[axes].Setpoint);
        }
        break;
    case LIMSWITCH_FALLING:
        if(pFlag->stateCmd[axes] == LIMSWITCH_FALLING)
        {
            printf("jaw%d switch fall\r\n",axes);
            jawControlByAxes[axes].MotorState = INIT_MOVE_FORWARD;
            jawControlByAxes[axes].startMovingFlag = 0;
        }
        break;
    case INIT_MOVE_FORWARD:
        if (pFlag->stateCmd[axes] == INIT_MOVE_FORWARD)
        {
            setEncodeTotalValue(1000, axes);
            motor_pid_spd[axes].Setpoint = INIT_SPEED;
            jawControlByAxes[axes].MotorState = LIMSWITCH_RISING;
            jawControlByAxes[axes].location_timer = 0;
            jawControlByAxes[axes].startMovingFlag = 1;
            motorEnable(axes);
            printf("jaw%d init move forward speed %lf rpm\r\n", axes, motor_pid_spd[axes].Setpoint);
        }
        break; 
    case LIMSWITCH_RISING:
        if(pFlag->stateCmd[axes] == LIMSWITCH_RISING)
        {
            printf("jaw%d switch rise\r\n",axes);
            jawControlByAxes[axes].MotorState = INIT_END;
        }
        break;   
    case INIT_END:
        if(pFlag->stateCmd[axes] == INIT_END)
        {
           // printf("ENC%d trigger Z %d!\r\n", axes,rtFeedback.jawRTPos[axes]);
            jawControlByAxes[axes].startMovingFlag = 0;
        }
        break;
    case IDLE:
        rtFeedback.jawInfo[axes] = 0;
        interlockFeedback.jawInterlock[axes] = 0;
        jawControlByAxes[axes].startMovingFlag = 0;
        jawControlByAxes[axes].posInPlan = 0;
        // get new plan pos
        if(pFlag->cmdPos[axes] > 0)
        {
            enc = pFlag->cmdPos[axes]/2.5;//posInPlan*2.5 before send for precision
            checkRet = planJawPosCheck(enc, rtFeedback.jawRTPos[axes], axes);
            if(checkRet >= 0){
               // jawControlByAxes[axes].posInPlan = pFlag->cmdPos[axes]; // get 1st RI for prepare
                jawControlByAxes[axes].preparePos = pFlag->cmdPos[axes]; // get 1st RI for prepare
                printf("Jaw%d prepare pos ok %d\r\n", axes, enc);
            }  
            else   printf("Jaw%d invalid prepare pos %d!\r\n", axes, pFlag->cmdPos[axes]);
        }
        else if(pFlag->stateCmd[axes] > 0)    printf("Jaw%d state %d\r\n", axes, pFlag->stateCmd[axes]);
        break;
    case PARK_START:
        checkRet = planJawPosCheck(jawParameterByAxes[axes].jawParkPos, rtFeedback.jawRTPos[axes], axes);
        if(checkRet == 0)
        {
            printf("Jaw%d close to park pos %d - %d\r\n",axes,enc, rtFeedback.jawRTPos[axes]);
            jawControlByAxes[axes].MotorState = PARK_END;
        }
        else if(checkRet == 1)
        {
            jawControlByAxes[axes].fSVG.StartPosition = (double)rtFeedback.jawRTPos[axes] / ENCODER_CNT_PER_MM; 
            jawControlByAxes[axes].fSVG.TargetPosition = (double)jawParameterByAxes[axes].jawParkPos / ENCODER_CNT_PER_MM;
            printf("park start jaw%d pos %lf(%d) -> %lf(%d)\r\n",
                axes, jawControlByAxes[axes].fSVG.StartPosition, rtFeedback.jawRTPos[axes],jawControlByAxes[axes].fSVG.TargetPosition,jawParameterByAxes[axes].jawParkPos);
            jawControlByAxes[axes].fSVG.Start = 1;
            jawControlByAxes[axes].fSVG.Enable = 1;
            jawControlByAxes[axes].uartPIDCmd = DOUBLE_ADJ;
            jawControlByAxes[axes].MotorState = PARK_END;
            jawControlByAxes[axes].location_timer = 0;
            jawControlByAxes[axes].startMovingFlag = 1;
            motorEnable(axes);
        }
        else printf("jaw%d Invalid park pos %lf(%d)\r\n", axes, (double)enc / ENCODER_CNT_PER_MM, enc);
    break;
    case PARK_END:
        if (pFlag->stateCmd[axes] == PARK_END)
        {
            printf("jaw%d park done %d\r\n", axes, rtFeedback.jawRTPos[axes]);
            jawControlByAxes[axes].startMovingFlag = 0;
        }
    break;
    case PREPARE_START:
       // enc = jawControlByAxes[axes].posInPlan/2.5;
        enc = jawControlByAxes[axes].preparePos/2.5;
        checkRet = planJawPosCheck(enc, rtFeedback.jawRTPos[axes], axes);
        if(checkRet > 0)//normal case
        {
            jawControlByAxes[axes].fSVG.StartPosition = (double)rtFeedback.jawRTPos[axes] / ENCODER_CNT_PER_MM; 
            jawControlByAxes[axes].fSVG.TargetPosition = (double)jawControlByAxes[axes].preparePos / (ENCODER_CNT_PER_MM*2.5); 
            printf("Jaw%d prepare start %lf(%d) end %lf(%d)\r\n", 
                axes, jawControlByAxes[axes].fSVG.StartPosition, rtFeedback.jawRTPos[axes],jawControlByAxes[axes].fSVG.TargetPosition,enc);
            jawControlByAxes[axes].fSVG.Start = 1;
            jawControlByAxes[axes].fSVG.Enable = 1;
            jawControlByAxes[axes].uartPIDCmd = DOUBLE_ADJ;
            jawControlByAxes[axes].MotorState = PREPARE_END;
            jawControlByAxes[axes].location_timer = 0;
            jawControlByAxes[axes].startMovingFlag = 1;
            motorEnable(axes);
        }
        else if(checkRet == 0)
        {
          //  motorEnable(axes);
            rtFeedback.jawInfo[axes] |= 0x04; // jaw prepare done
            jawControlByAxes[axes].startMovingFlag = 0;
          //  jawControlByAxes[axesType].MotorState = SERVO;
            printf("no move, jaw%d prepare done %d - %d\r\n", axes, enc, rtFeedback.jawRTPos[axes]);
        }
        else printf("jaw%d Invalid prepare pos %d\r\n", axes, jawControlByAxes[axes].posInPlan);
        break;
    case PREPARE_END:
        if (pFlag->stateCmd[axes] == PREPARE_END)
        {
           // motorParamInit(axesType); 
            rtFeedback.jawInfo[axes] |= 0x04; // jaw prepare done
            jawControlByAxes[axes].startMovingFlag = 0;
          //  jawControlByAxes[axesType].MotorState = SERVO;
            printf("jaw%d prepare done %d\r\n", axes, rtFeedback.jawRTPos[axes]);
        }
        break;
    case SERVO:
        if(pFlag->cmdPos[axes] > 0)
        {
            jawControlByAxes[axes].posInPlan = pFlag->cmdPos[axes]; // get RI for servo
            enc = jawControlByAxes[axes].posInPlan/2.5;
            checkRet = planJawPosCheck(enc, rtFeedback.jawRTPos[axes], axes);
            if(checkRet > 0)  //new pos
            { // get new plan cmd
           // printf("%c Jaw %d\r\n", axesType ? 'Y' : 'X', pFlag->masterCmd[axesType]);
                jawControlByAxes[axes].startMovingFlag = 0;
                rtFeedback.jawInfo[axes] &= ~0x04;
                jawControlByAxes[axes].fSVG.StartPosition = (double)rtFeedback.jawRTPos[axes] / ENCODER_CNT_PER_MM; // ENC: 2000
                jawControlByAxes[axes].fSVG.TargetPosition = (double)jawControlByAxes[axes].posInPlan/(ENCODER_CNT_PER_MM*2.5);//posInPlan*2.5 before send for precision
                printf("start %lf(%d) end %lf(%d)\r\n", 
                    jawControlByAxes[axes].fSVG.StartPosition, rtFeedback.jawRTPos[axes],jawControlByAxes[axes].fSVG.TargetPosition,enc);
                jawControlByAxes[axes].fSVG.Start = 1;
                jawControlByAxes[axes].fSVG.Enable = 1;
                jawControlByAxes[axes].uartPIDCmd = DOUBLE_ADJ;
                jawControlByAxes[axes].location_timer = 0;
                jawControlByAxes[axes].startMovingFlag = 1;
                motorEnable(axes);
            }
            else if(checkRet == 0)  printf("servo no move\r\n");
            else printf("jaw%d invalid work pos %d\r\n", axes, jawControlByAxes[axes].posInPlan);
        }
        else if(pFlag->stateCmd[axes] > 0){
            motorCtrlByPWM(0, axes);
            motorDisable(axes);
            motorParamInit(axes);
            printf("Jaw%d state %d\r\n", axes, pFlag->stateCmd[axes]);
        }  
        break;
    case SHUTDOWN:
    case POWER_SAVE:
        rtFeedback.jawInfo[axes] = 0;
        jawControlByAxes[axes].startMovingFlag = 0;
        break;
    case ERROR_STATE:
         printf("enter error state\r\n");
        jawControlByAxes[axes].startMovingFlag = 0;
        break;
    default:    break;
    }
}

int8_t doubleLoopPID(uint8_t axes)
{
    uint16_t crtPos = rtFeedback.jawRTPos[axes];
    int8_t ret = SVG(&jawControlByAxes[axes].fSVG);
    uint16_t pulse = (uint16_t)(jawControlByAxes[axes].fSVG.Position * ENCODER_CNT_PER_MM + 
        0.5 * jawControlByAxes[axes].fSVG.moveDirection);
      //  printf("pos:%d %lf\r\n",pulse, jawControlByAxes[axesType].fSVG.Position);
   
    float posLoopOutput = PositionPIDCtrl(crtPos, pulse, &motor_pid_pos[axes]);
    float actSpeed = (float)jawControlByAxes[axes].encoderDelta32/ ENCODER_PULSES_PER_TURN * 250 * 60; // rpm
    // printf("dlt %f ", (float)encoderDelta);

    float speedLoopOutput = SpeedPIDCtrl(actSpeed, posLoopOutput, &motor_pid_spd[axes]);
    motorCtrlByPWM(speedLoopOutput, axes);

    if((jawControlByAxes[axes].fSVG.State == STATE_STANDSTILL)&&(posLoopOutput < 1e-6)&&(speedLoopOutput < 1e-6))
    {
        uint16_t targetEnc = (uint16_t)(jawControlByAxes[axes].fSVG.TargetPosition*ENCODER_CNT_PER_MM);
        uint16_t diff = abs(targetEnc - crtPos);  
        if(diff <= MAX_PREPARE_INPOS)
        {
            rtFeedback.jawInfo[axes] &= ~0x20;
            rtFeedback.jawInfo[axes] |= 0x04; //jaw prepare done flag set, but still moving
        }
        else    rtFeedback.jawInfo[axes] |= 0x20;
        if(diff <= motor_pid_pos[axes].deadZone)    ret = 1;
    }

    if((crtPos >= jawParameterByAxes[axes].limitPos) && (jawControlByAxes[axes].fSVG.moveDirection > 0))
    {
        ret = -1;
        interlockFeedback.jawInterlock[axes] |= 0x2;
        rtFeedback.jawInfo[axes] |= 0x80;
    }
    else if((crtPos <= jawParameterByAxes[axes].limitNeg) && (jawControlByAxes[axes].fSVG.moveDirection < 0))
    {
        ret = -1;
        interlockFeedback.jawInterlock[axes] |= 0x1;
        rtFeedback.jawInfo[axes] |= 0x80;
    }
    if(((rtFeedback.jawRTPos[axes] + rtFeedback.jawTowardPos[axes]) >= avoidCollisionDist[axes]) 
        && (jawControlByAxes[axes].fSVG.moveDirection > 0))
    {
        printf("jaw%d too close%d + %d\r\n",axes, rtFeedback.jawRTPos[axes],rtFeedback.jawTowardPos[axes]);
        ret = -1;
    }
    #if 0
    if (jawControlByAxes[axes].location_timer < SAMP_BUF_SIZE)
    {
        prtBufCmd[jawControlByAxes[axes].location_timer] = pulse;
        prtBufAct[jawControlByAxes[axes].location_timer] = crtPos;
    }
    #endif

    return ret;
}

int8_t PID(uint8_t axes)
{
    static float actSpeed;
   // __IO uint16_t crtPos;
    int8_t ret = 0;

    switch (jawControlByAxes[axes].uartPIDCmd)
    {
    case SPEED_ADJ:
        actSpeed = (float)jawControlByAxes[axes].encoderDelta32 / ENCODER_PULSES_PER_TURN * 250 * 60; // rpm
      //  printf("tim %d\r\n", HAL_GetTick());
        float speedLoopOutput = SpeedPIDCtrl(actSpeed, motor_pid_spd[axes].Setpoint, &motor_pid_spd[axes]);
        motorCtrlByPWM(speedLoopOutput, axes);
       //  printf("pwm %f\r\n", speedLoopOutput);
       // jawControlByAxes[axesType].encoderLast = crtPos;
        //if(fabs(motor_pid_spd[axesType].Setpoint - actSpeed) < motor_pid_spd[axesType].deadZone)    reachFlag = 1;
#if 0
        int16_t i = jawControlByAxes[axes].location_timer;// - 800;
        if ((i >= 0)&&(i < SAMP_BUF_SIZE))
        {
            prtBufCmd[i] = jawControlByAxes[axes].encoderDelta32;
            prtBufAct[i] = jawControlByAxes[axes].encoderTotalCnt;//crtPos;
        }  
#endif
        break;
    case DOUBLE_ADJ:
        ret = doubleLoopPID(axes);
        break;
    default:
        break;
    }

    return ret;
}

void JAWCtrlTask(void *argument)
{
    /* USER CODE BEGIN JAWCtrlTask */
    struct JawFlagType JawRecvCmd;
   
    memset(&JawRecvCmd, 0, sizeof(struct JawFlagType));
    osMessageQueuePut(motor_signal_queueHandle, &JawRecvCmd, 0, 0);
    jawDeviceInit();

    /* Infinite loop */
    for (;;)
    {
        osMessageQueueGet(motor_signal_queueHandle, &JawRecvCmd, 0, osWaitForever);
      
        switch(JawRecvCmd.axes)
        {
            case X:
                JawCtrlLoop(&JawRecvCmd, X);
            break;
            case Y:
                JawCtrlLoop(&JawRecvCmd, Y);
            break;
            case XY:
                JawCtrlLoop(&JawRecvCmd, X);
                JawCtrlLoop(&JawRecvCmd, Y);
            break;
            default: break;
        }
        // osDelay(1);
    }
    /* USER CODE END JAWCtrlTask */
}

void movement_calculation_task(void)
{
    uint16_t cmd;
    struct JawFlagType JawFinishStep;
    int8_t ret = 0;
    static JawCtlFsm oldMotorState[XY] = {0};
   
    for (;;)
    {
        for (uint8_t axes = 0; axes < XY; axes++)
        {
            getEncodeTotalValue(axes);//update encode value
            if (jawControlByAxes[axes].startMovingFlag)
            {
                ret = PID(axes); 

                if(ret < 0){// LIMIT ERROR 
                    printf("pid pos err\r\n");
                    cmd = ERROR_STATE;
                    messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                } 
                else{
                    switch (jawControlByAxes[axes].MotorState)
                    {
                    case LIMSWITCH_RISING:
                        if(checkSwitchLevel(axes) == SWITCH_RELEASED)
                        {
                            cmd = LIMSWITCH_RISING;
                            messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                        }
                    break;
                    case PREPARE_END:
                        if(ret == 1)
                        {
                            cmd = PREPARE_END;
                            messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                            motorCtrlByPWM(0, axes);
                            motorDisable(axes);
                            motorParamInit(axes);
                        }
                    break;
                    case PARK_END:
                        if(ret == 1)
                        {
                            motorCtrlByPWM(0, axes);
                            motorDisable(axes);
                            cmd = PARK_END;
                            messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                        }
                    break;
                    default:    break;
                    }
                }
#if 0
                if((rtFeedback.jawRTPos[axes] + rtFeedback.jawTowardPos[axes]) >= avoidCollisionDist[axes])
                {
                    printf("jaw%d too close%d + %d\r\n",axes, rtFeedback.jawRTPos[axes],rtFeedback.jawTowardPos[axes]);
                    motorDisable(axes);
                    jawControlByAxes[axes].startMovingFlag = 0;
                    jawControlByAxes[axes].uartPIDCmd = 0;
                    jawControlByAxes[axes].location_timer = 0;
                }  
#endif
                jawControlByAxes[axes].location_timer++;  
            }
            else
            {
                if(oldMotorState[axes] != jawControlByAxes[axes].MotorState)
                {
                    switch (jawControlByAxes[axes].MotorState)
                    {
                    case INIT_MOVE_FORWARD:
                        motorCtrlByPWM(0, axes);
                        motorDisable(axes);
                        cmd = INIT_MOVE_FORWARD;
                        messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                        motorParamInit(axes);
                        break;
                    case INIT_END: 
                        printf("Jaw%d init end %d %d\r\n",axes, jawParameterByAxes[axes].homeEncodeValue, secondPosFeedback.jawSecondPos[axes]);
                     //   setEncodeTotalValue(jawControlByAxes[axes].homeEncodeValue, axes);
                        motorCtrlByPWM(0, axes);
                        motorDisable(axes);
                        rtFeedback.jawInfo[axes] = 0;
                        rtFeedback.jawInfo[axes] |= 0x01;  
                        motorParamInit(axes);
                    break;
                    case PARK_END:
                  //  case PREPARE_END:
                        motorCtrlByPWM(0, axes);
                        motorDisable(axes);
                        motorParamInit(axes);
                    break;
                    case IDLE:
                    case SHUTDOWN:
                    case POWER_SAVE:
                    case ERROR_STATE:
                    //    printf("disable jaw%d state %d pos %d\r\n", axes, jawControlByAxes[axes].MotorState, rtFeedback.jawRTPos);
                        motorCtrlByPWM(0, axes);
                        motorDisable(axes);
                        motorParamInit(axes);
                    break;
                    default:    break;
                    }
                    oldMotorState[axes] = jawControlByAxes[axes].MotorState;
                  //  printf("%d %d\r\n",axes, oldMotorState[axes]);
                }
            } 
            
            if(jawControlByAxes[axes].MotorState > INIT_END)
            {
                uint16_t sec2Enc = paraK[axes]*secondPosFeedback.jawSecondPos[axes] + paraB[axes];
                int16_t diff = rtFeedback.jawRTPos[axes] - sec2Enc;
                if( abs(diff) > MAX_TWO_CHANNEL_DIFF ){
                    interlockFeedback.jawInterlock[axes] |= 0x40;//bit 6
                    rtFeedback.jawInfo[axes] |= 0x40;
                    cmd = ERROR_STATE;
                    messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                    printf("Jaw%d dual channel diff is too high %d\r\n",axes, diff);
                }
                // if(rtFeedback.jawRTPos[axes] > jawControlByAxes[axes].limitPos){
                //     interlockFeedback.jawInterlock[axes] |= 0x2;
                //     rtFeedback.jawInfo[axes] |= 0x80;
                //     cmd = ERROR_STATE;
                //     messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                //     printf("Jaw%d pos out of limit %d\r\n",axes, rtFeedback.jawRTPos[axes]);
                // } 
            } 
        }
        if((jawControlByAxes[X].startMovingFlag) || (jawControlByAxes[Y].startMovingFlag))
        {
            uint8_t rtpos[4];
            memcpy(rtpos, rtFeedback.jawRTPos, 4);
            make_cmd_to_fpga(CMD_JAW_POS, rtpos);
           // printf("XY pos %d %d\r\n",jawControlByAxes[X].startMovingFlag,jawControlByAxes[Y].startMovingFlag);
        }

        osDelay(4);
    }
}

static int8_t jaw_thread_init(void)
{
    osThreadAttr_t JawFSM_attributes = {
        .name = "Jaw_FSM_thread",
        .stack_size = 1024 * 4,
        .priority = (osPriority_t)osPriorityNormal,
    };

    osThreadAttr_t motor_calc_thread_attributes = {
        .name = "movement_schedule_thread",
        .stack_size = 1024 * 4,
        .priority = (osPriority_t)osPriorityHigh,
    };

    osThreadId_t JawFSMHandle = osThreadNew(JAWCtrlTask, NULL, &JawFSM_attributes);
    if (JawFSMHandle == NULL)
    {
        printf("thread Jaw FSM create failed\r\n");
        return -1;
    }

    osThreadId_t move_schedule_threadHandle = osThreadNew(movement_calculation_task, NULL, &motor_calc_thread_attributes);
    if (move_schedule_threadHandle == NULL)
    {
        printf("thread move schedule create failed\r\n");
        return -1;
    }

    motor_signal_queueHandle = osMessageQueueNew(10, sizeof(struct JawFlagType), NULL);
    if (motor_signal_queueHandle == NULL)
    {
        printf("queue send to jaw fsm create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(jaw_thread_init);

int8_t ExecuteConsoleCmd(uint16_t _consoleCmd, uint16_t _consolePara, double _consoleFloat)
{
    uint8_t axes = _consolePara;
    uint16_t setValue = 0;
    char axeChar[] = {'X','Y','B'};
    printf("recv cmd:%d axes:%c\r\n", _consoleCmd, axeChar[axes]);

    switch (_consoleCmd)
    {
    case CMD_POS_KP_MODE:
        motor_pid_pos[axes].Kp = _consoleFloat;
        printf(" pkp: %lf\r\n", motor_pid_pos[axes].Kp);
        break;
    case CMD_POS_KI_MODE:
        motor_pid_pos[axes].Ki = _consoleFloat;
        printf(" pki: %lf\r\n", motor_pid_pos[axes].Ki);
        break;
    case CMD_POS_KD_MODE:
        motor_pid_pos[axes].Kd = _consoleFloat;
        printf(" pkd: %lf\r\n", motor_pid_pos[axes].Kd);
        break;
    case CMD_SPD_KP_MODE:
        motor_pid_spd[axes].Kp = _consoleFloat;
        printf(" skp: %lf\r\n", motor_pid_spd[axes].Kp);
        break;
    case CMD_SPD_KI_MODE:
        motor_pid_spd[axes].Ki = _consoleFloat;
        printf(" ski: %lf\r\n", motor_pid_spd[axes].Ki);
        break;
    case CMD_SPD_KD_MODE:
        motor_pid_spd[axes].Kd = _consoleFloat;
        printf(" skd: %lf\r\n", motor_pid_spd[axes].Kd);
        break;
    case CMD_POSITION_MODE:
        motorParamInit(axes);
      //  uint16_t tmpStart = rtFeedback.jawRTPos[axes];
        jawControlByAxes[axes].fSVG.StartPosition = (double)rtFeedback.jawRTPos[axes] / ENCODER_CNT_PER_MM;  
        printf("%d %f\r\n", rtFeedback.jawRTPos[axes], jawControlByAxes[axes].fSVG.StartPosition);    
        jawControlByAxes[axes].fSVG.TargetPosition = jawControlByAxes[axes].fSVG.StartPosition + _consoleFloat; // 1 turn = 1024cnt = 5mm
        if(jawControlByAxes[axes].fSVG.TargetPosition < 0)  jawControlByAxes[axes].fSVG.TargetPosition = 0;
        jawControlByAxes[axes].fSVG.Start = 1;
        jawControlByAxes[axes].fSVG.Enable = 1;
        jawControlByAxes[axes].uartPIDCmd = DOUBLE_ADJ;
        jawControlByAxes[axes].location_timer = 0;
        jawControlByAxes[axes].startMovingFlag = 1;
       // jawControlByAxes[!axes].startMovingFlag = 0;
        uint16_t tmpTarget = (uint16_t)(jawControlByAxes[axes].fSVG.TargetPosition*ENCODER_CNT_PER_MM);
        printf(" pos: %lf(%d) %lf(%d)\r\n", jawControlByAxes[axes].fSVG.StartPosition,rtFeedback.jawRTPos[axes],jawControlByAxes[axes].fSVG.TargetPosition,tmpTarget);
       // jawControlByAxes[axes].posInPlan = (uint16_t)(jawControlByAxes[axes].fSVG.TargetPosition*ENCODER_CNT_PER_MM * 2.5);//to keep the same as plan
        break;
    case CMD_SPEED_MODE:
       // motorParamInit(axes);
        motor_pid_spd[axes].Setpoint = _consoleFloat;
        jawControlByAxes[axes].uartPIDCmd = SPEED_ADJ;
        jawControlByAxes[axes].location_timer = 0;
        jawControlByAxes[axes].startMovingFlag = 1;
     //   jawControlByAxes[!axes].startMovingFlag = 0;
       // printf(" speed: %lf %d %d\r\n", motor_pid_spd[axes].Setpoint, jawControlByAxes[X].startMovingFlag, jawControlByAxes[Y].startMovingFlag);
        break;
    case CMD_POWER_MODE:
        if (_consoleFloat > 0)
        {
            printf("enable motor\r\n");
            motorEnable(axes);
        }
        else
        {
            printf("disable motor\r\n");
            jawControlByAxes[axes].startMovingFlag = 0;
            jawControlByAxes[axes].uartPIDCmd = 0;
            jawControlByAxes[axes].location_timer = 0;
            motorDisable(axes);      // tight brake
        }
        break;
    case CMD_PWM_MODE:
        printf("pwm duty %lf\r\n",_consoleFloat);
        motorCtrlByPWM(_consoleFloat, axes);
        break;
    case CMD_DISPLAY_MODE:
        for (int i = 0; i < SAMP_BUF_SIZE; i++)
        {
            printf("%f,%f\r\n", prtBufCmd[i],prtBufAct[i]);
           // printf("%f\r\n", prtBufAct[i]);
        }
        jawControlByAxes[axes].startMovingFlag = 0;
        memset(prtBufCmd, 0, SAMP_BUF_SIZE*sizeof(float));
        memset(prtBufAct, 0, SAMP_BUF_SIZE*sizeof(float));
        break;
    case CMD_ENCODE_MODE:// 0: read ENC | >0: set ENC
        if (_consoleFloat > 0)
        { 
           if (_consoleFloat < MAX_ENCODE_VAL)  setEncodeTotalValue((uint16_t)_consoleFloat, axes);
           else    setEncodeTotalValue(MAX_ENCODE_VAL, axes);
        }
        printf("ENC:%u(%u)\r\n", rtFeedback.jawRTPos[axes], jawControlByAxes[axes].encoderTotalCnt);
        break;
    case CMD_FSM_STATUS://   0: clear status | 65535: get status  | other: set status
        setValue = (uint16_t)_consoleFloat;
        if(setValue == 65535)   printf("current jaw fsm state %d\r\n", jawControlByAxes[axes].MotorState);  
        else if(setValue == 0)  jawControlByAxes[axes].MotorState = 0;
        else if(setValue < TOTAL_FSM){
            struct JawFlagType JawState;
            uint16_t state[2] = {setValue, setValue};
            messageToJawTask(JawState, COMMAND, axes, state);
        }  
        else{
            struct JawFlagType JawState;
            uint16_t pos[2] = {setValue, setValue};
            messageToJawTask(JawState, PLAN_DATA, axes, pos);
        }  
        break;
    case CMD_SECOND_POS:
        printf("second position %d\r\n",secondPosFeedback.jawSecondPos[axes]);
        break;
    case CMD_SWITCH_LEVEL:
        printf("switch%d level %d\r\n", axes, checkSwitchLevel(axes));
        break;
    case CMD_SET_PARAM:
        setValue = (uint16_t)_consoleFloat;
        jawParameterByAxes[axes].jawParkPos = setValue;
        printf("park pos %u\r\n", jawParameterByAxes[axes].jawParkPos);
       // struct JawFlagType JawPos;
      //  messageToJawTask(JawPos, COMMAND, axes, &setValue);
    break;
    default:    break;
    }

    return 0;
}

static int8_t cmd_motor_debug(uint8_t argc, uint8_t **argv) //uart cmd example: motor_debug pow Y 0 = disable Y Jaw
{                                                           //motor_debug pos Y 10 = Y Jaw move 10mm
    uint16_t type, axes;                                    //motor_debug dsp Y XX = print data in CmdBuf and ActBuf(XX can be any value)
    double value;
    char *validCmd[] = {"pkp", "pki", "pkd", "skp", "ski", "skd", "pwm", "spd", "pos", "pow", "dsp", "enc", "fsm", "adc", "lim", "par"};
    uint8_t cmdNum = sizeof(validCmd) / sizeof(char *);

    if (argc < 3)
    {
        printf("argv too few\r\n");
        return -1;
    }
    // printf("%s %s %s\r\n", argv[1],argv[2],argv[3]);

    for (uint8_t i = 0; i < cmdNum; i++)
    {
        if (strstr(argv[1], validCmd[i]) != NULL)
        {
            type = i;
            break;
        }
    }
    if (type >= cmdNum)
    {
        printf("Invalid cmd type %s!\r\n", argv[1]);
        return -1;
    }
    if (strcmp("X", argv[2]) == 0)
        axes = 0;
    else if (strcmp("Y", argv[2]) == 0)
        axes = 1;
    else if (strcmp("XY", argv[2]) == 0)
        axes = 2;
    else
    {
        printf("Invalid axes %s!\r\n", argv[2]);
        return -1;
    }

    return ExecuteConsoleCmd(type, axes, strtod(argv[3], NULL));
}
MSH_CMD_EXPORT_ALIAS(cmd_motor_debug, motor_debug, motor debug);