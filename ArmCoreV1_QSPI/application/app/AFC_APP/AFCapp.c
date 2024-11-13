#include "adcs7476.h"
#include "drv_spi.h"
#include "init_call.h"
#include "ulog.h"
#include "drv_flash.h"
#include "flash_port.h"
#include <stdbool.h>
#include "stm32h7xx_hal.h"
#include "drv_gpio.h"

#define FLASH_ADDRESS_BASE  (FLASH_BASE + FLASH_SECTOR_SIZE * 6)//0x08000000UL + 0x00020000UL* 6 = 0x080C0000UL
#define FLASH_VALID_SIZE    (FLASH_SECTOR_SIZE * 2) //0x00020000UL * 2 = 0x00040000UL

void bgm_trig_callback(void)
{
    HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_4);
}
static DEVICE_FLASH flash_bank1 = {0};
static DEVICE_FLASH *device_flash_get(void)
{
    return &flash_bank1;
}
static void Mag_MotorCtrl_thread_entry(void *argument)
{
    int8_t ret = 0;
    DEVICE_FLASH *flash = device_flash_get(); 
    uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE};
    gpio_pin_irq_callback_register("GPIOG_2",bgm_trig_callback);
    ret = flash_init(device_flash_get(), "DEVICE_NAME_FLASH_BANK1");
    if (ret != 0)
    {
        printf("flash init err:%d\r\n", ret);
        return -1;
    }
    ret = flash_operation_address_set(device_flash_get(), flash_cfg[0], flash_cfg[1]);
    if (ret != 0)
    {
        printf("flash operation address set err:%d\r\n", ret);
        return -2;
    }
    for (;;)
    {
        HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_5);
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
INIT_APP_EXPORT(Mag_MotorCtrl_thread_init);

static void AFC_DataTransmit_thread_entry(void *argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_6);
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