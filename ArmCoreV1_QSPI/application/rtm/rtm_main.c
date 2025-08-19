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
#include "app_fkp.h"
#include "fdcan_port.h"
#include "app_cpg.h"
#include "hw_crc.h"
#include "timestamp.h"
#include "app_search.h"
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
    uint16_t reserved;
    uint8_t beam_ID;
    uint8_t reserved1;
    uint16_t radiation_index;
    uint32_t not_ready_event;
    uint32_t warning_interlock;
    uint32_t minor_interlock;
    uint32_t serious_interlock;
} __attribute__((aligned(1), packed)) rtm_status_t;

int32_t rtm_set_data_distribute(osMessageQueueId_t queue_id, uint32_t ID, uint8_t cmd, uint8_t *data, uint16_t len);
static app_rtm_main_t app_rtm;

osThreadId_t app_rtm_main_threadId;

#if 1
#include "shell.h"
static uint8_t state_require = 0;
static uint8_t state_current = 0;
static uint32_t not_ready = 0;
static uint32_t warning_interlock = 0;
static uint32_t minor_interlock = 0;
static uint32_t serious_interlock = 0;
static uint32_t run_cnt = 0;
static int8_t rtm_state_get(int argc, char *argv[])
{
    LOG_I("RTM state require: %d\r\n", state_require);
    LOG_I("RTM state current: %d\r\n", state_current);
    LOG_I("RTM not_ready: 0x%x\r\n", not_ready);
    LOG_I("RTM warning_interlock: 0x%x\r\n", warning_interlock);
    LOG_I("RTM minor_interlock: 0x%x\r\n", minor_interlock);
    LOG_I("RTM serious_interlock: 0x%x\r\n", serious_interlock);
    LOG_I("RTM run_cnt: %d\r\n", run_cnt);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(rtm_state_get, rtm_state_get, rtm state get);
#endif

static void module_tx_queue_state_bit_set(osMessageQueueId_t queue_id)
{
    if (queue_id == NULL)
    {
        return;
    }
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        if (queue_id == app_rtm.rtm_module_info[i].module_queue)
        {
            manage_info_status_word_set(&app_rtm.rtm_module_info[i].manage_info, MODULE_TX_QUEUE_STATE_BIT, 1);
            return;
        }
    }
}
#define RTM_MAIN_THREAD_CYCLE_MS (1)
static void app_rtm_main_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    osStatus_t status = osOK;
    queue_frame_t queue_frame;
    rtm_status_t rtm_status = {0};
    rtm_status_t rtm_status_old = {0};
    dido_structure_t dido_structure = {0};
    dido_structure_t dido_structure_old = {0};

    uint32_t current_time = 0;
    uint32_t last_time = 0;
    current_time = osKernelGetTickCount();
    last_time = current_time;

    uint8_t system_state_require = 0;
    Event_t rtm_event = {0};
    fault_table_init(&(self->fault_check));
    rtm_state_machine_ctor(&(self->state_machine), self);
    for (;;)
    {
        status = osMessageQueueGet(self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].module_queue, &queue_frame, NULL, 0);
        if (status == osOK)
        {
            uint8_t type = queue_frame.payload.type;
            uint8_t cmd = queue_frame.payload.data[0];
            switch (type)
            {
            case 0x03: /*参数配置帧*/
                break;
            case 0x04: /*参数获取帧*/
                break;
            case 0x05: /*SET帧*/
            {
                switch (cmd)
                {
                case RECEIVE_RTM_OFF_ARM_REQUIRE_STATE_CMD: /*系统状态设置*/
                {
                    system_state_require = queue_frame.payload.data[1];

                    self->PLC_info = *(uint16_t *)&(queue_frame.payload.data[3]);
                    uint32_t interlock_override = *(uint32_t *)&(queue_frame.payload.data[5]);
                    uint32_t unready_override = *(uint32_t *)&(queue_frame.payload.data[9]);
                    self->led_belt = *(uint16_t *)&(queue_frame.payload.data[13]);
                    fault_override_set(&(self->fault_check), unready_override, interlock_override);
                    // search_timeout_set(*(uint16_t *)&(queue_frame.payload.data[15]));
                }
                break;
                case RECEIVE_FAULT_CLEAR_CMD: /*故障清除*/
                {
                    if (queue_frame.payload.data[1] & 0x01)
                    {
                        rtm_event.sig = ERROR_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                    }
                }
                break;
                case RECEIVE_FKP_BUTTON_CMD: /*FKP按键*/
                {
#define FKP_KEY_VALUE_MASK 0x0020
#define FKP_KEY_VALUE_EN_MASK 0x0001
                    uint16_t fkp_key_value = *(uint16_t *)&(queue_frame.payload.data[1]);
                    if (((fkp_key_value & FKP_KEY_VALUE_MASK) != 0) && ((fkp_key_value & FKP_KEY_VALUE_EN_MASK) != 0))
                    {
                        // LOG_I("FKP KEY PRESS\r\n");
                        rtm_event.sig = MANUAL_ENTER_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                    }
                    else
                    {
                        // LOG_I("FKP KEY RELEASE\r\n");
                        rtm_event.sig = MANUAL_EXIT_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                    }
                }
                break;
                case RECEIVE_CPG_BUTTON_CMD: /*CPG按键*/
                {
#define CPG_KEY_VALUE_MASK 0x00001FAE
#define CPG_KEY_VALUE_EN_MASK 0x00000041
                    uint32_t cpg_key_value = *(uint32_t *)&(queue_frame.payload.data[1]);
                    if (((cpg_key_value & CPG_KEY_VALUE_MASK) != 0) && ((cpg_key_value & CPG_KEY_VALUE_EN_MASK) != 0))
                    {
                        rtm_event.sig = MANUAL_ENTER_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                    }
                    else
                    {
                        rtm_event.sig = MANUAL_EXIT_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                    }
                }
                break;
                case SEND_GMM_CURRENT_STATE_CMD: /*GMM状态*/
                {
                    self->gmm_current_state = *(uint8_t *)&(queue_frame.payload.data[1]);
                }
                break;
                case SEND_PSM_CURRENT_STATE_CMD: /*PAM状态*/
                {
                    self->psm_current_state = *(uint8_t *)&(queue_frame.payload.data[1]);
                }
                break;
                case SEND_GMM_PLC_INFO_CMD: /*GMM PLC信息*/
                {
                    uint8_t plc_info = *(uint8_t *)&(queue_frame.payload.data[1]);
                    if (plc_info & 0x01)
                    {
                        rtm_event.sig = MANUAL_ENTER_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                    }
                    else
                    {
                        rtm_event.sig = MANUAL_EXIT_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                    }
                }
                break;
                default:
                    break;
                }
                break;
            }
            case 0x06: /*GET帧*/
                break;
            default:
                break;
            }
        }
        search_state_machine(&self->app_dido);
        // 轮询状态机
        rtm_event.sig = system_state_require;
        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);

        rtm_event.sig = TIME_SIG;
        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);

        rtm_status.fsm_state_current = rtm_get_state(&(self->state_machine));
        fault_interlock_table_get(&self->fault_check, &rtm_status.not_ready_event);

        if (memcmp(&rtm_status_old, &rtm_status, sizeof(rtm_status_t)) != 0)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON].module_queue, RTM_ON_PLC_ID, SEND_RTM_OFF_ARM_CURRENT_STATE_CMD, (uint8_t *)&rtm_status, sizeof(rtm_status_t));
            memcpy(&rtm_status_old, &rtm_status, sizeof(rtm_status_t));
            last_time = osKernelGetTickCount();
        }
        // 周期上报状态
        current_time = osKernelGetTickCount();
        if (current_time - last_time > 1000)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON].module_queue, RTM_ON_PLC_ID, SEND_RTM_OFF_ARM_CURRENT_STATE_CMD, (uint8_t *)&rtm_status, sizeof(rtm_status_t));
            last_time = current_time;
        }
        // DIDO上报
        app_do_get(&(self->app_dido), &dido_structure);
        app_di_get(&(self->app_dido), &dido_structure);
        if (memcmp(&dido_structure_old, &dido_structure, sizeof(dido_structure_t)) != 0)
        {
            memcpy(&dido_structure_old, &dido_structure, sizeof(dido_structure_t));
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON].module_queue, RTM_ON_PLC_ID, SEND_RTM_OFF_ARM_DIDO_CMD, (uint8_t *)&dido_structure, sizeof(dido_structure_t));
            uint8_t rtm_off_info = 0;
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].module_queue, GMM_ID, SEND_RTM_OFF_INFO_CMD, (uint8_t *)&rtm_off_info, sizeof(rtm_off_info));
        }

#if 1
        state_require = system_state_require;
        state_current = rtm_status.fsm_state_current;

        not_ready = rtm_status.not_ready_event;
        warning_interlock = rtm_status.warning_interlock;
        minor_interlock = rtm_status.minor_interlock;
        serious_interlock = rtm_status.serious_interlock;
        run_cnt++;
#endif

        osDelay(RTM_MAIN_THREAD_CYCLE_MS);
    }
exit:
    osThreadExit();
}

int32_t rtm_set_data_distribute(osMessageQueueId_t queue_id, uint32_t ID, uint8_t cmd, uint8_t *data, uint16_t len)
{
    if (queue_id == NULL || data == NULL || len == 0 || len > UART_PROTOCOL_DATA_MAX_LENGTH)
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
        module_tx_queue_state_bit_set(queue_id);
        LOG_E("RTM module tx queue full, ID:%d, cmd:%d, len:%d\r\n", ID, cmd, len);
        return -2;
    }
    return 0;
}
int32_t app_data_record_from_ethercat(rtm_module_info_t *self, uint32_t ID, uint8_t cmd, uint8_t *data, uint16_t len)
{
    if (self == NULL || data == NULL || len == 0 || len > UART_PROTOCOL_DATA_MAX_LENGTH)
    {
        return -1;
    }
    payload_t payload;
    payload.id_ack = ID << 1 | 0x01;
    payload.type = 0x05;
    payload.length = len + 1;
    payload.data[0] = cmd;
    memcpy(payload.data + 1, data, len);
    uint32_t length = payload.length + sizeof(payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH;
    int32_t ret = app_data_record(self->app_data_record, self->ID, &payload, length);
    if (ret != 0)
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
void app_rtm_ethercat_state_op_set(void)
{
    manage_info_status_word_set(&app_rtm.rtm_ethercat_info.manage_info, ETHERCAT_OP_STATE_BIT, 1);
}
void app_rtm_ethercat_state_op_clean(void)
{
    manage_info_status_word_set(&app_rtm.rtm_ethercat_info.manage_info, ETHERCAT_OP_STATE_BIT, 0);
}
enum
{
    OUTPUT_DATA_GMM_CURRENT_STATE,
    OUTPUT_DATA_GMM_MOVE_STATUS,
    OUTPUT_DATA_PSM_CURRENT_STATE,
    OUTPUT_DATA_PSM_MOVE_STATUS,
};
static void ethercat_output_data_distribute(rtm_module_info_t *const self, TOBJ7010 *data)
{
    static TOBJ7010 output_data = {0};
    static uint8_t flag = OUTPUT_DATA_GMM_CURRENT_STATE;
    uint16_t len = 0;
    uint32_t current_time = 0;
    static uint32_t last_time = 0;

    if (self->module_queue == NULL || data == NULL)
    {
        return;
    }
    current_time = osKernelGetTickCount();
    if (memcmp(&output_data, data, sizeof(TOBJ7010)) == 0)
    {
        if (current_time - last_time < 200)
        {
            return;
        }
        last_time = current_time;
        switch (flag)
        {
        case OUTPUT_DATA_GMM_CURRENT_STATE:
            flag = OUTPUT_DATA_GMM_MOVE_STATUS;
            len = (uint8_t *)&output_data.OutU32_gmm_move_status - (uint8_t *)&output_data.OutU8_gmm_fsm_state_current;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON], RTM_ON_PLC_ID | RTM_OFF_ARM_ID, SEND_GMM_CURRENT_STATE_CMD, &data->OutU8_gmm_fsm_state_current, len);
            app_data_record_from_ethercat(self, RTM_ON_PLC_ID | RTM_OFF_ARM_ID, SEND_GMM_CURRENT_STATE_CMD, &data->OutU8_gmm_fsm_state_current, len);
            break;
        case OUTPUT_DATA_GMM_MOVE_STATUS:
            flag = OUTPUT_DATA_GMM_CURRENT_STATE;
            len = (uint8_t *)&output_data.OutU8_psm_fsm_state_current - (uint8_t *)&output_data.OutU32_gmm_move_status;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON], ICM_ID | RTM_ON_PLC_ID | QAM_ID | BGM_ID, SEND_GMM_INFO_CMD, &data->OutU32_gmm_move_status, len);
            app_data_record_from_ethercat(self, ICM_ID | RTM_ON_PLC_ID | QAM_ID | BGM_ID, SEND_GMM_INFO_CMD, &data->OutU32_gmm_move_status, len);
            break;
        default:
            break;
        }
    }
    else
    {
        len = (uint8_t *)&output_data.OutU8_ethercat_Link_state - (uint8_t *)&output_data.OutU16_radiation_index;
        if (memcmp(&output_data.OutU16_radiation_index, &data->OutU16_radiation_index, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON], BROADCAST_ID, SEND_GMM_RADIATION_INDEX_CMD, &data->OutU16_radiation_index, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_PSM], BROADCAST_ID, SEND_GMM_RADIATION_INDEX_CMD, &data->OutU8_beam_id, len);

            app_data_record_from_ethercat(self, BROADCAST_ID, SEND_GMM_RADIATION_INDEX_CMD, &data->OutU16_radiation_index, len);
        }

        len = (uint8_t *)&output_data.OutU32_gmm_move_status - (uint8_t *)&output_data.OutU8_gmm_fsm_state_current;
        if (memcmp(&output_data.OutU8_gmm_fsm_state_current, &data->OutU8_gmm_fsm_state_current, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON], RTM_ON_PLC_ID | RTM_OFF_ARM_ID, SEND_GMM_CURRENT_STATE_CMD, &data->OutU8_gmm_fsm_state_current, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF_ARM], RTM_ON_PLC_ID | RTM_OFF_ARM_ID, SEND_GMM_CURRENT_STATE_CMD, &data->OutU8_gmm_fsm_state_current, len);
            app_data_record_from_ethercat(self, RTM_ON_PLC_ID | RTM_OFF_ARM_ID, SEND_GMM_CURRENT_STATE_CMD, &data->OutU8_gmm_fsm_state_current, len);
        }

        len = (uint8_t *)&output_data.OutU8_psm_fsm_state_current - (uint8_t *)&output_data.OutU32_gmm_move_status;
        if (memcmp(&output_data.OutU32_gmm_move_status, &data->OutU32_gmm_move_status, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON], ICM_ID | RTM_ON_PLC_ID | QAM_ID | BGM_ID, SEND_GMM_INFO_CMD, &data->OutU32_gmm_move_status, len);
            app_data_record_from_ethercat(self, ICM_ID | RTM_ON_PLC_ID | QAM_ID | BGM_ID, SEND_GMM_INFO_CMD, &data->OutU32_gmm_move_status, len);
        }

        len = (uint8_t *)&output_data.OutU8_gmm_fsm_state_current - (uint8_t *)&output_data.OutU8_plc_info;
        if (memcmp(&output_data.OutU8_plc_info, &data->OutU8_plc_info, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF_ARM], RTM_OFF_ARM_ID, SEND_GMM_PLC_INFO_CMD, &data->OutU8_plc_info, len);
            app_data_record_from_ethercat(self, RTM_OFF_ARM_ID, SEND_GMM_PLC_INFO_CMD, &data->OutU8_plc_info, len);
        }
        memcpy(&output_data, data, sizeof(TOBJ7010));
    }
}

static void ethercat_input_data_distribute(rtm_module_info_t *const self, TOBJ6000 *data, queue_frame_t *queue_frame)
{
    TOBJ6000 *input_data = data;

    if (queue_frame == NULL || data == NULL)
    {
        return;
    }

    if ((0 == ((queue_frame->payload.id_ack >> 1) & self->ID)) &&
        ((queue_frame->payload.id_ack >> 1) != 0x00))
    {
        return;
    }
    uint8_t cmd = queue_frame->payload.data[0];
    uint16_t len = queue_frame->payload.length - 1;
    // TODO:做长度判断
    switch (cmd)
    {
    case RECEIVE_BEAM_ID_CMD:
        memcpy(&input_data->InU8_beam_id, queue_frame->payload.data + 1, len);
        break;
    case RECEIVE_STATE_SYNC_CMD:
        memcpy(&input_data->InU8_state_sync, queue_frame->payload.data + 1, len);
        break;
    case RECEIVE_RADIATION_INDEX_CMD:
        memcpy(&input_data->InU16_radiation_index, queue_frame->payload.data + 1, len);
        break;
    case RECEIVE_FAULT_CLEAR_CMD:
        memcpy(&input_data->InU8_fault_clear, queue_frame->payload.data + 1, len);
        break;
    case RECEIVE_SYSTEM_STATE_CMD:
        break;
    case RECEIVE_GMM_REQUIRE_STATE_CMD:
        memcpy(&input_data->InU8_gmm_require_state, queue_frame->payload.data + 1, len);
        break;
    case RECEIVE_GMM_CTRL_CMD:
        memcpy(&input_data->InF_gmm_position_tar, queue_frame->payload.data + 1, len);
        break;
    case SEND_RTM_OFF_INFO_CMD:
        memcpy(&input_data->InU8_rtm_off_info, queue_frame->payload.data + 1, len);
        break;
    case RECEIVE_PSM_REQUIRE_STATE_CMD:
        memcpy(&input_data->InU8_psm_require_state, queue_frame->payload.data + 1, len);
        break;
    case RECEIVE_PSM_CTRL_CMD:
        memcpy(&input_data->InF_psm_position_x_tar, queue_frame->payload.data + 1, len);
        break;
    case RECEIVE_FKP_BUTTON_CMD:
        memcpy(&input_data->InU16_FkpButton, queue_frame->payload.data + 1, len);
        break;
    case RECEIVE_CPG_BUTTON_CMD:
        memcpy(&input_data->InU32_CpgButton, queue_frame->payload.data + 1, len);
        break;
    default:
        LOG_E("unknown cmd %x\r\n", cmd);
        break;
    }
}
static void app_ethercat_rx_thread(void *argument)
{
    uint32_t ret = 0;
    rtm_module_info_t *self = (rtm_module_info_t *)argument;
    TOBJ7010 output_data = {0};
    uint8_t ethercat_Link_state = 0;
    uint32_t current_time = 0;
    uint32_t last_time = 0;
    current_time = osKernelGetTickCount();
    last_time = current_time;
    for (;;)
    {
        ret = osEventFlagsWait(app_rtm.ethercat_Event, APP_RTM_EVENT_FLAG_OUTPUT, osFlagsWaitAny, 500);
        if (self->tx_disable == MODULE_TX_DISABLE)
        {
            continue;
        }
        if (ret & APP_RTM_EVENT_FLAG_OUTPUT)
        {
            ethercat_recv_data_get((uint16_t *)&output_data, sizeof(TOBJ7010));

            if (ethercat_Link_state != output_data.OutU8_ethercat_Link_state)
            {
                ethercat_Link_state = output_data.OutU8_ethercat_Link_state;
                last_time = osKernelGetTickCount();
                manage_info_status_word_set(&self->manage_info, MODULE_LINK_STATE_BIT, 0);
            }
            // TODO: 处理输出数据
            ethercat_output_data_distribute(self, &output_data);
        }
        current_time = osKernelGetTickCount();
        if (current_time - last_time > 1000)
        {
            last_time = current_time;
            manage_info_status_word_set(&self->manage_info, MODULE_LINK_STATE_BIT, 1);
        }
    }
exit:
    manage_info_status_word_set(&self->manage_info, MODULE_RX_INIT_BIT, 1);
    osThreadExit();
}

static void app_ethercat_tx_thread(void *argument)
{
    int32_t ret = 0;
    osStatus_t status = osOK;
    rtm_module_info_t *self = (rtm_module_info_t *)argument;
    queue_frame_t queue_frame;
    uint32_t current_time = 0;
    uint32_t last_time = 0;
    current_time = osKernelGetTickCount();
    last_time = current_time;
    TOBJ6000 input_data = {0};
    for (;;)
    {
        status = osMessageQueueGet(self->module_queue, &queue_frame, NULL, 100);
        if (status == osOK)
        {
            app_data_record(self->app_data_record, self->ID, &queue_frame.payload, queue_frame.length);
            ethercat_input_data_distribute(self, &input_data, &queue_frame);
            ethercat_send_data_update((uint16_t *)&input_data, sizeof(TOBJ6000));
        }
        current_time = osKernelGetTickCount();
        if (current_time - last_time > 500)
        {
            last_time = current_time;
            input_data.InU8_ethercat_Link_state = !input_data.InU8_ethercat_Link_state;
            ethercat_send_data_update((uint16_t *)&input_data, sizeof(TOBJ6000));
        }
    }
exit:
    manage_info_status_word_set(&self->manage_info, MODULE_TX_INIT_BIT, 1);
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

int32_t uart_protocol_pnt_rx_callback(struct uart_protocol *const self,
                                      uint32_t ID,
                                      const uint8_t *data,
                                      uint16_t *len,
                                      void *arg)
{
    uint64_t timestamp_ns = 0;
    memcpy(&timestamp_ns, data, *len);
    timestamp_ns_set(timestamp_ns);
    return 0;
}
int32_t uart_protocol_reboot_rx_callback(struct uart_protocol *const self,
                                         uint32_t ID,
                                         const uint8_t *data,
                                         uint16_t *len,
                                         void *arg)
{
    app_rtm_main_t *app_rtm_main = (app_rtm_main_t *)arg;

    if (ID == RTM_OFF_ARM_ID)
    {
        HAL_NVIC_SystemReset(); // reset system
    }
    else
    {
        // uart_protocol_reboot(&(app_rtm_main->rtm_module_info[ID].uart_protocol), ID, 1000);
    }
    return 0;
}
int32_t uart_protocol_heartbeat_rx_callback(struct uart_protocol *const self,
                                            uint32_t ID,
                                            const uint8_t *data,
                                            uint16_t *len,
                                            void *arg)
{
    rtm_module_info_t *rtm_module_info = (rtm_module_info_t *)arg;
    heartbeat_t *heartbeat = (heartbeat_t *)data;

    if (*len > sizeof(heartbeat_t))
    {
        LOG_E("%s heartbeat rx len err!\r\n", rtm_module_info->module_name);
    }

    // if (memcmp(&(rtm_module_info->heartbeat_info_rx), heartbeat, sizeof(heartbeat_t)) != 0)
    // {
    //     LOG_E("%s heartbeat rx err!\r\n", rtm_module_info->module_name);
    // }
    manage_info_status_word_set(&rtm_module_info->manage_info, MODULE_LINK_STATE_BIT, 0);
    app_rtm_thread_flag_set(rtm_module_info->module_thread_flags);
    return 0;
}
int32_t uart_protocol_heartbeat_rx_timeout_callback(struct uart_protocol *const self,
                                                    uint32_t ID,
                                                    const uint8_t *data,
                                                    uint16_t *len,
                                                    void *arg)
{
    rtm_module_info_t *rtm_module_info = (rtm_module_info_t *)arg;
    manage_info_status_word_set(&rtm_module_info->manage_info, MODULE_LINK_STATE_BIT, 1);
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
                if (self->ID == self->id_group[i])
                {
                    continue;
                }
                status = osMessageQueuePut(self->queue_group[i], &queue_frame, 0, 0);
                if (status != osOK)
                {
                    module_tx_queue_state_bit_set(self->queue_group[i]);
                    LOG_E("id %x queue put error, status = %d\r\n", self->id_group[i], status);
                }
            }
        }
        else // 单播或组播帧
        {
            for (int i = 0; i < RTM_MODULE_MAX; i++)
            {
                if (self->ID == self->id_group[i])
                {
                    continue;
                }
                if (ID & self->id_group[i])
                {
                    status = osMessageQueuePut(self->queue_group[i], &queue_frame, 0, 0);
                    if (status != osOK)
                    {
                        module_tx_queue_state_bit_set(self->queue_group[i]);
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
    if (strcmp(self->module_type, "SLAVE") == 0)
    {
        ret = uart_protocol_rx_RegisterCallback(&self->uart_protocol,
                                                UART_PROTOCOL_PNT_RX_CB_ID,
                                                uart_protocol_pnt_rx_callback,
                                                NULL);
        if (ret != 0)
        {
            LOG_E("%s register callback error, ret = %d\r\n", self->module_name, ret);
            goto exit;
        }
        ret = uart_protocol_rx_RegisterCallback(&self->uart_protocol,
                                                UART_PROTOCOL_REBOOT_RX_CB_ID,
                                                uart_protocol_reboot_rx_callback,
                                                &app_rtm);
        if (ret != 0)
        {
            LOG_E("%s register callback error, ret = %d\r\n", self->module_name, ret);
            goto exit;
        }
    }
    ret = uart_protocol_rx_RegisterCallback(&self->uart_protocol,
                                            UART_PROTOCOL_HEARTBEAT_RX_CB_ID,
                                            uart_protocol_heartbeat_rx_callback,
                                            self);
    if (ret != 0)
    {
        LOG_E("%s register callback error, ret = %d\r\n", self->module_name, ret);
        goto exit;
    }
    ret = uart_protocol_rx_RegisterCallback(&self->uart_protocol,
                                            UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID,
                                            uart_protocol_heartbeat_rx_timeout_callback,
                                            self);
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
            manage_info_status_word_set(&self->manage_info, MODULE_RX_STATE_BIT, 1);
            LOG_E("%s recv error, ret = %d\r\n", self->module_name, ret);
            continue;
        }
        manage_info_status_word_set(&self->manage_info, MODULE_RX_STATE_BIT, 0);
        ret = module_data_recv_handle(self, data, len);
        if (ret != 0)
        {
            LOG_E("%s recv handle error, ret = %d\r\n", self->module_name, ret);
            continue;
        }
        ret = app_data_record(self->app_data_record, self->ID, data, len);
        if (ret != 0)
        {
            LOG_E("%s data record send error, ret = %d\r\n", self->module_name, ret);
        }
    }
exit:
    manage_info_status_word_set(&self->manage_info, MODULE_RX_INIT_BIT, 1);
    LOG_E("%s rx thread exit\r\n", self->module_name);
    osThreadExit();
}

int32_t uart_protocol_pnt_tx_callback(struct uart_protocol *const self,
                                      uint8_t *data,
                                      uint16_t *len,
                                      void *arg)
{
    uint64_t timestamp_ns = timestamp_ns_get();
    memcpy(data, &timestamp_ns, sizeof(uint64_t));
    *len = sizeof(uint64_t);
    return 0;
}

int32_t uart_protocol_heartbeat_tx_callback(struct uart_protocol *const self,
                                            uint8_t *data,
                                            uint16_t *len,
                                            void *arg)
{
    rtm_module_info_t *rtm_module_info = (rtm_module_info_t *)arg;
    memcpy(data, &(rtm_module_info->heartbeat_info_tx), sizeof(uint64_t));
    *len = sizeof(rtm_module_info->heartbeat_info_tx);
    return 0;
}

static void app_module_tx_thread(void *argument)
{
    int32_t ret = 0;
    osStatus_t status = osOK;
    rtm_module_info_t *self = (rtm_module_info_t *)argument;
    queue_frame_t queue_frame;

    if (strcmp(self->module_type, "MASTER") == 0)
    {
        ret = uart_protocol_tx_RegisterCallback(&self->uart_protocol,
                                                UART_PROTOCOL_PNT_TX_CB_ID,
                                                uart_protocol_pnt_tx_callback,
                                                NULL);
        if (ret != 0)
        {
            LOG_E("%s register callback error, ret = %d\r\n", self->module_name, ret);
            goto exit;
        }
    }

    ret = uart_protocol_tx_RegisterCallback(&self->uart_protocol,
                                            UART_PROTOCOL_HEARTBEAT_TX_CB_ID,
                                            uart_protocol_heartbeat_tx_callback,
                                            self);
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
        status = osMessageQueueGet(self->module_queue, &queue_frame, NULL, 0xFFFFFFFF);
        if (status != osOK)
        {
            LOG_E("%s queue get error, status = %d\r\n", self->module_name, status);
            continue;
        }
        if (self->tx_disable == MODULE_TX_DISABLE)
        {
            continue;
        }
        ret = app_data_record(self->app_data_record, self->ID, (uint8_t *)&queue_frame, queue_frame.length);
        if (ret != 0)
        {
            LOG_E("%s data record send error, ret = %d\r\n", self->module_name, ret);
        }
        ret = uart_protocol_send(&self->uart_protocol, (uint8_t *)&queue_frame, queue_frame.length, 100);
        if (ret != 0)
        {
            manage_info_status_word_set(&self->manage_info, MODULE_TX_STATE_BIT, 1);
            LOG_E("%s send error, ret = %d\r\n", self->module_name, ret);
            continue;
        }
        manage_info_status_word_set(&self->manage_info, MODULE_TX_STATE_BIT, 0);
    }
exit:
    manage_info_status_word_set(&self->manage_info, MODULE_TX_INIT_BIT, 1);
    osThreadExit();
}
static void app_fkp_rx_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    device_err_t device_err = DEV_EOK;
    uint32_t crc = 0;
    uint16_t rx_len = sizeof(fkp_recv_structure_t) + sizeof(serial_frame_format_t) + sizeof(crc);
    int32_t ret = 0;
    struct
    {
        serial_frame_format_t format;
        fkp_recv_structure_t fkp_recv_structure;
        uint32_t crc;
    } __attribute__((aligned(1), packed)) recv_data = {0}, recv_data_bak = {0};

    device_err = dev_uart_open(self->uart_fkp);
    if (device_err != DEV_EOK)
    {
        LOG_E("uart_fkp open error, device_err = %d\r\n", device_err);
        goto exit;
    }
    for (;;)
    {
        device_err = dev_uart_recv(self->uart_fkp, &recv_data, sizeof(recv_data), 0xFFFFFFFF);
        if (device_err != DEV_EOK)
        {
            LOG_E("uart_fkp recv error, device_err = %d\r\n", device_err);
            continue;
        }
        crc = hardware_crc_calculate(CRC32, (uint8_t *)&recv_data + 2, rx_len - 6);
        crc ^= 0xFFFFFFFF;

        if (crc != *(uint32_t *)((uint8_t *)&recv_data + rx_len - 4))
        {
            continue;
        }
        // LOG_I("FkpButton:%x\r\n", recv_data.fkp_recv_structure.FkpButton);
        if (memcmp(&recv_data, &recv_data_bak, sizeof(recv_data)) != 0)
        {
            memcpy(&recv_data_bak, &recv_data, sizeof(recv_data));
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_FKP].queue_group[RTM_MODULE_RTM_OFF_PLC],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          RECEIVE_FKP_BUTTON_CMD,
                                          &recv_data.fkp_recv_structure.FkpButton,
                                          2);
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_FKP].queue_group[RTM_MODULE_RTM_ON],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          RECEIVE_FKP_BUTTON_CMD,
                                          &recv_data.fkp_recv_structure.FkpButton,
                                          2);
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_FKP].queue_group[RTM_MODULE_RTM_OFF_ARM],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          RECEIVE_FKP_BUTTON_CMD,
                                          &recv_data.fkp_recv_structure.FkpButton,
                                          2);
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_FKP].queue_group[RTM_MODULE_PSM],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          RECEIVE_FKP_BUTTON_CMD,
                                          &recv_data.fkp_recv_structure.FkpButton,
                                          2);
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
            // LOG_I("FkpButton:%x\r\n", recv_data.fkp_recv_structure.FkpButton);
        }
    }
exit:
    osThreadExit();
}
static void app_fkp_tx_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    device_err_t device_err = DEV_EOK;
    queue_frame_t queue_frame;
    osStatus_t status = osOK;
    uint32_t crc = 0;
    uint16_t tx_len = sizeof(fkp_send_structure_t) + sizeof(serial_frame_format_t) + sizeof(crc);
    struct
    {
        serial_frame_format_t format;
        fkp_send_structure_t fkp_send_structure;
        uint32_t crc;
    } __attribute__((aligned(1), packed)) send_data = {0}, send_data_bak = {0};
    send_data.format.header = 0xAA55;
    device_err = dev_uart_open(self->uart_fkp);
    if (device_err != DEV_EOK)
    {
        LOG_E("uart_fkp open error, device_err = %d\r\n", device_err);
        goto exit;
    }

    for (;;)
    {
        status = osMessageQueueGet(self->rtm_module_info[RTM_MODULE_FKP].module_queue, &queue_frame, NULL, 0xFFFFFFFF);
        if (status != osOK)
        {
            LOG_E("%s queue get error, status = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, status);
            continue;
        }
        uint8_t type = queue_frame.payload.type;
        uint8_t cmd = queue_frame.payload.data[0];
        uint16_t len = queue_frame.payload.length - 1;
        if (type == 0x05)
        {
            switch (cmd)
            {
            case RECEIVE_FKP_LED_BLINK_CMD:
                memcpy(&send_data.fkp_send_structure.FkpLedBlink, queue_frame.payload.data + 1, len);
                break;
            case RECEIVE_FKP_VIBRATION_CMD:
                memcpy(&send_data.fkp_send_structure.Vibration, queue_frame.payload.data + 1, len);
                break;
            case RECEIVE_FKP_TIMESTAMP_CMD:
                memcpy(&send_data.fkp_send_structure.year, queue_frame.payload.data + 1, len);
                break;
            case RECEIVE_SYSTEM_STATE_CMD:
                memcpy(&send_data.fkp_send_structure.SystemCurrentState, queue_frame.payload.data + 1, len);
                break;
            case SEND_FKP_POWER_OFF_CMD:
                memcpy(&send_data.fkp_send_structure.power_off, queue_frame.payload.data + 1, len);
                break;
            case RECEIVE_FKP_DOSE_CMD:
                memcpy(&send_data.fkp_send_structure.TotalDose, queue_frame.payload.data + 1, 8);
                break;
            default:
                continue;
                break;
            }
        }
        send_data.fkp_send_structure.beep = 70;
        crc = hardware_crc_calculate(CRC32, (uint8_t *)&send_data + 2, tx_len - 6);
        crc ^= 0xFFFFFFFF;
        send_data.crc = crc;

        if (memcmp(&send_data, &send_data_bak, sizeof(send_data)) != 0)
        {
            memcpy(&send_data_bak, &send_data, sizeof(send_data));
            device_err = dev_uart_send(self->uart_fkp, (uint8_t *)&send_data, tx_len, 100);
            if (device_err != DEV_EOK)
            {
                continue;
            }
#if 0
            LOG_I("SystemCurrentState:0x%x\r\n", send_data.fkp_send_structure.SystemCurrentState);
            LOG_I("TotalDose:%f\r\n", send_data.fkp_send_structure.TotalDose);
            LOG_I("DeliveredDose:%f\r\n", send_data.fkp_send_structure.DeliveredDose);
            LOG_I("FkpLedBlink:0x%x\r\n", send_data.fkp_send_structure.FkpLedBlink);
            LOG_I("Vibration:0x%x\r\n", send_data.fkp_send_structure.Vibration);
            LOG_I("beep:%d\r\n", send_data.fkp_send_structure.beep);
            LOG_I("power_off:0x%x\r\n", send_data.fkp_send_structure.power_off);
            LOG_I("year:%d\r\n", send_data.fkp_send_structure.year);
            LOG_I("month:%d\r\n", send_data.fkp_send_structure.month);
            LOG_I("day:%d\r\n", send_data.fkp_send_structure.day);
            LOG_I("hour:%d\r\n", send_data.fkp_send_structure.hour);
            LOG_I("minute:%d\r\n", send_data.fkp_send_structure.minute);
            LOG_I("fractions:%d\r\n", send_data.fkp_send_structure.fractions);
#endif
        }
    }
exit:
    osThreadExit();
}
static void app_cpg_rx_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    uint16_t rx_len = sizeof(cpg_recv_structure_t);
    int8_t ret = 0;
    struct fdcan_rx_msg msg = {0};
    uint32_t CpgButton = 0;
    uint32_t CpgButton_bak = 0;
    uint32_t CpgButton_1 = 0;
    uint32_t CpgButton_2 = 0;
    for (;;)
    {
        ret = fdcan1_data_read(&msg, 0xFFFFFFFF);
        if (ret != 0)
        {
            LOG_E("fdcan1_data_read error, ret = %d\r\n", ret);
            continue;
        }
        if ((msg.header.Identifier != 0x11) && (msg.header.Identifier != 0x12))
        {
            continue;
        }
        if (msg.header.Identifier == 0x11)
        {
            CpgButton_1 = (*(cpg_recv_structure_t *)&msg.buf).CpgButton;
        }
        else if (msg.header.Identifier == 0x12)
        {
            CpgButton_2 = (*(cpg_recv_structure_t *)&msg.buf).CpgButton;
        }
        CpgButton = CpgButton_1 | CpgButton_2;

        if (CpgButton != CpgButton_bak)
        {
            CpgButton_bak = CpgButton;
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_OFF_PLC],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          SEND_CPG_BUTTON_CMD,
                                          &CpgButton,
                                          sizeof(CpgButton));
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_ON],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          SEND_CPG_BUTTON_CMD,
                                          &CpgButton,
                                          sizeof(CpgButton));
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_OFF_ARM],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          SEND_CPG_BUTTON_CMD,
                                          &CpgButton,
                                          sizeof(CpgButton));
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_PSM],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          SEND_CPG_BUTTON_CMD,
                                          &CpgButton,
                                          sizeof(CpgButton));
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
        }
    }
exit:
    osThreadExit();
}
static void app_cpg_tx_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    queue_frame_t queue_frame;
    osStatus_t status = osOK;
    int8_t ret = 0;
    uint16_t tx_len = sizeof(cpg_send_structure_t);
    cpg_send_structure_t send_data = {0}, send_data_bak = {0};
    memset(&send_data, 0, sizeof(cpg_send_structure_t));
    memset(&send_data_bak, 0, sizeof(cpg_send_structure_t));

    send_data.OffGantryUnitInfo = 0xFFFF;
    send_data.BoardID = 0x01;
    send_data.HardwareVersion = 0x01;
    send_data.FirmWareVersion = 0x00000001;
    for (;;)
    {
        status = osMessageQueueGet(self->rtm_module_info[RTM_MODULE_CPG].module_queue, &queue_frame, NULL, 0xFFFFFFFF);
        if (status != osOK)
        {
            LOG_E("%s queue get error, status = %d\r\n", self->rtm_module_info[RTM_MODULE_CPG].module_name, status);
            continue;
        }

        uint8_t type = queue_frame.payload.type;
        uint8_t cmd = queue_frame.payload.data[0];
        uint16_t len = queue_frame.payload.length - 1;
        if (type == 0x05)
        {
            switch (cmd)
            {
            case RECEIVE_CPG_LED_BLINK_CMD:
                memcpy(&(send_data.CpgLedBlink), queue_frame.payload.data + 1, queue_frame.payload.length);
                break;
            case RECEIVE_SYSTEM_STATE_CMD:
                memcpy(&(send_data.SystemCurrentState), queue_frame.payload.data + 1, queue_frame.payload.length);
                break;
            default:
                continue;
                break;
            }
        }

        if (memcmp(&send_data, &send_data_bak, sizeof(send_data)) != 0)
        {
            memcpy(&send_data_bak, &send_data, sizeof(send_data));

            ret = fdcan1_data_write(0x01, (uint8_t *)&send_data, sizeof(send_data));
            if (ret != 0)
            {
                LOG_E("fdcan1_data_write error,id = 0x01, ret = %d\r\n", ret);
            }
            ret = fdcan1_data_write(0x02, (uint8_t *)&send_data, sizeof(send_data));
            if (ret != 0)
            {
                LOG_E("fdcan1_data_write error,id = 0x02, ret = %d\r\n", ret);
            }
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
    device_err_t device_err = DEV_EOK;
    if (self == NULL)
    {
        return -1;
    }
    memset(self, 0, sizeof(app_rtm_main_t));

    self->rtm_module_info[RTM_MODULE_RTM_ON].module_name = "RTM_ON";
    // self->rtm_module_info[RTM_MODULE_GMM].module_name = "GMM";
    self->rtm_module_info[RTM_MODULE_PSM].module_name = "PSM";
    self->rtm_module_info[RTM_MODULE_FKP].module_name = "FKP";
    self->rtm_module_info[RTM_MODULE_CPG].module_name = "CPG";
    self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].module_name = "RTM_OFF_ARM";
    self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].module_name = "RTM_OFF_PLC";

    self->rtm_module_info[RTM_MODULE_RTM_ON].module_type = "SLAVE";
    // self->rtm_module_info[RTM_MODULE_GMM].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_PSM].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_FKP].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_CPG].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].module_type = "MASTER";

    self->rtm_module_info[RTM_MODULE_RTM_ON].ID = RTM_ON_PLC_ID | RTM_ON_ARM_ID | ICM_ID | BGM_ID | QAM_ID | BSM_ID;
    // self->rtm_module_info[RTM_MODULE_GMM].ID = GMM_ID;
    self->rtm_module_info[RTM_MODULE_PSM].ID = PSM_ID;
    self->rtm_module_info[RTM_MODULE_FKP].ID = FKP_ID;
    self->rtm_module_info[RTM_MODULE_CPG].ID = CPG_ID;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].ID = RTM_OFF_ARM_ID;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].ID = GMM_ID;
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        for (uint8_t j = 0; j < RTM_MODULE_MAX; j++)
        {
            self->rtm_module_info[j].id_group[i] = self->rtm_module_info[i].ID;
        }
    }

    self->rtm_module_info[RTM_MODULE_RTM_ON].module_priority = osPriorityAboveNormal;
    // self->rtm_module_info[RTM_MODULE_GMM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_PSM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_FKP].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_CPG].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].module_priority = osPriorityAboveNormal;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].module_priority = osPriorityAboveNormal;

    self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].module_thread_flags = APP_RTM_THREAD_FLAG_ETHERCAT_READY;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].module_thread_flags = 0;
    self->rtm_module_info[RTM_MODULE_PSM].module_thread_flags = APP_RTM_THREAD_FLAG_PSM_READY;
    // self->rtm_module_info[RTM_MODULE_GMM].module_thread_flags = APP_RTM_THREAD_FLAG_GMM_READY;
    self->rtm_module_info[RTM_MODULE_FKP].module_thread_flags = APP_RTM_THREAD_FLAG_FKP_READY;
    self->rtm_module_info[RTM_MODULE_CPG].module_thread_flags = APP_RTM_THREAD_FLAG_CPG_READY;
    self->rtm_module_info[RTM_MODULE_RTM_ON].module_thread_flags = APP_RTM_THREAD_FLAG_RTM_ON_READY;

    self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].tx_disable = MODULE_TX_ENABLE;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].tx_disable = MODULE_TX_DISABLE;
    self->rtm_module_info[RTM_MODULE_PSM].tx_disable = MODULE_TX_DISABLE;
    self->rtm_module_info[RTM_MODULE_FKP].tx_disable = MODULE_TX_DISABLE;
    self->rtm_module_info[RTM_MODULE_CPG].tx_disable = MODULE_TX_DISABLE;
    // self->rtm_module_info[RTM_MODULE_GMM].tx_disable = MODULE_TX_DISABLE;
    self->rtm_module_info[RTM_MODULE_RTM_ON].tx_disable = MODULE_TX_ENABLE;
    // 远程模块信息赋值
    self->rtm_module_info[RTM_MODULE_RTM_ON].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_RTM_ON].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_ON].heartbeat_info_rx.FirmWareVersion = 0;
    // self->rtm_module_info[RTM_MODULE_GMM].heartbeat_info_rx.board_id = 0;
    // self->rtm_module_info[RTM_MODULE_GMM].heartbeat_info_rx.HardwareVersion = 0;
    // self->rtm_module_info[RTM_MODULE_GMM].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_PSM].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_PSM].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_PSM].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_FKP].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_FKP].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_FKP].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_CPG].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_CPG].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_CPG].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].heartbeat_info_rx.FirmWareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].heartbeat_info_rx.board_id = 0;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].heartbeat_info_rx.HardwareVersion = 0;
    self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].heartbeat_info_rx.FirmWareVersion = 0;
    // 本地模块信息赋值
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        self->rtm_module_info[i].heartbeat_info_tx.board_id = 0;
        self->rtm_module_info[i].heartbeat_info_tx.HardwareVersion = 0;
        self->rtm_module_info[i].heartbeat_info_tx.FirmWareVersion = 0;
    }

    ret = manage_info_init(&self->manage_info, 0x70);
    if (ret != 0)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("rtm main manage_info_init error, ret = %d\r\n", ret);
    }
    ret = manage_info_init(&self->app_data_record.manage_info, 0x00);
    if (ret != 0)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_data_record manage_info_init error, ret = %d\r\n", ret);
    }
    ret = app_data_record_init(&self->app_data_record);
    if (ret != 0)
    {
        manage_info_status_word_set(&self->app_data_record.manage_info, RECORD_PERIPHERAL_INIT_BIT, 1);
        LOG_E("app_data_record_init error, ret = %d\r\n", ret);
    }
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        self->rtm_module_info[i].app_data_record = &self->app_data_record;
    }

    ret = manage_info_init(&self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].manage_info, 0xC0);
    if (ret != 0)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("rtm_off_plc manage_info_init error, ret = %d\r\n", ret);
    }
    ret = ethercat_thread_init();
    if (ret != 0)
    {
        manage_info_status_word_set(&self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].manage_info, MODULE_PERIPHERAL_INIT_BIT, 1);
        LOG_E("ethercat_thread_init error, ret = %d\r\n", ret);
    }
    osEventFlagsAttr_t event_attributes = {
        .name = "output_event"};

    self->ethercat_Event = osEventFlagsNew(&event_attributes);
    if (self->ethercat_Event == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("osEventFlagsNew error\r\n");
    }
    ret = manage_info_init(&self->app_dido.manage_info, 0x00);
    if (ret != 0)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_dido manage_info_init error, ret = %d\r\n", ret);
    }
    ret = app_dido_create(&self->app_dido);
    if (ret != 0)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_dido_create error, ret = %d\r\n", ret);
    }

    ret = manage_info_init(&self->rtm_module_info[RTM_MODULE_PSM].manage_info, 0xC0);
    if (ret != 0)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("icm manage_info_init error, ret = %d\r\n", ret);
    }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_PSM].uart_protocol,
                             UART_DEV_NAME_USART2,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        manage_info_status_word_set(&self->rtm_module_info[RTM_MODULE_PSM].manage_info, MODULE_PERIPHERAL_INIT_BIT, 1);
        LOG_E("icm uart_protocol_init error, ret = %d\r\n", ret);
    }

    // ret = manage_info_init(&self->rtm_module_info[RTM_MODULE_GMM].manage_info, 0xC0);
    // if (ret != 0)
    // {
    //     manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
    //     LOG_E("gmm manage_info_init error, ret = %d\r\n", ret);
    // }
    // ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_GMM].uart_protocol,
    //                          UART_DEV_NAME_USART3,
    //                          1000,
    //                          10000,
    //                          10000);
    // if (ret != 0)
    // {
    //     return -7;
    // }
    self->uart_fkp = device_uart_find(UART_DEV_NAME_UART4);
    if (self->uart_fkp == NULL)
    {
        return -8;
    }
    device_err = dev_uart_init(self->uart_fkp, DEV_UART_IOCTL_USE_DMA, 5, (sizeof(fkp_recv_structure_t) + sizeof(serial_frame_format_t) + sizeof(uint32_t)) * 2);
    if (device_err != DEV_EOK)
    {
        return -9;
    }

    ret = fdcan1_init();
    if (ret != 0)
    {
        return -10;
    }

    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_RTM_ON].uart_protocol,
                             UART_DEV_NAME_UART5,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        manage_info_status_word_set(&self->rtm_module_info[RTM_MODULE_RTM_ON].manage_info, MODULE_PERIPHERAL_INIT_BIT, 1);
        LOG_E("rtm_on uart_protocol_init error, ret = %d\r\n", ret);
    }
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        osMessageQueueAttr_t queue_attributes = {
            .name = self->rtm_module_info[i].module_name,
        };
        self->rtm_module_info[i].module_queue = osMessageQueueNew(10, sizeof(queue_frame_t), NULL);
        if (self->rtm_module_info[i].module_queue == NULL)
        {
            manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
            LOG_E("osMessageQueueNew error, module_name = %s\r\n", self->rtm_module_info[i].module_name);
        }
        for (uint8_t j = 0; j < RTM_MODULE_MAX; j++)
        {
            self->rtm_module_info[j].queue_group[i] = self->rtm_module_info[i].module_queue;
        }
    }

    thread_attributes.name = "app_ethercat_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].module_priority;
    threadHandle = osThreadNew(app_ethercat_rx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC]), &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_ethercat_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_ethercat_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].module_priority;
    threadHandle = osThreadNew(app_ethercat_tx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC]), &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_ethercat_tx_thread create error\r\n");
    }
    thread_attributes.name = "app_rtm_main_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].module_priority;
    app_rtm_main_threadId = osThreadNew(app_rtm_main_thread, self, &thread_attributes);
    if (app_rtm_main_threadId == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_rtm_main_thread create error\r\n");
    }
    // thread_attributes.name = "app_gmm_rx_thread";
    // thread_attributes.stack_size = 1024 * 4;
    // thread_attributes.priority = self->rtm_module_info[RTM_MODULE_GMM].module_priority;
    // threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_GMM]), &thread_attributes);
    // if (threadHandle == NULL)
    // {
    //     return -16;
    // }
    // thread_attributes.name = "app_gmm_tx_thread";
    // thread_attributes.stack_size = 1024 * 4;
    // thread_attributes.priority = self->rtm_module_info[RTM_MODULE_GMM].module_priority + 1;
    // threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_GMM]), &thread_attributes);
    // if (threadHandle == NULL)
    // {
    //     return -17;
    // }
    thread_attributes.name = "app_psm_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_PSM].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_PSM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_psm_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_psm_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_PSM].module_priority + 1;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_PSM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_psm_tx_thread create error\r\n");
    }
    thread_attributes.name = "app_fkp_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_FKP].module_priority;
    threadHandle = osThreadNew(app_fkp_rx_thread, self, &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_fkp_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_fkp_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_FKP].module_priority + 1;
    threadHandle = osThreadNew(app_fkp_tx_thread, self, &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_fkp_tx_thread create error\r\n");
    }
    thread_attributes.name = "app_cpg_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_CPG].module_priority;
    threadHandle = osThreadNew(app_cpg_rx_thread, self, &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_cpg_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_cpg_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_CPG].module_priority + 1;
    threadHandle = osThreadNew(app_cpg_tx_thread, self, &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_cpg_tx_thread create error\r\n");
    }
    thread_attributes.name = "app_rtm_on_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_ON].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_ON]), &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_rtm_on_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_rtm_on_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_ON].module_priority + 1;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_ON]), &thread_attributes);
    if (threadHandle == NULL)
    {
        manage_info_status_word_set(&self->manage_info, RTM_MAIN_MEMORY_STATE_BIT, 1);
        LOG_E("app_rtm_on_tx_thread create error\r\n");
    }
    return 0;
}
INIT_APP_EXPORT(app_rtm_data_handle_create)

// #define FKP_TEST
#ifdef FKP_TEST
#include "shell.h"
static struct
{
    float dose_meter;
    float dose_cumulated;
    uint32_t trigger_interval;
    uint64_t timestamp;
} __attribute__((aligned(1), packed)) DoseData;
static osTimerId_t timerId = NULL;
void fkp_osTimerFunc(void *argument)
{

    DoseData.dose_cumulated += 0.1;
    if (DoseData.dose_cumulated > DoseData.dose_meter)
    {
        DoseData.dose_cumulated = DoseData.dose_meter;
        osTimerStop(timerId);
    }
    rtm_set_data_distribute(app_rtm.rtm_module_info->queue_group[RTM_MODULE_FKP], FKP_ID, RECEIVE_FKP_DOSE_CMD, &DoseData, sizeof(DoseData));
}

static int8_t fkp_test(int argc, char *argv[])
{
    if (argc < 2)
    {
        goto usage;
    }
    if (timerId == NULL)
    {
        timerId = osTimerNew(fkp_osTimerFunc, osTimerPeriodic, NULL, NULL);
    }

    if (0 == strcmp(argv[1], "dose"))
    {
        if (0 == strcmp(argv[2], "start"))
        {
            if (osTimerIsRunning(timerId))
            {
                return -1;
            }
            DoseData.dose_meter = 500.0f;
            DoseData.dose_cumulated = 0.0f;
            osTimerStart(timerId, 100);
            return 0;
        }
        else if (0 == strcmp(argv[2], "stop"))
        {
            if (!osTimerIsRunning(timerId))
            {
                return -1;
            }
            osTimerStop(timerId);
            return 0;
        }
        else
        {
            goto usage;
        }
    }
    return 0;
usage:
    LOG_I("usage: %s\n", argv[0]);
    return 0;
}

MSH_CMD_EXPORT_ALIAS(fkp_test, fkp_test, fkp test);
#endif

// #define PSM_TEST
#ifdef PSM_TEST
#include "shell.h"
static int8_t psm_test(int argc, char *argv[])
{
    int32_t ret = 0;
    uint16_t FkpButton = 0;
    uint32_t CpgButton = 0;
    if (argc < 4)
    {
        goto usage;
    }
    if (0 == strcmp(argv[1], "fkp"))
    {
        if (0 == strcmp(argv[2], "unload+en"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                FkpButton = 0x21;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                FkpButton = 0x00;
            }
        }
        else
        {
            goto usage;
        }
        ret = rtm_set_data_distribute(app_rtm.rtm_module_info[RTM_MODULE_FKP].queue_group[RTM_MODULE_PSM],
                                      GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                      RECEIVE_FKP_BUTTON_CMD,
                                      &FkpButton,
                                      sizeof(FkpButton));
        if (ret != 0)
        {
            LOG_I("fkp set data distribute error, ret = %d\r\n", ret);
        }
        ret = rtm_set_data_distribute(app_rtm.rtm_module_info[RTM_MODULE_FKP].queue_group[RTM_MODULE_RTM_OFF_ARM],
                                      GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                      RECEIVE_FKP_BUTTON_CMD,
                                      &FkpButton,
                                      2);
        if (ret != 0)
        {
            LOG_I("fkp set data distribute error, ret = %d\r\n", ret);
        }
        ret = rtm_set_data_distribute(app_rtm.rtm_module_info[RTM_MODULE_FKP].queue_group[RTM_MODULE_RTM_ON],
                                      GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                      RECEIVE_FKP_BUTTON_CMD,
                                      &FkpButton,
                                      sizeof(FkpButton));
        if (ret != 0)
        {
            LOG_I("fkp set data distribute error, ret = %d\r\n", ret);
        }
    }
    else if (0 == strcmp(argv[1], "cpg"))
    {
        if (0 == strcmp(argv[2], "setup+en"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x03;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "setup+EN"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x42;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "load+en"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x05;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "load+EN"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x44;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "unload+en"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x09;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "unload+EN"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x48;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "right"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0xC0;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "left"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x140;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "in"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x240;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "out"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x440;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "up"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x840;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "down"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x1040;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else if (0 == strcmp(argv[2], "brake"))
        {
            if (0 == strcmp(argv[3], "enable"))
            {
                CpgButton = 0x8040;
            }
            else if (0 == strcmp(argv[3], "disable"))
            {
                CpgButton = 0x00;
            }
        }
        else
        {
            goto usage;
        }
        ret = rtm_set_data_distribute(app_rtm.rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_PSM],
                                      GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                      SEND_CPG_BUTTON_CMD,
                                      &CpgButton,
                                      sizeof(CpgButton));
        if (ret != 0)
        {
            LOG_I("cpg set data distribute error, ret = %d\r\n", ret);
        }
        ret = rtm_set_data_distribute(app_rtm.rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_OFF_ARM],
                                      GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                      SEND_CPG_BUTTON_CMD,
                                      &CpgButton,
                                      sizeof(CpgButton));
        if (ret != 0)
        {
            LOG_I("cpg set data distribute error, ret = %d\r\n", ret);
        }
        ret = rtm_set_data_distribute(app_rtm.rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_ON],
                                      GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                      SEND_CPG_BUTTON_CMD,
                                      &CpgButton,
                                      sizeof(CpgButton));
        if (ret != 0)
        {
            LOG_I("cpg set data distribute error, ret = %d\r\n", ret);
        }
    }
    else if (0 == strcmp(argv[1], "icm"))
    {
        struct rtm_icm_data
        {
            float psm_position_x_tar;
            float psm_position_y_tar;
            float psm_position_z_tar;
            float psm_position_x_r_tar;
            float psm_position_y_r_tar;
            float psm_position_z_r_tar;
            float psm_velocity_x_tar;
            float psm_velocity_y_tar;
            float psm_velocity_z_tar;
            float psm_velocity_x_r_tar;
            float psm_velocity_y_r_tar;
            float psm_velocity_z_r_tar;
            uint8_t psm_move_ctrl;
            uint8_t reserved[3];
        } __attribute__((aligned(1), packed)) icm_data = {
            0};
        if (argc < 15)
        {
            goto usage;
        }

        icm_data.psm_position_x_tar = atof(argv[2]);
        icm_data.psm_position_y_tar = atof(argv[3]);
        icm_data.psm_position_z_tar = atof(argv[4]);
        icm_data.psm_position_x_r_tar = atof(argv[5]);
        icm_data.psm_position_y_r_tar = atof(argv[6]);
        icm_data.psm_position_z_r_tar = atof(argv[7]);
        icm_data.psm_velocity_x_tar = atof(argv[8]);
        icm_data.psm_velocity_y_tar = atof(argv[9]);
        icm_data.psm_velocity_z_tar = atof(argv[10]);
        icm_data.psm_velocity_x_r_tar = atof(argv[11]);
        icm_data.psm_velocity_y_r_tar = atof(argv[12]);
        icm_data.psm_velocity_z_r_tar = atof(argv[13]);
        icm_data.psm_move_ctrl = atoi(argv[14]);
        ret = rtm_set_data_distribute(app_rtm.rtm_module_info[RTM_MODULE_RTM_OFF_ARM].queue_group[RTM_MODULE_PSM],
                                      PSM_ID,
                                      RECEIVE_PSM_CTRL_CMD,
                                      &icm_data,
                                      sizeof(icm_data));
        if (ret != 0)
        {
            LOG_I("icm set data distribute error, ret = %d\r\n", ret);
        }
        LOG_I("icm set data:\r\n");
        LOG_I("psm_position_x_tar = %f\r\n", icm_data.psm_position_x_tar);
        LOG_I("psm_position_y_tar = %f\r\n", icm_data.psm_position_y_tar);
        LOG_I("psm_position_z_tar = %f\r\n", icm_data.psm_position_z_tar);
        LOG_I("psm_position_x_r_tar = %f\r\n", icm_data.psm_position_x_r_tar);
        LOG_I("psm_position_y_r_tar = %f\r\n", icm_data.psm_position_y_r_tar);
        LOG_I("psm_position_z_r_tar = %f\r\n", icm_data.psm_position_z_r_tar);
        LOG_I("psm_velocity_x_tar = %f\r\n", icm_data.psm_velocity_x_tar);
        LOG_I("psm_velocity_y_tar = %f\r\n", icm_data.psm_velocity_y_tar);
        LOG_I("psm_velocity_z_tar = %f\r\n", icm_data.psm_velocity_z_tar);
        LOG_I("psm_velocity_x_r_tar = %f\r\n", icm_data.psm_velocity_x_r_tar);
        LOG_I("psm_velocity_y_r_tar = %f\r\n", icm_data.psm_velocity_y_r_tar);
        LOG_I("psm_velocity_z_r_tar = %f\r\n", icm_data.psm_velocity_z_r_tar);
        LOG_I("psm_move_ctrl = %d\r\n", icm_data.psm_move_ctrl);
    }
    else
    {
        goto usage;
    }

    return 0;
usage:
    LOG_I("usage: %s\n", argv[0]);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(psm_test, psm_test, psm test);
#endif
#define DIDO_TEST
#ifdef DIDO_TEST

#include "shell.h"
#include "ulog.h"

int8_t dido_test(uint8_t argc, uint8_t **argv)
{
    dido_structure_t dido_value;
    if (argc < 3)
    {
        goto usage;
    }
    if (strcmp(argv[1], "di") == 0)
    {
        if (strcmp(argv[2], "read") == 0)
        {
            app_di_get(&app_rtm.app_dido, &dido_value);
            app_do_get(&app_rtm.app_dido, &dido_value);
            LOG_I("DI mcp23017_0x00 value: 0x%x\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00);
            LOG_I("DI_CITB_EMERGENCY2:%d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_CITB_EMERGENCY2);
            LOG_I("DI_UPS_LOAD_PORT:%d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_UPS_LOAD_PORT);
            LOG_I("DI_STAND_BREAKER1:%d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_STAND_BREAKER1);
            LOG_I("DI_STAND_EMERGENCY:%d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_STAND_EMERGENCY);
            LOG_I("DI_TouchGuard:%d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_TouchGuard);
            LOG_I("DI_HvEn:%d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_HvEn);
            LOG_I("DI_MV_TreatmentEN:%d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_MV_TreatmentEN);
            LOG_I("DI_KV_TreatmentEN:%d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_KV_TreatmentEN);
            LOG_I("\r\n");
            osDelay(100);
            LOG_I("DI mcp23017_0x01 value: 0x%x\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01);
            LOG_I("DI_STAND_BREAKER7:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_STAND_BREAKER7);
            LOG_I("DI_STAND_BREAKER3:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_STAND_BREAKER3);
            LOG_I("DI_UPS_ON_BYPASS:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_UPS_ON_BYPASS);
            LOG_I("DI_STAND_BREAKER5:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_STAND_BREAKER5);
            LOG_I("DI_CITB_EMERGENCY3:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_EMERGENCY3);
            LOG_I("DI_COVER3:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_COVER3);
            LOG_I("DI_POWER_CUT:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_POWER_CUT);
            LOG_I("DI_STAND_BREAKER4:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_STAND_BREAKER4);
            osDelay(100);
            LOG_I("DI_CITB_TREATMENT_ROOM_DOOR1:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR1);
            LOG_I("DI_CITB_TREATMENT_ROOM_DOOR2:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR2);
            LOG_I("DI_CITB_EMERGENCY1:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_EMERGENCY1);
            LOG_I("DI_CITB_SEARCH_TREATMENT_ROOM:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_SEARCH_TREATMENT_ROOM);
            LOG_I("DI_CONTROL_ROOM_BREAKER1:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CONTROL_ROOM_BREAKER1);
            LOG_I("DI_UPS_ON_BATT:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_UPS_ON_BATT);
            LOG_I("DI_CONTROL_ROOM_BREAKER5:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CONTROL_ROOM_BREAKER5);
            LOG_I("DI_STAND_BREAKER6:%d\r\n", dido_value.mcp23017_0x01_u.mcp23017_0x01_bit.DI_STAND_BREAKER6);
            LOG_I("\r\n");
            osDelay(100);
            LOG_I("DI mcp23017_0x02 value: 0x%x\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02);
            LOG_I("DI_CITB_EMERGENCY5:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_CITB_EMERGENCY5);
            LOG_I("DI_COVER1:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_COVER1);
            LOG_I("DI_STAND_RESERVE:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_STAND_RESERVE);
            LOG_I("DI_TREATMENT_ROOM_DOOR_READY:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_TREATMENT_ROOM_DOOR_READY);
            LOG_I("DI_COVER2:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_COVER2);
            LOG_I("DI_CONTROL_ROOM_BREAKER2:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_CONTROL_ROOM_BREAKER2);
            LOG_I("DI_CONTROL_ROOM_BREAKER4:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_CONTROL_ROOM_BREAKER4);
            LOG_I("DI_STAND_CONTACTOR2:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_STAND_CONTACTOR2);
            osDelay(100);
            LOG_I("DI_STAND_CONTACTOR1:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_STAND_CONTACTOR1);
            LOG_I("DI_UPS_LOW_BATT:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_UPS_LOW_BATT);
            LOG_I("DI_HvKey:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_HvKey);
            LOG_I("DI_COVER4:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_COVER4);
            LOG_I("DI_CITB_EMERGENCY4:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_CITB_EMERGENCY4);
            LOG_I("DI_CONTROL_ROOM_BREAKER3:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_CONTROL_ROOM_BREAKER3);
            LOG_I("DI_STAND_BREAKER2:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.DI_STAND_BREAKER2);
            LOG_I("RTC_WD_OK_IN:%d\r\n", dido_value.mcp23017_0x02_u.mcp23017_0x02_bit.RTC_WD_OK_IN);
            LOG_I("\r\n");
            LOG_I("DI gpio_di value: 0x%x\r\n", dido_value.gpio_di_u.gpio_di);
            LOG_I("\r\n");
            osDelay(100);
            LOG_I("DO_RTM_AutoPowerUp value: 0x%x\r\n", dido_value.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RTM_AutoPowerUp);
            LOG_I("DO_SearchTreatmentRoomRelay value: 0x%x\r\n", dido_value.mcp23017_0x03_u.mcp23017_0x03_bit.DO_SearchTreatmentRoomRelay);
            LOG_I("DO_Laser value: 0x%x\r\n", dido_value.mcp23017_0x03_u.mcp23017_0x03_bit.DO_Laser);
            LOG_I("DO_Power_CUT value: 0x%x\r\n", dido_value.mcp23017_0x03_u.mcp23017_0x03_bit.DO_Power_CUT);
            LOG_I("DO_STAND_RESERVE value: 0x%x\r\n", dido_value.mcp23017_0x03_u.mcp23017_0x03_bit.DO_STAND_RESERVE);
            LOG_I("DO_TreatmentRoomLight value: 0x%x\r\n", dido_value.mcp23017_0x03_u.mcp23017_0x03_bit.DO_TreatmentRoomLight);
            LOG_I("DO_RadiationIndicator value: 0x%x\r\n", dido_value.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RadiationIndicator);
            LOG_I("DO_ReadyIndicator value: 0x%x\r\n", dido_value.mcp23017_0x03_u.mcp23017_0x03_bit.DO_ReadyIndicator);
            LOG_I("DO_RTM_SystemShutDown value: 0x%x\r\n", dido_value.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RTM_SystemShutDown);
            osDelay(100);
            LOG_I("DO_SoftwareHvEn value: 0x%x\r\n", dido_value.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn);
            LOG_I("DO_SoftwareKVTreatmentEn value: 0x%x\r\n", dido_value.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn);
            LOG_I("DO_SoftwareMVTreatmentEn value: 0x%x\r\n", dido_value.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn);
            LOG_I("DO_ThreePhasePowerOn value: 0x%x\r\n", dido_value.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn);
            LOG_I("DO_softwareMoveEN value: 0x%x\r\n", dido_value.gpio_do_u.gpio_do_bit.DO_softwareMoveEN);
            LOG_I("DO_TreatmentMotionEnable value: 0x%x\r\n", dido_value.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable);
            LOG_I("DO_AsuMotionEnable value: 0x%x\r\n", dido_value.gpio_do_u.gpio_do_bit.DO_AsuMotionEnable);
        }
        else
        {
            goto usage;
        }
    }
    else if (strcmp(argv[1], "do") == 0)
    {
        if (argc < 4)
        {
            goto usage;
        }
        if (strcmp(argv[2], "write") == 0)
        {
            app_do_get(&app_rtm.app_dido, &dido_value);

            uint32_t value = atoi(argv[3]);
            dido_value.gpio_do_u.gpio_do = value & 0xFFFF;
            dido_value.mcp23017_0x03_u.mcp23017_0x03 = (value >> 16) & 0xFFFF;
            app_do_set(&app_rtm.app_dido, &dido_value);
            LOG_I("DO GPIO_DO value: 0x%x\r\n", dido_value.gpio_do_u.gpio_do);
        }
        else
        {
            goto usage;
        }
    }
    else
    {
        goto usage;
    }
    return 0;
usage:
    LOG_E("Usage: %s <di|do> <read|write> <value>\r\n", argv[0]);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(dido_test, dido_test, dido test);
#endif

#define RTM_STATUS_TEST
#ifdef RTM_STATUS_TEST
#include "shell.h"
#include "ulog.h"

int8_t rtm_status_test(uint8_t argc, uint8_t **argv)
{
    LOG_I("NOT_READY:0x%x\r\n", app_rtm.fault_check.fixed_interlock_table.not_ready_event);
    LOG_I("WARNING:0x%x\r\n", app_rtm.fault_check.fixed_interlock_table.warning_interlock);
    LOG_I("MINOR:0x%x\r\n", app_rtm.fault_check.fixed_interlock_table.minor_interlock);
    LOG_I("SERIOUS:0x%x\r\n", app_rtm.fault_check.fixed_interlock_table.serious_interlock);

    LOG_I("RTM_MODULE_RTM_MAIN:0x%x\r\n", app_rtm.fault_check.fixed_app_state_table.rtm_main_state);
    LOG_I("RTM_MODULE_PSM:0x%x\r\n", app_rtm.fault_check.fixed_app_state_table.psm_state);
    LOG_I("RTM_MODULE_GMM:0x%x\r\n", app_rtm.fault_check.fixed_app_state_table.gmm_state);
    LOG_I("RTM_MODULE_FKP:0x%x\r\n", app_rtm.fault_check.fixed_app_state_table.fkp_state);
    LOG_I("RTM_MODULE_CPG:0x%x\r\n", app_rtm.fault_check.fixed_app_state_table.cpg_state);
    LOG_I("RTM_MODULE_RTM_ON:0x%x\r\n", app_rtm.fault_check.fixed_app_state_table.rtm_on_state);
    LOG_I("RTM_MODULE_RTM_ON_PLC:0x%x\r\n", app_rtm.fault_check.fixed_app_state_table.plc_state);
    LOG_I("RTM_MODULE_RTM_DIDO:0x%x\r\n", app_rtm.fault_check.fixed_app_state_table.dido_state);
    LOG_I("RTM_MODULE_RTM_RECORD:0x%x\r\n", app_rtm.fault_check.fixed_app_state_table.data_record);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(rtm_status_test, rtm_status_test, rtm status test);
#endif