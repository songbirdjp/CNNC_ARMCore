#include "drv_flash.h"
#include "flash_port.h"
#include "stm32h7xx_hal.h"
#include "init_call.h"

#define FLASH_ADDRESS_BASE  (FLASH_BASE + FLASH_SECTOR_SIZE * 2)
#define FLASH_VALID_SIZE    (FLASH_SECTOR_SIZE * 6)

static DEVICE_FLASH flash_bank1 = {0};
static DEVICE_FLASH *device_flash_get(void)
{
    return &flash_bank1;
}

static int8_t device_flash_init(void)
{
    int8_t ret = 0;

    ret = flash_init(device_flash_get(), DEVICE_NAME_FLASH_BANK1);
    if (ret != 0)
    {
        printf("flash init err:%d\r\n", ret);
        return -1;
    }

    ret = flash_operation_address_set(device_flash_get(), FLASH_ADDRESS_BASE, FLASH_VALID_SIZE);
    if (ret != 0)
    {
        printf("flash operation address set err:%d\r\n", ret);
        return -2;
    }

    return 0;
}
// INIT_DEVICE_EXPORT(device_flash_init);


int8_t device_flash_erase(uint8_t *device_name, uint32_t offset, uint32_t size)
{
    if (device_name == NULL)
    {
        return -1;
    }

    uint32_t argv[2] = {FLASH_BASE + offset, size};

    if (strcmp(device_name, DEVICE_NAME_FLASH_BANK1) == 0)
    {
        return device_flash_get()->ioctl(device_flash_get(), FLASH_CMD_ERASE_SECTOR, argv);
    }

    return -1;
}

int8_t device_flash_read(uint8_t *device_name, uint32_t offset, uint8_t *buf, uint32_t len, uint32_t timeout)
{
    if (device_name == NULL || buf == NULL || len == 0) 
    {
        return -1;
    }

    if (strcmp(device_name, DEVICE_NAME_FLASH_BANK1) == 0)
    {
        return device_flash_get()->read(device_flash_get(), offset, buf, len, timeout);
    }

    return 0;
}

int8_t device_flash_write(uint8_t *device_name, uint32_t offset, uint8_t *buf, uint32_t len, uint32_t timeout)
{
    if (device_name == NULL || buf == NULL || len == 0)
    {
        return -1;
    }

    if (strcmp(device_name, DEVICE_NAME_FLASH_BANK1) == 0)
    {
        return device_flash_get()->write(device_flash_get(), offset, buf, len, timeout);
    }

    return 0;
}
