#ifndef __I2C_PORT_H__
#define __I2C_PORT_H__

#include <stdint.h>
#include "drv_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C4_BUS_NAME_DEFAULT    "i2c4"

int8_t i2c_device_write(uint8_t *bus_name, uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len);
int8_t i2c_device_read(uint8_t *bus_name, uint16_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_PORT_H__ */