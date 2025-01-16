#include "motorctrl.h"
#include "tim.h"
#include "drv_gpio.h"
#include "init_call.h"
#include "ulog.h"
#include <stdbool.h>
#include "stm32h7xx_hal.h"
#include "tim.h"
#include "shell.h"
#include "cmsis_os2.h"
#include "AFCapp.h"
static MotorCtrlSignalDef_t MotorCtrlSignal[2];
static PID_TypeDef MAGmotor_pid_para = {.Kp = 1.2,
                                        .Ki = 0.0,
                                        .Kd = 0,
                                        .Setpoint = 0,
                                        .IntegralLimit = 50,
                                        .OutputLimit = 40};
static PID_TypeDef AFTmotor_pid_para = {.Kp = 1,
                                        .Ki = 0.1,
                                        .Kd = 0,
                                        .Setpoint = 0,
                                        .IntegralLimit = 50,
                                        .OutputLimit = 100};

static MotorCtrlParam_TypeDef MagMotorParameter ={0};
static MotorCtrlParam_TypeDef AFTMotorParameter ={0};
static AFCConfigParam_TypeDef AFCConfigParam ={0};
MotorCtrlParam_TypeDef *AFT_motorParam_get(void)
{
    return &AFTMotorParameter;
}
MotorCtrlParam_TypeDef *MAG_motorParam_get(void)
{
    return &MagMotorParameter;
}
AFCConfigParam_TypeDef *AFC_ConfigParam_get(void)
{
    return &AFCConfigParam;
}

void AFTBrakeCtrl(AFTBrakeTypeDef _brakeCtrl)//only AFT motor has brake
{
    if (AFT_BRAKE_ON == _brakeCtrl)
    {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
    }
}

void Shell_AFTBrakeCtrl(uint8_t argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: MAGPWM <duty cycle>\r\n");
    }
    uint8_t AFTBrakeStatus = strtol((char *)argv[1], NULL, 10);
    AFTBrakeCtrl(AFTBrakeStatus);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFTBrakeCtrl, AFTBRK,AFT brake ctrl);

void SetMagMotorIO(uint16_t dir, uint16_t En)
{
    if (1 == En)
    {
        if (0 == dir)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
        }
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
    }
} 
void SetAFTMotorIO(uint16_t dir, uint16_t En)
{
    if (1 == En)
    {
        if (0 == dir)
        {
            HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, 0);
            HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, 1);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, 1);
            HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, 0);
        }
    }
    else
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, 1);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, 1);
    }
} 

void startEncodeTim(motorTypeDef motorType)
{
    if(motorType == MOTOR_MAG)  
    {
        HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    }
    else if(motorType == MOTOR_AFT)   
    {
        HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    }
}

void stopEncodeTim(motorTypeDef motorType)
{
    if(motorType == MOTOR_MAG) 
    {
        HAL_TIM_Encoder_Stop(&htim2, TIM_CHANNEL_ALL);
    }
    else if(motorType == MOTOR_AFT)  
    {
        HAL_TIM_Encoder_Stop(&htim3, TIM_CHANNEL_ALL);
    }
}

uint16_t getEncodeValue(motorTypeDef motorType)
{
    if(motorType == MOTOR_MAG){
        return __HAL_TIM_GET_COUNTER(&htim2);
    }
    else if(motorType == MOTOR_AFT) {
        return __HAL_TIM_GET_COUNTER(&htim3);
    }
}

void setEncodeValue(motorTypeDef motorType,uint16_t setValue)
{
    if(motorType == MOTOR_MAG){
        __HAL_TIM_SET_COUNTER(&htim2, setValue);
    }
    else if(motorType == MOTOR_AFT) {
        __HAL_TIM_SET_COUNTER(&htim3, setValue);
    }
}

void startPWMOutput(motorTypeDef motorType)
{
    if(motorType == MOTOR_MAG)  
    {
        HAL_TIM_Base_Start_IT(&htim24);
        HAL_TIM_PWM_Start_IT(&htim24, TIM_CHANNEL_3);
    }
    else if(motorType == MOTOR_AFT)
    {
        HAL_TIM_Base_Start_IT(&htim5);
        HAL_TIM_PWM_Start_IT(&htim5, TIM_CHANNEL_3);
    }
}

void stopPWMOutput(motorTypeDef motorType)
{
    if(motorType == MOTOR_MAG)  
    {
        HAL_TIM_Base_Stop_IT(&htim24);
        HAL_TIM_PWM_Stop_IT(&htim24, TIM_CHANNEL_3);
    }
    else if(motorType == MOTOR_AFT)
    {
        HAL_TIM_Base_Stop_IT(&htim5);
        HAL_TIM_PWM_Stop_IT(&htim5, TIM_CHANNEL_3);
    }
}

void motorCtrlByPWM(motorTypeDef motorType,float dutyCycle)
{
    uint16_t current_pwm = 0;
    if (motorType == MOTOR_MAG)
    {
        // current_pwm =  (__HAL_TIM_GET_COMPARE(&htim24, TIM_CHANNEL_3)) / 2;
        current_pwm =  __HAL_TIM_GET_COMPARE(&htim24, TIM_CHANNEL_3);
    }
    else if (motorType == MOTOR_AFT)
    {
        current_pwm =  (__HAL_TIM_GET_COMPARE(&htim5, TIM_CHANNEL_3) / (htim5.Init.Period + 1)) * 100;
    }
    if (dutyCycle > 0)
    {
        MotorCtrlSignal[motorType].MotorMoveEn = 1;
        MotorCtrlSignal[motorType].MotorDir = 1;
    }
    else if (dutyCycle < 0)
    {
        MotorCtrlSignal[motorType].MotorMoveEn = 1;
        MotorCtrlSignal[motorType].MotorDir = 0;
    }
    else
    {
        MotorCtrlSignal[motorType].MotorMoveEn = 0;
    }

    float absDutyCycle = fabs(dutyCycle);
    uint16_t pulseLength = 0;

    if ((absDutyCycle < 3) && (dutyCycle != 0)) absDutyCycle = 3;
    else if (absDutyCycle >40)  absDutyCycle = 40;// Assuming duty cycle is in percentage
   // printf("duty %d %lf\r\n",htim3.Init.Period, absDutyCycle);
  //  startPWMOutput(axesType);
    if(motorType == MOTOR_MAG)
    {
      //  printf("duty %d %lf\r\n",htim3.Init.Period, absDutyCycle);
        pulseLength = ((htim24.Init.Period + 1) * absDutyCycle) / 100;
        if(current_pwm != pulseLength)
        {
            LOG_I("%d    %d\r\n",current_pwm,pulseLength);
    
        }
        __HAL_TIM_SET_COMPARE(&htim24, TIM_CHANNEL_3,  pulseLength);
       
    }
    else if(motorType == MOTOR_AFT) 
    {
        pulseLength = (float)((htim5.Init.Period + 1) * absDutyCycle) / 100;
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, (uint16_t) pulseLength);
    }
   // printf("pulseLength = %lf %d\r\n",pulseLength, (uint16_t) pulseLength);
//    printf("%ld,%d,%f,%f,%d\r\n", encoder_val, _encoderYZ, MotorSpeed, pid_output, tmp);  
}
void Shell_MagMotorCtrlByPWM(uint8_t argc, char *argv[])
{
    uint16_t dutyCycle = strtol((char *)argv[1], NULL, 10);
    motorCtrlByPWM(MOTOR_MAG, dutyCycle);
}
MSH_CMD_EXPORT_ALIAS(Shell_MagMotorCtrlByPWM, MAGPWM,Mag motor pwm ctrl);

void Shell_AFTMotorCtrlByPWM(uint8_t argc, char *argv[])
{
    uint16_t dutyCycle = strtol((char *)argv[1], NULL, 10);
    motorCtrlByPWM(MOTOR_AFT, dutyCycle);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFTMotorCtrlByPWM, AFTPWM,AFT motor pwm ctrl);

void motorEnable(motorTypeDef motorType)
{
    if(motorType == MOTOR_AFT)
    {
        AFTBrakeCtrl(AFT_BRAKE_OFF);
    }
    startPWMOutput(motorType);
    startEncodeTim(motorType);
}

void motorDisable(motorTypeDef motorType)
{
    motorCtrlByPWM(0, motorType);
    stopEncodeTim(motorType);
    stopPWMOutput(motorType);
    if(motorType == MOTOR_AFT)
    {
        AFTBrakeCtrl(AFT_BRAKE_ON);
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)//todo
{
    if (htim->Instance == TIM24)    
    {
        MotorCtrlSignal[MOTOR_MAG].EnableTriggernFault = 0;
        SetMagMotorIO(0,0);
    }
    if (htim->Instance == TIM5)
    {
        MotorCtrlSignal[MOTOR_AFT].EnableTriggernFault = 0;
        SetAFTMotorIO(0,0);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM24)    
    {
        SetMagMotorIO(MotorCtrlSignal[MOTOR_MAG].MotorDir,MotorCtrlSignal[MOTOR_MAG].MotorMoveEn);
        MotorCtrlSignal[MOTOR_MAG].EnableTriggernFault = 1;
    }
    else if (htim->Instance == TIM5)
    {
        SetAFTMotorIO(MotorCtrlSignal[MOTOR_AFT].MotorDir,MotorCtrlSignal[MOTOR_AFT].MotorMoveEn);
        MotorCtrlSignal[MOTOR_AFT].EnableTriggernFault = 1;
    }
}

void MagMotor_nFault_callback(void)
{
    if(1 == MotorCtrlSignal[MOTOR_MAG].EnableTriggernFault)
    {
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_RESET)
        {
            SetMagMotorIO(0, 0);
        } 
        else 
        {
            SetMagMotorIO(MotorCtrlSignal[MOTOR_MAG].MotorDir, MotorCtrlSignal[MOTOR_MAG].MotorMoveEn);
        }
    }
}

void AFTMotor_nFault_callback(void)
{
    if(1 == MotorCtrlSignal[MOTOR_AFT].EnableTriggernFault)
    {
        if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET)
        {
            SetAFTMotorIO(0, 0);
        }
        else
        {   
            SetAFTMotorIO(MotorCtrlSignal[MOTOR_AFT].MotorDir, MotorCtrlSignal[MOTOR_AFT].MotorMoveEn);
        }
    }
}

float PID_Compute(PID_TypeDef *pid, float current, float setpoint)
{
    pid->Setpoint = setpoint;
    float error = pid->Setpoint - current;
    pid->Integral += error;
    if (pid->Integral > pid->IntegralLimit)
        pid->Integral = pid->IntegralLimit;
    if (pid->Integral < -pid->IntegralLimit)
        pid->Integral = -pid->IntegralLimit;

    float derivative = error - (pid->PreviousError);
    float output = (pid->Kp * error) + (pid->Ki * pid->Integral) + (pid->Kd * derivative);
    pid->PreviousError = error;
    if (output > pid->OutputLimit)
    {
        output = pid->OutputLimit;
    }
    else if (output < -pid->OutputLimit)
    {
        output = -pid->OutputLimit;
    }
    return output;
}

float PositionPIDCtrl(uint16_t current_position, uint16_t _setPosition, PID_TypeDef *pid)
{
    float _pidPositionOutput;
    //float current_position = __HAL_TIM_GET_COUNTER(&htim5);
    _pidPositionOutput = PID_Compute(pid, current_position, _setPosition);
    //printf("_setPosition%d \t %d\r\n",current_position,_setPosition);
    //MotorYCtrlByPWM((int16_t) pid_output);
    return _pidPositionOutput;
}

MotorFindingZeroFSM_t MagMotorState = MotorFSM_Init;
extern AFCApplicationParam_t AFCApplicationParam;
static uint8_t MagMotorInitDone = 0;
static uint16_t MagEncoderData;
static uint16_t IsKeyDown = 0;
static uint16_t MagMotorSetPos = 31000;
MotorFindingZeroFSM_t *MagMotorState_get(void)
{
    return &MagMotorState;
}
// static float AutoControl_pid_output;
uint16_t MagMotorAutoControltimes = 100;
uint16_t MagMotorAutoTarget;
extern uint16_t MagMotorADCValue[2];
void MagMotorInitFSM(void)
{
   // uint16_t AFTMotorPos = 20000;
    uint16_t MagForwardEncCounter;
    uint16_t MagForwardEncCounterPrev;
    uint16_t MagBackwardEncCounter;
    uint16_t MagBackwardEncCounterPrev;
    MotorCtrlParam_TypeDef *obj = MAG_motorParam_get();
    static float AutoControl_pid_output;
    //MagMotorParameter.encoderValTarget = 20000;
    // MagMotorAutoTarget = getEncodeValue(MOTOR_MAG);
    switch (MagMotorState)
    {
        case MotorFSM_Init:
            printf("MotorInit\r\n");
            if(0 == MagMotorInitDone)
            {
                motorEnable(MOTOR_MAG);
                __HAL_TIM_SET_COUNTER(&htim2, 32767);
                motorCtrlByPWM(MOTOR_MAG, 0);
            }
            MagMotorInitDone =1;
            motorCtrlByPWM(MOTOR_MAG, -30);
            osDelay(1000);
            MagMotorState = MotorFSM_Backward2FindZero;
            break;
        case MotorFSM_Backward2FindZero:
            motorCtrlByPWM(MOTOR_MAG, 30);
            MagForwardEncCounterPrev = __HAL_TIM_GET_COUNTER(&htim2);
            osDelay(500);
            MagForwardEncCounter = __HAL_TIM_GET_COUNTER(&htim2);
           // printf("MagForwardEncCounterPrev = %d MagForwardEncCounter = %d\r\n", MagForwardEncCounterPrev, MagForwardEncCounter);
            if(MagForwardEncCounterPrev == MagForwardEncCounter)
            {
                __HAL_TIM_SET_COUNTER(&htim2,35195);
                printf("231MagForwardEncCounter = %d\r\n",  getEncodeValue(MOTOR_MAG));
                motorCtrlByPWM(MOTOR_MAG, 0);
                MagMotorParameter.motorFindZeroOK = 0x01;
                printf("111Go to  presetPos= %d\r\n",MagMotorParameter.presetPos);
                MagMotorState = MotorFSM_ManualControl;
            }
            break;  
        case MotorFSM_ZERO_CONFIRMED:
            MagForwardEncCounter = getEncodeValue(MOTOR_MAG);
            if(MagMotorAutoControltimes % 500 == 0)
            {
                LOG_I("EncCounter = %d\r\n",MagForwardEncCounter);
            }
            motorCtrlByPWM(MOTOR_MAG, PositionPIDCtrl(getEncodeValue(MOTOR_MAG),MagMotorParameter.encoderValTarget, &MAGmotor_pid_para));
            MagMotorAutoControltimes++;
            osDelay(1);
            break;
        case MotorFSM_StayAtPresetPos:
            motorCtrlByPWM(MOTOR_MAG, PositionPIDCtrl(getEncodeValue(MOTOR_MAG),MagMotorParameter.presetPos, &MAGmotor_pid_para));
            osDelay(1);
        
            break;
        case MotorFSM_ManualControl:
            motorCtrlByPWM(MOTOR_MAG, PositionPIDCtrl(getEncodeValue(MOTOR_MAG),MagMotorParameter.encoderValTarget, &MAGmotor_pid_para));
            MagMotorAutoTarget = MagMotorParameter.encoderValTarget;
            osDelay(1);
            break;
        case MotorFSM_AutoControl:
        
        if(MagMotorAutoControltimes % 20 == 0)
        {
            // LOG_I("ADC1 = %d\r\n",MagMotorADCValue[0]);
            // LOG_I("ADC2 = %d\r\n",MagMotorADCValue[1]);
            MagForwardEncCounter = getEncodeValue(MOTOR_MAG);
            MagMotorAutoTarget = MagForwardEncCounter;
            // LOG_I("EncCounter = %d\r\n",MagForwardEncCounter);
            if(MagMotorADCValue[0] > MagMotorADCValue[1] + 150)
            {
                MagMotorAutoTarget = MagMotorAutoTarget + 20;
            }
            else if(MagMotorADCValue[0] < MagMotorADCValue[1] - 150)
            {
                MagMotorAutoTarget = MagMotorAutoTarget - 20;
            }
        }
            MagMotorAutoControltimes++;
            if(MagMotorAutoTarget > 21100)
            {
                MagMotorAutoTarget = 21050;
            }
            else if(MagMotorAutoTarget < 19000)
            {
                MagMotorAutoTarget = 19050;
            }
            AutoControl_pid_output = PositionPIDCtrl(getEncodeValue(MOTOR_MAG),MagMotorAutoTarget, &MAGmotor_pid_para);
            // LOG_I("pid_output = %f\r\n",AutoControl_pid_output);
            motorCtrlByPWM(MOTOR_MAG, AutoControl_pid_output);
            // motorCtrlByPWM(MOTOR_MAG, PositionPIDCtrl(getEncodeValue(MOTOR_MAG),MagMotorAutoTarget, &MAGmotor_pid_para));
            osDelay(1);
            break;
        case MotorFSM_ERROR_STATE:
            printf("ERROR_STATE\r\n");
            break;
    }
}
MotorFindingZeroFSM_t AFTMotorState = MotorFSM_Init;
uint8_t AFTMotorInitDone = 0;
uint16_t AFTMotorEncoderData;
uint16_t AFTMotorIsKeyDown = 0;
uint16_t AFTMotorSetPos = 20000;
void  AFTMotorInitFSM()
{
   // uint16_t AFTMotorPos = 20000;
    uint16_t AFTForwardEncCounter;
    uint16_t AFTForwardEncCounterPrev;
    uint16_t AFTBackwardEncCounter;
    uint16_t AFTBackwardEncCounterPrev;
//    printf("AFTMotorState = %d\r\n",AFTMotorState);
    MotorCtrlParam_TypeDef *obj = AFT_motorParam_get();
    switch (AFTMotorState)
    {
        case MotorFSM_Init:
            printf("MotorInit\r\n");
            if(0 == AFTMotorInitDone)
            {
                motorEnable(MOTOR_AFT);
                __HAL_TIM_SET_COUNTER(&htim3, 32767);
                motorCtrlByPWM(MOTOR_AFT, 0);
            }
            AFTMotorInitDone =1;
            AFTBrakeCtrl(AFT_BRAKE_ON);
            motorCtrlByPWM(MOTOR_AFT, -60);
            osDelay(1000);
            AFTMotorState = MotorFSM_Backward2FindZero;
            break;
        case MotorFSM_Backward2FindZero:
            motorCtrlByPWM(MOTOR_AFT, 60);
            AFTForwardEncCounterPrev = __HAL_TIM_GET_COUNTER(&htim3);
            osDelay(500);
            AFTForwardEncCounter = __HAL_TIM_GET_COUNTER(&htim3);   
           // printf("MagForwardEncCounterPrev = %d MagForwardEncCounter = %d\r\n", MagForwardEncCounterPrev, MagForwardEncCounter);
            if(AFTForwardEncCounterPrev == AFTForwardEncCounter)
            {
                __HAL_TIM_SET_COUNTER(&htim3,32767);
                printf("AFTForwardEncCounter = %d\r\n",  getEncodeValue(MOTOR_AFT));
                motorCtrlByPWM(MOTOR_AFT, 0);
                AFTMotorParameter.motorFindZeroOK = 0x01;
                AFTMotorState = MotorFSM_ZERO_CONFIRMED;
            }
            break;
        case MotorFSM_ZERO_CONFIRMED:
            // printf("AFTMotorParameter.encoderValTarget = %d\r\n",AFTMotorParameter.encoderValTarget);
            // osDelay(1000);
           // motorCtrlByPWM(MOTOR_AFT, PositionPIDCtrl(getEncodeValue(MOTOR_AFT),AFTMotorParameter.encoderValTarget, &AFTmotor_pid_para));
            // if(AFTMotorParameter.encoderValTarget < AFTMotorParameter.encoderValCurrent + 100)
            // {
            //     motorCtrlByPWM(MOTOR_AFT, 60);
            // }
            // else if(AFTMotorParameter.encoderValTarget > AFTMotorParameter.encoderValCurrent - 100)
            // {
            //     motorCtrlByPWM(MOTOR_AFT, -60);
            // }
            // else
            // {
            //     motorCtrlByPWM(MOTOR_AFT, 0);
            // }
            osDelay(1);
            break;
        case MotorFSM_ERROR_STATE:
            printf("ERROR_STATE\r\n");
            break;
    }
}
//static uint16_t MagMotorSetPos = 20;
void Shell_SetAFTMotorRunByStep(uint8_t argc, char *argv[])
{
    uint8_t dir = strtol((char *)argv[1], NULL, 10);
    if(dir == 1)
    {
        motorCtrlByPWM(MOTOR_AFT, 60);
        osDelay(100);
        motorCtrlByPWM(MOTOR_AFT, 0);
    }
    else if(dir == 2)
    {
        motorCtrlByPWM(MOTOR_AFT, -60);
        osDelay(100);
        motorCtrlByPWM(MOTOR_AFT, 0);   
    }
}
MSH_CMD_EXPORT_ALIAS(Shell_SetAFTMotorRunByStep, AFTStep,AFT motor run by step);
void Shell_SetMagMotorRunByStep(uint8_t argc, char *argv[])
{   
    uint8_t dir = strtol((char *)argv[1], NULL, 10);
    uint16_t step = strtol((char *)argv[2], NULL, 10) | (strtol((char *)argv[3], NULL, 10) << 8);
    
    // LOG_I("dir = %d step = %d\r\n",dir,step);
    if(dir == 1)
    {
        MagMotorSetPos += step;
    }
    else if(dir == 2)
    {
        MagMotorSetPos -= step;
    }
    if(MagMotorSetPos > 65535)
    {
        MagMotorSetPos = 65535;
    }
    else if(MagMotorSetPos < 10000)
    {
        MagMotorSetPos = 10000;
    }
    MagMotorParameter.encoderValTarget = MagMotorSetPos;
    // LOG_I("Target = %d\r\n",MagMotorParameter.encoderValTarget);
}
MSH_CMD_EXPORT_ALIAS(Shell_SetMagMotorRunByStep, MAGSTEP,Mag motor run by step);
void Shell_SetMagMotorSetPos(uint8_t argc, char *argv[])
{
    MagMotorSetPos = strtol((char *)argv[1], NULL, 10);
}
MSH_CMD_EXPORT_ALIAS(Shell_SetMagMotorSetPos, MAGPOS,Mag motor set position);
void Shell_GetMagMotorPos(uint8_t argc, char *argv[])
{
    LOG_E("MagMotorPos = %d\r\n",getEncodeValue(MOTOR_MAG));
}
MSH_CMD_EXPORT_ALIAS(Shell_GetMagMotorPos, MAGPOSGET,Mag motor get position);
void Shell_SetAFTMotorSetPos(uint8_t argc, char *argv[])
{
    AFTMotorSetPos = strtol((char *)argv[1], NULL, 10);
}
MSH_CMD_EXPORT_ALIAS(Shell_SetAFTMotorSetPos, AFTPOS,AFT motor set position);
void Shell_GetAFTMotorPos(uint8_t argc, char *argv[])
{
    LOG_E("AFTMotorPos = %d\r\n",getEncodeValue(MOTOR_AFT));
}
MSH_CMD_EXPORT_ALIAS(Shell_GetAFTMotorPos, AFTPOSGET,AFT motor get position);
static void MotorInitial_thread_entry(void *argument)
{
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM5_Init();
    MX_TIM24_Init();
    gpio_pin_irq_callback_register("GPIOA_6", MagMotor_nFault_callback);
    gpio_pin_irq_callback_register("GPIOE_4", AFTMotor_nFault_callback);
    MagMotorParameter.presetPos = 20500;// 25118;
    MagMotorParameter.encoderValTarget = MagMotorParameter.presetPos;
    AFCApplicationParam.positionCalculated = MagMotorParameter.presetPos;
    for (;;)
    {
        MagMotorInitFSM();
    }
}

static int8_t MotorInitial_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "MotorInitial_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityNormal,
    };

    osThreadId_t thread_id = osThreadNew(MotorInitial_thread_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread MotorInitial create failed\r\n");
        return -1;
    }
    return 0;
}
static void AFTMotorInitial_thread_entry(void *argument)
{
    for (;;)
    {
        AFTMotorInitFSM();
    }
}

static int8_t AFTMotorInitial_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "AFTMotorInitial_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityNormal,
    };

    osThreadId_t thread_id = osThreadNew(AFTMotorInitial_thread_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread AFTMotorInitial create failed\r\n");
        return -1;
    }
    return 0;
}

INIT_APP_EXPORT(MotorInitial_thread_init);
INIT_APP_EXPORT(AFTMotorInitial_thread_init);