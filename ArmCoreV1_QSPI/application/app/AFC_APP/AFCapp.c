#include "adcs7476.h"
#include "drv_spi.h"
#include "init_call.h"
#include "ulog.h"
#include "drv_flash.h"
#include "flash_port.h"
#include <stdbool.h>
#include "stm32h7xx_hal.h"
#include "drv_gpio.h"
#include "lptim.h"
#include "tim.h"
#include "shell.h"
#define FLASH_ADDRESS_BASE  (FLASH_BASE + FLASH_SECTOR_SIZE * 6)//0x08000000UL + 0x00020000UL* 6 = 0x080C0000UL
#define FLASH_VALID_SIZE    (FLASH_SECTOR_SIZE * 2) //0x00020000UL * 2 = 0x00040000UL

static DEVICE_FLASH flash_bank1 = {0};
static DEVICE_FLASH *device_flash_get(void)
{
    return &flash_bank1;
}
static uint8_t tim12_count = 0;
void TIM12PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM12)
    {
        tim12_count++;

        if (tim12_count >= 4)
        {
            HAL_TIM_Base_Stop(&htim12);
            __HAL_TIM_SET_COUNTER(&htim12, 0); 
            tim12_count = 0;  
        }
    }
}

// void TIM4PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//     if (htim->Instance == TIM4)
//     {
        
//         printf("TIM4 period elapsed\r\n");
//     }
// }   

void Set_TIM23_Period(uint32_t period)
{
    __HAL_TIM_SET_AUTORELOAD(&htim8, period);
}

static int shell_set_tim23_period(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: set_tim23_period <period>\n");
        return -1;
    }

    uint32_t period = atoi(argv[1]);
    Set_TIM23_Period(period);
    printf("TIM23 period set to %u\n", period);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(shell_set_tim23_period, TIM13Period, Set TIM23 period);
void Set_TIM8_Pulse(uint32_t pulse)
{
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, pulse);
}

static int shell_set_tim8_pulse(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: set_tim8_pulse <pulse>\n");
        return -1;
    }

    uint32_t pulse = atoi(argv[1]);
    Set_TIM8_Pulse(pulse);
    printf("TIM8 pulse set to %u\n", pulse);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(shell_set_tim8_pulse, TIM13Pulse, Set TIM8 pulse);



static void Mag_MotorCtrl_thread_entry(void *argument)
{
    MX_TIM1_Init();
    MX_TIM4_Init();
    MX_TIM8_Init();
    MX_TIM12_Init();
    MX_TIM23_Init();
    HAL_TIM_RegisterCallback(&htim12, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM12PeriodElapsedCallback);
    // HAL_TIM_RegisterCallback(&htim4, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM4PeriodElapsedCallback);
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    __HAL_TIM_SET_AUTORELOAD(&htim4, 9);
    __HAL_TIM_CLEAR_FLAG(&htim12, TIM_FLAG_UPDATE);
    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_Base_Start(&htim4);
    HAL_TIM_Base_Start(&htim8);
    HAL_TIM_Base_Start_IT(&htim12);
    HAL_TIM_Base_Start(&htim23);
    int8_t ret = 0;
    DEVICE_FLASH *flash = device_flash_get(); 
    uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE}; 
    flash_init(device_flash_get(), "DEVICE_NAME_FLASH_BANK1");
    flash_operation_address_set(device_flash_get(), flash_cfg[0], flash_cfg[1]);
    for (;;)
    {   
        adcs7476_sample_data_recv_process();
    }
}

static int8_t Mag_MotorCtrl_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "Mag_MotorCtrl_thread",
    .stack_size = 2048 * 4,
    .priority = osPriorityNormal,
    };

    osThreadId_t thread_id = osThreadNew(Mag_MotorCtrl_thread_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread Mag_MotorCtrl create failed\r\n");
        return -1;
    }
    return 0;
}
INIT_APP_EXPORT(Mag_MotorCtrl_thread_init);

static void AFC_DataTransmit_thread_entry(void *argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

static int8_t AFC_DataTransmit_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "AFC_DataTransmit_thread",
    .stack_size = 2048 * 4,
    .priority = osPriorityNormal,
    };

    osThreadId_t thread_id = osThreadNew(AFC_DataTransmit_thread_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread AFC_DataTransmit create failed\r\n");
        return -1;
    }
    return 0;
}
INIT_APP_EXPORT(AFC_DataTransmit_thread_init);