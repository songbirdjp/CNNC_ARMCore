/**
 * @file dev_uart.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-02-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "dev_uart.h"
#include <string.h>
#include <stdio.h>
#define UART_DEV_MAX_NUM 10
static uart_dev_t *uart_devs[UART_DEV_MAX_NUM] = {0};

int32_t device_uart_register(uart_dev_t *const self,
                             const char *name,
                             uart_type_t uart_type,
                             device_uart_ops_t const *const device_ops,
                             void *const user_data)
{
    if (self == NULL || name == NULL || device_ops == NULL)
    {
        return -1;
    }

    memset(self, 0, sizeof(uart_dev_t));

    self->name = name;
    self->device_type = uart_type;
    self->ops = (struct device_uart_ops const *)device_ops;
    self->user_data = user_data;
    self->open_state = false;
    self->open_count = 0;
    self->state = UART_STATE_INIT;
    const osMutexAttr_t mutex_attr =
        {
            .name = "mutex_uart_dev",
            .attr_bits = osMutexPrioInherit | osMutexRecursive,
            .cb_mem = NULL,
            .cb_size = 0,
        };
    self->mutex_rx = osMutexNew(&mutex_attr);
    self->mutex_tx = osMutexNew(&mutex_attr);
    if (self->mutex_rx == NULL || self->mutex_tx == NULL)
    {
        return -2;
    }

    for (uint8_t i = 0; i < sizeof(uart_devs) / sizeof(uart_devs[0]); i++)
    {
        if (uart_devs[i] == NULL)
        {
            uart_devs[i] = self;
            return 0;
        }
    }
    return -3;
}

static void uart_recv_buf_handle(uart_dev_t *const self, void *const buffer, uint32_t size)
{
    osStatus_t ret = osOK;
    uint16_t frame_len = 0;
    if (self == NULL || buffer == NULL || size == 0)
    {
        return;
    }
    for (uint32_t i = 0; i < size; i++)
    {
        if (*(uint16_t *)((uint8_t *)buffer + i) == 0xAA55)
        {
            frame_len = *(uint16_t *)((uint8_t *)buffer + i + 4) + 10;
            if (frame_len > size)
            {
                break;
            }
            memcpy(self->rx_buf_tmp, ((uint8_t *)buffer + i), frame_len);
            *(uint16_t *)(self->rx_buf_tmp + self->rx_buf_len - sizeof(uint16_t)) = frame_len;
            ret = osMessageQueuePut(self->osMessageQueueId_rx, self->rx_buf_tmp, 0, 0);
            if (ret != osOK)
            {
                // TODO: handle error
                break;
            }
            if (frame_len < size)
            {
                i += frame_len - 1;
                continue;
            }
            else
            {
                break;
            }
        }
    }
}

void device_uart_recv_handler(uart_dev_t *const self, void *const buffer, uint32_t size)
{
    if (self == NULL || buffer == NULL || size == 0)
    {
        return;
    }
    if (self->use_frame_format == true)
    {
        uart_recv_buf_handle(self, buffer, size);
    }
    else
    {
        osStatus_t ret = osOK;
        *(uint16_t *)((uint8_t *)buffer + self->rx_buf_len - sizeof(uint16_t)) = size;

        ret = osMessageQueuePut(self->osMessageQueueId_rx, buffer, 0, 0);
        if (ret != osOK)
        {
            // TODO: handle error
            return;
        }
    }
}
void device_uart_send_handler(uart_dev_t *const self)
{
    if (self == NULL)
    {
        return;
    }
    osStatus_t ret = osOK;
    ret = osSemaphoreRelease(self->osSemaphoreId_tx);
    if (ret != osOK)
    {
        // TODO: handle error
        return;
    }
}
uart_dev_t *device_uart_find(char const *name)
{
    if (name == NULL)
    {
        return NULL;
    }
    for (uint8_t i = 0; i < sizeof(uart_devs) / sizeof(uart_devs[0]); i++)
    {
        if ((uart_devs[i] != NULL) && (strcmp(uart_devs[i]->name, name) == 0))
        {
            return uart_devs[i];
        }
    }
    return NULL;
}

device_err_t dev_uart_init(uart_dev_t *dev, uint16_t oflags, uint32_t queueSpace, uint32_t queueMsgSize)
{
    if (dev == NULL || queueSpace == 0 || queueMsgSize == 0)
    {
        return DEV_EINVAL;
    }

    if (dev->ops == NULL ||
        dev->ops->open == NULL ||
        dev->ops->close == NULL ||
        dev->ops->read == NULL ||
        dev->ops->write == NULL ||
        dev->ops->ioctl == NULL)
    {
        return DEV_EINVAL;
    }

    osSemaphoreAttr_t sem_attr = {
        .name = "uart_tx_sem",
    };
    dev->osSemaphoreId_tx = osSemaphoreNew(1, 0, &sem_attr);

    osMessageQueueAttr_t queue_attributes = {
        .name = "uart_rx_queue"};
    dev->osMessageQueueId_rx = osMessageQueueNew(queueSpace, queueMsgSize, &queue_attributes);

    if ((dev->osSemaphoreId_tx == NULL) || (dev->osMessageQueueId_rx == NULL))
    {
        return DEV_ENOMEM;
    }

    dev->rx_buf = (uint8_t *)pvPortMalloc(queueMsgSize);
    if (dev->rx_buf == NULL)
    {
        return DEV_ENOMEM;
    }
    dev->rx_buf_len = queueMsgSize;

    dev->rx_buf_tmp = (uint8_t *)pvPortMalloc(queueMsgSize);
    if (dev->rx_buf_tmp == NULL)
    {
        return DEV_ENOMEM;
    }

    if (dev->ops->init(dev) != 0)
    {
        return DEV_EIO;
    }

    if (oflags & DEV_UART_IOCTL_USE_DMA)
    {
        dev->device_mode = UART_MODE_DMA;
    }
    else if (oflags & DEV_UART_IOCTL_USE_BLOCKING)
    {
        dev->device_mode = UART_MODE_BLOCKING;
    }

    if (dev->device_type == UART_TYPE_HALF_DUPLEX_SLAVE)
    {
        dev->state = UART_STATE_BUSY;
    }
    else
    {
        dev->state = UART_STATE_READY;
    }
    return DEV_EOK;
}

device_err_t dev_uart_open(uart_dev_t *dev)
{
    if (dev == NULL)
    {
        return DEV_EINVAL;
    }

    if (dev->open_state == true)
    {
        dev->open_count++;
        return DEV_EOK;
    }

    if (dev->ops->open(dev) != 0)
    {
        return DEV_EIO;
    }

    dev->open_state = true;
    dev->open_count++;

    return DEV_EOK;
}
device_err_t dev_uart_close(uart_dev_t *dev)
{
    if (dev == NULL)
    {
        return DEV_EINVAL;
    }

    if (dev->ops == NULL || dev->ops->close == NULL)
    {
        return DEV_EINVAL;
    }

    if (dev->open_state == false)
    {
        return DEV_ENOTOPEN;
    }

    if (dev->open_count > 1)
    {
        dev->open_count--;
        return DEV_EOK;
    }
    /*TODO: deinit uart resource, such as semaphore, message queue, mutex, etc.*/
    if (dev->ops->close(dev) != 0)
    {
        return DEV_EIO;
    }

    dev->open_state = false;
    dev->open_count--;
    dev->state = UART_STATE_INIT;

    return DEV_EOK;
}

device_err_t dev_uart_send(uart_dev_t *dev, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    osStatus_t ret = osOK;
    device_err_t err = DEV_EOK;
    int32_t ret_frame_format = 0;
    if (dev == NULL || buf == NULL || len == 0)
    {
        return DEV_EINVAL;
    }
    if (dev->ops == NULL || dev->ops->write == NULL)
    {
        return DEV_EINVAL;
    }
    if (dev->open_state == false)
    {
        return DEV_ENOTOPEN;
    }
    if ((dev->state == UART_STATE_BUSY) && (dev->device_type == UART_TYPE_HALF_DUPLEX_SLAVE))
    {
        return DEV_EBUSY;
    }

    ret = osMutexAcquire(dev->mutex_tx, timeout);
    if (ret != osOK)
    {
        dev->state = UART_STATE_BUSY;
        return ret;
    }

    if (dev->ops->write_before != NULL)
    {
        err = dev->ops->write_before(dev);
        if (err != DEV_EOK)
        {
            goto error;
        }
    }

    err = dev->ops->write(dev, buf, len, timeout);
    if (err != DEV_EOK)
    {
        goto error;
    }
    ret = osSemaphoreAcquire(dev->osSemaphoreId_tx, timeout);
    if (ret != DEV_EOK)
    {
        err = ret;
        goto error;
    }

    if (dev->ops->write_complete != NULL)
    {
        err = dev->ops->write_complete(dev);
        if (err != DEV_EOK)
        {
            goto error;
        }
    }

    ret = osMutexRelease(dev->mutex_tx);
    if (ret != osOK)
    {
        err = ret;
        goto err;
    }
    dev->state = UART_STATE_BUSY;
    return DEV_EOK;

error:
    osMutexRelease(dev->mutex_tx);
err:
    dev->state = UART_STATE_BUSY;
    return err;
}
device_err_t dev_uart_recv(uart_dev_t *dev, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    osStatus_t ret = osOK;
    device_err_t err = DEV_EOK;
    int32_t ret_frame_format = 0;
    if (dev == NULL || buf == NULL || len == 0)
    {
        return DEV_EINVAL;
    }
    if (dev->ops == NULL || dev->ops->read == NULL)
    {
        return DEV_EINVAL;
    }
    if (dev->open_state == false)
    {
        return DEV_ENOTOPEN;
    }
    ret = osMutexAcquire(dev->mutex_rx, timeout);
    if (ret != osOK)
    {
        return ret;
    }
    if (dev->device_mode == UART_MODE_BLOCKING)
    {
        err = dev->ops->read(dev, buf, len, timeout);
        if (err != DEV_EOK)
        {
            osMutexRelease(dev->mutex_rx);
            return err;
        }
    }
    else
    {
        ret = osMessageQueueGet(dev->osMessageQueueId_rx, buf, 0, timeout);
        if (ret != osOK)
        {
            osMutexRelease(dev->mutex_rx);
            return ret;
        }
    }
    ret = osMutexRelease(dev->mutex_rx);
    if (ret != osOK)
    {
        return ret;
    }
    dev->state = UART_STATE_READY;
    return DEV_EOK;
}
device_err_t dev_uart_config(uart_dev_t *dev, uint8_t cmd, void *arg)
{
    int32_t ret = 0;
    if (dev == NULL || arg == NULL)
    {
        return DEV_EINVAL;
    }
    if (dev->ops == NULL || dev->ops->ioctl == NULL)
    {
        return DEV_EINVAL;
    }

    if (cmd > DEV_UART_IOCTL_DRIVER_INIT && cmd < DEV_UART_IOCTL_MAX)
    {
        return dev->ops->ioctl(dev, cmd, arg);
    }
    switch (cmd)
    {
    case DEV_UART_IOCTL_USED_FRAME:
    {
        frame_used_arg_t *frame_arg = (frame_used_arg_t *)arg;
        dev->use_frame_format = frame_arg->use_frame_format;
    }
    break;
    default:
        return DEV_EINVAL;
        break;
    }
    return DEV_EOK;
}
