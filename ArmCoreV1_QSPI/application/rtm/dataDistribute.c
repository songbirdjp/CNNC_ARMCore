/**
 * @file dev_dataDistribute.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "dataDistribute.h"
#include "init_call.h"
#include "FreeRTOS.h"
TAG("dataDistribute.c");
static device_data_distribute_t data_distribute1;
device_err_t device_data_distribute_open(device_t *const self)
{
    dev_assert(self != NULL);
    osStatus_t ret = osOK;
    device_err_t device_err = DEV_EOK;
    device_data_distribute_t *device = (device_data_distribute_t *)self;

    osEventFlagsAttr_t event_attributes = {
        .name = "data_distribute_event"};

    ret = osMutexAcquire(self->mutex_rx, osWaitForever);
    dev_assert(ret == osOK);

    if ((self->open_state == true) && (self->device_attr.device_sole == DEVICE_NON_UNIQUENESS))
    {
        self->open_count++;
        device_err = DEV_EOK;
    }
    else
    {
        device->osEventFlagsId = osEventFlagsNew(&event_attributes);
        if (device->osEventFlagsId == NULL)
        {
            device_err = DEV_ENOMEM;
        }
        else
        {
            self->open_state = true;
            self->open_count++;
        }
    }
    ret = osMutexRelease(self->mutex_rx);
    dev_assert(ret == osOK);
    return device_err;
}
device_err_t device_data_distribute_close(device_t *const self)
{
    dev_assert(self != NULL);
    osStatus_t osStatus;
    device_err_t device_err;
    osStatus_t ret = osOK;
    device_data_distribute_t *device = (device_data_distribute_t *)self;

    if (self->open_state == false)
    {
        return DEV_EREPTD;
    }

    ret = osMutexAcquire(self->mutex_rx, osWaitForever);
    dev_assert(ret == osOK);

    if (self->open_count == 1)
    {
        /*TODO:注销掉无用的队列*/

        device_err = osEventFlagsDelete(device->osEventFlagsId);
        if (device_err == osOK)
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

    ret = osMutexRelease(self->mutex_rx);
    dev_assert(ret == osOK);

    return device_err;
}
device_err_t device_data_distribute_read(device_t *const self,
                                         void *const buffer,
                                         uint32_t size,
                                         uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buffer != NULL);

    (void)size;
    osStatus_t osStatus;
    uint32_t flags = 0;
    osThreadId_t osThreadId = NULL;
    uint32_t retVal;
    device_data_distribute_t *device = (device_data_distribute_t *)self;
    data_distribute_msg_t *data_distribute_msg = (data_distribute_msg_t *)buffer;

    if (0 == data_distribute_msg->topic)
    {
        return DEV_EINVAL;
    }
    if ((data_distribute_msg->topic & device->topic_mask) != data_distribute_msg->topic)
    {
        return DEV_EINVAL;
    }
    for (uint32_t i = 0; i < TOPIC_SUPPORT_NUM; i++)
    {
        if (0 != ((data_distribute_msg->topic >> i) & 0x01))
        {
            osThreadId = osThreadGetId();
            for (uint8_t j = 0; j < CONSUMER_SUPPORT_NUM; j++)
            {
                if (device->consumer[i][j].consumer_thread == osThreadId)
                {
                    flags |= device->consumer[i][j].consumer_event_flag;
                }
            }
        }
    }

    retVal = osEventFlagsWait(device->osEventFlagsId,
                              flags,
                              osFlagsWaitAny | osFlagsNoClear,
                              timeout);
    if (retVal & 0x80000000)
    {
        return retVal;
    }

    for (uint32_t i = 0; i < TOPIC_SUPPORT_NUM; i++)
    {
        if (0 != ((retVal >> (i * CONSUMER_SUPPORT_NUM)) & (0xFFFFFFFF >> (32 - CONSUMER_SUPPORT_NUM))))
        {
            for (uint8_t j = 0; j < CONSUMER_SUPPORT_NUM; j++)
            {
                if (device->consumer[i][j].consumer_thread == osThreadId)
                {
                    osStatus = osMessageQueueGet(device->consumer[i][j].consumer_queue,
                                                 data_distribute_msg->msg_ptr,
                                                 &(data_distribute_msg->msg_prio),
                                                 timeout);

                    if (osStatus != osOK)
                    {
                        return osStatus;
                    }
                    if (device->consumer[i][j].topic_callback != NULL)
                    {
                        device->consumer[i][j].topic_callback(0x00000001 << i,
                                                              device->consumer[i][j].argument,
                                                              data_distribute_msg->msg_ptr,
                                                              device->msg_size[i]);
                    }

                    if (0 == osMessageQueueGetCount(device->consumer[i][j].consumer_queue))
                    {
                        osEventFlagsClear(device->osEventFlagsId, 0x00000001 << (i * CONSUMER_SUPPORT_NUM + j));
                    }
                }
            }
        }
    }
    return DEV_EOK;
}
device_err_t device_data_distribute_write(device_t *const self,
                                          void const *const buffer,
                                          uint32_t size,
                                          uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buffer != NULL);

    (void)size;
    osStatus_t osStatus;
    uint32_t flags = 0;
    device_data_distribute_t *device = (device_data_distribute_t *)self;
    data_distribute_msg_t *data_distribute_msg = (data_distribute_msg_t *)buffer;

    if (0 == data_distribute_msg->topic)
    {
        return DEV_EINVAL;
    }
    if ((data_distribute_msg->topic & device->topic_mask) != data_distribute_msg->topic)
    {
        return DEV_EINVAL;
    }

    for (uint32_t i = 0; i < TOPIC_SUPPORT_NUM; i++)
    {
        if (0 != ((data_distribute_msg->topic >> i) & 0x01))
        {
            osThreadId_t osThreadId = osThreadGetId();
            if (osThreadId != device->producer[i])
            {
                return DEV_EINVAL;
            }
            for (uint8_t j = 0; j < CONSUMER_SUPPORT_NUM; j++)
            {
                if ((device->consumer[i][j].consumer_thread != NULL) &&
                    (device->consumer[i][j].consumer_queue != NULL) &&
                    (device->consumer[i][j].consumer_event_flag != 0x0))
                {
                    osStatus = osMessageQueuePut(device->consumer[i][j].consumer_queue,
                                                 data_distribute_msg->msg_ptr,
                                                 data_distribute_msg->msg_prio,
                                                 timeout);
                    if (osStatus != osOK)
                    {
                        return osStatus;
                    }
                    flags |= device->consumer[i][j].consumer_event_flag;
                }
            }
            uint32_t retVal = osEventFlagsSet(device->osEventFlagsId, flags);
            if (retVal & 0x80000000)
            {
                return retVal;
            }
            return DEV_EOK;
        }
    }
    return DEV_EOK;
}
device_err_t device_data_distribute_ioctl(device_t *const self,
                                          uint8_t cmd,
                                          void *const arg)
{
    dev_assert(self != NULL);
    dev_assert((cmd > DATA_DISTRIBUTE_CMD_MIN) && (cmd < DATA_DISTRIBUTE_CMD_MAX));
    dev_assert(arg != NULL);

    osStatus_t ret = osOK;
    device_err_t device_err;
    device_data_distribute_t *device = (device_data_distribute_t *)self;

    if (self->open_state == false)
    {
        return DEV_ENOTOPEN;
    }

    ret = osMutexAcquire(self->mutex_rx, osWaitForever);
    dev_assert(ret == osOK);

    switch (cmd)
    {
    case DATA_DISTRIBUTE_CMD_TOPIC:
    {
        data_distribute_cmd_topic_t *data_distribute_cmd_topic = (data_distribute_cmd_topic_t *)arg;

        if ((0 == data_distribute_cmd_topic->topic) ||
            (0 == data_distribute_cmd_topic->msg_count) ||
            (0 == data_distribute_cmd_topic->msg_size))
        {
            device_err = DEV_EINVAL;
            goto exit;
        }
        if (0 != (data_distribute_cmd_topic->topic & device->topic_mask))
        {
            device_err = DEV_EINVAL;
            goto exit;
        }
        for (uint32_t i = 0; i < TOPIC_SUPPORT_NUM; i++)
        {
            if (0 != ((data_distribute_cmd_topic->topic >> i) & 0x01))
            {
                device->msg_count[i] = data_distribute_cmd_topic->msg_count;
                device->msg_size[i] = data_distribute_cmd_topic->msg_size;

                device->topic_mask |= (0x01 << i);
            }
        }
        device_err = DEV_EOK;
        goto exit;
        break;
    }
    case DATA_DISTRIBUTE_CMD_PRODUCER:
    {
        data_distribute_cmd_producer_t *data_distribute_cmd_producer = (data_distribute_cmd_producer_t *)arg;

        if (0 == data_distribute_cmd_producer->topic)
        {
            device_err = DEV_EINVAL;
            goto exit;
        }
        if ((data_distribute_cmd_producer->topic & device->topic_mask) != data_distribute_cmd_producer->topic)
        {
            device_err = DEV_EINVAL;
            goto exit;
        }

        for (uint32_t i = 0; i < TOPIC_SUPPORT_NUM; i++)
        {
            if (0 != ((data_distribute_cmd_producer->topic >> i) & 0x01))
            {
                if (device->producer[i] != NULL)
                {
                    device_err = DEV_EEXIST;
                    goto exit;
                }
                device->producer[i] = osThreadGetId();
            }
        }
        device_err = DEV_EOK;
        goto exit;
        break;
    }
    case DATA_DISTRIBUTE_CMD_CONSUMER:
    {
        data_distribute_cmd_consumer_t *data_distribute_cmd_consumer = (data_distribute_cmd_consumer_t *)arg;

        if (0 == data_distribute_cmd_consumer->topic)
        {
            device_err = DEV_EINVAL;
            goto exit;
        }
        if ((data_distribute_cmd_consumer->topic & device->topic_mask) != data_distribute_cmd_consumer->topic)
        {
            device_err = DEV_EINVAL;
            goto exit;
        }

        for (uint32_t i = 0; i < TOPIC_SUPPORT_NUM; i++)
        {
            if (0 != ((data_distribute_cmd_consumer->topic >> i) & 0x01))
            {
                uint8_t j = 0;
                for (j = 0; j < CONSUMER_SUPPORT_NUM; j++)
                {
                    if ((device->consumer[i][j].consumer_thread == NULL) && (device->consumer[i][j].consumer_queue == NULL))
                    {
                        char *queue_name = "queue";
                        char *result = (char *)pvPortMalloc(strlen(queue_name) + 4);
                        sprintf(result, "%s_%d_%d", queue_name, i, j);
                        osMessageQueueAttr_t queue_attributes = {
                            .name = result};

                        device->consumer[i][j].consumer_queue = osMessageQueueNew(device->msg_count[i],
                                                                                  device->msg_size[i],
                                                                                  &queue_attributes);
                        if (device->consumer[i][j].consumer_queue == NULL)
                        {
                            device_err = DEV_ENOMEM;
                            goto exit;
                        }
                        device->consumer[i][j].consumer_thread = osThreadGetId();
                        device->consumer[i][j].consumer_event_flag = (0x00000001 << (i * CONSUMER_SUPPORT_NUM + j));
                        device->consumer[i][j].argument = data_distribute_cmd_consumer->argument;
                        device->consumer[i][j].topic_callback = data_distribute_cmd_consumer->topic_callback;
                        break;
                    }
                }
                if (j == CONSUMER_SUPPORT_NUM)
                {
                    device_err = DEV_ENOMEM;
                    goto exit;
                }
            }
        }
        device_err = DEV_EOK;
        goto exit;
        break;
    }
    default:
    {
        device_err = DEV_ENOTSUP;
        goto exit;
        break;
    }
    }
    device_err = DEV_EOK;
exit:
    ret = osMutexRelease(self->mutex_rx);
    dev_assert(ret == osOK);
    return device_err;
}

device_err_t device_data_distribute_register(device_data_distribute_t *const self,
                                             char const *name)
{
    dev_assert(self != NULL);
    dev_assert(name != NULL);
    dev_assert((device_data_distribute_t *)device_find(name) == NULL);

    memset(self, 0, sizeof(device_data_distribute_t));

    device_attr_t attr = {
        .device_sole = DEVICE_NON_UNIQUENESS,
        .device_type = DEVICE_DATA_DISTRIBUTE,
        .device_duplex = DEVICE_FULL_DUPLEX,
        .name = name};

    static device_ops_t device_ops = {
        .open = NULL,
        .close = NULL,
        .read = NULL,
        .write = NULL,
        .ioctl = NULL};

    device_register((device_t *)self, &attr, &device_ops, NULL);
    return DEV_EOK;
}
static int data_distribute_init(void)
{
    device_data_distribute_register(&data_distribute1, DEVICE_DATA_DISTRIBUTE_1_NAME);
}
INIT_DEVICE_EXPORT(data_distribute_init);
/*********************************TEST*****************************************/
#if 0
#include "shell.h"
#define TOPIC_TEST_1 (1 << 0)
#define TOPIC_TEST_2 (1 << 1)
static void test_thread_entry1(void *argument)
{
    device_err_t device_err;

    device_err = device_data_distribute_open(&data_distribute);
    if (device_err != DEV_EOK)
    {
        LOG_I("entry 1 open fail,error:%d\r\n", device_err);
        goto exit;
    }

    data_distribute_cmd_producer_t data_distribute_cmd_producer;
    data_distribute_cmd_producer.topic = TOPIC_TEST_1 | TOPIC_TEST_2;
    device_err = device_data_distribute_ioctl(&data_distribute,
                                              DATA_DISTRIBUTE_CMD_PRODUCER,
                                              &data_distribute_cmd_producer);
    if (device_err != DEV_EOK)
    {
        LOG_I("entry 1 ioctl producer fail,error:%d\r\n", device_err);
        goto exit;
    }
    data_distribute_msg_t data_distribute_msg;
    data_distribute_msg.topic = TOPIC_TEST_1 | TOPIC_TEST_2;
    uint8_t data[10] = {0};
    data_distribute_msg.msg_ptr = data;
    data_distribute_msg.msg_prio = NULL;
    LOG_I("data distribute entry 1 run!\r\n");
    for (;;)
    {
        data_distribute_msg.topic = TOPIC_TEST_1;
        data[0] = TOPIC_TEST_1;
        device_err = device_data_distribute_write(&data_distribute,
                                                  &data_distribute_msg,
                                                  NULL,
                                                  0);
        if (device_err != DEV_EOK)
        {
            LOG_I("entry 1 write TOPIC_TEST_1 fail,error:%d\r\n", device_err);
        }
        data_distribute_msg.topic = TOPIC_TEST_2;
        data[0] = TOPIC_TEST_2;
        device_err = device_data_distribute_write(&data_distribute,
                                                  &data_distribute_msg,
                                                  NULL,
                                                  0);
        if (device_err != DEV_EOK)
        {
            LOG_I("entry 1 write TOPIC_TEST_2 fail,error:%d\r\n", device_err);
        }
        osDelay(100);
    }
exit:
    osThreadExit();
}
void entry2_topic_callback(uint8_t topic, void *const msg_ptr)
{
    if (topic == TOPIC_TEST_1)
    {
        LOG_I("entry2 callback TOPIC_TEST_1:%d\r\n", *((uint8_t *)msg_ptr));
    }
    else if (topic == TOPIC_TEST_2)
    {
        LOG_I("entry2 callback TOPIC_TEST_2:%d\r\n", *((uint8_t *)msg_ptr));
    }
}

static void test_thread_entry2(void *argument)
{
    device_err_t device_err;

    device_err = device_data_distribute_open(&data_distribute);
    if (device_err != DEV_EOK)
    {
        LOG_I("entry 2 open fail,error:%d\r\n", device_err);
        goto exit;
    }
    data_distribute_cmd_consumer_t data_distribute_cmd_consumer;
    data_distribute_cmd_consumer.topic = TOPIC_TEST_1 | TOPIC_TEST_2;
    data_distribute_cmd_consumer.topic_callback = entry2_topic_callback;
    device_err = device_data_distribute_ioctl(&data_distribute,
                                              DATA_DISTRIBUTE_CMD_CONSUMER,
                                              &data_distribute_cmd_consumer);
    if (device_err != DEV_EOK)
    {
        LOG_I("entry 2 ioctl consumer fail,error:%d\r\n", device_err);
        goto exit;
    }
    data_distribute_msg_t data_distribute_msg;
    data_distribute_msg.topic = TOPIC_TEST_1 | TOPIC_TEST_2;
    uint8_t data[10] = {0};
    data_distribute_msg.msg_ptr = data;
    data_distribute_msg.msg_prio = NULL;
    LOG_I("data distribute entry 2 run!\r\n");
    for (;;)
    {
        device_err = device_data_distribute_read(&data_distribute,
                                                 &data_distribute_msg,
                                                 NULL,
                                                 0xFFFFFFFFUL);
        if (device_err != DEV_EOK)
        {
            LOG_I("entry 2 read fail,error:%d\r\n", device_err);
        }
        osDelay(100);
    }
exit:
    osThreadExit();
}
void entry3_topic_callback(uint8_t topic, void *const msg_ptr)
{
    if (topic == TOPIC_TEST_1)
    {
        LOG_I("entry3 callback TOPIC_TEST_1:%d\r\n", *((uint8_t *)msg_ptr));
    }
    else if (topic == TOPIC_TEST_2)
    {
        LOG_I("entry3 callback TOPIC_TEST_2:%d\r\n", *((uint8_t *)msg_ptr));
    }
}
static void test_thread_entry3(void *argument)
{
    device_err_t device_err;

    device_err = device_data_distribute_open(&data_distribute);
    if (device_err != DEV_EOK)
    {
        LOG_I("entry 3 open fail,error:%d\r\n", device_err);
        goto exit;
    }
    data_distribute_cmd_consumer_t data_distribute_cmd_consumer;
    data_distribute_cmd_consumer.topic = TOPIC_TEST_2;
    data_distribute_cmd_consumer.topic_callback = entry3_topic_callback;
    device_err = device_data_distribute_ioctl(&data_distribute,
                                              DATA_DISTRIBUTE_CMD_CONSUMER,
                                              &data_distribute_cmd_consumer);
    if (device_err != DEV_EOK)
    {
        LOG_I("entry 3 ioctl consumer fail,error:%d\r\n", device_err);
        goto exit;
    }
    data_distribute_msg_t data_distribute_msg;
    data_distribute_msg.topic = TOPIC_TEST_2;
    uint8_t data[10] = {0};
    data_distribute_msg.msg_ptr = data;
    data_distribute_msg.msg_prio = NULL;
    LOG_I("data distribute entry 2 run!\r\n");
    for (;;)
    {
        device_err = device_data_distribute_read(&data_distribute,
                                                 &data_distribute_msg,
                                                 NULL,
                                                 0xFFFFFFFFUL);
        if (device_err != DEV_EOK)
        {
            LOG_I("entry 3 read fail,error:%d\r\n", device_err);
        }
        LOG_I("entry 3 read msg:%x\r\n", data[0]);
        osDelay(100);
    }
exit:
    osThreadExit();
}
void data_distribute_test(void)
{
    device_err_t device_err;
    osThreadId_t data_distribute_test_threadHandle;

    device_err = device_data_distribute_open(&data_distribute);
    if (device_err != DEV_EOK)
    {
        LOG_I("data distribute open fail,error:%d\r\n", device_err);
        return;
    }

    data_distribute_cmd_topic_t data_distribute_cmd_topic;
    data_distribute_cmd_topic.topic = TOPIC_TEST_1 | TOPIC_TEST_2;
    data_distribute_cmd_topic.msg_count = 5;
    data_distribute_cmd_topic.msg_size = 5;

    device_err = device_data_distribute_ioctl(&data_distribute,
                                              DATA_DISTRIBUTE_CMD_TOPIC,
                                              &data_distribute_cmd_topic);
    if (device_err != DEV_EOK)
    {
        LOG_I("ioctl topic fail,error:%d\r\n", device_err);
        return;
    }

    osThreadAttr_t thread_attributes1 = {
        .name = "data_distribute_test_thread1",
        .stack_size = 512 * 4,
        .priority = (osPriority_t)osPriorityLow,
    };
    data_distribute_test_threadHandle = osThreadNew(test_thread_entry1, NULL, &thread_attributes1);
    if (data_distribute_test_threadHandle == NULL)
    {
        LOG_I("thread 1 test create failed\r\n");
        return;
    }

    osThreadAttr_t thread_attributes2 = {
        .name = "data_distribute_test_thread2",
        .stack_size = 512 * 4,
        .priority = (osPriority_t)osPriorityLow1,
    };
    data_distribute_test_threadHandle = osThreadNew(test_thread_entry2, NULL, &thread_attributes2);
    if (data_distribute_test_threadHandle == NULL)
    {
        LOG_I("thread 2 test create failed\r\n");
        return;
    }

    osThreadAttr_t thread_attributes3 = {
        .name = "data_distribute_test_thread3",
        .stack_size = 512 * 4,
        .priority = (osPriority_t)osPriorityLow,
    };
    data_distribute_test_threadHandle = osThreadNew(test_thread_entry3, NULL, &thread_attributes3);
    if (data_distribute_test_threadHandle == NULL)
    {
        LOG_I("thread 3 test create failed\r\n");
        return;
    }
}
MSH_CMD_EXPORT_ALIAS(data_distribute_test, data_distribute_test, "di data distribute test");
#endif