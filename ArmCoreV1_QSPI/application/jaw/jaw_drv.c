#include "tim.h"
#include "jaw_control.h"


static void motorPowerCtrl(uint8_t _powerCtrl,uint8_t axesType)
{
    if (1 == _powerCtrl)
    {
        if(axesType == X)  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
        else if(axesType == Y)   HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
        else if(axesType == XY){
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
        }
    }
    else
    {
        if(axesType == X)  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
        else if(axesType == Y)  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
        else if(axesType == XY){
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
        }
    }
}

static void startEncodeTim(uint8_t axesType)
{
    if(axesType == X)   HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
   else if(axesType == Y)   HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
}

static void stopEncodeTim(uint8_t axesType)
{
    if(axesType == X)   HAL_TIM_Encoder_Stop(&htim8, TIM_CHANNEL_ALL);
   else if(axesType == Y)    HAL_TIM_Encoder_Stop(&htim5, TIM_CHANNEL_ALL);
}

static void stopPWMOutput(uint8_t axesType)
{
    if(axesType == X)
    {
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
    }
    else if(axesType == Y)
    {
        HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_2);
    }
}

static void BrakeCtrl(uint8_t _brakeCtrl, uint8_t axesType)
{
    if (1 == _brakeCtrl)
    {
        if(axesType == X)   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
       else if(axesType == Y) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    }
    else
    {
        if(axesType == X)   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
       else if(axesType == Y) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
    }
}

uint16_t getEncodeValue(uint8_t axesType)
{
    if(axesType == X){
       // LOG_I("get encX: %u\r\n", __HAL_TIM_GET_COUNTER(&htim8));
        return __HAL_TIM_GET_COUNTER(&htim8);
    }
    else if(axesType == Y) {
       // LOG_I("get encY: %u\r\n", __HAL_TIM_GET_COUNTER(&htim5));
        return __HAL_TIM_GET_COUNTER(&htim5);
    }
}

void setEncodeValue(uint16_t setValue, uint8_t axesType)
{
    if(axesType == X){
        __HAL_TIM_SET_COUNTER(&htim8, setValue);
      //  LOG_I("set encX: %u\r\n", setValue);
    }
    else if(axesType == Y) {
        __HAL_TIM_SET_COUNTER(&htim5, setValue);
       // LOG_I("set encY: %u\r\n", setValue);
    }
}

static GPIO_PinState limitSwitchLevel(uint8_t axesType)
{
    if(axesType == X)   return HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_14);
    else if(axesType == Y)  return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
}

void motorCtrlByPWM(double dutyCycle, uint8_t axesType)
{
    double absDutyCycle = fabs(dutyCycle), pulseLength = 0;
    if ((absDutyCycle < 3) && (dutyCycle != 0))
    {
        absDutyCycle = 3;
    } 
    else if (absDutyCycle > 95) //if dutycycle is greater than 95, motor ctrl has some issues,unsolved
    {
        absDutyCycle = 95;
    }
    if (axesType == X) {
        pulseLength = (double)((htim3.Init.Period + 1) * absDutyCycle) / 100;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, (uint16_t)pulseLength);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, (uint16_t)pulseLength);

        if (dutyCycle > 0) {
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
            if (HAL_TIM_GetChannelState(&htim3, TIM_CHANNEL_4) == HAL_TIM_CHANNEL_STATE_READY) {
                HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
            }
        } else if (dutyCycle < 0) {
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
            if (HAL_TIM_GetChannelState(&htim3, TIM_CHANNEL_3) == HAL_TIM_CHANNEL_STATE_READY) {
                HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
            }
        } else {
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
        }
    }
    else if (axesType == Y) {
        pulseLength = (double)((htim15.Init.Period + 1) * absDutyCycle) / 100;
        __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, (uint16_t)pulseLength);
        __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_2, (uint16_t)pulseLength);

        if (dutyCycle > 0) {
            HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_2);
            if (HAL_TIM_GetChannelState(&htim15, TIM_CHANNEL_1) == HAL_TIM_CHANNEL_STATE_READY) {
                HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
            }
        } else if (dutyCycle < 0) {
            HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_1);
            if (HAL_TIM_GetChannelState(&htim15, TIM_CHANNEL_2) == HAL_TIM_CHANNEL_STATE_READY) {
                HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2);
            }
        } else {
            HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_1);
            HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_2);
        }
    }
}

void motorEnable(uint8_t axesType)
{
    motorPowerCtrl(1, axesType);//open motor power supply
    BrakeCtrl(1, axesType);//loose brake
    startPWMOutput(axesType);
    startEncodeTim(axesType);
}

void motorDisable(uint8_t axesType)
{
    motorCtrlByPWM(0, axesType);
    stopEncodeTim(axesType);
    stopPWMOutput(axesType);
    motorPowerCtrl(0, axesType);//close motor power supply
    BrakeCtrl(0, axesType);//tight brake
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)//todo
{
    // if (htim->Instance == TIM15)
    // {
    //     jawCtrlByAxes[Y].EnableTriggernFault = 0;
    //   //  LOG_I("44444444444444444\r\n");
    //    SetMotorYIO(0,0);
    // }
    // if (htim->Instance == TIM3)
    // {
    //     jawCtrlByAxes[X].EnableTriggernFault = 0;
    //    // LOG_I("11111111111111\r\n");
    //     SetMotorXIO(0, 0);
    // }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
//   if (htim->Instance == TIM4) {
//     HAL_IncTick();
//   }
  /* USER CODE BEGIN Callback 1 */
    // if (htim->Instance == TIM15)
    // {
    //     // LOG_I("33333333\r\n");

    //     SetMotorYIO(jawCtrlByAxes[Y].MotorDir,jawCtrlByAxes[Y].MotorMoveEn);
    //      jawCtrlByAxes[Y].EnableTriggernFault = 1;
    // }
    // else if (htim->Instance == TIM3)
    // {
    //    // LOG_I("2222222222222222\r\n");
    //     SetMotorXIO(jawCtrlByAxes[X].MotorDir,jawCtrlByAxes[X].MotorMoveEn);
    //     jawCtrlByAxes[X].EnableTriggernFault = 1;
    // }
  /* USER CODE END Callback 1 */
}

void yjaw_nfault_callback(void)
{
    // if(1 == jawCtrlByAxes[Y].EnableTriggernFault)
    // {
    //     if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == GPIO_PIN_RESET){
    //  // LOG_I("1\r\n");
    //         SetMotorYIO(0, 0);
    //     }
    //   else{
    //    //  LOG_I("0\r\n");
    //        SetMotorYIO(jawCtrlByAxes[Y].MotorDir, jawCtrlByAxes[Y].MotorMoveEn);
    //    }
    // }
}

void xjaw_nfault_callback(void)
{
    // if(1 == jawCtrlByAxes[X].EnableTriggernFault){
    //     if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET){
    //      //   LOG_I("3\r\n");
    //         SetMotorXIO(0, 0);
    //     } else {
    //       //  LOG_I("4\r\n");
    //         SetMotorXIO(jawCtrlByAxes[X].MotorDir, jawCtrlByAxes[X].MotorMoveEn);
    //     }
    // }
}


#ifndef PWM_TEST
#include "shell.h"
static int8_t motor_pwm_set(uint8_t argc, uint8_t **argv)
{
    motorCtrlByPWM(atoi(argv[1]), atoi(argv[2]));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(motor_pwm_set, motor_pwm_set, motor_pwm_set test);

#endif