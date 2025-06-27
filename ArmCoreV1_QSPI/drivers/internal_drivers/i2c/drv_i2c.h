/**
 * @file drv_i2c.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _DRV_I2C_H_
#define _DRV_I2C_H_

#include "dev_i2c.h"
#ifdef __cplusplus
extern "C" {
#endif
#define DEVICE_NAME_I2C1 "i2c1"
#define DEVICE_NAME_I2C2 "i2c2"
#define DEVICE_NAME_I2C3 "i2c3"
#define DEVICE_NAME_I2C4 "i2c4"

void driver_i2c_init(void);

int8_t i2c_test(void);

#ifdef __cplusplus
}
#endif

#endif
