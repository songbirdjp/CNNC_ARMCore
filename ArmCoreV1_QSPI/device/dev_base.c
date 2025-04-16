/**
 * @file dev_base.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "dev_base.h"
#include "dev_hash.h"

TAG("dev_base.c");

// static device_t *device_list = NULL;
hash_map_t hash_map_device =
    {
        .hash_map_state = HASH_TABLE_STATE_INACTIVE,
};

static osMutexId_t mutex_dev_id = NULL;
static const osMutexAttr_t mutex_attr =
    {
        .name = "mutex_dev",
        .attr_bits = osMutexPrioInherit | osMutexRecursive,
        .cb_mem = NULL,
        .cb_size = 0,
};

void device_register(device_t *const self,
                     device_attr_t *const attr,
                     device_ops_t *const device_ops,
                     void *const user_data)
{
    dev_assert(self != NULL);
    dev_assert(attr != NULL);
    dev_assert(attr->name != NULL);
    dev_assert(device_ops != NULL);

    hash_map_state_t hash_map_state = HASH_TABLE_STATE_INACTIVE;
    device_err_t device_err = DEV_ENOTOPEN;
    osStatus_t ret = osOK;

    if (mutex_dev_id == NULL)
    {
        mutex_dev_id = osMutexNew(&mutex_attr);
        dev_assert(mutex_dev_id != NULL);
    }
    ret = osMutexAcquire(mutex_dev_id, osWaitForever);
    dev_assert(ret == osOK);

    memcpy(&self->device_attr, attr, sizeof(device_attr_t));

    self->mutex_rx = osMutexNew(&mutex_attr);
    dev_assert(self->mutex_rx != NULL);

    dev_assert((self->device_attr.device_duplex == DEVICE_FULL_DUPLEX) ||
               (self->device_attr.device_duplex == DEVICE_HALF_DUPLEX));
    if (self->device_attr.device_duplex == DEVICE_FULL_DUPLEX)
    {
        self->mutex_tx = osMutexNew(&mutex_attr);
        dev_assert(self->mutex_tx != NULL);
    }
    else
    {
        self->mutex_tx = self->mutex_rx;
    }
    self->open_state = false;
    self->open_count = 0;

    self->ops = device_ops;
    self->user_data = user_data;

    hash_map_state = hash_map_get_state(&hash_map_device);
    if (HASH_TABLE_STATE_ACTIVE != hash_map_state)
    {
        device_err = hash_map_create(&hash_map_device);
        dev_assert(device_err == DEV_EOK);
        if (DEV_EOK != device_err)
        {
            goto exit;
        }
    }

    device_err = hash_map_insert(&hash_map_device, (void *)(self->device_attr.name), self);
    dev_assert(device_err == DEV_EOK);
    if (DEV_EOK != device_err)
    {
        goto exit;
    }
exit:
    ret = osMutexRelease(mutex_dev_id);
    dev_assert(ret == osOK);
}

device_t *device_find(char const *name)
{
    dev_assert(name != NULL);

    device_err_t device_err = DEV_ENOTOPEN;
    device_t *device = NULL;

    device_err = hash_map_search(&hash_map_device, (void *)name, (void *)&device);
    if (DEV_EOK != device_err)
    {
        device = NULL;
    }

    return device;
}

device_err_t device_open(device_t *const self)
{
    dev_assert(self != NULL);

    device_err_t device_err;
    osStatus_t ret = osOK;

    if ((self->open_state == true) && (self->device_attr.device_sole == DEVICE_UNIQUENESS))
    {
        return DEV_EREPTD;
    }
    ret = osMutexAcquire(self->mutex_rx, osWaitForever);
    dev_assert(ret == osOK);

    if (self->mutex_tx != self->mutex_rx)
    {
        ret = osMutexAcquire(self->mutex_tx, osWaitForever);
        dev_assert(ret == osOK);
    }
    if ((self->open_state == true) && (self->device_attr.device_sole == DEVICE_NON_UNIQUENESS))
    {
        self->open_count++;
        device_err = DEV_EOK;
    }
    else
    {
        dev_assert(self->ops->open != NULL);
        device_err = self->ops->open(self->user_data);
        if (device_err == DEV_EOK)
        {
            self->open_state = true;
            self->open_count++;
        }
    }

    if (self->mutex_tx != self->mutex_rx)
    {
        ret = osMutexRelease(self->mutex_tx);
        dev_assert(ret == osOK);
    }

    ret = osMutexRelease(self->mutex_rx);
    dev_assert(ret == osOK);

    return device_err;
}

device_err_t device_close(device_t *const self)
{
    dev_assert(self != NULL);

    device_err_t device_err;
    osStatus_t ret = osOK;

    if (self->open_state == false)
    {
        return DEV_EREPTD;
    }

    ret = osMutexAcquire(self->mutex_rx, osWaitForever);
    dev_assert(ret == osOK);

    if (self->mutex_tx != self->mutex_rx)
    {
        ret = osMutexAcquire(self->mutex_tx, osWaitForever);
        dev_assert(ret == osOK);
    }

    if ((self->device_attr.device_sole == DEVICE_UNIQUENESS) || (self->open_count == 1))
    {
        dev_assert(self->ops->close != NULL);
        device_err = self->ops->close(self->user_data);
        if (device_err == DEV_EOK)
        {
            self->open_state = false;
            self->open_count = 0;
        }
    }
    else
    {
        self->open_count--;
        device_err = DEV_EOK;
    }

    if (self->mutex_tx != self->mutex_rx)
    {
        ret = osMutexRelease(self->mutex_tx);
        dev_assert(ret == osOK);
    }

    ret = osMutexRelease(self->mutex_rx);
    dev_assert(ret == osOK);

    return device_err;
}

device_err_t device_read(device_t *const self, void *const buffer, uint32_t size, uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buffer != NULL);

    device_err_t device_err;
    osStatus_t ret = osOK;

    if (self->open_state == false)
    {
        return DEV_ENOTOPEN;
    }
    ret = osMutexAcquire(self->mutex_rx, timeout);
    dev_assert(ret == osOK);

    dev_assert(self->ops->read != NULL);
    device_err = self->ops->read(self->user_data, buffer, size, timeout);

    ret = osMutexRelease(self->mutex_rx);
    dev_assert(ret == osOK);

    return device_err;
}
device_err_t device_write(device_t *const self, void const *const buffer, uint32_t size, uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buffer != NULL);

    device_err_t device_err;
    osStatus_t ret = osOK;

    if (self->open_state == false)
    {
        return DEV_ENOTOPEN;
    }
    ret = osMutexAcquire(self->mutex_tx, timeout);
    dev_assert(ret == osOK);

    dev_assert(self->ops->write != NULL);
    device_err = self->ops->write(self->user_data, buffer, size, timeout);

    ret = osMutexRelease(self->mutex_tx);
    dev_assert(ret == osOK);

    return device_err;
}
device_err_t device_ioctl(device_t *const self, uint8_t cmd, void *const arg)
{
    dev_assert(self != NULL);

    device_err_t device_err;
    osStatus_t ret = osOK;

    // if (self->open_state == false)
    // {
    //     return DEV_ENOTOPEN;
    // }
    ret = osMutexAcquire(self->mutex_rx, osWaitForever);
    dev_assert(ret == osOK);

    if (self->mutex_tx != self->mutex_rx)
    {
        ret = osMutexAcquire(self->mutex_tx, osWaitForever);
        dev_assert(ret == osOK);
    }
    
    dev_assert(self->ops->ioctl != NULL);
    device_err = self->ops->ioctl(self->user_data, cmd, arg);

    if (self->mutex_tx != self->mutex_rx)
    {
        ret = osMutexRelease(self->mutex_tx);
        dev_assert(ret == osOK);
    }

    ret = osMutexRelease(self->mutex_rx);
    dev_assert(ret == osOK);

    return device_err;
}
void device_test(void)
{
    hash_map_show(&hash_map_device);
}
