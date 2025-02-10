#ifndef __I2C_PORT_H__
#define __I2C_PORT_H__

#include <stdint.h>
#include "drv_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

int8_t i2c_bus_init(DEVICE_I2C *i2c_bus, uint8_t *bus_name);
int8_t i2c_device_write(DEVICE_I2C *i2c_bus, uint16_t dev_addr, uint8_t *data, uint16_t len);
int8_t i2c_device_read(DEVICE_I2C *i2c_bus, uint16_t dev_addr, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_PORT_H__ */