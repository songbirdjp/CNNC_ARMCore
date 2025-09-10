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
#include "timestamp.h"
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

static int32_t rtm_set_data_distribute(osMessageQueueId_t queue_id, uint32_t ID, uint8_t cmd, uint8_t *data, uint16_t len);
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
            bit_set(app_rtm.rtm_module_info[i].manage_info.status_word, MODULE_TX_QUEUE_STATE_BIT);
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
    rtm_state_machine_ctor(&(self->state_machine), self);
    for (;;)
    {
        status = osMessageQueueGet(self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_queue, &queue_frame, NULL, 0);
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
                case OUTPUT_RTM_ON_ARM_REQUIRE_STATE_CMD: /*系统状态设置*/
                {
                    system_state_require = queue_frame.payload.data[1];

                    self->PLC_info = *(uint16_t *)&(queue_frame.payload.data[3]);
                    self->interlock_override = *(uint32_t *)&(queue_frame.payload.data[5]);
                    self->unready_override = *(uint32_t *)&(queue_frame.payload.data[9]);
                    break;
                }
                case OUTPUT_FAULT_CLEAR_CMD: /*故障清除*/
                {
                    // 清除故障
                    if (queue_frame.payload.data[1] & 0x01)
                    {
                        rtm_event.sig = ERROR_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                    }
                    break;
                }
                case INPUT_ICM_CURRENT_STATE_CMD: /*ICM当前状态*/
                {
                    self->icm_current_state = *(uint8_t *)&(queue_frame.payload.data[1]);
                    break;
                }
                case INPUT_BGM_CURRENT_STATE_CMD: /*BGM当前状态*/
                {
                    self->bgm_current_state = *(uint8_t *)&(queue_frame.payload.data[1]);
                    break;
                }
                case INPUT_QAM_CURRENT_STATE_CMD: /*QAM当前状态*/
                {
                    self->qam_current_state = *(uint8_t *)&(queue_frame.payload.data[1]);
                    break;
                }
                case OUTPUT_BEAM_ID_CMD: /*beamID*/
                {
                    self->beamID = *(uint8_t *)&(queue_frame.payload.data[1]);
                    break;
                }
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
        // 轮询状态机
        rtm_event.sig = system_state_require;
        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);

        rtm_event.sig = TIME_SIG;
        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);

        rtm_status.fsm_state_current = rtm_get_state(&(self->state_machine));
        rtm_status.beam_ID = self->beamID;
        rtm_status.not_ready_event = *(uint32_t *)&(self->interlock_table.not_ready_event);
        rtm_status.warning_interlock = *(uint32_t *)&(self->interlock_table.warning_interlock);
        rtm_status.minor_interlock = *(uint32_t *)&(self->interlock_table.minor_interlock);
        rtm_status.serious_interlock = *(uint32_t *)&(self->interlock_table.serious_interlock);

        if (memcmp(&rtm_status_old, &rtm_status, sizeof(rtm_status_t)) != 0)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_queue, RTM_ON_PLC_ID, INPUT_RTM_ON_ARM_CURRENT_STATE_CMD, (uint8_t *)&rtm_status, sizeof(rtm_status_t));
            memcpy(&rtm_status_old, &rtm_status, sizeof(rtm_status_t));
            last_time = osKernelGetTickCount();
        }
        // 周期上报状态
        current_time = osKernelGetTickCount();
        if (current_time - last_time > 1000)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_queue, RTM_ON_PLC_ID, INPUT_RTM_ON_ARM_CURRENT_STATE_CMD, (uint8_t *)&rtm_status, sizeof(rtm_status_t));
            last_time = current_time;
        }
        // DIDO上报
        app_do_get(&(self->app_dido), &dido_structure);
        app_di_get(&(self->app_dido), &dido_structure);
        if (memcmp(&dido_structure_old, &dido_structure, sizeof(dido_structure_t)) != 0)
        {
            memcpy(&dido_structure_old, &dido_structure, sizeof(dido_structure_t));
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_queue, RTM_ON_PLC_ID, INPUT_RTM_ON_ARM_DIDO_CMD, (uint8_t *)&dido_structure, sizeof(dido_structure_t));
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

static int32_t rtm_set_data_distribute(osMessageQueueId_t queue_id, uint32_t ID, uint8_t cmd, uint8_t *data, uint16_t len)
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
    bit_set(app_rtm.rtm_ethercat_info.manage_info.status_word, ETHERCAT_OP_STATE_BIT);
}
void app_rtm_ethercat_state_op_clean(void)
{
    bit_clean(app_rtm.rtm_ethercat_info.manage_info.status_word, ETHERCAT_OP_STATE_BIT);
}
enum
{
    OUTPUT_DATA_BEAM_ID = 0,
    OUTPUT_DATA_RADIATION_INDEX,
    OUTPUT_DATA_SYSTEM_STATE,
    OUTPUT_DATA_RTM_ON_REQUIRE_STATE,
    OUTPUT_DATA_ICM_REQUIRE_STATE,
    OUTPUT_DATA_BGM_REQUIRE_STATE,
    OUTPUT_DATA_QAM_REQUIRE_STATE,
    OUTPUT_DATA_RTM_OFF_REQUIRE_STATE,
    OUTPUT_DATA_GMM_REQUIRE_STATE,
    OUTPUT_DATA_PSM_REQUIRE_STATE,
    OUTPUT_DATA_FKP_TIME,
    OUTPUT_DATA_TREATMENT_RECORD_STATE,
};

static void ethercat_output_data_distribute(rtm_module_info_t *const self, TOBJ7010 *data)
{
    static TOBJ7010 output_data = {0};
    static uint8_t flag = OUTPUT_DATA_BEAM_ID;
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
        case OUTPUT_DATA_BEAM_ID:
            flag = OUTPUT_DATA_SYSTEM_STATE;
            len = (uint8_t *)&output_data.OutU8_state_sync - (uint8_t *)&output_data.OutU8_beam_id;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], BROADCAST_ID, OUTPUT_BEAM_ID_CMD, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], BROADCAST_ID, OUTPUT_BEAM_ID_CMD, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], BROADCAST_ID, OUTPUT_BEAM_ID_CMD, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], BROADCAST_ID, OUTPUT_BEAM_ID_CMD, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON_ARM], BROADCAST_ID, OUTPUT_BEAM_ID_CMD, &data->OutU8_beam_id, len);
            break;
        case OUTPUT_DATA_SYSTEM_STATE:
            flag = OUTPUT_DATA_RTM_ON_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU16_reboot - (uint8_t *)&output_data.OutU8_systemCurrentState;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], BROADCAST_ID, OUTPUT_SYSTEM_STATE_CMD, &data->OutU8_systemCurrentState, len);
            break;
        case OUTPUT_DATA_RTM_ON_REQUIRE_STATE:
            flag = OUTPUT_DATA_ICM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_icm_require_state - (uint8_t *)&output_data.OutU8_rtm_on_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON_ARM], RTM_ON_ARM_ID, OUTPUT_RTM_ON_ARM_REQUIRE_STATE_CMD, &data->OutU8_rtm_on_require_state, len);
            break;
        case OUTPUT_DATA_ICM_REQUIRE_STATE:
            flag = OUTPUT_DATA_BGM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_bgm_require_state - (uint8_t *)&output_data.OutU8_icm_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], ICM_ID, OUTPUT_ICM_REQUIRE_STATE_CMD, &data->OutU8_icm_require_state, len);
            break;
        case OUTPUT_DATA_BGM_REQUIRE_STATE:
            flag = OUTPUT_DATA_QAM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_qam_require_state - (uint8_t *)&output_data.OutU8_bgm_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], BGM_ID, OUTPUT_BGM_REQUIRE_STATE_CMD, &data->OutU8_bgm_require_state, len);
            break;
        case OUTPUT_DATA_QAM_REQUIRE_STATE:
            flag = OUTPUT_DATA_RTM_OFF_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_bsm_require_state - (uint8_t *)&output_data.OutU8_qam_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], QAM_ID, OUTPUT_QAM_REQUIRE_STATE_CMD, &data->OutU8_qam_require_state, len);
            break;
        case OUTPUT_DATA_RTM_OFF_REQUIRE_STATE:
            flag = OUTPUT_DATA_GMM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_gmm_require_state - (uint8_t *)&output_data.OutU8_rtm_off_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], RTM_OFF_ARM_ID, OUTPUT_RTM_OFF_REQUIRE_STATE_CMD, &data->OutU8_rtm_off_require_state, len);
            break;
        case OUTPUT_DATA_GMM_REQUIRE_STATE:
            flag = OUTPUT_DATA_PSM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_psm_require_state - (uint8_t *)&output_data.OutU8_gmm_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], GMM_ID, OUTPUT_GMM_REQUIRE_STATE_CMD, &data->OutU8_gmm_require_state, len);
            break;
        case OUTPUT_DATA_PSM_REQUIRE_STATE:
            flag = OUTPUT_DATA_FKP_TIME;
            len = (uint8_t *)&output_data.OutU16_fkp_year - (uint8_t *)&output_data.OutU8_psm_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], PSM_ID, OUTPUT_PSM_REQUIRE_STATE_CMD, &data->OutU8_psm_require_state, len);
            break;
        case OUTPUT_DATA_FKP_TIME:
            flag = OUTPUT_DATA_TREATMENT_RECORD_STATE;
            len = (uint8_t *)&output_data.OutU8_reserved3 - (uint8_t *)&output_data.OutU16_fkp_year;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], FKP_ID, OUTPUT_FKP_TIMESTAMP_CMD, &data->OutU16_fkp_year, len);
            break;
        case OUTPUT_DATA_TREATMENT_RECORD_STATE:
            flag = OUTPUT_DATA_BEAM_ID;
            len = (uint8_t *)&output_data.OutU8_ethercat_Link_state - (uint8_t *)&output_data.OutU8_treatment_record_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], RTM_OFF_ARM_ID, OUTPUT_RTM_OFF_ARM_TREATMENT_RECORD_CMD, &data->OutU8_treatment_record_state, len);
            break;
        default:
            break;
        }
    }
    else
    {
        // reboot
        len = (uint8_t *)&output_data.OutU8_treatment_record_state - (uint8_t *)&output_data.OutU16_reboot;
        if (memcmp(&output_data.OutU16_reboot, &data->OutU16_reboot, len) != 0)
        {
            // RTM_ON_ARM
            if (((output_data.OutU16_reboot & RTM_ON_ARM_ID) == 0) && ((data->OutU16_reboot & RTM_ON_ARM_ID) != 0))
            {
                HAL_NVIC_SystemReset(); // reset system
            }
            // ICM
            if (((output_data.OutU16_reboot & ICM_ID) == 0) && ((data->OutU16_reboot & ICM_ID) != 0))
            {
                uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_ICM].uart_protocol), ICM_ID, 1000);
            }
            // BGM
            if (((output_data.OutU16_reboot & BGM_ID) == 0) && ((data->OutU16_reboot & BGM_ID) != 0))
            {
                uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_BGM].uart_protocol), BGM_ID, 1000);
            }
            // QAM
            if (((output_data.OutU16_reboot & QAM_ID) == 0) && ((data->OutU16_reboot & QAM_ID) != 0))
            {
                uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_QAM].uart_protocol), QAM_ID, 1000);
            }
            // // BSM
            // if (((output_data.OutU16_reboot & BSM_ID) == 0) && ((data->OutU16_reboot & BSM_ID) != 0))
            // {
            //     uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_BSM].uart_protocol), BSM_ID, 1000);
            // }
            // RTM OFF ARM
            if (((output_data.OutU16_reboot & RTM_OFF_ARM_ID) == 0) && ((data->OutU16_reboot & RTM_OFF_ARM_ID) != 0))
            {
                uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_RTM_OFF].uart_protocol), RTM_OFF_ARM_ID, 1000);
            }
            // GMM
            if (((output_data.OutU16_reboot & GMM_ID) == 0) && ((data->OutU16_reboot & GMM_ID) != 0))
            {
                uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_RTM_OFF].uart_protocol), GMM_ID, 1000);
            }
            // PSM
            if (((output_data.OutU16_reboot & PSM_ID) == 0) && ((data->OutU16_reboot & PSM_ID) != 0))
            {
                uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_RTM_OFF].uart_protocol), PSM_ID, 1000);
            }
            // FKP
            if (((output_data.OutU16_reboot & FKP_ID) == 0) && ((data->OutU16_reboot & FKP_ID) != 0))
            {
                uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_RTM_OFF].uart_protocol), FKP_ID, 1000);
            }
            // // CPG
            // if (((output_data.OutU16_reboot & CPG_ID) == 0) && ((data->OutU16_reboot & CPG_ID) != 0))
            // {
            //     uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_RTM_OFF].uart_protocol), CPG_ID, 1000);
            // }
        }

        len = (uint8_t *)&output_data.OutU8_state_sync - (uint8_t *)&output_data.OutU8_beam_id;
        if (memcmp(&output_data.OutU8_beam_id, &data->OutU8_beam_id, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], BROADCAST_ID, OUTPUT_BEAM_ID_CMD, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], BROADCAST_ID, OUTPUT_BEAM_ID_CMD, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], BROADCAST_ID, OUTPUT_BEAM_ID_CMD, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], BROADCAST_ID, OUTPUT_BEAM_ID_CMD, &data->OutU8_beam_id, len);
        }

        len = (uint8_t *)&output_data.OutU16_radiation_index - (uint8_t *)&output_data.OutU8_state_sync;
        if (memcmp(&output_data.OutU8_state_sync, &data->OutU8_state_sync, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], BGM_ID | GMM_ID, OUTPUT_STATE_SYNC_CMD, &data->OutU8_state_sync, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], BGM_ID | GMM_ID, OUTPUT_STATE_SYNC_CMD, &data->OutU8_state_sync, len);
        }

        len = (uint8_t *)&output_data.OutU8_fault_clear - (uint8_t *)&output_data.OutU16_radiation_index;
        if (memcmp(&output_data.OutU16_radiation_index, &data->OutU16_radiation_index, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], BROADCAST_ID, OUTPUT_RADIATION_INDEX_CMD, &data->OutU16_radiation_index, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], BROADCAST_ID, OUTPUT_RADIATION_INDEX_CMD, &data->OutU16_radiation_index, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], BROADCAST_ID, OUTPUT_RADIATION_INDEX_CMD, &data->OutU16_radiation_index, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], BROADCAST_ID, OUTPUT_RADIATION_INDEX_CMD, &data->OutU16_radiation_index, len);
        }

        len = (uint8_t *)&output_data.OutU16_reboot - (uint8_t *)&output_data.OutU8_systemCurrentState;
        if (memcmp(&output_data.OutU8_systemCurrentState, &data->OutU8_systemCurrentState, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], BROADCAST_ID, OUTPUT_SYSTEM_STATE_CMD, &data->OutU8_systemCurrentState, len);
        }

        len = (uint8_t *)&output_data.OutU8_systemCurrentState - (uint8_t *)&output_data.OutU8_fault_clear;
        if (memcmp(&output_data.OutU8_fault_clear, &data->OutU8_fault_clear, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON_ARM], BROADCAST_ID, OUTPUT_FAULT_CLEAR_CMD, &data->OutU8_fault_clear, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], BROADCAST_ID, OUTPUT_FAULT_CLEAR_CMD, &data->OutU8_fault_clear, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], BROADCAST_ID, OUTPUT_FAULT_CLEAR_CMD, &data->OutU8_fault_clear, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], BROADCAST_ID, OUTPUT_FAULT_CLEAR_CMD, &data->OutU8_fault_clear, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], BROADCAST_ID, OUTPUT_FAULT_CLEAR_CMD, &data->OutU8_fault_clear, len);
        }

        len = (uint8_t *)&output_data.OutU8_icm_require_state - (uint8_t *)&output_data.OutU8_rtm_on_require_state;
        if (memcmp(&output_data.OutU8_rtm_on_require_state, &data->OutU8_rtm_on_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON_ARM], RTM_ON_ARM_ID, OUTPUT_RTM_ON_ARM_REQUIRE_STATE_CMD, &data->OutU8_rtm_on_require_state, len);
            app_data_record_from_ethercat(self, RTM_ON_ARM_ID, OUTPUT_RTM_ON_ARM_REQUIRE_STATE_CMD, &data->OutU8_rtm_on_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_bgm_require_state - (uint8_t *)&output_data.OutU8_icm_require_state;
        if (memcmp(&output_data.OutU8_icm_require_state, &data->OutU8_icm_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], ICM_ID, OUTPUT_ICM_REQUIRE_STATE_CMD, &data->OutU8_icm_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_qam_require_state - (uint8_t *)&output_data.OutU8_bgm_require_state;
        if (memcmp(&output_data.OutU8_bgm_require_state, &data->OutU8_bgm_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], BGM_ID, OUTPUT_BGM_REQUIRE_STATE_CMD, &data->OutU8_bgm_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_bsm_require_state - (uint8_t *)&output_data.OutU8_qam_require_state;
        if (memcmp(&output_data.OutU8_qam_require_state, &data->OutU8_qam_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], QAM_ID, OUTPUT_QAM_REQUIRE_STATE_CMD, &data->OutU8_qam_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_gmm_require_state - (uint8_t *)&output_data.OutU8_rtm_off_require_state;
        if (memcmp(&output_data.OutU8_rtm_off_require_state, &data->OutU8_rtm_off_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], RTM_OFF_ARM_ID, OUTPUT_RTM_OFF_REQUIRE_STATE_CMD, &data->OutU8_rtm_off_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_psm_require_state - (uint8_t *)&output_data.OutU8_gmm_require_state;
        if (memcmp(&output_data.OutU8_gmm_require_state, &data->OutU8_gmm_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], GMM_ID, OUTPUT_GMM_REQUIRE_STATE_CMD, &data->OutU8_gmm_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU16_fkp_year - (uint8_t *)&output_data.OutU8_psm_require_state;
        if (memcmp(&output_data.OutU8_psm_require_state, &data->OutU8_psm_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], PSM_ID, OUTPUT_PSM_REQUIRE_STATE_CMD, &data->OutU8_psm_require_state, len);
        }
        len = (uint8_t *)&output_data.OutU8_reserved3 - (uint8_t *)&output_data.OutU16_fkp_year;
        if (memcmp(&output_data.OutU16_fkp_year, &data->OutU16_fkp_year, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], FKP_ID, OUTPUT_FKP_TIMESTAMP_CMD, &data->OutU16_fkp_year, len);
        }
        len = (uint8_t *)&output_data.OutU8_ethercat_Link_state - (uint8_t *)&output_data.OutU8_treatment_record_state;
        if (memcmp(&output_data.OutU8_treatment_record_state, &data->OutU8_treatment_record_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], RTM_OFF_ARM_ID, OUTPUT_RTM_OFF_ARM_TREATMENT_RECORD_CMD, &data->OutU8_treatment_record_state, len);
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
    case INPUT_RADIATION_INDEX_CMD:
        memcpy(&input_data->InU16_radiation_index, queue_frame->payload.data + 1, len);
        break;
    case INPUT_RTM_ON_ARM_CURRENT_STATE_CMD:
        memcpy(&input_data->InU8_rtm_on_arm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case INPUT_RTM_ON_ARM_DIDO_CMD:
        memcpy((uint8_t *)(&input_data->InU32_rtm_on_arm_serious_interlock) + sizeof(input_data->InU32_rtm_on_arm_serious_interlock), queue_frame->payload.data + 1, len);
        break;
    case INPUT_ICM_CURRENT_STATE_CMD:
        memcpy(&input_data->InU8_icm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case INPUT_BGM_CURRENT_STATE_CMD:
        memcpy(&input_data->InU8_bgm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case INPUT_BGM_INFO_CMD:
        memcpy(&input_data->InF_beam_on_time, queue_frame->payload.data + 1, len);
        break;
    case INPUT_QAM_CURRENT_STATE_CMD:
        memcpy(&input_data->InU8_qam_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case INPUT_RTM_OFF_ARM_CURRENT_STATE_CMD:
        memcpy(&input_data->InU8_rtm_off_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case INPUT_RTM_OFF_ARM_DIDO_CMD:
        memcpy((uint8_t *)(&input_data->InU8_ON_DO_reserve2) + sizeof(input_data->InU8_ON_DO_reserve2), queue_frame->payload.data + 1, len);
        break;
    case INPUT_GMM_CURRENT_STATE_CMD:
        memcpy(&input_data->InU8_gmm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case INPUT_GMM_INFO_CMD:
        memcpy(&input_data->InU32_gmm_move_status, queue_frame->payload.data + 1, len);
        break;
    case INPUT_PSM_CURRENT_STATE_CMD:
        memcpy(&input_data->InU8_psm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case INPUT_PSM_INFO_CMD:
        memcpy(&input_data->InU32_psm_move_status, queue_frame->payload.data + 1, len);
        break;
    case INPUT_RTM_OFF_ARM_BUTTON_CMD:
        memcpy(&input_data->InU8_function_button, queue_frame->payload.data + 1, len);
        break;
    case INPUT_RTM_OFF_ARM_TRM_REQUIRE_CMD:
        memcpy(&input_data->InU8_trm_require_state, queue_frame->payload.data + 1, len);
        break;
    case INPUT_RTM_OFF_ARM_LOAD_POSITION_CMD:
        memcpy(&input_data->InU16_data_valid_flag, queue_frame->payload.data + 1, len);
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
                last_time = current_time;
            }
            // TODO: 处理输出数据
            ethercat_output_data_distribute(self, &output_data);
        }
        current_time = osKernelGetTickCount();
        if (current_time - last_time > 1000)
        {
            last_time = current_time;
            if (bit_get(self->manage_info.status_word, MODULE_LINK_STATE_BIT) == 0)
            {
                bit_set(self->manage_info.status_word, MODULE_LINK_STATE_BIT);
                LOG_E("%s ethercat unlink state\r\n", self->module_name);
            }
        }
    }
exit:
    bit_set(self->manage_info.status_word, MODULE_RX_INIT_BIT);
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
    bit_set(self->manage_info.status_word, MODULE_TX_INIT_BIT);
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
    HAL_NVIC_SystemReset(); // reset system
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
                                                NULL);
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
            bit_set(self->manage_info.status_word, MODULE_RX_STATE_BIT);
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
    bit_set(self->manage_info.status_word, MODULE_RX_INIT_BIT);
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
            bit_set(self->manage_info.status_word, MODULE_TX_STATE_BIT);
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
            bit_set(self->manage_info.status_word, MODULE_TX_STATE_BIT);
            LOG_E("%s send error, ret = %d\r\n", self->module_name, ret);
            continue;
        }
    }
exit:
    bit_set(self->manage_info.status_word, MODULE_TX_INIT_BIT);
    LOG_E("%s tx thread exit\r\n", self->module_name);
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

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_type = "SLAVE";
    self->rtm_module_info[RTM_MODULE_ICM].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_BGM].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_QAM].module_type = "MASTER";
    // self->rtm_module_info[RTM_MODULE_BSM].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_RTM_OFF].module_type = "MASTER";

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].ID = RTM_ON_PLC_ID;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].ID = RTM_ON_ARM_ID;
    self->rtm_module_info[RTM_MODULE_ICM].ID = ICM_ID;
    self->rtm_module_info[RTM_MODULE_BGM].ID = BGM_ID;
    self->rtm_module_info[RTM_MODULE_QAM].ID = QAM_ID;
    // self->rtm_module_info[RTM_MODULE_BSM].ID = BSM_ID;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].ID = RTM_OFF_ARM_ID | GMM_ID | PSM_ID | FKP_ID | CPG_ID;
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        for (uint8_t j = 0; j < RTM_MODULE_MAX; j++)
        {
            self->rtm_module_info[j].id_group[i] = self->rtm_module_info[i].ID;
        }
    }

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_priority = osPriorityAboveNormal;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_priority = osPriorityAboveNormal;
    self->rtm_module_info[RTM_MODULE_ICM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_BGM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_QAM].module_priority = osPriorityNormal;
    // self->rtm_module_info[RTM_MODULE_BSM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].module_priority = osPriorityAboveNormal;

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_thread_flags = APP_RTM_THREAD_FLAG_ETHERCAT_READY;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_thread_flags = 0;
    self->rtm_module_info[RTM_MODULE_ICM].module_thread_flags = APP_RTM_THREAD_FLAG_ICM_READY;
    self->rtm_module_info[RTM_MODULE_BGM].module_thread_flags = APP_RTM_THREAD_FLAG_BGM_READY;
    self->rtm_module_info[RTM_MODULE_QAM].module_thread_flags = APP_RTM_THREAD_FLAG_QAM_READY;
    // self->rtm_module_info[RTM_MODULE_BSM].module_thread_flags = APP_RTM_THREAD_FLAG_BSM_READY;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].module_thread_flags = APP_RTM_THREAD_FLAG_RTM_OFF_READY;

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].tx_disable = MODULE_TX_DISABLE;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].tx_disable = MODULE_TX_DISABLE;
    self->rtm_module_info[RTM_MODULE_ICM].tx_disable = MODULE_TX_DISABLE;
    self->rtm_module_info[RTM_MODULE_BGM].tx_disable = MODULE_TX_DISABLE;
    self->rtm_module_info[RTM_MODULE_QAM].tx_disable = MODULE_TX_DISABLE;
    // self->rtm_module_info[RTM_MODULE_BSM].tx_disable = MODULE_TX_DISABLE;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].tx_disable = MODULE_TX_DISABLE;
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
    ret = app_data_record_init(&self->app_data_record);
    if (ret != 0)
    {
        bit_set(self->app_data_record.manage_info.status_word, RECORD_PERIPHERAL_INIT_BIT);
        LOG_E("app_data_record_init error, ret = %d\r\n", ret);
    }
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        self->rtm_module_info[i].app_data_record = &self->app_data_record;
    }
    ret = ethercat_thread_init();
    if (ret != 0)
    {
        bit_set(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].manage_info.status_word, MODULE_PERIPHERAL_INIT_BIT);
        LOG_E("ethercat_thread_init error, ret = %d\r\n", ret);
    }
    osEventFlagsAttr_t event_attributes = {
        .name = "output_event"};

    self->ethercat_Event = osEventFlagsNew(&event_attributes);
    if (self->ethercat_Event == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("osEventFlagsNew error\r\n");
    }

    ret = app_dido_create(&self->app_dido);
    if (ret != 0)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_DIDO_STATE_BIT);
        LOG_E("app_dido_create error, ret = %d\r\n", ret);
    }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_ICM].uart_protocol,
                             UART_DEV_NAME_UART5,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        bit_set(self->rtm_module_info[RTM_MODULE_ICM].manage_info.status_word, MODULE_PERIPHERAL_INIT_BIT);
        LOG_E("icm uart_protocol_init error, ret = %d\r\n", ret);
    }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_BGM].uart_protocol,
                             UART_DEV_NAME_USART3,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        bit_set(self->rtm_module_info[RTM_MODULE_BGM].manage_info.status_word, MODULE_PERIPHERAL_INIT_BIT);
        LOG_E("bgm uart_protocol_init error, ret = %d\r\n", ret);
    }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_QAM].uart_protocol,
                             UART_DEV_NAME_UART7,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        bit_set(self->rtm_module_info[RTM_MODULE_QAM].manage_info.status_word, MODULE_PERIPHERAL_INIT_BIT);
        LOG_E("qam uart_protocol_init error, ret = %d\r\n", ret);
    }
    // ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_BSM].uart_protocol,
    //                          UART_DEV_NAME_UART4,
    //                          1000,
    //                          10000,
    //                          10000);
    // if (ret != 0)
    // {
    //     bit_set(self->rtm_module_info[RTM_MODULE_BSM].manage_info.status_word, MODULE_PERIPHERAL_INIT_BIT);
    //     LOG_E("bsm uart_protocol_init error, ret = %d\r\n", ret);
    // }
    ret = uart_protocol_init(&self->rtm_module_info[RTM_MODULE_RTM_OFF].uart_protocol,
                             UART_DEV_NAME_USART2,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        bit_set(self->rtm_module_info[RTM_MODULE_RTM_OFF].manage_info.status_word, MODULE_PERIPHERAL_INIT_BIT);
        LOG_E("rtm_off uart_protocol_init error, ret = %d\r\n", ret);
    }
    for (uint8_t i = 0; i < RTM_MODULE_MAX; i++)
    {
        osMessageQueueAttr_t queue_attributes = {
            .name = self->rtm_module_info[i].module_name,
        };
        self->rtm_module_info[i].module_queue = osMessageQueueNew(10, sizeof(queue_frame_t), NULL);
        if (self->rtm_module_info[i].module_queue == NULL)
        {
            bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
            LOG_E("osMessageQueueNew error, module_name = %s\r\n", self->rtm_module_info[i].module_name);
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
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_ethercat_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_ethercat_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_priority;
    threadHandle = osThreadNew(app_ethercat_tx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC]), &thread_attributes);
    if (threadHandle == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_ethercat_tx_thread create error\r\n");
    }
    thread_attributes.name = "app_rtm_main_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_priority;
    app_rtm_main_threadId = osThreadNew(app_rtm_main_thread, self, &thread_attributes);
    if (app_rtm_main_threadId == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_rtm_main_thread create error\r\n");
    }
    thread_attributes.name = "app_icm_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_ICM].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_ICM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_icm_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_icm_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_ICM].module_priority + 1;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_ICM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_icm_tx_thread create error\r\n");
    }
    thread_attributes.name = "app_bgm_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_BGM].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_BGM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_bgm_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_bgm_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_BGM].module_priority + 1;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_BGM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_bgm_tx_thread create error\r\n");
    }
    thread_attributes.name = "app_qam_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_QAM].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_QAM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_qam_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_qam_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_QAM].module_priority + 1;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_QAM]), &thread_attributes);
    if (threadHandle == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_qam_tx_thread create error\r\n");
    }
    // thread_attributes.name = "app_bsm_rx_thread";
    // thread_attributes.stack_size = 1024 * 4;
    // thread_attributes.priority = self->rtm_module_info[RTM_MODULE_BSM].module_priority;
    // threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_BSM]), &thread_attributes);
    // if (threadHandle == NULL)
    // {
    // bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
    // LOG_E("app_bsm_rx_thread create error\r\n");
    // }
    // thread_attributes.name = "app_bsm_tx_thread";
    // thread_attributes.stack_size = 1024 * 4;
    // thread_attributes.priority = self->rtm_module_info[RTM_MODULE_BSM].module_priority + 1;
    // threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_BSM]), &thread_attributes);
    // if (threadHandle == NULL)
    // {
    // bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
    // LOG_E("app_bsm_tx_thread create error\r\n");
    // }
    thread_attributes.name = "app_rtm_off_rx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_OFF].module_priority;
    threadHandle = osThreadNew(app_module_rx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_OFF]), &thread_attributes);
    if (threadHandle == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_rtm_off_rx_thread create error\r\n");
    }
    thread_attributes.name = "app_rtm_off_tx_thread";
    thread_attributes.stack_size = 1024 * 4;
    thread_attributes.priority = self->rtm_module_info[RTM_MODULE_RTM_OFF].module_priority + 1;
    threadHandle = osThreadNew(app_module_tx_thread, &(self->rtm_module_info[RTM_MODULE_RTM_OFF]), &thread_attributes);
    if (threadHandle == NULL)
    {
        bit_set(self->manage_info.status_word, RTM_MAIN_MEMORY_STATE_BIT);
        LOG_E("app_rtm_off_tx_thread create error\r\n");
    }
    return 0;
}
INIT_APP_EXPORT(app_rtm_data_handle_create)
#define REBOOT_TEST
#ifdef REBOOT_TEST
#include "shell.h"

static int reboot_test(int argc, char **argv)
{
    int32_t ret = 0;
    ret = uart_protocol_reboot(&(app_rtm.rtm_module_info[RTM_MODULE_RTM_OFF].uart_protocol), RTM_OFF_ARM_ID, 1000);
    if (ret != 0)
    {
        LOG_E("uart_protocol_reboot error, ret = %d", ret);
    }
    return 0;
}
MSH_CMD_EXPORT_ALIAS(reboot_test, reboot_test, reboot test);
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
            LOG_I("DI RTC_WD_OK_IN value: %d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.RTC_WD_OK_IN);
            LOG_I("DI DI_BSM_NOT_READY value: %d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_BSM_NOT_READY);
            LOG_I("DI DI_MV_TreatmentEN value: %d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_MV_TreatmentEN);
            LOG_I("DI DI_HVEN value: %d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_HVEN);
            LOG_I("DI DI_Pulse_Inhibit value: %d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_Pulse_Inhibit);
            LOG_I("DI DI_KV_TreatmentEN value: %d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_KV_TreatmentEN);
            LOG_I("DI DI_Power_cut_FB value: %d\r\n", dido_value.mcp23017_0x00_u.mcp23017_0x00_bit.DI_Power_cut_FB);

            LOG_I("DI DI_GATING_IN value: %d\r\n", dido_value.gpio_di_u.gpio_di_bit.DI_GATING_IN);
            LOG_I("DI DI_Slipring_HVEN_IN value: %d\r\n", dido_value.gpio_di_u.gpio_di_bit.DI_Slipring_HVEN_IN);
            LOG_I("DI DI_Slipring_KV_TreatmentEN_IN value: %d\r\n", dido_value.gpio_di_u.gpio_di_bit.DI_Slipring_KV_TreatmentEN_IN);
            LOG_I("DI DI_Slipring_MV_TreatmentEN_IN value: %d\r\n", dido_value.gpio_di_u.gpio_di_bit.DI_Slipring_MV_TreatmentEN_IN);
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

            dido_value.gpio_do_u.gpio_do = atoi(argv[3]);

            app_do_set(&app_rtm.app_dido, &dido_value);
            LOG_I("DO GPIO_DO value: %d\r\n", dido_value.gpio_do_u.gpio_do);
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
    LOG_I("RTM_STATUS_TEST\r\n");
    LOG_I("rtn_main:0x%x\r\n", app_rtm.manage_info.status_word);
    LOG_I("RTM_MODULE_ICM:0x%x\r\n", app_rtm.rtm_module_info[RTM_MODULE_ICM].manage_info.status_word);
    LOG_I("RTM_MODULE_BGM:0x%x\r\n", app_rtm.rtm_module_info[RTM_MODULE_BGM].manage_info.status_word);
    LOG_I("RTM_MODULE_QAM:0x%x\r\n", app_rtm.rtm_module_info[RTM_MODULE_QAM].manage_info.status_word);
    // LOG_I("RTM_MODULE_BSM:0x%x\r\n", app_rtm.rtm_module_info[RTM_MODULE_BSM].manage_info.status_word);
    LOG_I("RTM_MODULE_RTM_OFF:0x%x\r\n", app_rtm.rtm_module_info[RTM_MODULE_RTM_OFF].manage_info.status_word);
    LOG_I("RTM_MODULE_RTM_ON_PLC:0x%x\r\n", app_rtm.rtm_module_info[RTM_MODULE_RTM_ON_PLC].manage_info.status_word);
    LOG_I("RTM_MODULE_RTM_ON_ARM:0x%x\r\n", app_rtm.rtm_module_info[RTM_MODULE_RTM_ON_ARM].manage_info.status_word);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(rtm_status_test, rtm_status_test, rtm status test);
#endif