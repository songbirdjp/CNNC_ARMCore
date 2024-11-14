#include "jaw_control.h"
#include "main_app.h"
#include "shell.h"
#include "planData.h"
#include <stdlib.h>

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

JawFeedbackInfo jawFeedbackByAxes[2];
JAW_SET_PARAM jawParameterByAxes[2];
osMessageQueueId_t motor_signal_queueHandle = NULL;
static JawControlInfo jawControlByAxes[2];
static float prtBufCmd[SAMP_BUF_SIZE]; // sample command position
static float prtBufAct[SAMP_BUF_SIZE]; // sample actual encoder value

PID_TypeDef motor_pid_pos = {.Kp = 3, .Ki = 0, .Kd = 0.0, .Setpoint = 15000, .IntegralLimit = 50, .OutputLimit = 999, .deadZone = 20, .integralStartErr = 200};
PID_TypeDef motor_pid_spd = {.Kp = .8, .Ki = 0.8, .Kd = 0.0, .Setpoint = 0, .IntegralLimit = 50, .OutputLimit = 100, .deadZone = 3, .integralStartErr = 50};
PID_TypeDef motor_pid_double = {.Kp = 0.1, .Ki = 0.1, .Kd = 0, .Setpoint = 10000, .IntegralLimit = 50, .OutputLimit = 100};

void jawDeviceInit(void)
{
    memset(&jawControlByAxes, 0, sizeof(JawControlInfo) * 2);
    memset(&jawFeedbackByAxes, 0, sizeof(JawFeedbackInfo) * 2);
    memset(prtBufAct, 0, SAMP_BUF_SIZE * sizeof(float));
    memset(prtBufCmd, 0, SAMP_BUF_SIZE * sizeof(float));
    for(uint8_t i = 0; i < XY; i++) initSVG(&jawControlByAxes[i].fSVG, i);

    gpio_pin_irq_callback_register(JAWX_NFAULT_NAME, xjaw_nfault_callback);
    gpio_pin_irq_callback_register(JAWY_NFAULT_NAME, yjaw_nfault_callback);
    gpio_pin_irq_callback_register(JAWX_ENCZ_NAME, xjaw_EncZ_callback);
    gpio_pin_irq_callback_register(JAWY_ENCZ_NAME, yjaw_EncZ_callback);
    gpio_pin_irq_callback_register(JAWX_LIMIT_NAME, xjaw_limitSwitch_callback);
    gpio_pin_irq_callback_register(JAWY_LIMIT_NAME, yjaw_limitSwitch_callback);
}

void setJawParam(uint8_t *pData)
{
    jawParameterByAxes[X].jawMinADSetting = (pData[69] << 8) + pData[68];//fetch jaw parameter
    jawParameterByAxes[X].jawMaxADSetting = (pData[71] << 8) + pData[70];
    jawControlByAxes[X].homeEncodeValue = (pData[73] << 8) + pData[72];
    jawParameterByAxes[X].jaw2ndEncCalibrationPK = (pData[81] << 8) + pData[80];
    jawParameterByAxes[X].jaw2ndEncCalibrationPB = (pData[83] << 8) + pData[82];
    jawParameterByAxes[Y].jawMinADSetting = (pData[91] << 8) + pData[90];
    jawParameterByAxes[Y].jawMaxADSetting = (pData[93] << 8) + pData[92];
    jawControlByAxes[Y].homeEncodeValue = (pData[95] << 8) + pData[94];
    jawParameterByAxes[Y].jaw2ndEncCalibrationPK = (pData[103] << 8) + pData[102];
    jawParameterByAxes[Y].jaw2ndEncCalibrationPB = (pData[105] << 8) + pData[104];
}

void JawCtrlLoop(struct JawFlagType *pFlag, uint8_t axesType)
{
    //   if(axesType)    printf("jaw Y ");
    //   else    printf("jaw X ");
    //   if(pFlag->JawLimit[axesType])   printf("limit %d ", pFlag->JawLimit[axesType]);
    //   if(pFlag->JawEncZ[axesType])   printf("EncZ ");
    //   if(pFlag->masterCmd[axesType])   printf("cmd %u ",pFlag->masterCmd[axesType]);

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
    case FSM_PREPARE:
        jawControlByAxes[axesType].MotorState = PREPARE_START;
        break;
    case FSM_SERVO:
        jawControlByAxes[axesType].MotorState = SERVO;
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
        printf("%d jaw init move backward\r\n", axesType);
#if 1
        motorEnable(axesType);
        jawControlByAxes[axesType].uartPIDCmd = SPEED_ADJ;
        motor_pid_spd.Setpoint = 
      //  motorCtrlByPWM(-10, axesType);
        jawControlByAxes[axesType].MotorState = INIT_MOVE_FORWARD;
        if(!jawControlByAxes[axesType].startMovingFlag) jawControlByAxes[axesType].location_timer = 0;
        jawControlByAxes[axesType].startMovingFlag = 1;
#else
        jawFeedbackByAxes[axesType].jawStatusInfo |= 0x01;
#endif
        break;
    case INIT_MOVE_FORWARD:
        if (pFlag->masterCmd[axesType] == INIT_MOVE_FORWARD)
        {
            printf("%d jaw init move forward\r\n", axesType);
            motorCtrlByPWM(10, axesType);
            jawControlByAxes[axesType].MotorState = INIT_END;
        }
        break;    
    case INIT_END:
        if (pFlag->JawEncZ[axesType])
        {
            setEncodeValue(jawControlByAxes[axesType].homeEncodeValue, axesType);
            motorCtrlByPWM(0, axesType);
           // jawControlByAxes[axesType].timer7Flag = 0;//stop moving
            jawFeedbackByAxes[axesType].jawStatusInfo |= 0x01;                    // jaw init done
            jawFeedbackByAxes[axesType].jawCurrentPos = getEncodeValue(axesType); // init end feedback init pos
            printf("%d jaw init finished %u\r\n", axesType, jawFeedbackByAxes[axesType].jawCurrentPos);
        }
        break;
    case IDLE:
        jawFeedbackByAxes[axesType].jawStatusInfo = 0;
        motorDisable(axesType);
        jawControlByAxes[X].startMovingFlag = jawControlByAxes[Y].startMovingFlag = 0;
        if ((pFlag->masterCmd[axesType] > jawControlByAxes[axesType].homeEncodeValue) && (pFlag->masterCmd[axesType] != jawControlByAxes[axesType].oldPlanCmd))
        { // get new plan cmd
            printf("%c Jaw %d: \r\n", axesType ? 'Y' : 'X', pFlag->masterCmd[axesType]);
            jawControlByAxes[axesType].posInPlan = pFlag->masterCmd[axesType]; // get 1st RI for prepare
            jawControlByAxes[axesType].oldPlanCmd = jawControlByAxes[axesType].posInPlan;
        }
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
    case PREPARE_START:
#if 1
        motorEnable(axesType);
        if (jawControlByAxes[axesType].posInPlan >= jawControlByAxes[axesType].homeEncodeValue)
        {
            jawControlByAxes[axesType].fSVG.StartPosition = (double)getEncodeValue(axesType) / ENCODER_CNT_PER_MM; 
            jawControlByAxes[axesType].fSVG.TargetPosition = jawControlByAxes[axesType].posInPlan/(ENCODER_CNT_PER_MM*2.5);//posInPlan*2.5 before send for precision
           // jawControlByAxes[axesType].fSVG.TargetPosition = jawControlByAxes[axesType].posInPlan / ENCODER_CNT_PER_MM;
            jawControlByAxes[axesType].fSVG.Start = 1;
            jawControlByAxes[axesType].fSVG.Enable = 1;
            jawControlByAxes[axesType].uartPIDCmd = DOUBLE_ADJ;
            printf("%d start %lf end %lf\r\n", axesType, jawControlByAxes[axesType].fSVG.StartPosition, jawControlByAxes[axesType].fSVG.TargetPosition);

         /*   if (!jawControlByAxes[X].timer7Flag && !jawControlByAxes[Y].timer7Flag)
            {
                location_timer = 0;
                //  HAL_TIM_Base_Start_IT(&htim7);
            }*/
            if(!jawControlByAxes[axesType].startMovingFlag) jawControlByAxes[axesType].location_timer = 0;
            jawControlByAxes[axesType].startMovingFlag = 1;
            jawControlByAxes[axesType].MotorState = PREPARE_END;
        }
#endif
        break;
    case PREPARE_END:
        if (pFlag->masterCmd[axesType] == PREPARE_END)
        {
            jawFeedbackByAxes[axesType].jawStatusInfo |= 0x04; // jaw prepare done
            jawControlByAxes[axesType].MotorState = SERVO;
            printf("prepare done %d\r\n", axesType);
        }
        break;
    case SERVO:
        if ((pFlag->masterCmd[axesType] > jawControlByAxes[axesType].homeEncodeValue) && (pFlag->masterCmd[axesType] != jawControlByAxes[axesType].oldPlanCmd))
        { // get new plan cmd
            printf("%c Jaw %d\r\n", axesType ? 'Y' : 'X', pFlag->masterCmd[axesType]);
            jawFeedbackByAxes[axesType].jawStatusInfo &= ~0x04;
            jawControlByAxes[axesType].posInPlan = pFlag->masterCmd[axesType]; // get RI for servo
            jawControlByAxes[axesType].oldPlanCmd = jawControlByAxes[axesType].posInPlan;

            jawControlByAxes[axesType].fSVG.StartPosition = (double)getEncodeValue(axesType) / ENCODER_CNT_PER_MM; // ENC: 2000
            jawControlByAxes[axesType].fSVG.TargetPosition = jawControlByAxes[axesType].posInPlan/(ENCODER_CNT_PER_MM*2.5);//posInPlan*2.5 before send for precision
          //  jawControlByAxes[axesType].fSVG.TargetPosition = jawControlByAxes[axesType].posInPlan / ENCODER_CNT_PER_MM;
            jawControlByAxes[axesType].fSVG.Start = 1;
            jawControlByAxes[axesType].fSVG.Enable = 1;
            jawControlByAxes[axesType].uartPIDCmd = DOUBLE_ADJ;
            printf("start %lf end %lf\r\n", jawControlByAxes[axesType].fSVG.StartPosition, jawControlByAxes[axesType].fSVG.TargetPosition);
        }
        break;
    case ERROR_STATE:
        osDelay(1);
        break;
    default:
        break;
    }
}

int8_t ExecuteConsoleCmd(uint16_t _consoleCmd, uint16_t _consolePara, double _consoleFloat)
{
    bool axes = _consolePara;
    printf("recv cmd:%d axes:%c | ", _consoleCmd, axes ? 'Y' : 'X');

    switch (_consoleCmd)
    {
    case CMD_POS_KP_MODE:
        motor_pid_pos.Kp = _consoleFloat;
        printf(" pkp: %lf\r\n", motor_pid_pos.Kp);
        break;
    case CMD_POS_KI_MODE:
        motor_pid_pos.Ki = _consoleFloat;
        printf(" pki: %lf\r\n", motor_pid_pos.Ki);
        break;
    case CMD_POS_KD_MODE:
        motor_pid_pos.Kd = _consoleFloat;
        printf(" pkd: %lf\r\n", motor_pid_pos.Kd);
        break;
    case CMD_SPD_KP_MODE:
        motor_pid_spd.Kp = _consoleFloat;
        printf(" skp: %lf\r\n", motor_pid_spd.Kp);
        break;
    case CMD_SPD_KI_MODE:
        motor_pid_spd.Ki = _consoleFloat;
        printf(" ski: %lf\r\n", motor_pid_spd.Ki);
        break;
    case CMD_SPD_KD_MODE:
        motor_pid_spd.Kd = _consoleFloat;
        printf(" skd: %lf\r\n", motor_pid_spd.Kd);
        break;
    case CMD_POSITION_MODE:
        initSVG(&jawControlByAxes[axes].fSVG, axes);
        uint16_t tmpStart = getEncodeValue(axes);
        jawControlByAxes[axes].fSVG.StartPosition = (double)tmpStart / ENCODER_CNT_PER_MM;      
        jawControlByAxes[axes].fSVG.TargetPosition = jawControlByAxes[axes].fSVG.StartPosition + _consoleFloat; // 1 turn = 1024cnt = 5mm
        jawControlByAxes[axes].fSVG.Start = 1;
        jawControlByAxes[axes].fSVG.Enable = 1;
        jawControlByAxes[axes].uartPIDCmd = DOUBLE_ADJ;
        jawControlByAxes[axes].location_timer = 0;
        //    if(!jawControlByAxes[X].timer7Flag && !jawControlByAxes[Y].timer7Flag)    HAL_TIM_Base_Start_IT(&htim7);
        jawControlByAxes[axes].startMovingFlag = 1;
        jawControlByAxes[!axes].startMovingFlag = 0;
        jawControlByAxes[axes].posInPlan = (uint16_t)(jawControlByAxes[axes].fSVG.TargetPosition*ENCODER_CNT_PER_MM);
        printf(" pos: %lf(%d) %lf(%d)\r\n", jawControlByAxes[axes].fSVG.StartPosition,tmpStart, 
            jawControlByAxes[axes].fSVG.TargetPosition,jawControlByAxes[axes].posInPlan);
        jawControlByAxes[axes].posInPlan = (uint16_t)(jawControlByAxes[axes].fSVG.TargetPosition*ENCODER_CNT_PER_MM * 2.5);//to keep the same as plan
        break;
    case CMD_SPEED_MODE:
        motor_pid_spd.Setpoint = _consoleFloat;
        motor_pid_spd.Integral = 0;
        motor_pid_spd.PreviousError = 0;
        jawControlByAxes[axes].uartPIDCmd = SPEED_ADJ;
        jawControlByAxes[axes].location_timer = 0;
        //      if(!jawControlByAxes[X].timer7Flag && !jawControlByAxes[Y].timer7Flag)    HAL_TIM_Base_Start_IT(&htim7);
        jawControlByAxes[axes].startMovingFlag = 1;
        jawControlByAxes[!axes].startMovingFlag = 0;
        printf(" speed: %lf %d %d\r\n", motor_pid_spd.Setpoint, jawControlByAxes[X].startMovingFlag, jawControlByAxes[Y].startMovingFlag);
        break;
    case CMD_POWER_MODE:
        if (_consoleFloat > 0)
        {
            printf("enable motor\r\n");
            motorPowerCtrl(1, axes); // open motor power supply
            BrakeCtrl(1, axes);      // loose brake
            startPWMOutput(axes);
            startEncodeTim(axes);
        }
        else
        {
            printf("disable motor\r\n");
            //   HAL_TIM_Base_Stop_IT(&htim7);
            jawControlByAxes[axes].startMovingFlag = 0;
            jawControlByAxes[axes].uartPIDCmd = 0;
            jawControlByAxes[axes].location_timer = 0;
            stopEncodeTim(axes);
            stopPWMOutput(axes);
            motorCtrlByPWM(0, axes);
            motorPowerCtrl(0, axes); // close motor power supply
            BrakeCtrl(0, axes);      // tight brake
        }
        break;
    case CMD_PWM_MODE:
        //  printf("pwm duty %lf\r\n",_consoleFloat);
        motorCtrlByPWM(_consoleFloat, axes);
        break;
    case CMD_DISPLAY_MODE:
        // printf("%lu\r\n",location_timer);
        for (int i = 0; i < SAMP_BUF_SIZE; i++)
        {
            // prtBuf[i] = i;
            printf("%f,%f\r\n", prtBufCmd[i],prtBufAct[i]);
        }
     /*   printf("======\r\n");
        for (int i = 0; i < SAMP_BUF_SIZE; i++)
        {
            // prtBuf[i] = i;
            printf("%f\r\n", prtBufAct[i]);
            //  printf("%f\r\n", prtBuf0[i]);
        }*/
        jawControlByAxes[axes].startMovingFlag = 0;
        memset(prtBufCmd, 0, SAMP_BUF_SIZE);
        memset(prtBufAct, 0, SAMP_BUF_SIZE);
        break;
    case CMD_ENCODE_MODE:
        if (_consoleFloat > 0)
        { // set encoder
            if (_consoleFloat < 65535)
                setEncodeValue((uint16_t)_consoleFloat, axes);
            else
                setEncodeValue(65535, axes);
        }
        else
        { // get current encoder
            printf("ENC:%u\r\n", getEncodeValue(axes));
        }
        break;
    case CMD_CLEAR_STATUS:
        printf("clear jaw fsm state\r\n");
        jawControlByAxes[axes].MotorState = 0;
        break;
    default:
        break;
    }

    return 0;
}

static int8_t cmd_motor_debug(uint8_t argc, uint8_t **argv) //uart cmd example: motor_debug pow Y 0 = disable Y Jaw
{                                                           //motor_debug pos Y 10 = Y Jaw move 10mm
    uint16_t type, axes;                                    //motor_debug dsp Y XX = print data in CmdBuf and ActBuf(XX can be any value)
    double value;
    char *validCmd[] = {"pkp", "pki", "pkd", "skp", "ski", "skd", "pwm", "spd", "pos", "pow", "dsp", "enc", "sta"};
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

void doubleLoopPID(uint8_t axesType)
{
    uint16_t crtPos = getEncodeValue(axesType);
    int32_t encoderDelta = crtPos - jawControlByAxes[axesType].encoderLast;
    jawFeedbackByAxes[axesType].jawCurrentPos = jawControlByAxes[axesType].encoderLast = crtPos; // update encode feedback 
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
    if(abs(pulse - crtPos) > PERMIT_FOLLOWING_ERR){
        motorDisable(axesType);
        jawControlByAxes[axesType].startMovingFlag = 0;
        jawControlByAxes[axesType].uartPIDCmd = 0;
        jawControlByAxes[axesType].location_timer = 0;
        printf("following error(%d - %d = %d)!\r\n", pulse, crtPos, pulse - crtPos);
        return;
    }  
    float posLoopOutput = PositionPIDCtrl(crtPos, pulse, &motor_pid_pos);
    
    float actSpeed = (float)encoderDelta / ENCODER_PULSES_PER_REVOLUTION * 250 * 60; // rpm
    // printf("dlt %f ", (float)encoderDelta);
    float speedLoopOutput;
    if (jawControlByAxes[axesType].location_timer > 2)
    {
        speedLoopOutput = SpeedPIDCtrl(actSpeed, posLoopOutput, &motor_pid_spd);
        motorCtrlByPWM(speedLoopOutput, axesType);
     //   printf("%lf\r\n",speedLoopOutput);
    }
   // printf("pos %d %d\r\n",abs(jawControlByAxes[axesType].posInPlan - crtPos), motor_pid_pos.deadZone);
    uint16_t actualPlanPos = (uint16_t)(jawControlByAxes[axesType].posInPlan/2.5);
    if ((jawControlByAxes[axesType].MotorState == PREPARE_END) && (abs(actualPlanPos - crtPos) < motor_pid_pos.deadZone))
    {
        struct JawFlagType JawPrepareFinish;
        memset(&JawPrepareFinish, 0, sizeof(struct JawFlagType));
        JawPrepareFinish.axes = axesType;
        JawPrepareFinish.masterCmd[axesType] = PREPARE_END;
        osMessageQueuePut(motor_signal_queueHandle, &JawPrepareFinish, 0, 0);
    }
    #if 0
    if (jawControlByAxes[axesType].location_timer < SAMP_BUF_SIZE)
    {
        prtBufCmd[jawControlByAxes[axesType].location_timer] = abs(actualPlanPos - crtPos);
        prtBufAct[jawControlByAxes[axesType].location_timer] = motor_pid_pos.deadZone;
    }
    #endif
    #if 0
    if(jawControlByAxes[axesType].location_timer < SAMP_BUF_SIZE){
        prtBufCmd[jawControlByAxes[axesType].location_timer] = jawControlByAxes[axesType].fSVG.Speed*60*ENCODER_CNT_PER_MM/ENCODER_PULSES_PER_REVOLUTION;//mm/s->rpm
        prtBufAct[jawControlByAxes[axesType].location_timer] = actSpeed;
    }
    #endif
}

void PID(uint8_t axesType)
{
    static float actSpeed;
    __IO uint16_t crtPos;
    int32_t encoderDelta;

    switch (jawControlByAxes[axesType].uartPIDCmd)
    {
    case SPEED_ADJ:
        // if((location_timer % 20) == 0){
        crtPos = getEncodeValue(axesType);
        encoderDelta = crtPos - jawControlByAxes[axesType].encoderLast;
        jawControlByAxes[axesType].encoderLast = crtPos;
        actSpeed = (float)encoderDelta / ENCODER_PULSES_PER_REVOLUTION * 250 * 60; // rpm
        //if ((jawControlByAxes[axesType].location_timer % 50) == 0)
            // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
        float speedLoopOutput = SpeedPIDCtrl(actSpeed, motor_pid_spd.Setpoint, &motor_pid_spd);
        motorCtrlByPWM(speedLoopOutput, axesType);
        //  }

        if (jawControlByAxes[axesType].location_timer < SAMP_BUF_SIZE){
            prtBufCmd[jawControlByAxes[axesType].location_timer] = speedLoopOutput;
            prtBufAct[jawControlByAxes[axesType].location_timer] = actSpeed;//crtPos;
        }  
        break;
    case DOUBLE_ADJ:
        // if ((location_timer % 500) == 0) HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
        doubleLoopPID(axesType);
        break;
    default:
        break;
    }
}
#if 1
void JAWCtrlTask(void *argument)
{
    /* USER CODE BEGIN JAWCtrlTask */
    struct JawFlagType JawflagInitial;
    //    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);//X power
    //    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);//Y power
    //    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);//X break
    //    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);//Y break
    //    HAL_TIM_Base_Start_IT(&htim15);
    //    HAL_TIM_PWM_Start_IT(&htim15, TIM_CHANNEL_1);
    //    motorCtrlByPWM(5, Y);
    memset(&JawflagInitial, 0, sizeof(struct JawFlagType));
    osMessageQueuePut(motor_signal_queueHandle, &JawflagInitial, 0, 0);
    jawDeviceInit();

    /* Infinite loop */
    for (;;)
    {
        osMessageQueueGet(motor_signal_queueHandle, &JawflagInitial, 0, osWaitForever);
        //  printf("%u\r\n",getEncodeValue(Y));
        switch(JawflagInitial.axes)
        {
            case X:
                JawCtrlLoop(&JawflagInitial, X);
            break;
            case Y:
                JawCtrlLoop(&JawflagInitial, Y);
            break;
            case XY:
                JawCtrlLoop(&JawflagInitial, X);
                JawCtrlLoop(&JawflagInitial, Y);
            break;
            default: break;
        }
        // osDelay(1);
    }
    /* USER CODE END JAWCtrlTask */
}

void movement_calculation_task(void)
{
    for (;;)
    {
        for (uint8_t axes = 0; axes < 2; axes++)
        {
            if (jawControlByAxes[axes].startMovingFlag)
            {
                if (jawControlByAxes[axes].MotorState == INIT_MOVE_FORWARD)
                {
                   // printf("time: %d %d\r\n", axes,jawControlByAxes[axes].location_timer);
                    #if 1
                    if (jawControlByAxes[axes].location_timer > 500)
                    {
                          //  motorCtrlByPWM(0, axes);
                        jawControlByAxes[axes].startMovingFlag = 0; // stop moving
                        struct JawFlagType JawInitFinish;
                        memset(&JawInitFinish, 0, sizeof(struct JawFlagType));
                        JawInitFinish.axes = axes;
                        JawInitFinish.masterCmd[axes] = INIT_MOVE_FORWARD;
                        osMessageQueuePut(motor_signal_queueHandle, &JawInitFinish, 0, 0);
                    }
                    #else
                    if ((jawControlByAxes[axes].location_timer % 5) == 0)
                    {
                        uint16_t CurrentEncoderFindZero = getEncodeValue(axes);
                       
                        // if (jawControlByAxes[axes].location_timer < SAMP_BUF_SIZE) {
                        //     if (axes == 1) {
                        //         prtBufCmd[jawControlByAxes[axes].location_timer] = CurrentEncoderFindZero;
                        //         prtBufAct[jawControlByAxes[axes].location_timer] = jawControlByAxes[axes].LastEncoderFindZero;
                        //     }
                        // }
                        
                        if ((CurrentEncoderFindZero != 0)&&(CurrentEncoderFindZero == jawControlByAxes[axes].LastEncoderFindZero))//reach back limit
                        {
                          //  motorCtrlByPWM(0, axes);
                            jawControlByAxes[axes].timer7Flag = 0;//stop moving
                            //  if(!jawControlByAxes[axes].passZEnc)   setEncodeValue(jawCtrlByAxes[axes].homeEncodeValue, axes);

                            struct JawFlagType JawInitFinish;
                            memset(&JawInitFinish, 0, sizeof(struct JawFlagType));
                            JawInitFinish.axes = axes;
                            JawInitFinish.masterCmd[axes] = INIT_MOVE_FORWARD;
                            osMessageQueuePut(motor_signal_queueHandle, &JawInitFinish, 0, 0);
                        } else {
                            jawControlByAxes[axes].LastEncoderFindZero = CurrentEncoderFindZero;
                        }
                    }
                    #endif
                }
                else{
                    PID(axes);
                }    
                jawControlByAxes[axes].location_timer++;
            }
            //        if (location_timer < SAMP_BUF_SIZE) {
            //                        if (axes == 1) {
            //                            prtBufCmd[location_timer] = jawCtrlByAxes[axes].timer7Flag;
            //                           // prtBufAct[location_timer] = jawCtrlByAxes[axes].LastEncoderFindZero;
            //                        }
            //             }
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
#endif