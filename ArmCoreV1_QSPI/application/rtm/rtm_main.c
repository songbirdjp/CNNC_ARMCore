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
#define RTM_MAIN_THREAD_CYCLE_MS (1)
static void app_rtm_main_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    osStatus_t status = osOK;
    queue_frame_t queue_frame;
    rtm_status_t rtm_status = {0};
    rtm_status_t rtm_status_old = {0};

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
                case 0x1F: /*系统状态设置*/
                {
                    system_state_require = queue_frame.payload.data[1];

                    self->interlock_override = *(uint32_t *)&(queue_frame.payload.data[3]);
                    self->unready_override = *(uint32_t *)&(queue_frame.payload.data[7]);
                    break;
                }
                case 0x02: /*故障清除*/
                {
                    // 清除故障
                    if (queue_frame.payload.data[1] & 0x01)
                    {
                        rtm_event.sig = ERROR_SIG;
                        rtm_state_dispatch(&(self->state_machine), (Event_t *)&rtm_event);
                    }
                    break;
                }
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
        rtm_status.not_ready_event = *(uint32_t *)&(self->interlock_table.not_ready_event);
        rtm_status.warning_interlock = *(uint32_t *)&(self->interlock_table.warning_interlock);
        rtm_status.minor_interlock = *(uint32_t *)&(self->interlock_table.minor_interlock);
        rtm_status.serious_interlock = *(uint32_t *)&(self->interlock_table.serious_interlock);

        if (memcmp(&rtm_status_old, &rtm_status, sizeof(rtm_status_t)) != 0)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_queue, 0x01, 0x1E, (uint8_t *)&rtm_status, sizeof(rtm_status_t));
            memcpy(&rtm_status_old, &rtm_status, sizeof(rtm_status_t));
            last_time = osKernelGetTickCount();
        }
        // 周期上报状态
        current_time = osKernelGetTickCount();
        if (current_time - last_time > 1000)
        {
            rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_queue, 0x01, 0x1E, (uint8_t *)&rtm_status, sizeof(rtm_status_t));
            last_time = current_time;
        }

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
    OUTPUT_DATA_BEAM_ID = 0,
    OUTPUT_DATA_RADIATION_INDEX,
    OUTPUT_DATA_RTM_ON_REQUIRE_STATE,
    OUTPUT_DATA_ICM_REQUIRE_STATE,
    OUTPUT_DATA_BGM_REQUIRE_STATE,
    OUTPUT_DATA_QAM_REQUIRE_STATE,
    OUTPUT_DATA_RTM_OFF_REQUIRE_STATE,
    OUTPUT_DATA_GMM_REQUIRE_STATE,
    OUTPUT_DATA_PSM_REQUIRE_STATE,
    OUTPUT_DATA_FKP_LED_BLINK,
    OUTPUT_DATA_CPG_LED_BLINK,
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
            flag = OUTPUT_DATA_RTM_ON_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_state_sync - (uint8_t *)&output_data.OutU8_beam_id;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], 0x0, 0x0, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], 0x0, 0x0, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], 0x0, 0x0, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x0, 0x0, &data->OutU8_beam_id, len);
            break;
        case OUTPUT_DATA_RTM_ON_REQUIRE_STATE:
            flag = OUTPUT_DATA_ICM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_icm_require_state - (uint8_t *)&output_data.OutU8_rtm_on_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON_ARM], 0x2, 0x1F, &data->OutU8_rtm_on_require_state, len);
            break;
        case OUTPUT_DATA_ICM_REQUIRE_STATE:
            flag = OUTPUT_DATA_BGM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_bgm_require_state - (uint8_t *)&output_data.OutU8_icm_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], 0x4, 0x12, &data->OutU8_icm_require_state, len);
            break;
        case OUTPUT_DATA_BGM_REQUIRE_STATE:
            flag = OUTPUT_DATA_QAM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_qam_require_state - (uint8_t *)&output_data.OutU8_bgm_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], 0x8, 0x13, &data->OutU8_bgm_require_state, len);
            break;
        case OUTPUT_DATA_QAM_REQUIRE_STATE:
            flag = OUTPUT_DATA_RTM_OFF_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_bsm_require_state - (uint8_t *)&output_data.OutU8_qam_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], 0x10, 0x14, &data->OutU8_qam_require_state, len);
            break;
        case OUTPUT_DATA_RTM_OFF_REQUIRE_STATE:
            flag = OUTPUT_DATA_GMM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_gmm_require_state - (uint8_t *)&output_data.OutU8_rtm_off_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x40, 0x16, &data->OutU8_rtm_off_require_state, len);
            break;
        case OUTPUT_DATA_GMM_REQUIRE_STATE:
            flag = OUTPUT_DATA_PSM_REQUIRE_STATE;
            len = (uint8_t *)&output_data.OutU8_psm_require_state - (uint8_t *)&output_data.OutU8_gmm_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x80, 0x19, &data->OutU8_gmm_require_state, len);
            break;
        case OUTPUT_DATA_PSM_REQUIRE_STATE:
            flag = OUTPUT_DATA_BEAM_ID;
            len = (uint8_t *)&output_data.OutU8_fkp_led_blink - (uint8_t *)&output_data.OutU8_psm_require_state;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x100, 0x1A, &data->OutU8_psm_require_state, len);
            break;
        case OUTPUT_DATA_FKP_LED_BLINK:
            flag = OUTPUT_DATA_CPG_LED_BLINK;
            len = (uint8_t *)&output_data.OutU16_fkp_year - (uint8_t *)&output_data.OutU8_fkp_led_blink;
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x200, 0x17, &data->OutU8_fkp_led_blink, len);
            break;
        case OUTPUT_DATA_CPG_LED_BLINK:
            flag = OUTPUT_DATA_BEAM_ID;
            len = sizeof(output_data.OutU8_cpg_led_blink);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x400, 0x18, &data->OutU8_cpg_led_blink, len);
            break;
        default:
            break;
        }
    }
    else
    {
        len = (uint8_t *)&output_data.OutU8_state_sync - (uint8_t *)&output_data.OutU8_beam_id;
        if (memcmp(&output_data.OutU8_beam_id, &data->OutU8_beam_id, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], 0x0, 0x0, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], 0x0, 0x0, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], 0x0, 0x0, &data->OutU8_beam_id, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x0, 0x0, &data->OutU8_beam_id, len);
        }

        len = (uint8_t *)&output_data.OutU16_radiation_index - (uint8_t *)&output_data.OutU8_state_sync;
        if (memcmp(&output_data.OutU8_state_sync, &data->OutU8_state_sync, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], 0x08 | 0x80, 0x1E, &data->OutU8_state_sync, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x08 | 0x80, 0x1E, &data->OutU8_state_sync, len);
        }

        len = (uint8_t *)&output_data.OutU8_fault_clear - (uint8_t *)&output_data.OutU16_radiation_index;
        if (memcmp(&output_data.OutU16_radiation_index, &data->OutU16_radiation_index, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], 0x0, 0x1, &data->OutU16_radiation_index, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], 0x0, 0x1, &data->OutU16_radiation_index, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], 0x0, 0x1, &data->OutU16_radiation_index, len);
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x0, 0x1, &data->OutU16_radiation_index, len);
        }

        len = (uint8_t *)&output_data.OutU8_ethercat_Link_state - (uint8_t *)&output_data.OutU8_fault_clear;
        if (memcmp(&output_data.OutU8_fault_clear, &data->OutU8_fault_clear, len) != 0)
        {
            if (data->OutU8_fault_clear & 0x01)
            {
                rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON_ARM], 0x0, 0x2, &data->OutU8_fault_clear, len);
                rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], 0x0, 0x2, &data->OutU8_fault_clear, len);
                rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], 0x0, 0x2, &data->OutU8_fault_clear, len);
                rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], 0x0, 0x2, &data->OutU8_fault_clear, len);
                rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x0, 0x2, &data->OutU8_fault_clear, len);
            }
        }
        len = (uint8_t *)&output_data.OutU8_icm_require_state - (uint8_t *)&output_data.OutU8_rtm_on_require_state;
        if (memcmp(&output_data.OutU8_rtm_on_require_state, &data->OutU8_rtm_on_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_ON_ARM], 0x2, 0x1F, &data->OutU8_rtm_on_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_bgm_require_state - (uint8_t *)&output_data.OutU8_icm_require_state;
        if (memcmp(&output_data.OutU8_icm_require_state, &data->OutU8_icm_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_ICM], 0x4, 0x12, &data->OutU8_icm_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_qam_require_state - (uint8_t *)&output_data.OutU8_bgm_require_state;
        if (memcmp(&output_data.OutU8_bgm_require_state, &data->OutU8_bgm_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_BGM], 0x8, 0x13, &data->OutU8_bgm_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_bsm_require_state - (uint8_t *)&output_data.OutU8_qam_require_state;
        if (memcmp(&output_data.OutU8_qam_require_state, &data->OutU8_qam_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_QAM], 0x10, 0x14, &data->OutU8_qam_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_gmm_require_state - (uint8_t *)&output_data.OutU8_rtm_off_require_state;
        if (memcmp(&output_data.OutU8_rtm_off_require_state, &data->OutU8_rtm_off_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x40, 0x16, &data->OutU8_rtm_off_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_psm_require_state - (uint8_t *)&output_data.OutU8_gmm_require_state;
        if (memcmp(&output_data.OutU8_gmm_require_state, &data->OutU8_gmm_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x80, 0x19, &data->OutU8_gmm_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_fkp_led_blink - (uint8_t *)&output_data.OutU8_psm_require_state;
        if (memcmp(&output_data.OutU8_psm_require_state, &data->OutU8_psm_require_state, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x100, 0x1A, &data->OutU8_psm_require_state, len);
        }

        len = (uint8_t *)&output_data.OutU8_fkp_vibration - (uint8_t *)&output_data.OutU8_fkp_led_blink;
        if (memcmp(&output_data.OutU8_fkp_led_blink, &data->OutU8_fkp_led_blink, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x200, 0x17, &data->OutU8_fkp_led_blink, len);
        }

        len = (uint8_t *)&output_data.OutU16_fkp_year - (uint8_t *)&output_data.OutU8_fkp_vibration;
        if (memcmp(&output_data.OutU8_fkp_vibration, &data->OutU8_fkp_vibration, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x200, 0x1B, &data->OutU8_fkp_vibration, len);
        }

        len = (uint8_t *)&output_data.OutU8_fkp_systemCurrentState - (uint8_t *)&output_data.OutU16_fkp_year;
        if (memcmp(&output_data.OutU16_fkp_year, &data->OutU16_fkp_year, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x200, 0x1C, &data->OutU16_fkp_year, len);
        }

        len = (uint8_t *)&output_data.OutU8_cpg_led_blink - (uint8_t *)&output_data.OutU8_fkp_systemCurrentState;
        if (memcmp(&output_data.OutU8_fkp_systemCurrentState, &data->OutU8_fkp_systemCurrentState, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x200, 0x1D, &data->OutU8_fkp_systemCurrentState, len);
        }

        len = sizeof(output_data.OutU8_cpg_led_blink);
        if (memcmp(&output_data.OutU8_cpg_led_blink, &data->OutU8_cpg_led_blink, len) != 0)
        {
            rtm_set_data_distribute(self->queue_group[RTM_MODULE_RTM_OFF], 0x400, 0x18, &data->OutU8_cpg_led_blink, len);
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
    case 0x01:
        memcpy(&input_data->InU16_radiation_index, queue_frame->payload.data + 1, len);
        break;
    case 0x1E:
        memcpy(&input_data->InU8_rtm_on_arm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case 0x21:
        memcpy(&input_data->InU8_icm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case 0x31:
        memcpy(&input_data->InU8_bgm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case 0x32:
        memcpy(&input_data->InF_beam_on_time, queue_frame->payload.data + 1, len);
        break;
    case 0x41:
        memcpy(&input_data->InU8_qam_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case 0x61:
        memcpy(&input_data->InU8_rtm_off_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case 0x81:
        memcpy(&input_data->InU8_gmm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case 0x82:
        memcpy(&input_data->InU32_gmm_move_status, queue_frame->payload.data + 1, len);
        break;
    case 0x71:
        memcpy(&input_data->InU8_psm_fsm_state_current, queue_frame->payload.data + 1, len);
        break;
    case 0x72:
        memcpy(&input_data->InU32_psm_move_status, queue_frame->payload.data + 1, len);
        break;
    case 0x91:
        memcpy(&input_data->InU16_FkpButton, queue_frame->payload.data + 1, len);
        break;
    case 0xA1:
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
        if (current_time - last_time > 1000)
        {
            LOG_I("%s unlink\r\n", self->module_name);
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
        status = osMessageQueueGet(self->module_queue, &queue_frame, NULL, 500);
        if (status == osOK)
        {
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
            LOG_E("%s send error, ret = %d\r\n", self->module_name, ret);
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

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_type = "SLAVE";
    self->rtm_module_info[RTM_MODULE_ICM].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_BGM].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_QAM].module_type = "MASTER";
    // self->rtm_module_info[RTM_MODULE_BSM].module_type = "MASTER";
    self->rtm_module_info[RTM_MODULE_RTM_OFF].module_type = "MASTER";

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

    self->rtm_module_info[RTM_MODULE_RTM_ON_PLC].module_priority = osPriorityAboveNormal;
    self->rtm_module_info[RTM_MODULE_RTM_ON_ARM].module_priority = osPriorityAboveNormal;
    self->rtm_module_info[RTM_MODULE_ICM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_BGM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_QAM].module_priority = osPriorityNormal;
    // self->rtm_module_info[RTM_MODULE_BSM].module_priority = osPriorityNormal;
    self->rtm_module_info[RTM_MODULE_RTM_OFF].module_priority = osPriorityAboveNormal;

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