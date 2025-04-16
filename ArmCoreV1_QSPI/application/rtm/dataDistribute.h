/**
 * @file dev_dataDistribute.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef _DATA_DISTRIBUTE_H_
#define _DATA_DISTRIBUTE_H_

#include "dev_base.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define DEVICE_DATA_DISTRIBUTE_1_NAME "data_distribute1"
#define DEVICE_DATA_DISTRIBUTE_2_NAME "data_distribute2"
    /*****************************ioctl cmd:public*****************************/

    typedef enum data_distribute_cmd
    {
        DATA_DISTRIBUTE_CMD_MIN = 0,

        DATA_DISTRIBUTE_CMD_TOPIC,
        DATA_DISTRIBUTE_CMD_PRODUCER,
        DATA_DISTRIBUTE_CMD_CONSUMER,
        DATA_DISTRIBUTE_CMD_MAX
    } data_distribute_cmd_t;

    typedef struct data_distribute_cmd_topic
    {
        uint32_t topic;
        uint32_t msg_count;
        uint32_t msg_size;
    } data_distribute_cmd_topic_t;

    typedef struct data_distribute_cmd_producer
    {
        uint32_t topic;
    } data_distribute_cmd_producer_t;

    typedef void (*topic_callback_t)(uint32_t topic, void *argument,void *const msg_ptr, uint32_t msg_size);

    typedef struct data_distribute_cmd_consumer
    {
        uint32_t topic;
        void *argument;
        topic_callback_t topic_callback;
    } data_distribute_cmd_consumer_t;
    /******************************msg struct:public***************************/

    typedef struct data_distribute_msg
    {
        uint32_t topic;
        void * msg_ptr;
        uint8_t msg_prio;
    } data_distribute_msg_t;
    /******************************device struct:private***********************/
    typedef struct consumer
    {
        void *consumer_thread;
        void *consumer_queue;
        uint32_t consumer_event_flag;
        void *argument;
        topic_callback_t topic_callback;
    } consumer_t;

    typedef struct device_data_distribute
    {
        device_t super;

        uint32_t topic_mask;

        uint32_t event_flag;
        void *osEventFlagsId;

#define TOPIC_SUPPORT_NUM (12)
        uint32_t msg_count[TOPIC_SUPPORT_NUM];
        uint32_t msg_size[TOPIC_SUPPORT_NUM];

#define CONSUMER_SUPPORT_NUM (2)
        void *producer[TOPIC_SUPPORT_NUM];                            // 存放生产者线程ID
        consumer_t consumer[TOPIC_SUPPORT_NUM][CONSUMER_SUPPORT_NUM]; // 存放消费者队列ID，可以有多个消费者
    } device_data_distribute_t;

    /******************************low level:public****************************/
    device_err_t device_data_distribute_register(device_data_distribute_t *const self,
                                                 char const *name);
    /***
     * 使用方法
     * 1.管理线程创建话题；
     * 2.生产者和消费者各自注册；
     * 3.生产者发送和消费者读取,生产者一次只能发布一个消息，消费者可以同时获取多个消息；
     */
    device_err_t device_data_distribute_open(device_t *const self);
    device_err_t device_data_distribute_close(device_t *const self);
    device_err_t device_data_distribute_read(device_t *const self,
                                             void *const buffer,
                                             uint32_t size,
                                             uint32_t timeout);
    device_err_t device_data_distribute_write(device_t *const self,
                                              void const *const buffer,
                                              uint32_t size,
                                              uint32_t timeout);
    device_err_t device_data_distribute_ioctl(device_t *const self,
                                              uint8_t cmd,
                                              void *const arg);
#ifdef __cplusplus
}
#endif

#endif
