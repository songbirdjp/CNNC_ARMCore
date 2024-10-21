#include "drv_tim.h"
#include "cmsis_os.h"
#include "tim.h"
#include "init_call.h"

void delay_us(uint32_t _timeToDelay)
{
    uint32_t tim5CounterStart = 0;
    uint32_t tim5CounterEnd;
    __HAL_TIM_SetCounter(&htim5,0);
    while ((__HAL_TIM_GET_COUNTER(&htim5) - tim5CounterStart) < _timeToDelay);
    tim5CounterEnd = __HAL_TIM_GET_COUNTER(&htim5);
    //printf("tim5CounterEnd = %ld\r\n",tim5CounterEnd);
}

static int8_t delay_us_init(void)
{
    MX_TIM5_Init();
    HAL_TIM_Base_Start(&htim5);
}
INIT_DEVICE_EXPORT(delay_us_init);