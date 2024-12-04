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

static MotorCtrlSignalDef_t MotorCtrlSignal[2];

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
    double dutyCycle = atof(argv[1]);
    motorCtrlByPWM(MOTOR_MAG, dutyCycle);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFTBrakeCtrl, AFTBRK,AFT brake ctrl);

void SetMagMotorIO(uint16_t dir, uint16_t En)
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
            HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, 0);
        }
    }
    else
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, 0);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, 0);
    }
} 
void SetAFTMotorIO(uint16_t dir, uint16_t En)
{
    if (1 == En)
    {
        if (0 == dir)
        {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
        }
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
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
    else if(motorType == MOTOR_AFT) {
        HAL_TIM_Base_Stop_IT(&htim5);
        HAL_TIM_PWM_Stop_IT(&htim5, TIM_CHANNEL_3);
    }
}

void motorCtrlByPWM(motorTypeDef motorType,double dutyCycle)
{
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

    double absDutyCycle = fabs(dutyCycle), pulseLength = 0;

    if ((absDutyCycle < 3) && (dutyCycle != 0)) absDutyCycle = 3;
    else if (absDutyCycle > 100)  absDutyCycle = 100;// Assuming duty cycle is in percentage
   // printf("duty %d %lf\r\n",htim3.Init.Period, absDutyCycle);
  //  startPWMOutput(axesType);
    if(motorType == MOTOR_MAG)
    {
      //  printf("duty %d %lf\r\n",htim3.Init.Period, absDutyCycle);
        pulseLength = (double)((htim24.Init.Period + 1) * absDutyCycle) / 100;
        __HAL_TIM_SET_COMPARE(&htim24, TIM_CHANNEL_3, (uint16_t) pulseLength);
    }
    else if(motorType == MOTOR_AFT) 
    {
        pulseLength = (double)((htim5.Init.Period + 1) * absDutyCycle) / 100;
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, (uint16_t) pulseLength);
    }
   // printf("pulseLength = %lf %d\r\n",pulseLength, (uint16_t) pulseLength);
//    printf("%ld,%d,%f,%f,%d\r\n", encoder_val, _encoderYZ, MotorSpeed, pid_output, tmp);
}
void Shell_MagMotorCtrlByPWM(uint8_t argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: MAGPWM <duty cycle>\r\n");
    }
    double dutyCycle = atof(argv[1]);
    motorCtrlByPWM(MOTOR_MAG, dutyCycle);
}
MSH_CMD_EXPORT_ALIAS(Shell_MagMotorCtrlByPWM, MAGPWM,Mag motor pwm ctrl);

void Shell_AFTMotorCtrlByPWM(uint8_t argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: MAGPWM <duty cycle>\r\n");
    }
    double dutyCycle = atof(argv[1]);
    motorCtrlByPWM(MOTOR_MAG, dutyCycle);
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
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET)
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
        if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == GPIO_PIN_RESET)
        {
            SetAFTMotorIO(0, 0);
        }
        else
        {   
            SetAFTMotorIO(MotorCtrlSignal[MOTOR_AFT].MotorDir, MotorCtrlSignal[MOTOR_AFT].MotorMoveEn);
        }
    }
}
static void MotorInitial_thread_entry(void *argument)
{
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM5_Init();
    MX_TIM24_Init();
    gpio_pin_irq_callback_register("GPIOA_6", MagMotor_nFault_callback);
    gpio_pin_irq_callback_register("GPIOE_4", AFTMotor_nFault_callback);
    motorEnable(MOTOR_MAG);
    motorEnable(MOTOR_AFT);
    for (;;)
    {
        printf("mag encoder = %d\r\n",getEncodeValue(MOTOR_MAG));
        printf("aft encoder = %d\r\n",getEncodeValue(MOTOR_AFT));
        osDelay(1000);
    }
}

static int8_t MotorInitial_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "MotorInitial_thread",
    .stack_size = 2048 * 4,
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
INIT_APP_EXPORT(MotorInitial_thread_init);