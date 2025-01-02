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
#include "AFCapp.h"
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

int8_t WriteArrayToFlash(uint16_t *data, uint32_t len)
{
    int8_t ret = 0;
    DEVICE_FLASH *flash = device_flash_get();
    uint32_t flash_AFC_Offset = 0;

    ret = flash->write(flash, flash_AFC_Offset, data, len * sizeof(uint16_t), 1000);
    flash_AFC_Offset += 16 * sizeof(uint16_t);
    if (ret != 0)
    {
        printf("flash write err:%d\r\n", ret);
        return -1;
    }

    return 0;
}
AFCApplicationParam_t AFCApplicationParam = {   .whichData = 1,
                                                .positionDeadzone = 15,
                                                .A1In_Para = 1,
                                                .A2In_Para = 1,
                                                .B1In_Para = 0,
                                                .B2In_Para = 0,
                                                .positionStep = 10};
uint16_t *AFCApplicationParamGet(void)
{
    return &AFCApplicationParam;
}
void MagMotorCtrlbyADC(uint16_t *data)
{
    AFCApplicationParam_t *obj = AFCApplicationParamGet();
    obj->positionCurrent = __HAL_TIM_GET_COUNTER(&htim2); 
    uint16_t dataADC1[8], dataADC2[8] = {0};
    memcpy(dataADC1, data, 8 * sizeof(uint16_t));
    memcpy(dataADC2, data + 8, 8 * sizeof(uint16_t));
    uint16_t phaseA = obj->A1In_Para * dataADC1[obj->whichData] + obj->B1In_Para;
    uint16_t phaseB = obj->A2In_Para * dataADC2[obj->whichData] + obj->B2In_Para;
    // printf("phaseA = %d\r\n",phaseA);
    // printf("phaseB = %d\r\n",phaseB);
    // printf("phaseA - phaseB = %d\r\n",phaseA - phaseB);
    if(phaseA > phaseB + obj->positionDeadzone)
    {
        obj->positionCalculated = obj->positionCurrent - obj->positionStep;
    }
    else if(phaseA < phaseB - obj->positionDeadzone)
    {
        obj->positionCalculated = obj->positionCurrent + obj->positionStep;
    }
    else
    {
        obj->positionCalculated = obj->positionCurrent;
    }
    LOG_I("posCalculated = %d\r\n\r\n",obj->positionCalculated);
    LOG_I("posCurrent = %d\r\n\r\n",obj->positionCurrent);
}

static void Mag_MotorCtrl_thread_entry(void *argument)
{
    MX_TIM1_Init();
    MX_TIM4_Init();
    MX_TIM8_Init();
    //MX_TIM24_Init();
    MX_TIM23_Init();
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_Base_Start(&htim4);
    HAL_TIM_Base_Start(&htim8);
    HAL_TIM_Base_Start(&htim23);
   // HAL_TIM_Base_Start(&htim24);

    // flash = device_flash_get();
    // uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE}; 
    // flash_init(flash, "DEVICE_NAME_FLASH_BANK1");
    // flash_operation_address_set(flash, flash_cfg[0], flash_cfg[1]);
    // flash->ioctl(flash, FLASH_CMD_ERASE_SECTOR, (void *)flash_cfg);
    uint16_t data[16] = {0};
    for (;;)
    {   
        // AFC_ADCSampleRecvProcess();
        MagMotorCtrlbyADC(AFC_ADCSampleRecvProcess());
    }
}
static void AFC_DataTransmit_thread_entry(void *argument)
{
    for (;;)
    {
        osDelay(1000);
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
INIT_APP_EXPORT(Mag_MotorCtrl_thread_init);
INIT_APP_EXPORT(AFC_DataTransmit_thread_init);