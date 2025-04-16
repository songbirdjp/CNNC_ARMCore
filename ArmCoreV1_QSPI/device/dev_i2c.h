/**
 * @file dev_i2c.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DEV_I2C_H_
#define DEV_I2C_H_

#include "dev_base.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*************************************ioctl cmd*******************************/
    typedef enum i2c_cmd
    {
        I2C_CMD_MIN = 0,

        I2C_CMD_SET_ADDR_LEN,
        I2C_CMD_SET_MODE,
        I2C_CMD_SET_TYPE,

        I2C_CMD_GET_ACK_POLLING,

        I2C_CMD_INIT,

        I2C_CMD_MAX
    } i2c_cmd_t;

    typedef enum i2c_ack_polling_state
    {
        I2C_ACK_POLLING_READY = 0,
        I2C_ACK_POLLING_BUSY,
    } i2c_ack_polling_state_t;

    typedef struct i2c_ack_polling_arg
    {
        uint16_t device_addr;
        i2c_ack_polling_state_t i2c_ack_polling_state;
    } i2c_ack_polling_arg_t;

    typedef enum i2c_addr_len
    {
        I2C_ADDR_7BIT = 0,
        I2C_ADDR_10BIT,
    } i2c_addr_len_t;

    typedef enum i2c_mode
    {
        I2C_MODE_HARDWARE_POLLING = 0,
        I2C_MODE_HARDWARE_INTERRUPT,
        I2C_MODE_HARDWARE_DMA,
        I2C_MODE_SOFTWARE,
    } i2c_mode_t;

    typedef enum i2c_type
    {
        I2C_TYPE_MEN = 0,
        I2C_TYPE_BUS,
    } i2c_type_t;

    typedef struct i2c_msg
    {
        uint16_t dev_addr;
        uint16_t reg_addr;
        uint8_t *data;
        uint16_t dataLen;
    } i2c_msg_t;

    typedef struct device_i2c
    {
        device_t super;

        i2c_addr_len_t i2c_addr_len;
        i2c_mode_t i2c_mode;
        i2c_type_t i2c_type;

        osEventFlagsId_t rxtx_event;

        struct device_i2c_ops *device_i2c_ops;
    } device_i2c_t;

    typedef struct device_i2c_ops
    {
        device_err_t (*open)(device_i2c_t *const self);
        device_err_t (*close)(device_i2c_t *const self);
        device_err_t (*read)(device_i2c_t *const self, i2c_msg_t *const buf, uint32_t timeout);
        device_err_t (*write)(device_i2c_t *const self, i2c_msg_t const *const buf, uint32_t timeout);
        device_err_t (*ioctl)(device_i2c_t *const self, i2c_cmd_t cmd, void *const arg);
    } device_i2c_ops_t;
    /*************************************************low level****************/
    void device_i2c_register(device_i2c_t *const self,
                             char const *name,
                             device_i2c_ops_t *const device_i2c_ops,
                             void *const user_data);

    void device_i2c_xfer_end(device_i2c_t *const self);

#ifdef __cplusplus
}
#endif

#endif /* DEV_I2C_H_ */
