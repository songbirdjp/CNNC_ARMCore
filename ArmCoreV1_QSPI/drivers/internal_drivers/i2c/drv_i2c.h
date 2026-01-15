#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include "stm32h7xx_hal.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME_LENGTH      16

#define DEVICE_NAME_I2C1       "i2c1"
#define DEVICE_NAME_I2C2       "i2c2"
#define DEVICE_NAME_I2C3       "i2c3"
#define DEVICE_NAME_I2C4       "i2c4"
#define DEVICE_NAME_I2C5       "i2c5"


struct drv_i2c
{
    I2C_HandleTypeDef hi2c;
    uint8_t name[DEVICE_NAME_LENGTH];
    uint8_t open_state;
    osMutexId_t mutex;
    osEventFlagsId_t event;

    int8_t (*open)(struct drv_i2c *i2c);
    int8_t (*close)(struct drv_i2c *i2c);
    int8_t (*write)(struct drv_i2c *i2c, uint16_t addr, uint16_t reg_addr, uint8_t *buf, uint16_t size, uint32_t timeout);
    int8_t (*read)(struct drv_i2c *i2c, uint16_t addr, uint16_t reg_addr, uint8_t *buf, uint16_t size, uint32_t timeout);
    int8_t (*ioctl)(struct drv_i2c *i2c, uint8_t cmd, void *arg);
};

typedef struct drv_i2c DEVICE_I2C;

int8_t i2c_init(DEVICE_I2C *i2c, uint8_t *device_name);


#endif /* __DRV_I2C_H__ */