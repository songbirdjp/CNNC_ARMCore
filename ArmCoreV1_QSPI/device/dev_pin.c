/**
 * @file dev_pin.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "dev_pin.h"

TAG("dev_pin.c");

static device_err_t device_pin_open(device_t *const self)
{
    dev_assert(self != NULL);

    device_pin_t *device = (device_pin_t *)self;

    dev_assert(device->device_pin_ops->open != NULL);

    return device->device_pin_ops->open(device);
}
static device_err_t device_pin_close(device_t *const self)
{
    dev_assert(self != NULL);

    device_pin_t *device = (device_pin_t *)self;

    dev_assert(device->device_pin_ops->close != NULL);

    return device->device_pin_ops->close(device);
}
static device_err_t device_pin_read(device_t *const self,
                                    void *const buffer,
                                    uint32_t size,
                                    uint32_t timeout)
{
    dev_assert(self != NULL);

    device_pin_t *device = (device_pin_t *)self;

    dev_assert(device->device_pin_ops->read != NULL);

    (void)size;
    device_err_t device_err;

    device_err = device->device_pin_ops->read(device, (pin_msg_t *)buffer, timeout);
    if (device_err != DEV_EOK)
    {
        return device_err;
    }
    device->pin_state = *((pin_msg_t *)buffer);

    return DEV_EOK;
}
static device_err_t device_pin_write(device_t *const self,
                                     void const *const buffer,
                                     uint32_t size,
                                     uint32_t timeout)
{
    dev_assert(self != NULL);

    device_pin_t *device = (device_pin_t *)self;

    dev_assert(device->device_pin_ops->write != NULL);

    (void)size;
    device_err_t device_err;

    if (device->pin_state == *((pin_msg_t *)buffer))
    {
        return DEV_EOK;
    }

    device_err = device->device_pin_ops->write(device, (pin_msg_t *)buffer, timeout);
    if (device_err != DEV_EOK)
    {
        return device_err;
    }
    device->pin_state = *((pin_msg_t *)buffer);

    return DEV_EOK;
}
static device_err_t device_pin_ioctl(device_t *const self,
                                     uint8_t cmd,
                                     void *const arg)
{
    dev_assert(self != NULL);

    device_pin_t *device = (device_pin_t *)self;

    switch (cmd)
    {
    case PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_HIGH:
    case PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_MIDDLE:
    case PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_LOW:
    case PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_HIGH:
    case PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_MIDDLE:
    case PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_LOW:
    {
        device->irq_handle_arg[cmd - PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_HIGH].callback = ((pin_cmd_arg_t *)arg)->callback;
        device->irq_handle_arg[cmd - PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_HIGH].arg = ((pin_cmd_arg_t *)arg)->arg;
        break;
    }
    default:
    {
        dev_assert(device->device_pin_ops->ioctl != NULL);

        device_err_t device_err;

        device_err = device->device_pin_ops->ioctl(device, (pin_cmd_t)cmd, arg);
        if (device_err != DEV_EOK)
        {
            return device_err;
        }
        break;
    }
    }
    return DEV_EOK;
}
void device_pin_register(device_pin_t *const self,
                         char const *name,
                         device_pin_ops_t *const device_pin_ops,
                         void *const user_data)
{
    dev_assert(self != NULL);
    dev_assert(name != NULL);
    dev_assert(user_data != NULL);
    dev_assert((device_pin_t *)device_find(name) == NULL);

    memset(self, 0, sizeof(device_pin_t));

    device_attr_t attr = {
        .device_sole = DEVICE_NON_UNIQUENESS,
        .device_type = DEVICE_PIN,
        .device_duplex = DEVICE_FULL_DUPLEX,
        .name = name};

    self->pin_state = PIN_STATE_NONE;

    static device_ops_t device_ops = {
        .open = device_pin_open,
        .close = device_pin_close,
        .read = device_pin_read,
        .write = device_pin_write,
        .ioctl = device_pin_ioctl};

    self->device_pin_ops = device_pin_ops;

    device_register((device_t *)self, &attr, &device_ops, user_data);
}

void device_pin_rising_irq_handel(device_pin_t *const self)
{
    dev_assert(self != NULL);
    for (uint8_t i = 0; i < (CALLBACK_TIRE_LIMIT / 2); i++)
    {
        if (self->irq_handle_arg[i].callback != NULL)
        {
            self->irq_handle_arg[i].callback(self->irq_handle_arg[i].arg);
        }
    }
}
void device_pin_falling_irq_handel(device_pin_t *const self)
{
    dev_assert(self != NULL);

    for (uint8_t i = (CALLBACK_TIRE_LIMIT / 2); i < CALLBACK_TIRE_LIMIT; i++)
    {
        if (self->irq_handle_arg[i].callback != NULL)
        {
            self->irq_handle_arg[i].callback(self->irq_handle_arg[i].arg);
        }
    }
}
