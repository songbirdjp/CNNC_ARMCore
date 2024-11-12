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
static DEVICE_FLASH flash_bank1 = {0};
void bgm_trig_callback(void)
{
    HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_4);
}
static DEVICE_FLASH *device_flash_get(void)
{
    return &flash_bank1;
}
DEVICE_FLASH *flash = NULL;//device_flash_get();

static void AFC_thread_entry(void *argument)
{
    gpio_pin_irq_callback_register("GPIOG_2",bgm_trig_callback);
    int32_t len;  
    uint32_t flash_addr = FLASH_ADDRESS_BASE;
    uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE};
    // flash = device_flash_get();
    // flash_init(flash, "DEVICE_NAME_FLASH_BANK1");
    // flash_operation_address_set(flash, flash_cfg[0], flash_cfg[1]);
    // flash->ioctl(flash, FLASH_CMD_ERASE_SECTOR, (void *)flash_cfg);
    for (;;)
    {
        // adcs7476_sample_data_recv_process();
        osDelay(1000);
    }
}

static int8_t AFC_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "AFC_thread",
    .stack_size = 2048 * 4,
    .priority = osPriorityNormal,
    };

    osThreadId_t thread_id = osThreadNew(AFC_thread_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread AFC create failed\r\n");
        return -1;
    }
    return 0;
}
// INIT_APP_EXPORT(AFC_thread_init);