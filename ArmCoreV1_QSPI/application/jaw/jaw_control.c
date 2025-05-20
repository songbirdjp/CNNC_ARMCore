#include "jaw_control.h"
#include "main_app.h"
#include "shell.h"
#include "planData.h"
#include <stdlib.h>
#include "ad7927_app.h"

#define ENCODE_INIT_VAL 2000
#define PERMIT_FOLLOWING_ERR  250  //when PWM = 100, speed is 10count/4ms
#define MAX_ENCODE_VAL  65535

#define JAWY_NFAULT_NAME "GPIOD_6"
#define JAWX_NFAULT_NAME "GPIOB_4"
#define JAWX_ENCZ_NAME "GPIOA_8"
#define JAWY_ENCZ_NAME "GPIOB_10"
#define JAWX_LIMIT_NAME "GPIOG_14"
#define JAWY_LIMIT_NAME "GPIOB_11"

#define SAMP_BUF_SIZE    400

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

#define SECOND_PERMIT_ERROR    8 // 0.244mm
static uint16_t jawHomePos[2] = {2765,2355};
//static uint16_t avoidCollisionDist[2] = {32413,31896};//= Cx1/Cy1+Cx2/Cy2+204.8, 2 jaw minimum distance is 1mm
static uint16_t avoidCollisionDist[2] = {32003,31486};
#ifdef BANKA
    static uint16_t jawEncZero[2] = {2258,1845};
    static uint16_t initSecondPosCheck[2] = {873,848};
    double paraK[2] = {6.527, 6.5444};
    double paraB[2] = {-2929.3, -3204.6};
#else
    static uint16_t jawEncZero[2] = {2254,1843};
    static uint16_t initSecondPosCheck[2] = {908,754};
    double paraK[2] = {6.5323, 6.4959};
    double paraB[2] = {-3229.2, -2886.9};
#endif

static void motorParamInit(uint8_t axes)
{
    memset(&motor_pid_pos[axes], 0, sizeof(PID_TypeDef));
    memset(&motor_pid_spd[axes], 0, sizeof(PID_TypeDef));

    motor_pid_pos[axes].Kp = 2.0;
    motor_pid_pos[axes].Ki = 0;
    motor_pid_pos[axes].Kd = 0;
    motor_pid_pos[axes].Setpoint = 15000;
    motor_pid_pos[axes].IntegralLimit =50;
    motor_pid_pos[axes].OutputLimit = 999;
    motor_pid_pos[axes].deadZone = 20;
    motor_pid_pos[axes].integralStartErr = 200;

    motor_pid_spd[axes].Kp = .8;
    motor_pid_spd[axes].Ki = .8;
    motor_pid_spd[axes].Kd = 0;
    motor_pid_spd[axes].Setpoint = 0;
    motor_pid_spd[axes].IntegralLimit =50;
    motor_pid_spd[axes].OutputLimit = 100;
    motor_pid_spd[axes].deadZone = 3;
    motor_pid_spd[axes].integralStartErr = 50;

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

    motorParamInit(X);
    motorParamInit(Y);
   // jawControlByAxes[X].homeEncodeValue = jawHomePos[0];
   // jawControlByAxes[Y].homeEncodeValue = jawHomePos[1];
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
                source.masterCmd[X] = value[X];
                source.masterCmd[Y] = value[Y];
            }
            else source.masterCmd[axes] = *value;
        break;
        case LIMIT_SWITCH:
            source.JawLimit[axes] = *value;
        break;
        case ENC_Z:
            source.JawEncZ[axes] = *value;
        break;
        default: break;
    }  
    osMessageQueuePut(motor_signal_queueHandle, &source, 0, 0);
}

void yjaw_EncZ_callback(void)
{
    struct JawFlagType Jawflag;
    uint16_t value = 1;
    uint8_t axes = Y;

    if(jawControlByAxes[axes].MotorState == INIT_END)
        messageToJawTask(Jawflag, ENC_Z, axes, &value);
    else if(jawControlByAxes[axes].MotorState == UART_DEBUG)
        printf("Y:%d %d\r\n",getEncodeValue(axes),ADCgetValue(&hspi4, 5));
}

void xjaw_EncZ_callback(void)
{
    struct JawFlagType Jawflag;
    uint16_t value = 1;
    uint8_t axes = X;

    if(jawControlByAxes[axes].MotorState == INIT_END)
        messageToJawTask(Jawflag, ENC_Z, axes, &value);
    else if(jawControlByAxes[axes].MotorState == UART_DEBUG)
        printf("X:%d %d\r\n",getEncodeValue(axes),ADCgetValue(&hspi4, 6));
}

void yjaw_limitSwitch_callback(void)
{
    struct JawFlagType Jawflag;
    uint16_t value = 0;
   
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) value = 1;//falling
    else value = 2;//rising 

     messageToJawTask(Jawflag, LIMIT_SWITCH, Y, &value);    
}

void xjaw_limitSwitch_callback(void)
{
    struct JawFlagType Jawflag;
    uint16_t value = 0;

    if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_14) == GPIO_PIN_RESET)   value = 1;//falling
    else value = 2;//rising 

     messageToJawTask(Jawflag, LIMIT_SWITCH, X, &value);
 }

void plcSetJawParam(uint8_t *pData)
{
    jawParameterByAxes[X].jawMinADSetting = (pData[91] << 8) + pData[90];//fetch jaw parameter
    jawParameterByAxes[X].jawMaxADSetting = (pData[93] << 8) + pData[92];
    jawControlByAxes[X].homeEncodeValue = (pData[95] << 8) + pData[94];
    jawParameterByAxes[X].jawMaxVelocity = (pData[109] << 8) + pData[108];
    jawParameterByAxes[X].jawTimeConst = (pData[111] << 8) + pData[110];
    jawParameterByAxes[X].jawAcceleration = (pData[113] << 8) + pData[112];
    jawParameterByAxes[X].jawHomeVelocity = (pData[115] << 8) + pData[114];
    jawParameterByAxes[X].jawParkPos = (pData[117] << 8) + pData[116];
   // printf("param %x %x %x %x %x %x", pData[114],pData[115],pData[116],pData[117],pData[118],pData[119]);
    jawParameterByAxes[X].jaw2ndEncCalibrationPK = (pData[119] << 8) + pData[118];
    jawParameterByAxes[X].jaw2ndEncCalibrationPB = (pData[121] << 8) + pData[120];
    jawParameterByAxes[Y].jawMinADSetting = (pData[129] << 8) + pData[128];
    jawParameterByAxes[Y].jawMaxADSetting = (pData[131] << 8) + pData[130];
    jawControlByAxes[Y].homeEncodeValue = (pData[133] << 8) + pData[132];
    jawParameterByAxes[Y].jawMaxVelocity = (pData[147] << 8) + pData[146];
    jawParameterByAxes[Y].jawTimeConst = (pData[149] << 8) + pData[148];
    jawParameterByAxes[Y].jawAcceleration = (pData[151] << 8) + pData[150];
    jawParameterByAxes[Y].jawHomeVelocity = (pData[153] << 8) + pData[152];
    jawParameterByAxes[Y].jawParkPos = (pData[155] << 8) + pData[154];
    jawParameterByAxes[Y].jaw2ndEncCalibrationPK = (pData[157] << 8) + pData[156];
    jawParameterByAxes[Y].jaw2ndEncCalibrationPB = (pData[159] << 8) + pData[158];
    printf("JawX park:%d\r\n",jawParameterByAxes[X].jawParkPos);
    printf("JawY park:%d\r\n",jawParameterByAxes[Y].jawParkPos);
}

static int8_t prepareCheck(uint16_t planPos, uint16_t actPos, uint8_t axes)
{
    uint16_t actualPlanPos = (uint16_t)(planPos/2.5);

    //if(planPos == jawControlByAxes[axes].oldPlanCmd) return 0;//plan pos no change 
    if((actualPlanPos < (actPos + 40)) && (actualPlanPos > (actPos - 40))) return 0;
    if((actualPlanPos < jawControlByAxes[axes].homeEncodeValue - ENCODER_PULSES_PER_REVOLUTION) || (actualPlanPos > MAX_ENCODE_VAL))   
        return -1; //pos error

    return 1;//normal
}

static int8_t planCheck(uint16_t planPos, uint8_t axes)
{
    uint16_t actualPlanPos = (uint16_t)(planPos/2.5);

    if(planPos == jawControlByAxes[axes].oldPlanCmd) return 0;//plan pos no change 
    if((actualPlanPos < jawControlByAxes[axes].homeEncodeValue - ENCODER_PULSES_PER_REVOLUTION) || (actualPlanPos > MAX_ENCODE_VAL))   
        return -1; //pos error

    return 1;//normal
}

static bool initSecPosCheck(uint8_t axes)
{
    if((secondPosFeedback.jawSecondPos[axes] < (initSecondPosCheck[axes]+SECOND_PERMIT_ERROR)) &&
        (secondPosFeedback.jawSecondPos[axes] > (initSecondPosCheck[axes]-SECOND_PERMIT_ERROR)))
        return 1;
    else  return 0;
}

//static uint16_t initSecondPos[2] = {0,1833};
void JawCtrlLoop(struct JawFlagType *pFlag, uint8_t axesType)
{
    int8_t planPosCheck = 0;
    uint16_t jawEncInitVal = 0;
    bool findZero = 0;

    switch (pFlag->masterCmd[axesType])
    {
    case FSM_INIT:
        jawControlByAxes[axesType].MotorState = INIT_MOVE_BACKWARD;
           // printf("Jaw INIT\r\n");
        break;
    case FSM_IDLE:
        jawControlByAxes[axesType].MotorState = IDLE;
        //  printf("jaw idle\r\n");
        break;
    case FSM_PARK:
        jawControlByAxes[axesType].MotorState = PARK_START;
       // printf("jaw%d  PARK\r\n", axesType);
        break;
    case FSM_PREPARE:
        jawControlByAxes[axesType].MotorState = PREPARE_START;
        break;
    case FSM_SERVO:
        jawControlByAxes[axesType].MotorState = SERVO;
        break;
    case FSM_SHUTDOWN:
        jawControlByAxes[axesType].MotorState = SHUTDOWN;
        break;
    case FSM_POWERSAVE:
        jawControlByAxes[axesType].MotorState = POWER_SAVE;
        break;
    default:    break;
    }

    // printf("state %d\r\n",MotorState[axesType]);
    switch (jawControlByAxes[axesType].MotorState)
    {
    case INIT_MOVE_BACKWARD:
#if 0
            jawCtrlByAxes[axesType].setInitCounter = jawCtrlByAxes[axesType].releaseSwitch = 0;
            motorEnable(axesType);
            if(limitSwitchLevel(axesType) == GPIO_PIN_SET){
                jawCtrlByAxes[axesType].MotorState = LIMSWITCH_FALLING;
                motorCtrlByPWM(-10, axesType);
                printf("level high!\r\n");
            }
            else{
                motorCtrlByPWM(10, axesType);
                jawCtrlByAxes[axesType].MotorState = LIMSWITCH_RISING;
                printf("level low!\r\n");
            }
#endif
#if 1
        if( initSecPosCheck(axesType) ) jawControlByAxes[axesType].MotorState = INIT_END;
        else
        {
            motorEnable(axesType);
            jawEncInitVal = (uint16_t)(secondPosFeedback.jawSecondPos[axesType]*paraK[axesType]+paraB[axesType]+0.5);
            if((jawEncInitVal > 0)&&(jawEncInitVal < MAX_ENCODE_VAL)) setEncodeValue(jawEncInitVal, axesType);
            else
            {
                printf("ENC%d initVal is invalid %d!\r\n", axesType, jawEncInitVal);
                rtFeedback.jawInfo[axesType] |= 0x02;
                return;
            }
        
            if(abs(jawEncInitVal - jawEncZero[axesType]) < ENCODER_PULSES_PER_REVOLUTION/2){
                findZero = 1;
                printf("jaw%d too close, directly find zero\r\n",axesType);
            }   
            jawControlByAxes[axesType].fSVG.StartPosition = (double)(jawEncInitVal / ENCODER_CNT_PER_MM);
            if(findZero) jawControlByAxes[axesType].fSVG.TargetPosition = (double)(jawEncInitVal/ENCODER_CNT_PER_MM + ENCODER_PULSES_PER_REVOLUTION/ENCODER_CNT_PER_MM);
            else jawControlByAxes[axesType].fSVG.TargetPosition = (double)(jawEncZero[axesType] / ENCODER_CNT_PER_MM);
            jawControlByAxes[axesType].fSVG.Start = 1;
            jawControlByAxes[axesType].fSVG.Enable = 1;
            jawControlByAxes[axesType].uartPIDCmd = DOUBLE_ADJ;
            
            if(!jawControlByAxes[axesType].startMovingFlag)
            {
                jawControlByAxes[axesType].location_timer = 0;
                jawControlByAxes[axesType].startMovingFlag = 1;
            } 
            if(findZero) jawControlByAxes[axesType].MotorState = INIT_END;
            else jawControlByAxes[axesType].MotorState = INIT_MOVE_FORWARD;

            printf("jaw%d init move backward %lf(%d) -> %lf(%d)\r\n", 
            axesType, jawControlByAxes[axesType].fSVG.StartPosition, jawEncInitVal,
            jawControlByAxes[axesType].fSVG.TargetPosition, jawEncZero[axesType]);
        }
#else
        jawFeedbackByAxes[axesType].jawStatusInfo |= 0x01;
#endif
        break;
    case INIT_MOVE_FORWARD:
        if (pFlag->masterCmd[axesType] == INIT_MOVE_FORWARD)
        {
            uint16_t enc = getEncodeValue(axesType);
            jawControlByAxes[axesType].fSVG.StartPosition = (double)enc/ENCODER_CNT_PER_MM;
            jawControlByAxes[axesType].fSVG.TargetPosition = (double)(jawControlByAxes[axesType].fSVG.StartPosition + ENCODER_PULSES_PER_REVOLUTION/ENCODER_CNT_PER_MM);//forward move 1 turn to find Z*
            jawControlByAxes[axesType].fSVG.Start = 1;
            jawControlByAxes[axesType].fSVG.Enable = 1;
            jawControlByAxes[axesType].MotorState = INIT_END;
            jawControlByAxes[axesType].location_timer = 0;
            printf("jaw%d init move forward %lf(%d) -> %lf(%d)\r\n", 
                axesType, jawControlByAxes[axesType].fSVG.StartPosition, enc, 
                jawControlByAxes[axesType].fSVG.TargetPosition, enc+ENCODER_PULSES_PER_REVOLUTION);
        }
        break;    
    case INIT_END:
        if (pFlag->JawEncZ[axesType])
        {
           // printf("ENC%d trigger Z!\r\n", axesType);
            jawControlByAxes[axesType].startMovingFlag = 0;
        }
        break;
    case IDLE:
      //  jawControlByAxes[axesType].posInPlan = 31893; // only for test
      //  jawControlByAxes[axesType].oldPlanCmd = jawControlByAxes[axesType].posInPlan;
        rtFeedback.jawInfo[axesType] = 0;
        jawControlByAxes[axesType].startMovingFlag = 0;

        // get new plan pos
        printf("Jaw%d prepare pos %d\r\n", axesType, pFlag->masterCmd[axesType]);
        if(pFlag->masterCmd[axesType] > jawControlByAxes[axesType].homeEncodeValue)  
            jawControlByAxes[axesType].posInPlan = pFlag->masterCmd[axesType]; // get 1st RI for prepare
        break;
#if 0
        case LIMSWITCH_FALLING:
            if (pFlag->JawLimit[axesType] == 1){
                motorCtrlByPWM(10, axesType);
                jawCtrlByAxes[axesType].MotorState = LIMSWITCH_RISING;
                osDelay(10);
            }
            break;
        case LIMSWITCH_RISING:
            if(pFlag->JawEncZ[axesType]){
                setEncodeValue(ENCODE_INIT_VAL, axesType);
                jawCtrlByAxes[axesType].setInitCounter = 1;
            }
            else if (pFlag->JawLimit[axesType] == 2){
               // motorCtrlByPWM(0, axesType);
                jawCtrlByAxes[axesType].releaseSwitch = 1;
            }

            if(jawCtrlByAxes[axesType].releaseSwitch && jawCtrlByAxes[axesType].setInitCounter){
               // motorDisable(axesType);
                motorCtrlByPWM(0, axesType);
                jawCtrlByAxes[axesType].MotorState = ZERO_CONFIRMED;
                jawCtrlByAxes[axesType].setInitCounter = jawCtrlByAxes[axesType].releaseSwitch = 0;
                HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
                HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
              //  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);
               // HAL_GPIO_DeInit(GPIOG, GPIO_PIN_14);
                printf("zero finished!\r\n");
            }

            break;
#endif
    case PARK_START:
        if(jawParameterByAxes[axesType].jawParkPos > 0)
        {
            uint16_t enc = getEncodeValue(axesType);
            if((enc < (jawParameterByAxes[axesType].jawParkPos+200)) && (enc > (jawParameterByAxes[axesType].jawParkPos-200)))
            {
                printf("Jaw%d close to park pos %d\r\n",axesType,enc);
                jawControlByAxes[axesType].MotorState = PARK_END;
            }
            else
            {
                motorEnable(axesType);
                jawControlByAxes[axesType].fSVG.StartPosition = (double)enc / ENCODER_CNT_PER_MM; 
                jawControlByAxes[axesType].fSVG.TargetPosition = jawParameterByAxes[axesType].jawParkPos/ENCODER_CNT_PER_MM;
                jawControlByAxes[axesType].fSVG.Start = 1;
                jawControlByAxes[axesType].fSVG.Enable = 1;
                jawControlByAxes[axesType].uartPIDCmd = DOUBLE_ADJ;

                jawControlByAxes[axesType].location_timer = 0;
                jawControlByAxes[axesType].startMovingFlag = 1;
                jawControlByAxes[axesType].MotorState = PARK_END;
                printf("park start jaw%d pos %d -> %d...\r\n",axesType, enc,jawParameterByAxes[axesType].jawParkPos);
            }
        }
        else printf("Invalid park pos!\r\n");
    break;
    case PARK_END:
        if (pFlag->masterCmd[axesType] == PARK_END)
        {
            printf("park end %d\r\n", axesType);
          //  motorParamInit(axesType);
            jawControlByAxes[axesType].startMovingFlag = 0;
        }
    break;
    case PREPARE_START:
        uint16_t enc = getEncodeValue(axesType);
        planPosCheck = prepareCheck(jawControlByAxes[axesType].posInPlan, enc, axesType);
        if(planPosCheck > 0)//normal case
        {
            motorEnable(axesType);
            jawControlByAxes[axesType].fSVG.StartPosition = (double)getEncodeValue(axesType) / ENCODER_CNT_PER_MM; 
            jawControlByAxes[axesType].fSVG.TargetPosition = jawControlByAxes[axesType].posInPlan/(ENCODER_CNT_PER_MM*2.5);//posInPlan*2.5 before send for precision
            jawControlByAxes[axesType].fSVG.Start = 1;
            jawControlByAxes[axesType].fSVG.Enable = 1;
            jawControlByAxes[axesType].uartPIDCmd = DOUBLE_ADJ;

            jawControlByAxes[axesType].location_timer = 0;
            jawControlByAxes[axesType].startMovingFlag = 1;
            jawControlByAxes[axesType].MotorState = PREPARE_END;
            printf("Jaw%d start %lf end %lf\r\n", axesType, jawControlByAxes[axesType].fSVG.StartPosition, jawControlByAxes[axesType].fSVG.TargetPosition);
        }
        else if(planPosCheck == 0)//plan pos no change, power on but not move
        {
            motorEnable(axesType);
            rtFeedback.jawInfo[axesType] |= 0x04; // jaw prepare done
            jawControlByAxes[axesType].startMovingFlag = 0;
          //  jawControlByAxes[axesType].MotorState = SERVO;
            printf("no move, prepare done %d\r\n", axesType);
        }
        else printf("jaw%d plan pos is error %d\r\n", axesType, jawControlByAxes[axesType].posInPlan);
        break;
    case PREPARE_END:
        if (pFlag->masterCmd[axesType] == PREPARE_END)
        {
           // motorParamInit(axesType);
            rtFeedback.jawInfo[axesType] |= 0x04; // jaw prepare done
            jawControlByAxes[axesType].startMovingFlag = 0;
          //  jawControlByAxes[axesType].MotorState = SERVO;
            printf("prepare done %d\r\n", axesType);
        }
        break;
    case SERVO:
        planPosCheck = planCheck(pFlag->masterCmd[axesType], axesType);
        if(planPosCheck > 0)  //new pos
        { // get new plan cmd
           // printf("%c Jaw %d\r\n", axesType ? 'Y' : 'X', pFlag->masterCmd[axesType]);
           jawControlByAxes[axesType].startMovingFlag = 0;
            rtFeedback.jawInfo[axesType] &= ~0x04;
            jawControlByAxes[axesType].posInPlan = pFlag->masterCmd[axesType]; // get RI for servo
            jawControlByAxes[axesType].oldPlanCmd = jawControlByAxes[axesType].posInPlan;

            jawControlByAxes[axesType].fSVG.StartPosition = (double)getEncodeValue(axesType) / ENCODER_CNT_PER_MM; // ENC: 2000
            jawControlByAxes[axesType].fSVG.TargetPosition = jawControlByAxes[axesType].posInPlan/(ENCODER_CNT_PER_MM*2.5);//posInPlan*2.5 before send for precision
            jawControlByAxes[axesType].fSVG.Start = 1;
            jawControlByAxes[axesType].fSVG.Enable = 1;
            jawControlByAxes[axesType].uartPIDCmd = DOUBLE_ADJ;
            jawControlByAxes[axesType].location_timer = 0;
            jawControlByAxes[axesType].startMovingFlag = 1;
            printf("start %lf end %lf\r\n", jawControlByAxes[axesType].fSVG.StartPosition, jawControlByAxes[axesType].fSVG.TargetPosition);
        }
        else if(planPosCheck == 0)  printf("servo no move\r\n");
        else printf("jaw%d plan pos is error %d\r\n", axesType, pFlag->masterCmd[axesType]);
        break;
    case SHUTDOWN:
    case POWER_SAVE:
        rtFeedback.jawInfo[axesType] = 0;
        jawControlByAxes[axesType].startMovingFlag = 0;
        break;
    default:
        break;
    }
}

bool doubleLoopPID(uint8_t axesType)
{
    uint16_t crtPos = getEncodeValue(axesType);
    int32_t encoderDelta = crtPos - jawControlByAxes[axesType].encoderLast;
    rtFeedback.jawRTPos[axesType] = jawControlByAxes[axesType].encoderLast = crtPos; // update encode feedback 
    int8_t ret =  SVG(&jawControlByAxes[axesType].fSVG);
    if(ret < 0)
    {
        if(jawControlByAxes[axesType].fSVG.Status == ERROR_SVG_LIMIT_POS_POS)
        {
            printf("position is out of positive limit!\r\n");
            interlockFeedback.jawInterlock[axesType] |= 0x2;
            return;
        }  
        else if(jawControlByAxes[axesType].fSVG.Status == ERROR_SVG_LIMIT_POS_NEG)
        {
            printf("position is out of negative limit!\r\n");
            interlockFeedback.jawInterlock[axesType] |= 0x1;
            return;
        }  
    }

    //printf("pos:%d %lf\r\n",ret, jawControlByAxes[axesType].fSVG.Position);
    uint16_t pulse = (uint16_t)(jawControlByAxes[axesType].fSVG.Position * ENCODER_CNT_PER_MM + 
        0.5 * jawControlByAxes[axesType].fSVG.moveDirection);
    #if 0
    if(abs(pulse - crtPos) > PERMIT_FOLLOWING_ERR){
        motorDisable(axesType);
        jawControlByAxes[axesType].startMovingFlag = 0;
        jawControlByAxes[axesType].uartPIDCmd = 0;
        jawControlByAxes[axesType].location_timer = 0;
        printf("following error(%d - %d = %d)!\r\n", pulse, crtPos, pulse - crtPos);
        return;
    }  
    #endif
    float posLoopOutput = PositionPIDCtrl(crtPos, pulse, &motor_pid_pos[axesType]);
    
    float actSpeed = (float)encoderDelta / ENCODER_PULSES_PER_REVOLUTION * 250 * 60; // rpm
    // printf("dlt %f ", (float)encoderDelta);
    float speedLoopOutput;
    if (jawControlByAxes[axesType].location_timer > 2)
    {
        speedLoopOutput = SpeedPIDCtrl(actSpeed, posLoopOutput, &motor_pid_spd[axesType]);
        motorCtrlByPWM(speedLoopOutput, axesType);
     //   printf("%lf\r\n",speedLoopOutput);
    }

    bool reachFlag = 0;
    if((jawControlByAxes[axesType].fSVG.State == STATE_STANDSTILL)&&(posLoopOutput < 1e-6)&&(speedLoopOutput < 1e-6))
    {
        uint16_t targetEnc = (uint16_t)(jawControlByAxes[axesType].fSVG.TargetPosition*ENCODER_CNT_PER_MM);
        if(abs(targetEnc - crtPos) < 2*motor_pid_pos[axesType].deadZone)    reachFlag = 1;
    }
    #if 0
    if (jawControlByAxes[axesType].location_timer < SAMP_BUF_SIZE)
    {
        prtBufCmd[jawControlByAxes[axesType].location_timer] = posLoopOutput;
        prtBufAct[jawControlByAxes[axesType].location_timer] = speedLoopOutput;
    }
    #endif

    return reachFlag;
}

bool PID(uint8_t axesType)
{
    static float actSpeed;
    __IO uint16_t crtPos;
    int32_t encoderDelta;
    bool reachFlag = 0;

    switch (jawControlByAxes[axesType].uartPIDCmd)
    {
    case SPEED_ADJ:
        crtPos = getEncodeValue(axesType);
        encoderDelta = crtPos - jawControlByAxes[axesType].encoderLast;
        jawControlByAxes[axesType].encoderLast = crtPos;
        actSpeed = (float)encoderDelta / ENCODER_PULSES_PER_REVOLUTION * 250 * 60; // rpm
       
        float speedLoopOutput = SpeedPIDCtrl(actSpeed, motor_pid_spd[axesType].Setpoint, &motor_pid_spd[axesType]);
        motorCtrlByPWM(speedLoopOutput, axesType);

        if(fabs(motor_pid_spd[axesType].Setpoint - actSpeed) < motor_pid_spd[axesType].deadZone)    reachFlag = 1;
#if 0
        if (jawControlByAxes[axesType].location_timer < SAMP_BUF_SIZE){
            prtBufCmd[jawControlByAxes[axesType].location_timer] = speedLoopOutput;
            prtBufAct[jawControlByAxes[axesType].location_timer] = actSpeed;//crtPos;
        }  
#endif
        break;
    case DOUBLE_ADJ:
        reachFlag = doubleLoopPID(axesType);
        break;
    default:
        break;
    }

    return reachFlag;
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
    bool reachFlag = 0;
    static JawCtlFsm oldMotorState[XY] = {0};

    for (;;)
    {
        for (uint8_t axes = 0; axes < XY; axes++)
        {
            if (jawControlByAxes[axes].startMovingFlag)
            {
                reachFlag = PID(axes); 
                switch (jawControlByAxes[axes].MotorState)
                {
                case INIT_MOVE_FORWARD:
                    if(reachFlag)
                    {
                        cmd = INIT_MOVE_FORWARD;
                        messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                    }
                    #if 0
                    else{
                        if(jawControlByAxes[axes].location_timer > 15000) //wait for 1 minute
                        {
                            rtFeedback.jawInfo[axes] |= 0x02;
                            printf("jaw%d init timeout!\r\n",axes);
                        }
                    }
                    #endif
                break;
                case PREPARE_END:
                    if(reachFlag)
                    {
                        cmd = PREPARE_END;
                        messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                    }
                    #if 0
                    else{
                        if(jawControlByAxes[axes].location_timer > 2500) //wait for 10s
                        {
                            rtFeedback.jawInfo[axes] |= 0x08;
                            printf("jaw%d prepare timeout!\r\n",axes);
                        }
                    }  
                    #endif
                break;
                case PARK_END:
                    if(reachFlag)
                    {
                        cmd = PARK_END;
                        messageToJawTask(JawFinishStep, COMMAND, axes, &cmd);
                    }
                    #if 0
                    else{
                        if(jawControlByAxes[axes].location_timer > 2500) //wait for 10s
                        {
                            rtFeedback.jawInfo[axes] |= 0x08;
                            printf("jaw%d prepare timeout!\r\n",axes);
                        }
                    }  
                    #endif
                break;
                default:    break;
                }

                if((rtFeedback.jawRTPos[axes] + rtFeedback.jawTowardPos[axes]) >= avoidCollisionDist[axes])
                {
                    printf("jaw%d too close%d + %d\r\n",axes, rtFeedback.jawRTPos[axes],rtFeedback.jawTowardPos[axes]);
                    motorDisable(axes);
                    jawControlByAxes[axes].startMovingFlag = 0;
                    jawControlByAxes[axes].uartPIDCmd = 0;
                    jawControlByAxes[axes].location_timer = 0;
                }  
                jawControlByAxes[axes].location_timer++;  
            }
            else
            {
                if(oldMotorState[axes] != jawControlByAxes[axes].MotorState)
                {
                    switch (jawControlByAxes[axes].MotorState)
                    {
                    case INIT_END:
                        //use second pos to check if init pos is right  
                        printf("Jaw%d init end\r\n",axes);
                        setEncodeValue(jawControlByAxes[axes].homeEncodeValue, axes);
                        motorCtrlByPWM(0, axes);
                        rtFeedback.jawInfo[axes] = 0;
                        rtFeedback.jawInfo[axes] |= 0x01;                    // jaw init done
                        rtFeedback.jawRTPos[axes] = getEncodeValue(axes); // init end feedback init pos    
                        motorParamInit(axes);
                        // if( initSecPosCheck(axes) ){
                        //     printf("jaw%d init finished %d %d\r\n", axes, rtFeedback.jawRTPos[axes],secondPosFeedback.jawSecondPos[axes]);
                        // }    
                        // else{
                        //     rtFeedback.jawInfo[axes] |= 0x02; 
                        //     printf("jaw%d init failed %d\r\n", axes, secondPosFeedback.jawSecondPos[axes]);
                        // }
                    break;
                    case PARK_END:
                    case PREPARE_END:
                        motorCtrlByPWM(0, axes);
                        rtFeedback.jawRTPos[axes] = getEncodeValue(axes);
                    break;
                    case IDLE:
                    case SHUTDOWN:
                    case POWER_SAVE:
                        printf("disable jaw%d state %d pos %d\r\n", axes, jawControlByAxes[axes].MotorState, getEncodeValue(axes));
                        motorCtrlByPWM(0, axes);
                        motorDisable(axes);
                     //   motorParamInit(axes);
                    break;
                    default:    break;
                    }
                    oldMotorState[axes] = jawControlByAxes[axes].MotorState;
                  //  printf("%d %d\r\n",axes, oldMotorState[axes]);
                }
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
    bool axes = _consolePara;
    printf("recv cmd:%d axes:%c | ", _consoleCmd, axes ? 'Y' : 'X');

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
        uint16_t tmpStart = getEncodeValue(axes);
        jawControlByAxes[axes].fSVG.StartPosition = (double)tmpStart / ENCODER_CNT_PER_MM;      
        jawControlByAxes[axes].fSVG.TargetPosition = jawControlByAxes[axes].fSVG.StartPosition + _consoleFloat; // 1 turn = 1024cnt = 5mm
        jawControlByAxes[axes].fSVG.Start = 1;
        jawControlByAxes[axes].fSVG.Enable = 1;
        jawControlByAxes[axes].uartPIDCmd = DOUBLE_ADJ;
        jawControlByAxes[axes].location_timer = 0;
        jawControlByAxes[axes].startMovingFlag = 1;
        jawControlByAxes[!axes].startMovingFlag = 0;
        uint16_t tmpTarget = (uint16_t)(jawControlByAxes[axes].fSVG.TargetPosition*ENCODER_CNT_PER_MM);
        printf(" pos: %lf(%d) %lf(%d)\r\n", jawControlByAxes[axes].fSVG.StartPosition,tmpStart,jawControlByAxes[axes].fSVG.TargetPosition,tmpTarget);
       // jawControlByAxes[axes].posInPlan = (uint16_t)(jawControlByAxes[axes].fSVG.TargetPosition*ENCODER_CNT_PER_MM * 2.5);//to keep the same as plan
        break;
    case CMD_SPEED_MODE:
        motorParamInit(axes);
        motor_pid_spd[axes].Setpoint = _consoleFloat;
        jawControlByAxes[axes].uartPIDCmd = SPEED_ADJ;
        jawControlByAxes[axes].location_timer = 0;
        jawControlByAxes[axes].startMovingFlag = 1;
        jawControlByAxes[!axes].startMovingFlag = 0;
        printf(" speed: %lf %d %d\r\n", motor_pid_spd[axes].Setpoint, jawControlByAxes[X].startMovingFlag, jawControlByAxes[Y].startMovingFlag);
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
        }
        jawControlByAxes[axes].startMovingFlag = 0;
        memset(prtBufCmd, 0, SAMP_BUF_SIZE);
        memset(prtBufAct, 0, SAMP_BUF_SIZE);
        break;
    case CMD_ENCODE_MODE:// 0: read ENC | >0: set ENC
        if (_consoleFloat > 0)
        { 
            if (_consoleFloat < 65535)  setEncodeValue((uint16_t)_consoleFloat, axes);
            else    setEncodeValue(65535, axes);
        }
        printf("ENC:%u\r\n", getEncodeValue(axes));
        break;
    case CMD_FSM_STATUS://   0: read status | [1, 99]: clear current status | [100, 110]: set status
        if(_consoleFloat > 0)  jawControlByAxes[axes].MotorState = (uint16_t)_consoleFloat;
        printf("current jaw fsm state %d\r\n", jawControlByAxes[axes].MotorState);  
        break;
    case CMD_SECOND_POS:
        printf("second position %d\r\n",secondPosFeedback.jawSecondPos[axes]);
        break;
    default:    break;
    }

    return 0;
}

static int8_t cmd_motor_debug(uint8_t argc, uint8_t **argv) //uart cmd example: motor_debug pow Y 0 = disable Y Jaw
{                                                           //motor_debug pos Y 10 = Y Jaw move 10mm
    uint16_t type, axes;                                    //motor_debug dsp Y XX = print data in CmdBuf and ActBuf(XX can be any value)
    double value;
    char *validCmd[] = {"pkp", "pki", "pkd", "skp", "ski", "skd", "pwm", "spd", "pos", "pow", "dsp", "enc", "sta", "adc"};
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
    else
    {
        printf("Invalid axes %s!\r\n", argv[2]);
        return -1;
    }

    return ExecuteConsoleCmd(type, axes, strtod(argv[3], NULL));
}
MSH_CMD_EXPORT_ALIAS(cmd_motor_debug, motor_debug, motor debug);