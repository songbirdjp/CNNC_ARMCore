/**
 * @file drv_tca9535.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef _DRV_TCA9535_I2C_H_
#define _DRV_TCA9535_I2C_H_

#include "dev_i2c.h"
#include "drv_i2c.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define DEVICE_NAME_TCA9535_0 "tca9535.0"

    typedef enum driver_tca9535_reg
    {
        DRIVER_TCA9535_REG_INPUT_PORT_0 = 0x00,              /**< Input Port 0,Read byte*/
        DRIVER_TCA9535_REG_INPUT_PORT_1 = 0x01,              /**< Input Port 1,Read byte*/
        DRIVER_TCA9535_REG_OUTPUT_PORT_0 = 0x02,             /**< Output Port 0,Read-write byte*/
        DRIVER_TCA9535_REG_OUTPUT_PORT_1 = 0x03,             /**< Output Port 1,Read-write byte*/
        DRIVER_TCA9535_REG_POLARITY_INVERSION_PORT_0 = 0x04, /**< Polarity Inversion Port 0,Read-write byte*/
        DRIVER_TCA9535_REG_POLARITY_INVERSION_PORT_1 = 0x05, /**< Polarity Inversion Port 1,Read-write byte*/
        DRIVER_TCA9535_REG_CONFIGURATION_PORT_0 = 0x06,      /**< Configuration Port 0,0 output, 1 intput,Read-write byte*/
        DRIVER_TCA9535_REG_CONFIGURATION_PORT_1 = 0x07,      /**< Configuration Port 1,0 output, 1 intput,Read-write byte*/
    } driver_tca9535_reg_t;

    typedef struct tca9535_msg
    {
        driver_tca9535_reg_t driver_tca9535_reg;
        uint8_t *data;
        uint16_t dataLen;
    } tca9535_msg_t;

    typedef enum tca9535_ack_polling_state
    {
        TCA9535_ACK_POLLING_READY = 0,
        TCA9535_ACK_POLLING_BUSY,
    } tca9535_ack_polling_state_t;
    typedef enum driver_tca9535_cmd
    {
        DRIVER_TCA9535_CMD_MIN = (I2C_CMD_MAX + 1),

        DRIVER_TCA9535_CMD_POLARITY_INVERSION_PORT,
        DRIVER_TCA9535_CMD_CONFIGURATION_PORT,
        DRIVER_TCA9535_CMD_GET_ACK_POLLING,

        DRIVER_TCA9535_CMD_MAX
    } driver_tca9535_cmd_t;

void driver_tca9535_init(void);
#ifdef __cplusplus
}
#endif

#endif
