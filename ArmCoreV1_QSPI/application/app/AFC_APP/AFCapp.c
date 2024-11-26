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
int8_t Shell_ReadFlash(uint8_t argc, char *argv[])
{
    int8_t ret = 0;
    DEVICE_FLASH *flash = device_flash_get();
    uint8_t data[1024] = {0};
    uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE};
    ret = flash->read(flash, 0, data, sizeof(data), 1000);
    if (ret != 0)
    {
        printf("flash read err:%d\r\n", ret);
        return -3;
    }

    for (uint32_t i = 0; i < 256; i += 16) 
    {
        LOG_E("0x%08x: ", FLASH_ADDRESS_BASE + i);
        for (uint32_t j = 0; j < 32; j++)
        {
            LOG_E("%02x ", data[i + j]);
        }
        LOG_E("\r\n");
    }
    return 0;
}
MSH_CMD_EXPORT_ALIAS(Shell_ReadFlash, ReadFlash, flash test);
DEVICE_FLASH *flash;
static void Mag_MotorCtrl_thread_entry(void *argument)
{
    MX_TIM1_Init();
    MX_TIM4_Init();
    MX_TIM8_Init();
    MX_TIM23_Init();
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    __HAL_TIM_SET_AUTORELOAD(&htim4, 9);
    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_Base_Start(&htim4);
    HAL_TIM_Base_Start(&htim8);
    HAL_TIM_Base_Start(&htim23);
    int8_t ret = 0;
    flash = device_flash_get();
    uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE}; 
    flash_init(flash, "DEVICE_NAME_FLASH_BANK1");
    flash_operation_address_set(flash, flash_cfg[0], flash_cfg[1]);
    flash->ioctl(flash, FLASH_CMD_ERASE_SECTOR, (void *)flash_cfg);
    for (;;)
    {   
        AFC_ADCSampleRecvProcess();
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