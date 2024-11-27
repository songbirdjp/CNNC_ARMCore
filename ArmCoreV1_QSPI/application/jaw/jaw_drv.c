#include "tim.h"
#include "jaw_control.h"

static JawMotorSignals jawCtrlByAxes[2];

/* | dir |  En |       status        |*/
/* |  0  |  0  |       brake         |*/ //low side slow decay
/* |  0  |  1  |       forward       |*/
/* |  1  |  0  |       brake         |*/
/* |  1  |  1  |       reverse       |*/ //low side slow decay
void SetMotorYIO(uint16_t dir, uint16_t En)
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
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
    }
}

void SetMotorXIO(uint16_t dir, uint16_t En)
{
    if (1 == En)
    {
        if (0 == dir)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
        }
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
    }
}

void motorPowerCtrl(uint8_t _powerCtrl,uint8_t axesType)
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

void startEncodeTim(uint8_t axesType)
{
    if(axesType == X)   HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
   else if(axesType == Y)   HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
}

void stopEncodeTim(uint8_t axesType)
{
    if(axesType == X)   HAL_TIM_Encoder_Stop(&htim8, TIM_CHANNEL_ALL);
   else if(axesType == Y)    HAL_TIM_Encoder_Stop(&htim5, TIM_CHANNEL_ALL);
}

void startPWMOutput(uint8_t axesType)
{
    if(axesType == X){
        HAL_TIM_Base_Start_IT(&htim3);
        HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
    }
    else if(axesType == Y) {
        HAL_TIM_Base_Start_IT(&htim15);
        HAL_TIM_PWM_Start_IT(&htim15, TIM_CHANNEL_1);
    }
}

void stopPWMOutput(uint8_t axesType)
{
    if(axesType == X){
        HAL_TIM_Base_Stop_IT(&htim3);
        HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_1);
    }
    else if(axesType == Y) {
        HAL_TIM_Base_Stop_IT(&htim15);
        HAL_TIM_PWM_Stop_IT(&htim15, TIM_CHANNEL_1);
    }
}

void BrakeCtrl(uint8_t _brakeCtrl, uint8_t axesType)
{
    if (1 == _brakeCtrl)
    {
        if(axesType == X)   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
       else if(axesType == Y) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    }
    else
    {
        if(axesType == X)   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
       else if(axesType == Y) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
    }
}

uint16_t getEncodeValue(uint8_t axesType)
{
    if(axesType == X){
        return __HAL_TIM_GET_COUNTER(&htim8);
      //  printf("get encX: %lf\r\n", current_position);
    }
    else if(axesType == Y) {
        return __HAL_TIM_GET_COUNTER(&htim5);
       // printf("get encY: %lf\r\n", current_position);
    }
}

void setEncodeValue(uint16_t setValue, uint8_t axesType)
{
    if(axesType == X){
        __HAL_TIM_SET_COUNTER(&htim8, setValue);
      //  printf("set encX: %u\r\n", setValue);
    }
    else if(axesType == Y) {
        __HAL_TIM_SET_COUNTER(&htim5, setValue);
       // printf("set encY: %u\r\n", setValue);
    }
}

GPIO_PinState limitSwitchLevel(uint8_t axesType)
{
    if(axesType == X)   return HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_14);
    else if(axesType == Y)  return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
}

void motorCtrlByPWM(double dutyCycle, uint8_t axesType)
{
    if (dutyCycle > 0)
    {
        jawCtrlByAxes[axesType].MotorMoveEn = 1;
        jawCtrlByAxes[axesType].MotorDir = 1;
    }
    else if (dutyCycle < 0)
    {
        jawCtrlByAxes[axesType].MotorMoveEn = 1;
        jawCtrlByAxes[axesType].MotorDir = 0;
    }
    else
    {
        jawCtrlByAxes[axesType].MotorMoveEn = 0;
    }

    double absDutyCycle = fabs(dutyCycle), pulseLength = 0;

    if ((absDutyCycle < 3) && (dutyCycle != 0)) absDutyCycle = 3;
    else if (absDutyCycle > 100)  absDutyCycle = 100;// Assuming duty cycle is in percentage
   // printf("duty %d %lf\r\n",htim3.Init.Period, absDutyCycle);
  //  startPWMOutput(axesType);
    if(axesType == X){
      //  printf("duty %d %lf\r\n",htim3.Init.Period, absDutyCycle);
        pulseLength = (double)((htim3.Init.Period + 1) * absDutyCycle) / 100;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint16_t) pulseLength);
    }
    else if(axesType == Y) {
        pulseLength = (double)((htim15.Init.Period + 1) * absDutyCycle) / 100;
        __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, (uint16_t) pulseLength);
    }
   // printf("pulseLength = %lf %d\r\n",pulseLength, (uint16_t) pulseLength);
//    printf("%ld,%d,%f,%f,%d\r\n", encoder_val, _encoderYZ, MotorSpeed, pid_output, tmp);
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
    if (htim->Instance == TIM15)
    {
        jawCtrlByAxes[Y].EnableTriggernFault = 0;
      //  printf("44444444444444444\r\n");
       SetMotorYIO(0,0);
    }
    if (htim->Instance == TIM3)
    {
        jawCtrlByAxes[X].EnableTriggernFault = 0;
       // printf("11111111111111\r\n");
        SetMotorXIO(0, 0);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
//   if (htim->Instance == TIM4) {
//     HAL_IncTick();
//   }
  /* USER CODE BEGIN Callback 1 */
    if (htim->Instance == TIM15)
    {
        // printf("33333333\r\n");

        SetMotorYIO(jawCtrlByAxes[Y].MotorDir,jawCtrlByAxes[Y].MotorMoveEn);
         jawCtrlByAxes[Y].EnableTriggernFault = 1;
    }
    else if (htim->Instance == TIM3)
    {
       // printf("2222222222222222\r\n");
        SetMotorXIO(jawCtrlByAxes[X].MotorDir,jawCtrlByAxes[X].MotorMoveEn);
        jawCtrlByAxes[X].EnableTriggernFault = 1;
    }
  /* USER CODE END Callback 1 */
}

void yjaw_nfault_callback(void)
{
    if(1 == jawCtrlByAxes[Y].EnableTriggernFault)
    {
        if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == GPIO_PIN_RESET){
     // printf("1\r\n");
            SetMotorYIO(0, 0);
        }
      else{
        // printf("0\r\n");
           SetMotorYIO(jawCtrlByAxes[Y].MotorDir, jawCtrlByAxes[Y].MotorMoveEn);
       }
    }
}

void xjaw_nfault_callback(void)
{
    if(1 == jawCtrlByAxes[X].EnableTriggernFault){
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET){
            SetMotorXIO(0, 0);
        } else {
            SetMotorXIO(jawCtrlByAxes[X].MotorDir, jawCtrlByAxes[X].MotorMoveEn);
        }
    }
}
