/**
 * @file drv_tca9535.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "drv_tca9535.h"
#include "dev_i2c.h"
#include "init_call.h"
TAG("drv_tca9535.c");

typedef struct driver_tca9535
{
    device_t super;

    device_i2c_t *device_i2c;

    i2c_addr_len_t tca9535_i2c_addr_len;
    i2c_mode_t tca9535_i2c_mode_t;
    i2c_type_t tca9535_i2c_type;

    uint8_t device_addr;
} driver_tca9535_t;

static driver_tca9535_t driver_tca9535_0 = {0};

static device_err_t driver_tca9535_open(device_t *const self)
{
    dev_assert(self != NULL);

    driver_tca9535_t *device = (driver_tca9535_t *)self;

    dev_assert(device->device_i2c != NULL);

    return device_open((device_t *)(device->device_i2c));
}
static device_err_t driver_tca9535_close(device_t *const self)
{
    dev_assert(self != NULL);

    driver_tca9535_t *device = (driver_tca9535_t *)self;

    dev_assert(device->device_i2c != NULL);

    return device_close((device_t *)(device->device_i2c));
}

static device_err_t driver_tca9535_read(device_t *const self,
                                        void *const buffer,
                                        uint32_t size,
                                        uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buffer != NULL);

    driver_tca9535_t *device = (driver_tca9535_t *)self;

    dev_assert(device->device_i2c != NULL);

    device_err_t device_err;

    tca9535_msg_t *tca9535_msg = (tca9535_msg_t *)buffer;
    i2c_msg_t i2c_msg = {
        .dev_addr = device->device_addr,
        .reg_addr = tca9535_msg->driver_tca9535_reg,
        .data = tca9535_msg->data,
        .dataLen = tca9535_msg->dataLen};

    if (device->tca9535_i2c_addr_len != device->device_i2c->i2c_addr_len)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_ADDR_LEN,
                                  &(device->tca9535_i2c_addr_len));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    if (device->tca9535_i2c_mode_t != device->device_i2c->i2c_mode)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_MODE,
                                  &(device->tca9535_i2c_mode_t));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    if (device->tca9535_i2c_type != device->device_i2c->i2c_type)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_TYPE,
                                  &(device->tca9535_i2c_type));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    return device_read((device_t *)(device->device_i2c), (void *)&i2c_msg, size, timeout);
}
static device_err_t driver_tca9535_write(device_t *const self,
                                         void const *const buffer,
                                         uint32_t size,
                                         uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buffer != NULL);

    driver_tca9535_t *device = (driver_tca9535_t *)self;

    dev_assert(device->device_i2c != NULL);

    device_err_t device_err;

    tca9535_msg_t *tca9535_msg = (tca9535_msg_t *)buffer;
    i2c_msg_t i2c_msg = {
        .dev_addr = device->device_addr,
        .reg_addr = tca9535_msg->driver_tca9535_reg,
        .data = tca9535_msg->data,
        .dataLen = tca9535_msg->dataLen};

    if (device->tca9535_i2c_addr_len != device->device_i2c->i2c_addr_len)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_ADDR_LEN,
                                  &(device->tca9535_i2c_addr_len));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    if (device->tca9535_i2c_mode_t != device->device_i2c->i2c_mode)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_MODE,
                                  &(device->tca9535_i2c_mode_t));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    if (device->tca9535_i2c_type != device->device_i2c->i2c_type)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_TYPE,
                                  &(device->tca9535_i2c_type));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    return device_write((device_t *)(device->device_i2c), (void *)&i2c_msg, size, timeout);
}
static device_err_t driver_tca9535_ioctl(device_t *const self, uint8_t cmd, void *const arg)
{
    dev_assert(self != NULL);

    driver_tca9535_t *device = (driver_tca9535_t *)self;

    device_err_t device_err = DEV_EINVAL;

    if (cmd > DRIVER_TCA9535_CMD_MIN)
    {
        switch (cmd)
        {
        case DRIVER_TCA9535_CMD_POLARITY_INVERSION_PORT:
        {

            tca9535_msg_t tca9535_msg = {
                .driver_tca9535_reg = DRIVER_TCA9535_REG_POLARITY_INVERSION_PORT_0,
                .dataLen = 2,
                .data = (uint8_t *)arg};

            device_err = driver_tca9535_write((device_t *)device, &tca9535_msg, 0, 1000);
            break;
        }
        case DRIVER_TCA9535_CMD_CONFIGURATION_PORT:
        {

            tca9535_msg_t tca9535_msg = {
                .driver_tca9535_reg = DRIVER_TCA9535_REG_CONFIGURATION_PORT_0,
                .dataLen = 2,
                .data = (uint8_t *)arg};

            device_err = driver_tca9535_write((device_t *)device, &tca9535_msg, 0, 1000);
            break;
        }
        case DRIVER_TCA9535_CMD_GET_ACK_POLLING:
        {
            i2c_ack_polling_arg_t i2c_ack_polling_arg = {
                .device_addr = device->device_addr,
                .i2c_ack_polling_state = I2C_ACK_POLLING_BUSY};

            device_err = device_ioctl((device_t *)(device->device_i2c), I2C_CMD_GET_ACK_POLLING, &i2c_ack_polling_arg);

            *(i2c_ack_polling_state_t *)arg = i2c_ack_polling_arg.i2c_ack_polling_state;
            break;
        }
        default:
            device_err = DEV_EINVAL;
            break;
        }
    }
    else
    {
        dev_assert(device->device_i2c != NULL);
        return device_ioctl((device_t *)(device->device_i2c), cmd, arg);
    }

    return device_err;
}
static void driver_tca9535_register(driver_tca9535_t *const self,
                                    char const *name,
                                    char const *i2c_name,
                                    uint8_t device_addr)
{
    dev_assert(self != NULL);

    dev_assert((driver_tca9535_t *)device_find(name) == NULL);
    dev_assert((device_i2c_t *)device_find(i2c_name) != NULL);

    memset(self, 0, sizeof(driver_tca9535_t));

    self->device_i2c = (device_i2c_t *)device_find(i2c_name);

    device_attr_t attr = {
        .device_sole = DEVICE_NON_UNIQUENESS,
        .device_type = DEVICE_I2C_MEM,
        .device_duplex = DEVICE_HALF_DUPLEX,
        .name = name};

    self->tca9535_i2c_addr_len = I2C_ADDR_7BIT;
    self->tca9535_i2c_mode_t = I2C_MODE_HARDWARE_DMA;
    self->tca9535_i2c_type = I2C_TYPE_MEN;

    self->device_addr = device_addr;

    static device_ops_t device_tca9535_ops = {
        .open = driver_tca9535_open,
        .close = driver_tca9535_close,
        .read = driver_tca9535_read,
        .write = driver_tca9535_write,
        .ioctl = driver_tca9535_ioctl};

    device_register((device_t *)self, &attr, &device_tca9535_ops, self);
}
void driver_tca9535_init(void)
{
#define TCA9535_BOARD_ID_ADDR (0x20 << 1)
    driver_tca9535_register(&driver_tca9535_0,
                            DEVICE_NAME_TCA9535_0,
                            DEVICE_NAME_I2C1,
                            TCA9535_BOARD_ID_ADDR);
}
// INIT_DEVICE_EXPORT(driver_tca9535_init);
