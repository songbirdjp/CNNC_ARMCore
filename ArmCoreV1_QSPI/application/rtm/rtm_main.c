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

#define RTM_MAIN_THREAD_CYCLE_MS (1)
static void app_rtm_main_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    osStatus_t status = osOK;
    queue_frame_t queue_frame;
    rtm_status_t rtm_status = {0};
    rtm_status_t rtm_status_old = {0};

    uint8_t trm_require_state = 0;
    uint8_t trm_require_state_old = 0;
    uint32_t last_time_trm_require = 0;

    dido_structure_t dido_structure = {0};
    dido_structure_t dido_structure_old = {0};

    psm_position_t psm_position = {0};
    psm_position_t psm_position_old = {0};
    uint32_t last_time_psm_position = 0;

    prompt_state_t prompt_state = {0};
    prompt_state_t prompt_state_old = {0};
    uint32_t last_time_prompt_state = 0;

    uint8_t treatment_record_state = 0;
    uint8_t treatment_record_state_old = 0;

    uint32_t current_time = 0;
    uint32_t last_time_rtm_state = 0;
    current_time = osKernelGetTickCount();
    last_time_rtm_state = current_time;
    last_time_trm_require = current_time;
    last_time_psm_position = current_time;
    last_time_prompt_state = current_time;

    uint8_t system_state_require = 0;
    Event_t rtm_event = {0};
    rtm_state_machine_ctor(&(self->state_machine), self);
    Event_t prompt_event = {0};
    prompt_state_machine_ctor(&(self->app_prompt), self);
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
                    self->interlock_override = *(uint32_t *)&(queue_frame.payload.data[5]);
                    self->unready_override = *(uint32_t *)&(queue_frame.payload.data[9]);
                    self->led_belt = *(uint16_t *)&(queue_frame.payload.data[13]);
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
                case SEND_RTM_OFF_BUTTON_CMD: /*FKP按键*/
                {
                    button_state_t button_state = *(button_state_t *)&(queue_frame.payload.data[1]);
                    if ((0 == button_state.special_button.emergency_state_fkp) ||
                        (0 == button_state.special_button.emergency_state_cpg_l) ||
                        (0 == button_state.special_button.emergency_state_cpg_r))
                    {
                        self->cpg_fkp_emergency_stop = 0;
                    }
                    else
                    {
                        self->cpg_fkp_emergency_stop = 1;
                    }
                    switch (button_state.function_state)
                    {
                    case BUTTON_STATE_RELEASE:
                        rtm_event.sig = MANUAL_EXIT_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                        break;
                    case BUTTON_STATE_SETUP:
                        rtm_event.sig = MANUAL_ENTER_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                        prompt_event.sig = PROMPT_BUTTON_SETUP_SIG;
                        prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
                        break;
                    case BUTTON_STATE_LOAD:
                        rtm_event.sig = MANUAL_ENTER_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                        prompt_event.sig = PROMPT_BUTTON_LOAD_SIG;
                        prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
                        break;
                    case BUTTON_STATE_PREPARE:
                        prompt_event.sig = PROMPT_BUTTON_PREPARE_SIG;
                        prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
                        break;
                    case BUTTON_STATE_UNLOAD:
                        rtm_event.sig = MANUAL_ENTER_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                        prompt_event.sig = PROMPT_BUTTON_UNLOAD_SIG;
                        prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
                        break;
                    case BUTTON_STATE_RIGHT:
                    case BUTTON_STATE_LEFT:
                    case BUTTON_STATE_FORWARD:
                    case BUTTON_STATE_BACK:
                    case BUTTON_STATE_UP:
                    case BUTTON_STATE_DOWN:
                    case BUTTON_STATE_GOTO:
                    case BUTTON_STATE_OVERRIDE_RIGHT:
                    case BUTTON_STATE_OVERRIDE_LEFT:
                    case BUTTON_STATE_OVERRIDE_FORWARD:
                    case BUTTON_STATE_OVERRIDE_BACK:
                    case BUTTON_STATE_OVERRIDE_UP:
                    case BUTTON_STATE_OVERRIDE_DOWN:
                        rtm_event.sig = MANUAL_ENTER_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                        break;
                    case BUTTON_STATE_LIGHT_ON:
                        break;
                    case BUTTON_STATE_LIGHT_OFF:
                        break;
                    case BUTTON_STATE_LASER_ON:
                        break;
                    case BUTTON_STATE_LASER_OFF:
                        break;
                    default:
                        break;
                    }
                }
                break;
                case SEND_BEAM_ID_CMD: /*beamID*/
                {
                    self->beamID = *(uint8_t *)&(queue_frame.payload.data[1]);
                    break;
                }
                case SEND_GMM_CURRENT_STATE_CMD: /*GMM状态*/
                {
                    self->gmm_current_state = *(uint8_t *)&(queue_frame.payload.data[1]);
                }
                break;
                case SEND_PSM_CURRENT_STATE_CMD: /*PSM状态*/
                {
                    self->psm_current_state = *(uint8_t *)&(queue_frame.payload.data[1]);
                    self->psm_not_ready_event = *(uint32_t *)&(queue_frame.payload.data[9]);
                    // LOG_I("PSM_CURRENT_STATE:%d, PSM_NOT_READY_EVENT:0x%x\r\n", self->psm_current_state, self->psm_not_ready_event);
                    if (0 == (self->psm_not_ready_event & 0x02))
                    {
                        prompt_event.sig = PROMPT_ARRIVE_SETUP_SIG;
                        prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
                    }
                    if (0 == (self->psm_not_ready_event & 0x04))
                    {
                        prompt_event.sig = PROMPT_ARRIVE_LOAD_SIG;
                        prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
                    }
                    if (0 == (self->psm_not_ready_event & 0x08))
                    {
                        prompt_event.sig = PROMPT_ARRIVE_PREPARE_SIG;
                        prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
                    }
                    if (0 == (self->psm_not_ready_event & 0x10))
                    {
                        prompt_event.sig = PROMPT_ARRIVE_UNLOAD_SIG;
                        prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
                    }
                }
                break;
                case SEND_PSM_INFO_CMD: /*PSM位置*/
                {
                    memcpy(&self->psm_info, (uint8_t *)&(queue_frame.payload.data[1]), sizeof(psm_info_t));
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
                case RECEIVE_SYSTEM_STATE_CMD: /*系统状态机*/
                {
                    uint8_t system_state = *(uint8_t *)&(queue_frame.payload.data[1]);
                    switch (system_state)
                    {
                    case PROMPT_SYSTEM_ON_STATE:
                        prompt_event.sig = PROMPT_SYSTEM_ON_SIG;
                        break;
                    case PROMPT_MV_PREPARE_STATE:
                        prompt_event.sig = PROMPT_MV_PREPARE_SIG;
                        break;
                    case PROMPT_MV_READY_STATE:
                        prompt_event.sig = PROMPT_MV_READY_SIG;
                        break;
                    case PROMPT_MV_RADIATION_STATE:
                        prompt_event.sig = PROMPT_MV_RADIATION_SIG;
                        break;
                    case PROMPT_MV_COMPLETE_STATE:
                        prompt_event.sig = PROMPT_MV_COMPLETE_SIG;
                        break;
                    case PROMPT_MV_INTERRUPT_STATE:
                        prompt_event.sig = PROMPT_MV_INTERRUPT_SIG;
                        break;
                    case PROMPT_MV_TERMINATE_STATE:
                        prompt_event.sig = PROMPT_MV_TERMINATE_SIG;
                        break;
                    case PROMPT_KV_PRELIMINARY_STATE:
                        prompt_event.sig = PROMPT_KV_PRELIMINARY_SIG;
                        break;
                    case PROMPT_KV_PREPARE_STATE:
                        prompt_event.sig = PROMPT_KV_PREPARE_SIG;
                        break;
                    case PROMPT_SURVIEW_READY_STATE:
                    case PROMPT_CT_READY_STATE:
                        prompt_event.sig = PROMPT_KV_READY_SIG;
                        break;
                    case PROMPT_SURVIEW_RADIATION_STATE:
                    case PROMPT_CT_RADIATION_STATE:
                        prompt_event.sig = PROMPT_KV_RADIATION_SIG;
                        break;
                    case PROMPT_KV_COMPLETE_STATE:
                        prompt_event.sig = PROMPT_KV_COMPLETE_SIG;
                        break;
                    default:
                        prompt_event.sig = PROMPT_NULL_SIG;
                        break;
                    }
                    prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
                }
                break;
                case RECEIVE_TREATMENT_RECORD_STATE_CMD: /*treatment record state*/
                {
                    treatment_record_state = *(uint8_t *)&(queue_frame.payload.data[1]);
                    treatment_record_state_old = treatment_record_state;
                    switch (treatment_record_state)
                    {
                    case TREATMENT_RECORD_SEND_SUCCEED:
                        prompt_event.sig = PROMPT_RT_SEND_SUCCEED_SIG;
                        break;
                    case TREATMENT_RECORD_SWITCH:
                        prompt_event.sig = PROMPT_RT_SWITCH_SIG;
                        break;
                    case TREATMENT_RECORD_FINISH:
                        prompt_event.sig = PROMPT_RT_FINISH_SIG;
                        break;
                    case TREATMENT_RECORD_CLEAR:
                        prompt_event.sig = PROMPT_RT_CLEAR_SIG;
                        break;
                    }
                    prompt_state_dispatch(&(self->app_prompt), (Event_t *)&prompt_event);
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
        rtm_status.beam_ID = self->beamID;
        rtm_status.fsm_state_current = rtm_get_state(&(self->state_machine));
        rtm_status.not_ready_event = *(uint32_t *)&(self->interlock_table.not_ready_event);
        rtm_status.warning_interlock = *(uint32_t *)&(self->interlock_table.warning_interlock);
        rtm_status.minor_interlock = *(uint32_t *)&(self->interlock_table.minor_interlock);
        rtm_status.serious_interlock = *(uint32_t *)&(self->interlock_table.serious_interlock);

        if (memcmp(&rtm_status_old, &rtm_status, sizeof(rtm_status_t)) != 0)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON].module_queue, RTM_ON_PLC_ID, SEND_RTM_OFF_ARM_CURRENT_STATE_CMD, (uint8_t *)&rtm_status, sizeof(rtm_status_t));
            memcpy(&rtm_status_old, &rtm_status, sizeof(rtm_status_t));
            last_time_rtm_state = osKernelGetTickCount();
        }

        // 周期上报状态
        current_time = osKernelGetTickCount();
        if (current_time - last_time_rtm_state > 1000)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON].module_queue, RTM_ON_PLC_ID, SEND_RTM_OFF_ARM_CURRENT_STATE_CMD, (uint8_t *)&rtm_status, sizeof(rtm_status_t));
            last_time_rtm_state = current_time;
        }

        prompt_state_trm_require_get(&(self->app_prompt), &trm_require_state);
        if (trm_require_state != trm_require_state_old)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON].module_queue, RTM_ON_PLC_ID, SEND_RTM_OFF_TRM_STATE_CMD, (uint8_t *)&trm_require_state, sizeof(trm_require_state));
            trm_require_state_old = trm_require_state;
            last_time_trm_require = current_time;
        }
        else if (current_time - last_time_trm_require > 100)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON].module_queue, RTM_ON_PLC_ID, SEND_RTM_OFF_TRM_STATE_CMD, (uint8_t *)&trm_require_state, sizeof(trm_require_state));
            last_time_trm_require = current_time;
        }
        prompt_state_load_position_get(&(self->app_prompt), &psm_position);
        if (memcmp(&psm_position_old, &psm_position, sizeof(psm_position_t)) != 0)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON].module_queue, RTM_ON_PLC_ID, SEND_RTM_OFF_LOAD_POSITION_CMD, (uint8_t *)&psm_position, sizeof(psm_position_t));
            memcpy(&psm_position_old, &psm_position, sizeof(psm_position_t));
            last_time_psm_position = current_time;
        }
        else if (current_time - last_time_psm_position > 100)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON].module_queue, RTM_ON_PLC_ID, SEND_RTM_OFF_LOAD_POSITION_CMD, (uint8_t *)&psm_position, sizeof(psm_position_t));
            last_time_psm_position = current_time;
        }
        prompt_state_prompt_get(&(self->app_prompt), &prompt_state);
        if (memcmp(&prompt_state_old, &prompt_state, sizeof(prompt_state_t)) != 0)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_FKP].module_queue, FKP_ID | CPG_ID, SEND_RTM_OFF_PROMPT_CMD, (uint8_t *)&prompt_state, sizeof(prompt_state_t));
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].module_queue, FKP_ID | CPG_ID, SEND_RTM_OFF_PROMPT_CMD, (uint8_t *)&prompt_state, sizeof(prompt_state_t));
            memcpy(&prompt_state_old, &prompt_state, sizeof(prompt_state_t));
            last_time_prompt_state = current_time;
        }
        else if (current_time - last_time_prompt_state > 100)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_FKP].module_queue, FKP_ID | CPG_ID, SEND_RTM_OFF_PROMPT_CMD, (uint8_t *)&prompt_state, sizeof(prompt_state_t));
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].module_queue, FKP_ID | CPG_ID, SEND_RTM_OFF_PROMPT_CMD, (uint8_t *)&prompt_state, sizeof(prompt_state_t));
            last_time_prompt_state = current_time;
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
    case SEND_RTM_OFF_BUTTON_CMD:
        memcpy(&input_data->InU8_function_button, queue_frame->payload.data + 1, len);
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
        if (ret & APP_RTM_EVENT_FLAG_OUTPUT)
        {
            ethercat_recv_data_get((uint16_t *)&output_data, sizeof(TOBJ7010));

            if (ethercat_Link_state != output_data.OutU8_ethercat_Link_state)
            {
                ethercat_Link_state = output_data.OutU8_ethercat_Link_state;
                last_time = osKernelGetTickCount();
            }
            // TODO: 处理输出数据
            ethercat_output_data_distribute(self, &output_data);
        }
        current_time = osKernelGetTickCount();
        if (current_time - last_time > 1000)
        {
            // LOG_I("%s unlink, time:%d\r\n", self->module_name, current_time - last_time);
            bit_set(app_rtm.rtm_ethercat_info.manage_info.status_word, ETHERCAT_LINK_STATE_BIT);
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

    if (memcmp(&(rtm_module_info->heartbeat_info_rx), heartbeat, sizeof(heartbeat_t)) != 0)
    {
        // LOG_E("%s heartbeat rx err!\r\n", rtm_module_info->module_name);
    }
    return 0;
}
int32_t uart_protocol_heartbeat_rx_timeout_callback(struct uart_protocol *const self,
                                                    uint32_t ID,
                                                    const uint8_t *data,
                                                    uint16_t *len,
                                                    void *arg)
{
    rtm_module_info_t *rtm_module_info = (rtm_module_info_t *)arg;
    bit_set(rtm_module_info->manage_info.status_word, MODULE_LINK_STATE_BIT);
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
            LOG_E("%s recv error, ret = %d\r\n", self->module_name, ret);
            continue;
        }
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
            bit_set(self->manage_info.status_word, MODULE_LINK_STATE_BIT);
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
    device_err_t device_err = DEV_EOK;
    if (self == NULL)
    {
        return -1;
    }
    memset(self, 0, sizeof(app_rtm_main_t));
    self->cpg_fkp_emergency_stop = 1;
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
    ret = app_keyboard_init(&self->app_keyboard);
    if (ret != 0)
    {
        return -2;
    }

    ret = app_data_record_init(&self->app_data_record);
    if (ret != 0)
    {
        return -3;
    }
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        self->rtm_module_info[i].app_data_record = &self->app_data_record;
    }
    ret = ethercat_thread_init();
    if (ret != 0)
    {
        bit_set(self->rtm_ethercat_info.manage_info.status_word, ETHERCAT_SLAVE_INIT_BIT);
        return -4;
    }
    osEventFlagsAttr_t event_attributes = {
        .name = "output_event"};

    self->ethercat_Event = osEventFlagsNew(&event_attributes);
    if (self->ethercat_Event == NULL)
    {
        return -5;
    }

    ret = app_dido_create(&self->app_dido);
    if (ret != 0)
    {
        return -6;
    }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_PSM].uart_protocol,
                             UART_DEV_NAME_USART2,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        return -7;
    }
    // ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_GMM].uart_protocol,
    //                          UART_DEV_NAME_USART3,
    //                          1000,
    //                          10000,
    //                          10000);
    // if (ret != 0)
    // {
    //     return -8;
    // }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol,
                             UART_DEV_NAME_UART4,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        return -9;
    }

    ret = fdcan1_init();
    if (ret != 0)
    {
        return -11;
    }

    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_RTM_ON].uart_protocol,
                             UART_DEV_NAME_UART5,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        printf("uart_protocol_init error, ret = %d", ret);
        return -12;
    }
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        osMessageQueueAttr_t queue_attributes = {
            .name = self->rtm_module_info[i].module_name,
        };
        self->rtm_module_info[i].module_queue = osMessageQueueNew(10, sizeof(queue_frame_t), NULL);
        if (self->rtm_module_info[i].module_queue == NULL)
        {
            return -13;
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
        return -14;
    }
    thread_attributes.name = "app_ethercat_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].module_priority;
    threadHandle = osThreadNew(app_ethercat_tx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_OFF_PLC]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -15;
    }
    thread_attributes.name = "app_rtm_main_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_OFF_ARM].module_priority;
    app_rtm_main_threadId = osThreadNew(app_rtm_main_thread, self, &thread_attributes);
    if (app_rtm_main_threadId == NULL)
    {
        return -16;
    }
    // thread_attributes.name = "app_gmm_rx_thread";
    // thread_attributes.stack_size = 1024 * 4;
    // thread_attributes.priority = self->rtm_module_info[RTM_MODULE_GMM].module_priority;
    // threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_GMM]), &thread_attributes);
    // if (threadHandle == NULL)
    // {
    //     return -17;
    // }
    // thread_attributes.name = "app_gmm_tx_thread";
    // thread_attributes.stack_size = 1024 * 4;
    // thread_attributes.priority = self->rtm_module_info[RTM_MODULE_GMM].module_priority + 1;
    // threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_GMM]), &thread_attributes);
    // if (threadHandle == NULL)
    // {
    //     return -18;
    // }
    thread_attributes.name = "app_psm_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_PSM].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_PSM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -19;
    }
    thread_attributes.name = "app_psm_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_PSM].module_priority + 1;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_PSM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -20;
    }
    thread_attributes.name = "app_fkp_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_FKP].module_priority;
    threadHandle = osThreadNew(app_fkp_rx_thread, self, &thread_attributes);
    if (threadHandle == NULL)
    {
        return -21;
    }
    thread_attributes.name = "app_fkp_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_FKP].module_priority + 1;
    threadHandle = osThreadNew(app_fkp_tx_thread, self, &thread_attributes);
    if (threadHandle == NULL)
    {
        return -22;
    }
    thread_attributes.name = "app_cpg_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_CPG].module_priority;
    threadHandle = osThreadNew(app_cpg_rx_thread, self, &thread_attributes);
    if (threadHandle == NULL)
    {
        return -23;
    }
    thread_attributes.name = "app_cpg_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_CPG].module_priority + 1;
    threadHandle = osThreadNew(app_cpg_tx_thread, self, &thread_attributes);
    if (threadHandle == NULL)
    {
        return -24;
    }
    thread_attributes.name = "app_rtm_on_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_ON].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_ON]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -25;
    }
    thread_attributes.name = "app_rtm_on_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_ON].module_priority + 1;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_ON]), &thread_attributes);
    if (threadHandle == NULL)
    {
        return -26;
    }
    return 0;
}
INIT_APP_EXPORT(app_rtm_data_handle_create)

#define FKP_TEST
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
    int32_t ret = rtm_set_data_distribute(app_rtm.rtm_module_info->queue_group[RTM_MODULE_FKP], FKP_ID, RECEIVE_FKP_DOSE_CMD, &DoseData, sizeof(DoseData));
    if (ret != 0)
    {
        LOG_I("fkp set data distribute error, ret = %d\r\n", ret);
    }
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

#define PSM_TEST
#ifdef PSM_TEST
#include "shell.h"
static int8_t psm_test(int argc, char *argv[])
{
    int32_t ret = 0;

    rtm_status_t psm_status = {0};
    if (0 == strcmp(argv[1], "INIT"))
    {
        psm_status.not_ready_event = 0x1E;
    }
    else if (0 == strcmp(argv[1], "SETUP"))
    {
        psm_status.not_ready_event = 0x1C;
    }
    else if (0 == strcmp(argv[1], "LOAD"))
    {
        psm_status.not_ready_event = 0x18;
    }
    else if (0 == strcmp(argv[1], "PREPARE"))
    {
        psm_status.not_ready_event = 0x10;
    }
    else if (0 == strcmp(argv[1], "UNLOAD"))
    {
        psm_status.not_ready_event = 0x00;
    }
    else
    {
        goto usage;
    }
    ret = rtm_set_data_distribute(app_rtm.rtm_module_info[RTM_MODULE_PSM].queue_group[RTM_MODULE_RTM_OFF_ARM],
                                  RTM_OFF_ARM_ID,
                                  SEND_PSM_CURRENT_STATE_CMD,
                                  &psm_status,
                                  sizeof(rtm_status_t));
    if (ret != 0)
    {
        LOG_I("psm set data distribute error, ret = %d\r\n", ret);
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