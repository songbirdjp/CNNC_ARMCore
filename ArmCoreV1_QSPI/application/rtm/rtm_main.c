/**
 * @file rtm_uart_handle.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-04-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <string.h>
#include "rtm_main.h"
#include "uart_protocol.h"
#include "ulog.h"
#include "ethercat.h"
#include "lan9252_app.h"
#include "init_call.h"
typedef struct
{
    uint32_t id_ack;
    uint8_t type;
    uint16_t length;
    uint8_t data[UART_PROTOCOL_DATA_MAX_LENGTH];
} __attribute__((aligned(1), packed)) payload_t;

typedef struct
{
    payload_t payload;
    uint16_t length;
} __attribute__((aligned(1), packed)) queue_frame_t;

typedef struct
{
    uint8_t require_state;
    uint8_t require_ctrl_mode;
    uint32_t interlock_override;
    uint32_t unready_override;
} __attribute__((aligned(1), packed)) rtm_require_t;

typedef struct
{
    uint8_t fsm_state_current;
    uint8_t ctrl_mode_cur;
    uint32_t not_ready_event;
    uint32_t warning_interlock;
    uint32_t minor_interlock;
    uint32_t serious_interlock;
} __attribute__((aligned(1), packed)) rtm_status_t;

static int32_t rtm_set_data_distribute(osMessageQueueId_t queue_id, uint32_t ID, uint8_t cmd, uint8_t *data, uint16_t len);
static app_rtm_main_t app_rtm;

static void app_rtm_fault_detect_entry(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    for (;;)
    {
        osDelay(100);
    }
}
osThreadId_t app_rtm_main_threadId;
static void app_rtm_main_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    osStatus_t status = osOK;
    queue_frame_t queue_frame;
    rtm_status_t rtm_status = {0};
    dido_structure_t dido_structure = {0};
    rtm_event_t rtm_event = {
        .dido_structure = &dido_structure,
        .super.sig = SYSTEM_STATE_SYSTEM_ON};

    app_di_get(&(self->app_dido), &dido_structure);

    rtm_status.fsm_state_current = SYSTEM_STATE_INITIALIZATION;
    rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_queue, 0x01, 0x1F, (uint8_t *)&rtm_status, sizeof(rtm_status_t));

    rtm_state_machine_ctor((stateTable_t *)&(self->state_machine));
    stateTable_init((stateTable_t *)&(self->state_machine), (Event_t *)&rtm_event);
    for (;;)
    {
        status = osMessageQueueGet(self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_queue, &queue_frame, NULL, 0);
        if (status == osOK)
        {
            if (queue_frame.payload.type == 0x03) /*参数配置帧*/
            {
                // TODO: 处理参数配置帧
            }
            else if (queue_frame.payload.type == 0x04) /*参数获取帧*/
            {
                // TODO: 处理参数获取帧
            }
            else if (queue_frame.payload.type == 0x05) /*SET帧*/
            {
                // TODO: 处理SET帧
                if (queue_frame.payload.data[0] == 0x1F)
                {
                    rtm_event.super.sig = queue_frame.payload.data[1];
                }
            }
            else if (queue_frame.payload.type == 0x06) /*GET帧*/
            {
                // TODO: 处理GET帧
            }
            else
            {
                // TODO: 处理未知帧
            }
        }
        // 轮询状态机
        app_di_get(&(self->app_dido), &dido_structure);

        stateTable_dispatch((stateTable_t *)&(self->state_machine), (Event_t *)&rtm_event);

        app_do_set(&(self->app_dido), &dido_structure);
        // TODO:变化时发送
        rtm_status.fsm_state_current = stateTable_get_state((stateTable_t *)&(self->state_machine));
        // TODO:联锁更新
        rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_queue, 0x01, 0x1F, (uint8_t *)&rtm_status, sizeof(rtm_status_t));

        osDelay(1);
    }
exit:
    osThreadExit();
}

static int32_t rtm_set_data_distribute(osMessageQueueId_t queue_id, uint32_t ID, uint8_t cmd, uint8_t *data, uint16_t len)
{
    if (queue_id == NULL || data == NULL || len == 0 || len > UART_PROTOCOL_DATA_MAX_LENGTH || cmd == 0)
    {
        return -1;
    }
    queue_frame_t queue_frame;
    queue_frame.payload.id_ack = ID << 1 | 0x01;
    queue_frame.payload.type = 0x05;
    queue_frame.payload.length = len + 1;
    queue_frame.payload.data[0] = cmd;
    memcpy(queue_frame.payload.data + 1, data, len);
    queue_frame.length = queue_frame.payload.length + sizeof(payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH;
    osStatus_t status = osMessageQueuePut(queue_id, &queue_frame, 0, 0);
    if (status != osOK)
    {
        return -2;
    }
    return 0;
}
#define APP_RTM_EVENT_FLAG_OUTPUT (1 << 0)

void app_rtm_event_output_set(void)
{
    osEventFlagsSet(app_rtm.ethercat_Event, APP_RTM_EVENT_FLAG_OUTPUT);
}

static void app_ethercat_rx_thread(void *argument)
{
    uint32_t ret = 0;
    rtm_module_info_t *self = (rtm_module_info_t *)argument;
    TOBJ7010 output_data;
    for (;;)
    {
        ret = osEventFlagsWait(app_rtm.ethercat_Event, APP_RTM_EVENT_FLAG_OUTPUT, osFlagsWaitAny, osWaitForever);
        if (ret & APP_RTM_EVENT_FLAG_OUTPUT)
        {
            ethercat_recv_data_get((uint16_t *)&output_data, sizeof(TOBJ7010));
            // TODO: 处理输出数据
        }
    }
exit:
    osThreadExit();
}

static void app_ethercat_tx_thread(void *argument)
{
    int32_t ret = 0;
    osStatus_t status = osOK;
    rtm_module_info_t *self = (rtm_module_info_t *)argument;
    queue_frame_t queue_frame;
    TOBJ6000 input_data;
    for (;;)
    {
        status = osMessageQueueGet(self->module_queue, &queue_frame, NULL, 0xFFFFFFFF);
        if (status != osOK)
        {
            LOG_E("%s queue get error, status = %d\r\n", self->module_name, status);
            continue;
        }
        // TODO: 处理输入数据
        ethercat_send_data_update((uint16_t *)&input_data, sizeof(TOBJ6000));
    }
exit:
    osThreadExit();
}

int32_t uart_protocol_set_rx_callback(struct uart_protocol *const self,
                                      uint32_t ID,
                                      const uint8_t *data,
                                      uint16_t *len,
                                      void *arg)
{
    return 0;
}
static int32_t module_data_recv_handle(rtm_module_info_t *const self,
                                       uint8_t *data,
                                       uint16_t len)
{
    payload_t *payload = (payload_t *)data;
    queue_frame_t queue_frame;
    osStatus_t status = osOK;
    switch (payload->type)
    {
    case 0x03: /*参数配置帧*/
    case 0x04: /*参数获取帧*/
    case 0x05: /*SET帧*/
    case 0x06: /*GET帧*/
    {
        memcpy(&queue_frame, payload, len);
        queue_frame.length = len;
        uint32_t ID = payload->id_ack >> 1;
        if (ID == 0x00) // 广播帧
        {
            for (int i = 0; i < RTM_MODULE_MAX; i++)
            {
                status = osMessageQueuePut(self->queue_group[i], &queue_frame, 0, 0);
                if (status != osOK)
                {
                    LOG_E("id %x queue put error, status = %d\r\n", self->id_group[i], status);
                }
            }
        }
        else // 单播或组播帧
        {
            for (int i = 0; i < RTM_MODULE_MAX; i++)
            {
                if (ID & self->id_group[i])
                {
                    status = osMessageQueuePut(self->queue_group[i], &queue_frame, 0, 0);
                    if (status != osOK)
                    {
                        LOG_E("id %x queue put error, status = %d\r\n", self->id_group[i], status);
                    }
                }
            }
        }
    }

    break;
    default:
        break;
    }
    return 0;
}
static void app_module_rx_thread(void *argument)
{
    uint8_t data[sizeof(payload_t)] = {0x00};
    rtm_module_info_t *self = (rtm_module_info_t *)argument;
    int32_t ret = 0;
    uint16_t len = 0;
    ret = uart_protocol_rx_RegisterCallback(&self->uart_protocol,
                                            UART_PROTOCOL_SET_RX_CB_ID,
                                            uart_protocol_set_rx_callback,
                                            NULL);
    if (ret != 0)
    {
        LOG_E("%s register callback error, ret = %d\r\n", self->module_name, ret);
        goto exit;
    }
    ret = uart_protocol_open(&self->uart_protocol);
    if (ret != 0)
    {
        LOG_E("%s open error, ret = %d\r\n", self->module_name, ret);
        goto exit;
    }

    for (;;)
    {
        ret = uart_protocol_recv(&self->uart_protocol, data, &len, 0xFFFFFFFF);
        if (ret != 0)
        {
            LOG_E("%s recv error, ret = %d\r\n", self->module_name, ret);
            continue;
        }
        ret = module_data_recv_handle(self, data, len);
        if (ret != 0)
        {
            LOG_E("%s recv handle error, ret = %d\r\n", self->module_name, ret);
            continue;
        }
    }
exit:
    osThreadExit();
}
static void app_module_tx_thread(void *argument)
{
    int32_t ret = 0;
    osStatus_t status = osOK;
    rtm_module_info_t *self = (rtm_module_info_t *)argument;
    queue_frame_t queue_frame;

    ret = uart_protocol_open(&self->uart_protocol);
    if (ret != 0)
    {
        LOG_E("%s open error, ret = %d\r\n", self->module_name, ret);
        goto exit;
    }
    for (;;)
    {
        status = osMessageQueueGet(self->module_queue, &queue_frame, NULL, 0xFFFFFFFF);
        if (status != osOK)
        {
            LOG_E("%s queue get error, status = %d\r\n", self->module_name, status);
            continue;
        }
        ret = uart_protocol_send(&self->uart_protocol, (uint8_t *)&queue_frame, queue_frame.length, 100);
        if (ret != 0)
        {
            // LOG_E("%s send error, ret = %d\r\n", self->module_name, ret);
            continue;
        }
    }
exit:
    osThreadExit();
}
int app_rtm_data_handle_create(void)
{
    int32_t ret = 0;
    osThreadAttr_t thread_attributes = {0};
    osThreadId_t threadHandle = NULL;
    app_rtm_main_t *self = &app_rtm;
    if (self == NULL)
    {
        return -1;
    }
    memset(self, 0, sizeof(app_rtm_main_t));

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_name = "RTM_ON_PLC";
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_name = "RTM_ON_ARM";
    self->rtm_module_info[RTM_MODULE_ICM].module_name = "ICM";
    self->rtm_module_info[RTM_MODULE_BGM].module_name = "BGM";
    self->rtm_module_info[RTM_MODULE_QAM].module_name = "QAM";
    // self->rtm_module_info[RTM_MODULE_BSM].module_name = "BSM";
    self->rtm_module_info[RTM_MODULE_RTM_OFF].module_name = "RTM_OFF";

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].ID = 0x01;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].ID = 0x02;
    self->rtm_module_info[RTM_MODULE_ICM].ID = 0x04;
    self->rtm_module_info[RTM_MODULE_BGM].ID = 0x08;
    self->rtm_module_info[RTM_MODULE_QAM].ID = 0x10;
    // self->rtm_module_info[RTM_MODULE_BSM].ID = 0x20;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].ID = 0x40 | 0x80 | 0x100 | 0x200 | 0x400;
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        for (uint8_t j = 0; j < RTM_MODULE_MAX; j++)
        {
            self->rtm_module_info[j].id_group[i] = self->rtm_module_info[i].ID;
        }
    }

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_priority = osPriorityRealtime;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_priority = osPriorityHigh;
    self->rtm_module_info[RTM_MODULE_ICM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_BGM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_QAM].module_priority = osPriorityNormal;
    // self->rtm_module_info[RTM_MODULE_BSM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].module_priority = osPriorityRealtime;

    // 远程模块信息赋值
    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_ICM].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_ICM].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_ICM].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_BGM].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_BGM].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_BGM].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_QAM].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_QAM].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_QAM].heartbeat_info_rx.FirmWareVersion = 0;
    // self->rtm_module_info[RTM_MODULE_BSM].heartbeat_info_rx.board_id = 0;
    // self->rtm_module_info[RTM_MODULE_BSM].heartbeat_info_rx.HardwareVersion = 0;
    // self->rtm_module_info[RTM_MODULE_BSM].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].heartbeat_info_rx.FirmWareVersion = 0;
    // 本地模块信息赋值
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        self->rtm_module_info[i].heartbeat_info_tx.board_id = 0;
        self->rtm_module_info[i].heartbeat_info_tx.HardwareVersion = 0;
        self->rtm_module_info[i].heartbeat_info_tx.FirmWareVersion = 0;
    }
    ret = ethercat_thread_init();
    if (ret != 0)
    {
        return -1;
    }
    osEventFlagsAttr_t event_attributes = {
        .name = "output_event"};

    self->ethercat_Event = osEventFlagsNew(&event_attributes);
    if (self->ethercat_Event == NULL)
    {
        return -1;
    }
    
    ret = app_dido_create(&self->app_dido);
    if (ret != 0)
    {
        return -1;
    }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_ICM].uart_protocol,
                             UART_DEV_NAME_USART2,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        return -2;
    }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_BGM].uart_protocol,
                             UART_DEV_NAME_USART3,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        return -3;
    }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_QAM].uart_protocol,
                             UART_DEV_NAME_UART4,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        return -4;
    }
    // ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_BSM].uart_protocol,
    //                          UART_DEV_NAME_USART3,
    //                          1000,
    //                          10000,
    //                          10000);
    // if (ret != 0)
    // {
    //     return -5;
    // }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_RTM_OFF].uart_protocol,
                             UART_DEV_NAME_UART5,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        printf("uart_protocol_init error, ret = %d", ret);
        return -6;
    }
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        osMessageQueueAttr_t queue_attributes = {
            .name = self->rtm_module_info[i].module_name,
        };
        self->rtm_module_info[i].module_queue = osMessageQueueNew(5, sizeof(queue_frame_t), NULL);
        if (self->rtm_module_info[i].module_queue == NULL)
        {
            return -7;
        }
        for (uint8_t j = 0; j < RTM_MODULE_MAX; j++)
        {
            self->rtm_module_info[j].queue_group[i] = self->rtm_module_info[i].module_queue;
        }
    }

    thread_attributes.name = "app_ethercat_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_priority;
    threadHandle = osThreadNew(app_ethercat_rx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -8;
    }
    thread_attributes.name = "app_ethercat_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_priority;
    threadHandle = osThreadNew(app_ethercat_tx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -9;
    }
    thread_attributes.name = "app_rtm_main_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_priority;
    app_rtm_main_threadId = osThreadNew(app_rtm_main_thread, self, &thread_attributes);
    if (app_rtm_main_threadId == NULL)
    {
        return -8;
    }
    thread_attributes.name = "app_icm_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_ICM].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_ICM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -10;
    }
    thread_attributes.name = "app_icm_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_ICM].module_priority;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_ICM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -11;
    }
    thread_attributes.name = "app_bgm_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_BGM].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_BGM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -12;
    }
    thread_attributes.name = "app_bgm_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_BGM].module_priority;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_BGM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -13;
    }
    thread_attributes.name = "app_qam_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_QAM].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_QAM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -14;
    }
    thread_attributes.name = "app_qam_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_QAM].module_priority;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_QAM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -15;
    }
    // thread_attributes.name = "app_bsm_rx_thread";
    // thread_attributes.stack_size = 1024 * 4;
    // thread_attributes.priority = self->rtm_module_info[RTM_MODULE_BSM].module_priority;
    // threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_BSM]), &thread_attributes);
    // if (threadHandle == NULL)
    // {
    //     return -16;
    // }
    // thread_attributes.name = "app_bsm_tx_thread";
    // thread_attributes.stack_size = 1024 * 4;
    // thread_attributes.priority = self->rtm_module_info[RTM_MODULE_BSM].module_priority;
    // threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_BSM]), &thread_attributes);
    // if (threadHandle == NULL)
    // {
    //     return -17;
    // }
    thread_attributes.name = "app_rtm_off_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_OFF].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_OFF]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -18;
    }
    thread_attributes.name = "app_rtm_off_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_OFF].module_priority;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_OFF]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -19;
    }
    return 0;
}
INIT_APP_EXPORT(app_rtm_data_handle_create)