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
                             device_ops_t const *const device_ops,
                             void *const user_data)
{
    if (self == NULL || name == NULL || device_ops == NULL)
    {
        return -1;
    }

    memset(self, 0, sizeof(uart_dev_t));

    self->name = name;
    self->device_type = uart_type;
    self->ops = (struct device_ops const *)device_ops;
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
void device_uart_recv_handler(uart_dev_t *const self, void *const buffer, uint32_t size)
{
    if (self == NULL || buffer == NULL || size == 0)
    {
        return;
    }
    osStatus_t ret = osOK;
    *(uint16_t *)&(self->rx_buf[self->rx_buf_len - sizeof(uint16_t)]) = size;

    ret = osMessageQueuePut(self->osMessageQueueId_rx, buffer, 0, 0);
    if (ret != osOK)
    {
        // TODO: handle error
        return;
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

    if (dev->open_state == true)
    {
        dev->open_count++;
        return DEV_EOK;
    }

    char *name = "uart_tx_sem_";
    char *result = pvPortMalloc(strlen(name) + strlen(dev->name) + 1);
    strcpy(result, name);
    strcat(result, dev->name);
    osSemaphoreAttr_t sem_attr = {
        .name = result};
    vPortFree(result);
    dev->osSemaphoreId_tx = osSemaphoreNew(1, 0, &sem_attr);

    name = "uart_rx_queue_";
    result = pvPortMalloc(strlen(name) + strlen(dev->name) + 1);
    strcpy(result, name);
    strcat(result, dev->name);
    osMessageQueueAttr_t queue_attributes = {
        .name = result};
    vPortFree(result);
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

    if (dev->ops->open(dev) != 0)
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

    dev->open_state = true;
    dev->open_count++;
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
device_err_t dev_uart_deinit(uart_dev_t *dev)
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
#ifdef USE_FRAME_FORMAT
    if (dev->use_frame_format == true)
    {
        ret_frame_format = frame_format_send(&dev->frame_format, buf, len, timeout);
        if (ret_frame_format != 0)
        {
            dev->state = UART_STATE_BUSY;
            return DEV_EIO;
        }
        dev->state = UART_STATE_BUSY;
        return DEV_EOK;
    }
#endif
    ret = osMutexAcquire(dev->mutex_tx, timeout);
    if (ret != osOK)
    {
        dev->state = UART_STATE_BUSY;
        return ret;
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
#ifdef USE_FRAME_FORMAT
    if (dev->use_frame_format == true)
    {
        ret_frame_format = frame_format_recv(&dev->frame_format, buf, len, timeout);
        if (ret_frame_format != 0)
        {
            osMutexRelease(dev->mutex_rx);
            return DEV_EIO;
        }
    }
    else
#endif
    {
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
    }
    ret = osMutexRelease(dev->mutex_rx);
    if (ret != osOK)
    {
        return ret;
    }
    dev->state = UART_STATE_READY;
    return DEV_EOK;
}
#ifdef USE_FRAME_FORMAT
static int32_t uart_send_func(uint8_t *data, uint16_t data_len, uint32_t timeout, void *arg)
{
    uart_dev_t *dev = (uart_dev_t *)arg;
    if (dev == NULL)
    {
        return -1;
    }
    osStatus_t ret = osOK;
    device_err_t err = DEV_EOK;
    int32_t ret_frame_format = 0;
    if (dev == NULL || data == NULL || data_len == 0)
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

    ret = osMutexAcquire(dev->mutex_tx, timeout);
    if (ret != osOK)
    {
        return ret;
    }

    err = dev->ops->write(dev, data, data_len, timeout);
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

    ret = osMutexRelease(dev->mutex_tx);
    if (ret != osOK)
    {
        err = ret;
        goto err;
    }
    return DEV_EOK;

error:
    osMutexRelease(dev->mutex_tx);
err:
    return err;
}
static int32_t uart_recv_func(uint8_t *data, uint16_t data_len, uint32_t timeout, void *arg)
{
    uart_dev_t *dev = (uart_dev_t *)arg;
    osStatus_t ret = osOK;
    if (dev == NULL)
    {
        return -1;
    }

    ret = osMessageQueueGet(dev->osMessageQueueId_rx, data, 0, timeout);
    if (ret != osOK)
    {
        return ret;
    }
    data_len = *(uint16_t *)&(dev->rx_buf[dev->rx_buf_len - sizeof(uint16_t)]);

    return 0;
}
#endif
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
    if (dev->open_state == false)
    {
        return DEV_ENOTOPEN;
    }
    if (cmd > DEV_UART_IOCTL_DRIVER_INIT && cmd < DEV_UART_IOCTL_MAX)
    {
        return dev->ops->ioctl(dev, cmd, arg);
    }
    switch (cmd)
    {
#ifdef USE_FRAME_FORMAT
    case DEV_UART_IOCTL_SET_FRAME_FORMAT:
    {
        frame_format_arg_t *frame_format = (frame_format_arg_t *)arg;
        if (frame_format->use_frame_format)
        {
            dev->use_frame_format = true;
            ret = frame_format_init(&dev->frame_format,
                                    frame_format->crc_check_state,
                                    frame_format->retry_count,
                                    frame_format->timeout_ms,
                                    dev->rx_buf_len,
                                    dev->rx_buf_len);
            if (ret != 0)
            {
                return DEV_EINVAL;
            }
            
            ret = frame_format_send_func_register(&dev->frame_format, uart_send_func, dev);
            if (ret != 0)
            {
                return DEV_EINVAL;
            }

            ret = frame_format_recv_func_register(&dev->frame_format, uart_recv_func, dev);
            if (ret != 0)
            {
                return DEV_EINVAL;
            }
        }
    }
    break;
#endif
    default:
        return DEV_EINVAL;
        break;
    }
    return DEV_EOK;
}

// #define UART_DEV_TEST

#ifdef UART_DEV_TEST
#include "init_call.h"
#include "shell.h"
#include "ulog.h"
int8_t uart_test_statistics(uint8_t argc, uint8_t **argv)
{
    uart_dev_t *dev = NULL;
    if (argc < 2 || argc > 3)
    {
        LOG_I("uart_test_statistics error\r\n");
        goto Usage;
    }
    dev = device_uart_find(argv[1]);
    if (dev == NULL)
    {
        LOG_I("uart_test_statistics find dev error\r\n");
        return -1;
    }
    frame_format_statistics_t statistics;
    if (argv[2] != NULL && strcmp(argv[2], "clear") == 0)
    {
        frame_format_clear_statistics(&dev->frame_format);
    }
    else if (argv[2] != NULL && strcmp(argv[2], "get") == 0)
    {
        frame_format_get_statistics(&dev->frame_format, &statistics);
        LOG_I("frame_format_statistics:\r\n");
        LOG_I("\tsend_total_count:%d\r\n", statistics.send_total_count);
        LOG_I("\tsend_error_count:%d\r\n", statistics.send_error_count);
        LOG_I("\tsend_retry_count:%d\r\n", statistics.send_retry_count);
        LOG_I("\tsend_lose_rate:%f\r\n", statistics.send_lose_rate);
        LOG_I("\trecv_total_count:%d\r\n", statistics.recv_total_count);
        LOG_I("\trecv_lose_count:%d\r\n", statistics.recv_lose_count);
        LOG_I("\trecv_crc_error_count:%d\r\n", statistics.recv_crc_error_count);
        LOG_I("\trecv_lose_rate:%f\r\n", statistics.recv_lose_rate);
    }
    else
    {
        LOG_I("uart_test_statistics option error\r\n");
        goto Usage;
    }
    return 0;
Usage:
    LOG_I("Usage: uart_test_statistics [uart_dev_name] [option]\r\n");
    LOG_I("uart_dev_name:");
    for (uint8_t i = 0; i < sizeof(uart_devs) / sizeof(uart_devs[0]); i++)
    {
        if (uart_devs[i] == NULL)
        {
            break;
        }
        LOG_I("%s\t", uart_devs[i]->name);
    }
    LOG_I("\r\n");
    LOG_I("option:<get> or <clear> \r\n");
    return 0;
}
MSH_CMD_EXPORT_ALIAS(uart_test_statistics, uart_test_statistics, uart test statistics);

static void uart_test_send_thread(void *argument)
{
    uart_dev_t *dev = NULL;
    uint8_t data[128] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    uint16_t len = 100;
    uint32_t timeout = 1000;
    device_err_t err = DEV_EOK;
    for (uint8_t i = 0; i < len; i++)
    {
        data[i] = i;
    }
    dev = device_uart_find(UART_DEV_NAME_UART5);
    if (dev == NULL)
    {
        LOG_I("uart_test_send_thread find dev error\r\n");
        goto thread_exit;
    }
    err = dev_uart_init(dev, DEV_UART_IOCTL_USE_DMA, 5, 128);
    if (err != DEV_EOK)
    {
        LOG_I("uart_test_send_thread init dev error:%d\r\n", err);
        goto thread_exit;
    }
#ifdef USE_FRAME_FORMAT
    frame_format_arg_t frame_format = {
        .use_frame_format = true,
        .crc_check_state = true,
        .retry_count = 3,
        .timeout_ms = 3,
    };
    err = dev_uart_config(dev, DEV_UART_CMD_SET_FRAME_FORMAT, &frame_format);
    if (err != DEV_EOK)
    {
        LOG_I("uart_test_send_thread config dev error:%d\r\n", err);
        goto thread_exit;
    }
#endif

    LOG_I("uart_test_send_thread start\r\n");

    for (;;)
    {
        err = dev_uart_send(dev, data, len, timeout);
        if (err != DEV_EOK)
        {
            LOG_I("uart_test_send_thread error:%d\r\n", err);
            continue;
        }
        osDelay(10);
    }
thread_exit:
    osThreadExit();
}
static void uart_test_recv_thread(void *argument)
{
    uart_dev_t *dev = NULL;
    uint8_t data[128] = {0x00};
    uint16_t len = 10;
    uint32_t timeout = 3000;
    device_err_t err = DEV_EOK;

    dev = device_uart_find(UART_DEV_NAME_UART5);
    if (dev == NULL)
    {
        LOG_I("uart_test_recv_thread find dev error\r\n");
        goto thread_exit;
    }
    err = dev_uart_init(dev, DEV_UART_IOCTL_USE_DMA, 5, 128);
    if (err != DEV_EOK)
    {
        LOG_I("uart_test_recv_thread init dev error:%d\r\n", err);
        goto thread_exit;
    }
#ifdef USE_FRAME_FORMAT
    frame_format_arg_t frame_format = {
        .use_frame_format = true,
        .crc_check_state = true,
        .retry_count = 3,
        .timeout_ms = 3,
    };
    err = dev_uart_config(dev, DEV_UART_CMD_SET_FRAME_FORMAT, &frame_format);
    if (err != DEV_EOK)
    {
        LOG_I("uart_test_recv_thread config dev error:%d\r\n", err);
        goto thread_exit;
    }
#endif

    LOG_I("uart_test_recv_thread start\r\n");
    for (;;)
    {
        device_err_t err = dev_uart_recv(dev, data, len, timeout);
        if (err != 0)
        {
            LOG_I("uart_test_recv_thread error:%d\r\n", err);
            continue;
        }
        // for (uint8_t i = 0; i < len; i++)
        // {
        //     printf("%02X ", data[i]);
        // }
        // printf("\r\n");
    }
thread_exit:
    osThreadExit();
}

int uart_test(void)
{
    device_err_t err = DEV_EOK;
    osThreadAttr_t thread_attributes = {
        .name = "uart_test_send_thread",
        .stack_size = 1024 * 4,
        .priority = (osPriority_t)osPriorityAboveNormal,
    };

    osThreadId_t thread_id;
    thread_id = osThreadNew(uart_test_send_thread, NULL, &thread_attributes);
    if (thread_id == NULL)
    {
        return -4;
    }

    thread_attributes.name = "uart_test_recv_thread";
    thread_id = osThreadNew(uart_test_recv_thread, NULL, &thread_attributes);
    if (thread_id == NULL)
    {
        return -5;
    }
    return 0;
}

INIT_COMPONENT_EXPORT(uart_test);
#endif