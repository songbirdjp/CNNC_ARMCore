/**
 * @file dev_i2c.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "dev_i2c.h"

TAG("dev_i2c.c");

#define I2C_SUCCEED_EVENT (1 << 0)
/**
 * @brief 
 * 
 * @param self 
 * @return device_err_t 
 */
static device_err_t device_i2c_open(device_t *const self)
{
    dev_assert(self != NULL);

    device_i2c_t *device = (device_i2c_t *)self;

    dev_assert(device->device_i2c_ops->open != NULL);

    return device->device_i2c_ops->open(device);
}
/**
 * @brief 
 * 
 * @param self 
 * @return device_err_t 
 */
static device_err_t device_i2c_close(device_t *const self)
{
    dev_assert(self != NULL);

    device_i2c_t *device = (device_i2c_t *)self;

    dev_assert(device->device_i2c_ops->close != NULL);

    return device->device_i2c_ops->close(device);
}
/**
 * @brief 
 * 
 * @param self 
 * @param buffer 
 * @param size 
 * @param timeout 
 * @return device_err_t 
 */
static device_err_t device_i2c_read(device_t *const self,
                                    void *const buffer,
                                    uint32_t size,
                                    uint32_t timeout)
{
    dev_assert(self != NULL);

    device_i2c_t *device = (device_i2c_t *)self;

    dev_assert(device->device_i2c_ops->read != NULL);

    (void)size;
    device_err_t device_err;

    device_err = device->device_i2c_ops->read(device, (i2c_msg_t *)buffer, timeout);
    if (device_err != DEV_EOK)
    {
        return device_err;
    }
    if ((device->i2c_mode == I2C_MODE_HARDWARE_INTERRUPT) ||
        (device->i2c_mode == I2C_MODE_HARDWARE_DMA))
    {
        if (I2C_SUCCEED_EVENT != osEventFlagsWait(device->rxtx_event,
                                                  I2C_SUCCEED_EVENT,
                                                  osFlagsWaitAny,
                                                  timeout))
        {
            return DEV_ETIMEOUT;
        }
    }

    return DEV_EOK;
}
/**
 * @brief 
 * 
 * @param self 
 * @param buffer 
 * @param size 
 * @param timeout 
 * @return device_err_t 
 */
static device_err_t device_i2c_write(device_t *const self,
                                     void const *const buffer,
                                     uint32_t size,
                                     uint32_t timeout)
{
    dev_assert(self != NULL);

    device_i2c_t *device = (device_i2c_t *)self;

    dev_assert(device->device_i2c_ops->write != NULL);

    (void)size;
    device_err_t device_err;

    device_err = device->device_i2c_ops->write(device, (i2c_msg_t *)buffer, timeout);
    if (device_err != DEV_EOK)
    {
        return device_err;
    }

    if ((device->i2c_mode == I2C_MODE_HARDWARE_INTERRUPT) ||
        (device->i2c_mode == I2C_MODE_HARDWARE_DMA))
    {
        if (I2C_SUCCEED_EVENT != osEventFlagsWait(device->rxtx_event,
                                                  I2C_SUCCEED_EVENT,
                                                  osFlagsWaitAny,
                                                  timeout))
        {
            return DEV_ETIMEOUT;
        }
    }
    return DEV_EOK;
}
/**
 * @brief 
 * 
 * @param self 
 * @param cmd 
 * @param arg 
 * @return device_err_t 
 */
static device_err_t device_i2c_ioctl(device_t *const self, uint8_t cmd, void *const arg)
{
    dev_assert(self != NULL);

    device_i2c_t *device = (device_i2c_t *)self;

    dev_assert(device->device_i2c_ops->ioctl != NULL);

    device_err_t device_err;

    device_err = device->device_i2c_ops->ioctl(device, (i2c_cmd_t)cmd, arg);
    if (device_err != DEV_EOK)
    {
        return device_err;
    }
    return DEV_EOK;
}
/**
 * @brief 
 * 
 * @param self 
 * @param name 
 * @param device_i2c_ops 
 * @param user_data 
 */
void device_i2c_register(device_i2c_t *const self,
                             char const *name,
                             device_i2c_ops_t *const device_i2c_ops,
                             void *const user_data)
{
    dev_assert(self != NULL);
    dev_assert(name != NULL);
    dev_assert(user_data != NULL);
    dev_assert((device_i2c_t *)device_find(name) == NULL);

    char *event_name = "i2c_event_";

    char *result = pvPortMalloc(strlen(event_name) + strlen(name) + 1);
    strcpy(result, event_name);
    strcat(result, name);

    device_attr_t attr = {
        .device_sole = DEVICE_NON_UNIQUENESS,
        .device_type = DEVICE_I2C,
        .device_duplex = DEVICE_HALF_DUPLEX,
        .name = name};

    self->i2c_addr_len = I2C_ADDR_7BIT;
    self->i2c_mode = I2C_MODE_HARDWARE_DMA;
    self->i2c_type = I2C_TYPE_MEN;

    static device_ops_t device_ops = {
        .open = device_i2c_open,
        .close = device_i2c_close,
        .read = device_i2c_read,
        .write = device_i2c_write,
        .ioctl = device_i2c_ioctl};

    const osEventFlagsAttr_t i2c_event_attributes = {
        .name = result};
    self->rxtx_event = osEventFlagsNew(&i2c_event_attributes);
    dev_assert(self->rxtx_event != NULL);

    self->device_i2c_ops = device_i2c_ops;

    device_register((device_t *)self, &attr, &device_ops, user_data);
}

void device_i2c_xfer_end(device_i2c_t *const self)
{
    dev_assert(self != NULL);
    osEventFlagsSet(self->rxtx_event, I2C_SUCCEED_EVENT);
}
