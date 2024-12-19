#ifndef __DRV_FLASH_H__
#define __DRV_FLASH_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define OS_FREERTOS     1
#ifdef OS_FREERTOS
#include "cmsis_os2.h"
#endif

#define DEVICE_NAME_LENGTH          16

enum
{
    FLASH_CMD_ERASE_SECTOR = 0,
};

struct drv_flash
{
    uint8_t name[DEVICE_NAME_LENGTH];
    uint32_t addr_base;
    uint32_t size;
    uint8_t open_state;

#ifdef OS_FREERTOS
    osMutexId_t rw_mutex;
    osEventFlagsId_t operation_event;
#endif

    int8_t (*open)(struct drv_flash *flash);
    int8_t (*close)(struct drv_flash *flash);
    int8_t (*write)(struct drv_flash *flash, uint32_t offset, uint8_t *buf, uint32_t size, uint32_t timeout);
    int8_t (*read)(struct drv_flash *flash, uint32_t offset, uint8_t *buf, uint32_t size, uint32_t timeout);
    int8_t (*ioctl)(struct drv_flash *flash, uint8_t cmd, void *arg);
};

typedef struct drv_flash DEVICE_FLASH;


int8_t flash_init(DEVICE_FLASH *flash, uint8_t *device_name);
int8_t flash_operation_address_set(DEVICE_FLASH *flash, uint32_t addr_base, uint32_t size);


#ifdef __cplusplus
}
#endif

#endif /* __DRV_FLASH_H__ */
