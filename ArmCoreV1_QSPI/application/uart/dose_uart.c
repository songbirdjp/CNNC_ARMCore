#include "dose_uart.h"
#include "uart_port.h"
#include "init_call.h"
#include "sys_cfg.h"
#include "fsm_app.h"
#include "plan_data.h"
#include "mcu_adc.h"
#include "ltc2632.h"
#include "gpio_app.h"
#include "interlock_app.h"
#include "radiation_app.h"
#include "ulog.h"
#include "timestamp.h"

static struct control_para control_data = 
{
    .board_id = DOSE_BOARD_TRIGGER_OUT,
    .calibration = {.adc_factor = {2376000, 2376000, 2376000, 2376000, 2376000}, 
                    .dac_factor = 30,
                    .trig_interval_min = 4000},
    .treatment = {.prf_hz = 1},
    .interlock = {.threshold_dose_rate = {.low = 10, .high = 10}, 
                  .threshold_dose_cp = {.low = 10, .high = 10}, 
                  .one_pulse = {.threshold_low = 10, .threshold_high = 10}, 
                  .threshold_symmetry = 10, 
                  .communication_timeout = 5000},
    .radiation_ctrl = {.radiation_enable = 1},
};
static struct control_para *control_data_get(void)
{
    return &control_data;
}

int8_t control_data_pointer_get(void **ptr)
{
    if (ptr == NULL)
    {
        return -1;
    }

    *ptr = (void *)control_data_get();

    return 0;
}

static int8_t (*callback)(uint32_t time_excess_ms) = NULL;
int8_t radiation_index_update_callback(int8_t (*cb)(uint32_t time_excess_ms))
{
    callback = cb;
    return 0;
}

static osMessageQueueId_t dose_uart_send_queue = NULL;
int8_t dose_uart_cmd_write(struct dose_object *cmd)
{
    uint8_t buf[DOSE_UART_FRAME_SIZE_MAX] = {0};

    buf[0] = cmd->id.byte;
    buf[1] = cmd->type;
    buf[2] = *cmd->len & 0xff;
    buf[3] = *cmd->len >> 8;
    memcpy(&buf[4], cmd->data, *cmd->len);

    osStatus_t stat = osMessageQueuePut(dose_uart_send_queue, buf, 0, 0);
    if (stat != osOK)
    {
        LOG_E("dose uart send queue put err: %d\r\n", stat);
    }

    return stat;
}

static int8_t dose_handshake_frame_parse(struct dose_object *cmd)
{
    int8_t ret = 0;

    struct control_para *obj = control_data_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    obj->board_id = cmd->data[4];
    osMutexRelease(obj->mutex);

#if 1
    LOG_I("bgm arm core handshake frame parse: %d\r\n", *cmd->len);
    LOG_I("hardware version: %#.2x\r\n", cmd->data[0]);
    LOG_I("software version: %u.%u.%u\r\n", cmd->data[1], cmd->data[2], cmd->data[3]);
    LOG_I("dose id: %u\r\n", cmd->data[4]);
#endif

    uint8_t *fw_ver = system_info_get()->fw_version;

    *cmd->len = 5;
    cmd->data[0] = 0x01;    /* 0: dose1     1: dose2 */
    cmd->data[1] = 0x00;    /* hardware version */
    cmd->data[2] = strtoul(&fw_ver[0], NULL, 10);  /* software version */
    cmd->data[3] = strtoul(&fw_ver[3], NULL, 10);
    cmd->data[4] = strtoul(&fw_ver[6], NULL, 10);

    if (fsm_state_switch(FSM_STATE_IDLE) != 0)
    {
        LOG_E("fsm state switch err\r\n");
    }

    return ret;
}

static int8_t dose_calibration_parse(struct dose_object *cmd)
{
    int8_t ret = 0;
    struct control_para *obj = control_data_get();

    osMutexAcquire(obj->mutex, osWaitForever);

    switch (cmd->data[0])
    {
    case 0x01:
        switch (cmd->data[1])
        {
        case 0x00:
            obj->calibration.status.bits.lock = (cmd->data[2] == 0) ? 0 : 1;
            break;
        case 0x01:
            obj->calibration.status.bits.valid = (cmd->data[2] == 0) ? 0 : 1;
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x02:
        if (cmd->data[1] < sizeof(obj->calibration.adc_factor) / sizeof(obj->calibration.adc_factor[0]))
        {
            obj->calibration.adc_factor[cmd->data[1]] = cmd->data[4] << 16 | cmd->data[3] << 8 | cmd->data[2];
            LOG_I("set adc factor [%d]: %u\r\n", cmd->data[1], obj->calibration.adc_factor[cmd->data[1]]);
        }
        else
        {
            ret = -1;
        }
        break;
    case 0x03:
        obj->calibration.dac_factor = cmd->data[3] << 8 | cmd->data[2];
        LOG_I("set dac factor: %u\r\n", obj->calibration.dac_factor);
        break;
    case 0x04:
        obj->calibration.trig_interval_min = cmd->data[3] << 8 | cmd->data[2];
        LOG_I("set trigger interval min: %u us\r\n", obj->calibration.trig_interval_min);
        break;
    default:
        ret = -1;
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}

static int8_t dose_treatment_parse(struct dose_object *cmd)
{
    int8_t ret = 0;
    struct control_para *obj = control_data_get();

    osMutexAcquire(obj->mutex, osWaitForever);

    switch (cmd->data[0])
    {
    case 0x40:
        obj->treatment.dose_mode = (cmd->data[2] == 0) ? 0 : 1;
        LOG_I("set dose mode: %d\r\n", cmd->data[2]);
        break;
    case 0x41:
        switch (cmd->data[1])
        {
        case 0x00:
            obj->treatment.pulse_mode = (cmd->data[2] == 0) ? 0 : 1;
            LOG_I("set pulse mode: %d\r\n", cmd->data[2]);
            break;
        case 0x01:
            LOG_I("dose uart prf set: %d\r\n", cmd->data[2]);
            if (cmd->data[2] == 0 || 1000000 / cmd->data[2] < obj->calibration.trig_interval_min)
            {
                ret = -1;
                break;
            }
            obj->treatment.prf_hz = cmd->data[2];
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x42:
        switch (cmd->data[1])
        {
        case 0x00:
            obj->treatment.ri_src = (cmd->data[2] == 0) ? 0 : 1;
            break;
        case 0x01:
            uint32_t meter = cmd->data[2] | cmd->data[3] << 8 | cmd->data[4] << 16 | cmd->data[5] << 24;

            LOG_I("dose beam meter set: %f\r\n", *(float *)&meter);
            ret = beam_data_value_set(0, BEAM_DOSE_METER, 0, *(float *)&meter);
            if (ret != 0)
            {
                LOG_E("beam data dose meter set err: %d\r\n", ret);
            }
            break;
        case 0x02:
            ret = beam_data_value_set(0, BEAM_TOTAL_CP, 0, cmd->data[3] << 8 | cmd->data[2]);
            if (ret != 0)
            {
                LOG_E("beam data total cp set err: %d\r\n", ret);
            }
            ret |= beam_data_value_set(0, BEAM_TOTAL_RI, 0, cmd->data[5] << 8 | cmd->data[4]);
            if (ret != 0)
            {
                LOG_E("beam data total ri set err: %d\r\n", ret);
            }
            LOG_I("dose beam total cp set: %d\r\n", cmd->data[3] << 8 | cmd->data[2]);
            LOG_I("dose beam total ri set: %d\r\n", cmd->data[5] << 8 | cmd->data[4]);
            break;
        case 0x03:
            break;
        case 0x04:
            ret = beam_data_value_set(0, BEAM_CP_RI_MAP, cmd->data[3] << 8 | cmd->data[2], cmd->data[5] << 8 | cmd->data[4]);
            if (ret != 0)
            {
                LOG_E("beam data cp ri map set err: %d\r\n", ret);
            }
            // LOG_I("cp_ri_map[%d]: %d\r\n", cmd->data[3] << 8 | cmd->data[2], cmd->data[5] << 8 | cmd->data[4]);
            break;
        case 0x05:
            uint32_t dose_cumulative = cmd->data[4] | cmd->data[5] << 8 | cmd->data[6] << 16 | cmd->data[7] << 24;
            ret = beam_data_value_set(0, BEAM_RI_CUMULATIVE, cmd->data[3] << 8 | cmd->data[2], *(float *)&dose_cumulative);
            if (ret != 0)
            {
                LOG_E("beam data ri cumulative set err: %d\r\n", ret);
            }
            uint32_t dose_rate = cmd->data[8] | cmd->data[9] << 8 | cmd->data[10] << 16 | cmd->data[11] << 24;
            ret = beam_data_value_set(0, BEAM_RI_DOSE_RATE, cmd->data[3] << 8 | cmd->data[2], *(float *)&dose_rate);
            if (ret != 0)
            {
                LOG_E("beam data ri dose rate set err: %d\r\n", ret);
            }
            uint32_t time_expected = cmd->data[12] | cmd->data[13] << 8 | cmd->data[14] << 16 | cmd->data[15] << 24;
            ret = beam_data_value_set(0, BEAM_RI_TIME_EXPECTED, cmd->data[3] << 8 | cmd->data[2], *(float *)&time_expected);
            if (ret != 0)
            {
                LOG_E("beam data ri time expected set err: %d\r\n", ret);
            }
            // LOG_I("ri_data[%d]: dose: %f, dose_rate: %f, dose_expect_time: %f ms\r\n", cmd->data[3] << 8 | cmd->data[2], *(float *)&dose_cumulative, *(float *)&dose_rate, *(float *)&time_expected);
            break;
        case 0x06:
            ret = beam_data_value_set(0, BEAM_TYPE, 0, cmd->data[2]);
            if (ret != 0)
            {
                LOG_E("beam data type set err: %d\r\n", ret);
            }
            LOG_I("beam type: %d\r\n", cmd->data[2]);
            ret = beam_data_value_set(0, BEAM_RADIATION_TYPE, 0, cmd->data[3]);
            if (ret != 0)
            {
                LOG_E("beam data radiation type set err: %d\r\n", ret);
            }
            LOG_I("beam radiation type: %d\r\n", cmd->data[3]);
            ret = beam_data_value_set(0, BEAM_DELIVER_TYPE, 0, cmd->data[4]);
            if (ret != 0)
            {
                LOG_E("beam data deliver type set err: %d\r\n", ret);
            }
            LOG_I("beam deliver type: %d\r\n", cmd->data[4]);
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x43:
        switch (cmd->data[1])
        {
        case 0x00:
            obj->treatment.status.bits.lock = (cmd->data[2] == 0) ? 0 : 1;
            *cmd->len = 3;
            break;
        case 0x01:
            obj->treatment.status.bits.check = 1;
            obj->treatment.status.bits.lock = 1;
            cmd->data[2] = obj->treatment.status.bits.check ^ obj->treatment.status.bits.lock;
            *cmd->len = 3;
            break;
        default:
            ret = -1;
            break;
        }
        break;
    default:
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}

static int8_t dose_interlock_parse(struct dose_object *cmd)
{
    int8_t ret = 0;
    uint16_t value = 0;
    struct control_para *obj = control_data_get();

    osMutexAcquire(obj->mutex, osWaitForever);

    switch (cmd->data[0])
    {
    case 0x80:
        switch (cmd->data[1])
        {
        case 0x00:
            obj->interlock.threshold_dose_rate.low = (cmd->data[2] > 100) ? 100 : cmd->data[2];
            break;
        case 0x01:
            obj->interlock.threshold_dose_rate.high = (cmd->data[2] > 100) ? 100 : cmd->data[2];
            break;
        case 0x02:
            obj->interlock.threshold_dose_cp.low = (cmd->data[2] > 100) ? 100 : cmd->data[2];
            break;
        case 0x03:
            obj->interlock.threshold_dose_cp.high = (cmd->data[2] > 100) ? 100 : cmd->data[2];
            break;
        case 0x04:
            obj->interlock.threshold_symmetry = (cmd->data[2] > 100) ? 100 : cmd->data[2];
            break;
        case 0x05:
            obj->interlock.one_pulse.threshold_high = (cmd->data[2] > 100) ? 100 : cmd->data[2];
            break;
        case 0x06:
            obj->interlock.one_pulse.threshold_low = (cmd->data[2] > 100) ? 100 : cmd->data[2];
            break;
        case 0x07:
            obj->interlock.communication_timeout = cmd->data[3] << 8 | cmd->data[2];
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x81:
        switch (cmd->data[1])
        {
        case 0x00:
            cmd->data[2] = obj->interlock.one_pulse.count_high;
            cmd->data[3] = obj->interlock.one_pulse.count_high >> 8;
            *cmd->len = 4;
            break;
        case 0x01:
            cmd->data[2] = obj->interlock.one_pulse.count_low;
            cmd->data[3] = obj->interlock.one_pulse.count_low >> 8;
            *cmd->len = 4;
            break;
        case 0x02:
            cmd->data[2] = obj->interlock.one_pulse.count_abnormal;
            cmd->data[3] = obj->interlock.one_pulse.count_abnormal >> 8;
            *cmd->len = 4;
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x82:
        switch (cmd->data[1])
        {
        case 0x00:
            value = mcu_adc_value_get(MCU_ADC_CHANNEL_N500V) / 10;
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            *cmd->len = 4;
            break;
        case 0x01:
            value = mcu_adc_value_get(MCU_ADC_CHANNEL_P5V) / 10;
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            *cmd->len = 4;
            break;
        case 0x02:
            value = mcu_adc_value_get(MCU_ADC_CHANNEL_N5V) / 10;
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            *cmd->len = 4;
            break;
        case 0x03:
            value = ltc2632_data_value_get(LTC2632_CHANNEL_OUTA);
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            *cmd->len = 4;
            break;
        case 0x04:
            value = ltc2632_data_value_get(LTC2632_CHANNEL_OUTB);
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            *cmd->len = 4;
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0xB0:
        value = interlock_status_get();
        cmd->data[2] = value;
        cmd->data[3] = value >> 8;
        *cmd->len = 4;
        break;
    case 0xB1:
        switch (cmd->data[1])
        {
        case 0x00:
            break;
        case 0x01:
            break;
        default:
            ret = -1;
            break;
        }
        break;
    default:
        ret = -1;
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}

static int8_t fsm_state_switch_check(enum fsm_state new_state)
{
    if (new_state >= FSM_STATE_MAX)
    {
        LOG_E("invalid fsm state: %d\r\n", new_state);
        return -1;
    }

    enum fsm_state state = fsm_state_get();
    if (new_state == state)
    {
        return 0;
    }

    int8_t ret = 0;

    switch (state)
    {
    case FSM_STATE_INIT:
        if (new_state != FSM_STATE_IDLE && new_state != FSM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_IDLE:
        if (new_state == FSM_STATE_INIT || new_state == FSM_STATE_TERMINATE || new_state == FSM_STATE_PARK || 
            new_state == FSM_STATE_MANUAL || new_state == FSM_STATE_POWERSAVER || new_state == FSM_STATE_SHUTDOWN)
        {
            /* do nothing */
        }
        else if (new_state == FSM_STATE_PRELIMINARY_BEGIN)
        {
            /* check lock status */
            struct control_para *obj = control_data_get();
            osMutexAcquire(obj->mutex, osWaitForever);
            if (obj->calibration.status.bits.lock == 0 || obj->treatment.status.bits.lock == 0)
            {
                ret = -1;
            }
            if (obj->treatment.dose_mode != 0)
            {
                ret = -1;
            }
            osMutexRelease(obj->mutex);
        }
        else
        {
            ret = -1;
        }
        break;
    case FSM_STATE_PRELIMINARY_BEGIN:
        if (new_state != FSM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_PRELIMINARY:
        if (new_state != FSM_STATE_PREPARE && new_state != FSM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_PREPARE:
        if (new_state == FSM_STATE_TERMINATE)
        {

        }
        else if (new_state == FSM_STATE_READY)
        {
            /* check lock status */
            struct control_para *obj = control_data_get();
            osMutexAcquire(obj->mutex, osWaitForever);
            if (obj->calibration.status.bits.lock == 0 || obj->treatment.status.bits.lock == 0)
            {
                LOG_I("lock: %d, %d\r\n", obj->calibration.status.bits.lock, obj->treatment.status.bits.lock);
                ret = -1;
            }
            if (obj->treatment.dose_mode != 1)
            {
                LOG_I("dose mode: %d\r\n", obj->treatment.dose_mode);
                ret = -1;
            }
            osMutexRelease(obj->mutex);
        }
        else
        {
            ret = -1;
        }
        break;
    case FSM_STATE_READY:
        if (new_state != FSM_STATE_WORK && new_state != FSM_STATE_INTERRUPT && new_state != FSM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_WORK:
        struct control_para *obj = control_data_get();
        osMutexAcquire(obj->mutex, osWaitForever);
        enum dose_board board = obj->board_id;
        osMutexRelease(obj->mutex);
        switch (board)
        {
        case DOSE_BOARD_TRIGGER_OUT:
            if (new_state != FSM_STATE_TERMINATE && new_state != FSM_STATE_INTERRUPT)
            {
                ret = -1;
            }
            break;
        case DOSE_BOARD_NO_TRIGGER_OUT:
            if (new_state != FSM_STATE_TERMINATE && new_state != FSM_STATE_INTERRUPT && new_state != FSM_STATE_IDLE && new_state != FSM_STATE_PREPARE)
            {
                ret = -1;
            }
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case FSM_STATE_PARK:
        if (new_state != FSM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_MANUAL:
        if (new_state != FSM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_COMPLETE:
        if (new_state != FSM_STATE_IDLE && new_state != FSM_STATE_PREPARE && new_state != FSM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_SHUTDOWN:
        ret = -1;
        break;
    case FSM_STATE_POWERSAVER:
        if (new_state != FSM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_INTERRUPT:
        if (new_state != FSM_STATE_TERMINATE && new_state != FSM_STATE_READY)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_TERMINATE:
        if (new_state != FSM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}

static int8_t dose_state_control_parse(struct dose_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0xC0:
        switch (cmd->data[1])
        {
        case 0x00:
            ret = fsm_state_switch_check(cmd->data[2]);
            if (ret != 0)
            {
                LOG_E("fsm state switch [%d] check err: %d\r\n", cmd->data[2], ret);
                break;
            }
            cmd->data[2] = ret = fsm_state_switch(cmd->data[2]);
            if (ret != 0)
            {
                LOG_E("fsm state switch err: %d\r\n", ret);
                break;
            }
            *cmd->len = 3;
            break;
        case 0x01:
            cmd->data[2] = fsm_state_get();
            *cmd->len = 3;
            break;
        case 0x02:
            struct control_para *obj = control_data_get();
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->radiation_ctrl.radiation_enable = cmd->data[2] & 0x01;
            osMutexRelease(obj->mutex);
            break;
        default:
            break;
        }
        break;
    case 0xC1:
        switch (cmd->data[1])
        {
        case 0x00:
            {
                struct control_para *obj = control_data_get();
                osMutexAcquire(obj->mutex, osWaitForever);
                obj->interlock.one_pulse.count_low = 0;
                obj->interlock.one_pulse.count_high = 0;
                obj->interlock.one_pulse.count_abnormal = 0;
                osMutexRelease(obj->mutex);
            }
            break;
        case 0x01:
            ret = beam_data_cleanup(0);
            if (ret != 0)
            {
                LOG_E("beam data clean err: %d\r\n", ret);
            }
            break;
        case 0x02:
            ret = dose_value_status_set(DOSE_ACCUMULATED, 0);
            ret |= dose_value_status_set(DOSE_RATE_CURRENT, 0);
            ret |= dose_value_status_set(ONE_PULSE_DOSE, 0);
            ret |= dose_value_status_set(ONE_PULSE_COUNT, 0);
            if (ret != 0)
            {
                LOG_E("dose value status set err: %d\r\n", ret);
            }
            LOG_I("dose uart accumulated reset\r\n");
            break;
        case 0x03:
            ret = interlock_status_cleanup();
            if (ret != 0)
            {
                LOG_E("interlock status cleanup err: %d\r\n", ret);
            }
            break;
        case 0x04:
            ret = dose_value_status_set(ONE_PULSE_COMPLETE, 0);
            if (ret != 0)
            {
                LOG_E("dose value status set err: %d\r\n", ret);
            }
            break;
        case 0x05:
            {
                struct control_para *obj = control_data_get();
                osMutexAcquire(obj->mutex, osWaitForever);
                obj->interlock.one_pulse.count_low = 0;
                obj->interlock.one_pulse.count_high = 0;
                obj->interlock.one_pulse.count_abnormal = 0;
                osMutexRelease(obj->mutex);

                ret = beam_data_cleanup(0);
                ret |= dose_value_status_set(DOSE_ACCUMULATED, 0);
                ret |= interlock_status_cleanup();
                ret |= dose_value_status_set(ONE_PULSE_COMPLETE, 0);
                if (ret != 0)
                {
                    LOG_E("dose fault clear err: %d\r\n", ret);
                }
                LOG_I("dose fault clear\r\n");
            }
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0xC2:
        switch (cmd->data[1])
        {
        case 0x00:
            ret = wdt_reset_set(1);
            ret |= wdt_reset_set(0);
            break;
        case 0x01:
            if (cmd->id.bits.cmd_ack != 0)  /* need ack */
            {
                cmd->id.bits.cmd_ack = 0;

                cmd->type |= 0x80;

                ret = dose_uart_cmd_write(cmd);
                if (ret != 0)
                {
                    LOG_E("dose uart cmd write err: %d\r\n", ret);
                }
            }

            osDelay(100);

            HAL_NVIC_SystemReset();
            break;
        case 0x02:
            uint64_t timestamp = (uint64_t)cmd->data[2] | (uint64_t)cmd->data[3] << 8 | (uint64_t)cmd->data[4] << 16 | (uint64_t)cmd->data[5] << 24 | 
                                (uint64_t)cmd->data[6] << 32 | (uint64_t)cmd->data[7] << 40 | (uint64_t)cmd->data[8] << 48 | (uint64_t)cmd->data[9] << 56;
            ret = timestamp_ns_set(timestamp);
            break;
        default:
            ret = -1;
            break;
        }
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}

static int8_t dose_command_frame_parse(struct dose_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])   /* first cmd */
    {
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
        ret = dose_calibration_parse(cmd);
        break;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
        ret = dose_treatment_parse(cmd);
        break;
    case 0x80:
    case 0x81:
    case 0x82:
    case 0xB0:
    case 0xB1:
        ret = dose_interlock_parse(cmd);
        break;
    case 0xC0:
    case 0xC1:
    case 0xC2:
        ret = dose_state_control_parse(cmd);
        break;
    default:
        break;
    }

    if (ret != 0)
    {
        LOG_E("dose uart command [%.2x, %.2x] parse err: %d\r\n", cmd->data[0], cmd->data[1], ret);
    }

    return ret;
}

int8_t radiation_status_get(uint8_t *buf, uint16_t *len, uint32_t trigger_interval_ms)
{
    int8_t ret = 0;

    struct expo_stat
    {
        uint8_t interlock : 1;
        uint8_t complete : 1;
        uint8_t radiation : 1;
        uint8_t ready : 1;
        uint8_t local_ri : 1;
        uint8_t beam_lock : 1;
        uint8_t cali_lock : 1;
        uint8_t normal : 1;
    }stat;

    uint16_t interlock = interlock_status_get();
    struct control_para *data = control_data_get();

    osMutexAcquire(data->mutex, osWaitForever);
    stat.interlock = (interlock == 0) ? 0 : 1;
    stat.complete = dose_value_status_get(ONE_BEAM_COMPLETE);
    stat.radiation = fsm_state_get() == FSM_STATE_WORK;
    stat.ready = fsm_state_get() == FSM_STATE_READY;
    stat.local_ri = data->treatment.ri_src == 1;
    stat.beam_lock = data->treatment.status.bits.lock;
    stat.cali_lock = data->calibration.status.bits.lock;
    stat.normal = data->treatment.dose_mode == 1;

    buf[0] = *(uint8_t *)&stat;     /* status */
    buf[1] = fsm_state_get();       /* fsm state */
    memcpy(&buf[2], &interlock, sizeof(uint16_t));    /* interlock */
    memcpy(&buf[4], &data->radiation.cp, sizeof(uint16_t));   /* cp */
    memcpy(&buf[6], &data->radiation.index, sizeof(uint16_t));    /* radiation index */
    float dose_meter = beam_data_value_get(0, BEAM_DOSE_METER, 0);
    memcpy(&buf[8], (float *)&dose_meter, sizeof(float));  /* dose meter */
    double dose_cumulated = dose_value_status_get(DOSE_ACCUMULATED);
    float dose = (float)(dose_cumulated / control_data_get()->calibration.adc_factor[0]);
    memcpy(&buf[12], (float *)&dose, sizeof(float));   /* dose cumulated */
    double dose_rate = dose_value_status_get(DOSE_RATE_CURRENT);
    float dose_rate_f = (float)(dose_rate / control_data_get()->calibration.adc_factor[0]);
    memcpy(&buf[16], (float *)&dose_rate_f, sizeof(float)); /* dose rate */

    buf[20] = dose_value_status_get(PRF_CURRENT);//data->treatment.prf_hz; /* PRF */
    memcpy(&buf[21], &data->interlock.one_pulse.count_abnormal, sizeof(uint16_t));    /* pulse abnormal */
    uint8_t one_pulse_valid = dose_value_status_get(ONE_PULSE_COMPLETE);
    uint32_t one_pulse_dose = dose_value_status_get(ONE_PULSE_DOSE);
    buf[23] = one_pulse_valid;    /* one pulse valid */
    memcpy(&buf[24], &one_pulse_dose, sizeof(uint32_t));  /* dose one pulse */

    uint64_t timestamp = timestamp_ns_get();
    memcpy(&buf[28], &trigger_interval_ms, sizeof(uint32_t));  /* trigger interval */
    memcpy(&buf[32], &timestamp, sizeof(uint64_t));  /* timestamp */

    osMutexRelease(data->mutex);

    *len = 40;

    return ret;
}

static int8_t dose_realtime_frame_parse(struct dose_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0x00:  /* set radiation point */
        if (cmd->data[3] == 0x01)
        {
            /* emergency stop, just change machine state */
            LOG_I("emergency stop\r\n");
            ret = fsm_state_switch(FSM_STATE_TERMINATE);
            if (ret != 0)
            {
                LOG_E("fsm state switch err: %d\r\n", ret);
            }
        }
        else
        {
            /* 
             * update radiation index
             * calculate cp, and max radiation index in cp
             * calculate interpolation for dose
             */
            struct control_para *obj = control_data_get();

            osMutexAcquire(obj->mutex, osWaitForever);
            obj->radiation.index = cmd->data[2] << 8 | cmd->data[1];
            obj->radiation.cp_prev = obj->radiation.cp;
            obj->radiation.cp = beam_data_value_get(0, BEAM_RI_IN_CP, obj->radiation.index);
            obj->radiation.index_max_in_cp = beam_data_value_get(0, BEAM_RI_IN_CP_MAX, obj->radiation.index);
            osMutexRelease(obj->mutex);

            if (callback != NULL)
            {
                ret = callback(0);
                if (ret != 0)
                {
                    LOG_E("dose_uart_radiation_index_update_callback err: %d\r\n", ret);
                }
            }
            // LOG_I("dose uart radiation index: %d\r\n", obj->radiation.index);
        }
        break;
    case 0x01:  /* get radiation status */
        ret = radiation_status_get(&cmd->data[1], cmd->len, 0);
        if (ret != 0)
        {
            LOG_E("radiation status get err: %d\r\n", ret);
        }
        *cmd->len += 1;
        break;
    default:
        LOG_E("invalid dose uart realtime frame type: %d\r\n", cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}

// #define LOG_OUTPUT_TO_ARM_IO
#ifdef LOG_OUTPUT_TO_ARM_IO
enum log_output_type
{
    LOG_OUTPUT_MSG = 0,
};
static int8_t dose_debug_cmd_parse(struct dose_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0x01:
        switch (cmd->data[1])
        {
        case LOG_OUTPUT_MSG:
            shell_cmd_parse_entry(&cmd->data[2], *cmd->len - 2);
            break;
        default:
            LOG_E("invalid dose uart debug sub cmd type: %d\r\n", cmd->data[1]);
            return -1;
            break;
        }
        break;

    default:
        LOG_E("invalid dose uart debug cmd type: %d\r\n", cmd->data[0]);
        return -2;
        break;
    }

    return 0;
}
#endif

static int8_t dose_cmd_parse(struct dose_object *obj)
{
    if (obj == NULL)
    {
        LOG_E("cmd is NULL\r\n");
        return -1;
    }

    /* 0. get valid data in object */
    struct dose_object cmd = {0};

    cmd.id.byte = obj->data[1];
    cmd.type = obj->data[2];
    cmd.len = (uint16_t *)&obj->data[3];
    cmd.data = obj->data + 5;

#if 0
    for (uint8_t i = 0; i < *(cmd.len); i++)
    {
        LOG_I("%02x ", cmd.data[i]);
    }
    LOG_I("\r\n");
#endif

    /* 1. check cmd id */
    if (cmd.id.bits.cmd_id != DOSE_UART_ID)
    {
        LOG_E("cmd id err: %d\r\n", cmd.id.bits.cmd_id);
        return 0;
    }

    int8_t ret = 0;

    /* 2. parse cmd type */
    switch (cmd.type)
    {
    case 0x01:  /* handshake frame */
        ret = dose_handshake_frame_parse(&cmd);
        if (ret != 0)
        {
            LOG_E("dose_handshake_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x02:  /* command frame */
        ret = dose_command_frame_parse(&cmd);
        if (ret != 0)
        {
            LOG_E("dose_command_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x03:  /* realtime frame */
        ret = dose_realtime_frame_parse(&cmd);
        if (ret != 0)
        {
            LOG_E("dose_realtime_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x84:  /* realtime data feedback */
        break;
#ifdef LOG_OUTPUT_TO_ARM_IO
    case 0x05:  /* log cmd frame */
        ret = dose_debug_cmd_parse(&cmd);
        if (ret != 0)
        {
            LOG_E("dose_debug_cmd_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x85:  /* log data feedback */
        break;
#endif
    default:
        LOG_E("invalid cmd type: %d\r\n", cmd.type);
        return -2;
        break;
    }

    *obj->len = *cmd.len + 5;

    if (cmd.id.bits.cmd_ack != 0)  /* need ack */
    {
        obj->data[1] &= ~(1 << 7);

        obj->data[2] |= 0x80;

        // ret = dose_uart_cmd_write(cmd);
        // if (ret != 0)
        // {
        //     LOG_E("dose uart cmd write err: %d\r\n", ret);
        // }
    }

    return ret;
}

static int8_t dose_uart_cmd_process(struct dose_object *obj)
{
    if (obj == NULL)
    {
        return -1;
    }

#if 0
    LOG_I("recv len: %d\r\n", *obj->len);
    for (uint8_t i = 0; i < *obj->len; i++)
    {
        LOG_I("%02x ", obj->data[i]);
    }
    LOG_I("\r\n");
#endif

    return dose_cmd_parse(obj);
}

enum uart_cmd_type
{
    UART_CMD_HEAERBEAT = 1,
    UART_CMD_TIME_SYNC,
    UART_CMD_PARA_SET,
    UART_CMD_PARA_GET,
    UART_CMD_DATA_SET,
    UART_CMD_DATA_GET,
    UART_CMD_PARA_SET_ACK = 0x80 | UART_CMD_PARA_SET,
    UART_CMD_PARA_GET_ACK = 0x80 | UART_CMD_PARA_GET,
    UART_CMD_DATA_SET_ACK = 0x80 | UART_CMD_DATA_SET,
    UART_CMD_DATA_GET_ACK = 0x80 | UART_CMD_DATA_GET,
    UART_CMD_SYSTEM_RESRT = 0xEB,
};

static osEventFlagsId_t uart_rx_event_id = NULL;
#define UART_RX_HEARTBEAT_TIMEOUT_EVENT (1 << 0)
#define UART_RX_HEARTBEAT_CMD_EVENT     (1 << 1)
#define UART_RX_REBOOT_CMD_EVENT        (1 << 2)
static int8_t uart_recv_heartbeat_timeout_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return osEventFlagsSet(uart_rx_event_id, UART_RX_HEARTBEAT_TIMEOUT_EVENT);
}
static int8_t uart_recv_heartbeat_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return osEventFlagsSet(uart_rx_event_id, UART_RX_HEARTBEAT_CMD_EVENT);
}
static int8_t uart_recv_time_sync_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return timestamp_ns_set(*(uint64_t *)data);
}
static int8_t uart_recv_set_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    struct dose_object obj = {.id.byte = id, .type = UART_CMD_DATA_SET, .len = len, .data = data};
    return dose_uart_cmd_process(&obj);
}
static int8_t uart_recv_get_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    struct dose_object obj = {.id.byte = id, .type = UART_CMD_DATA_GET, .len = len, .data = data};
    return dose_uart_cmd_process(&obj);
}
static int8_t uart_recv_reboot_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return osEventFlagsSet(uart_rx_event_id, UART_RX_REBOOT_CMD_EVENT);
}
static int8_t uart_send_heartbeat_cmd_callback(struct uart_protocol *const self, uint8_t *data, uint16_t *len, void *arg)
{
    uint8_t *fw_ver = system_info_get()->fw_version;

    *len = 5;
    data[0] = 0xFF; /* TODO: board id + hardware version */
    data[1] = 0x00; /* software version */
    data[2] = strtoul(&fw_ver[0], NULL, 10);
    data[3] = strtoul(&fw_ver[3], NULL, 10);
    data[4] = strtoul(&fw_ver[6], NULL, 10);

    return 0;
}

static int8_t dose_uart_recv_entry(void *argument)
{
    int8_t ret = 0;
    uint8_t buf[DOSE_UART_FRAME_SIZE_MAX] = {0};

    // ret = uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID, uart_recv_heartbeat_timeout_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_HEARTBEAT_RX_CB_ID, uart_recv_heartbeat_cmd_callback,  NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_PNT_RX_CB_ID, uart_recv_time_sync_cmd_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_SET_RX_CB_ID, uart_recv_set_cmd_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_GET_RX_CB_ID, uart_recv_get_cmd_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_REBOOT_RX_CB_ID, uart_recv_reboot_cmd_callback, NULL);
    // ret |= uart_protocol_tx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_HEARTBEAT_TX_CB_ID, uart_send_heartbeat_cmd_callback, NULL);
    if (ret != 0)
    {
        LOG_E("uart_protocol_rx_RegisterCallback err: %d\r\n", ret);
        osThreadExit();
    }

    ret = device_dose_uart_open();
    if (ret != 0)
    {
        LOG_E("device dos uart open err: %d\r\n", ret);
        osThreadExit();
    }

    for (;;)
    {
        ret = device_dose_uart_data_recv_with_block(buf, sizeof(buf), osWaitForever);
        if (ret != 0)
        {
            LOG_E("device dose uart data recv with block err: %d\r\n", ret);
            continue;
        }
    }

    return 0;
}

static int8_t link_status_entry(void *argument)
{
    int8_t ret = 0;
    int32_t event_flags = 0;

    for (;;)
    {
        event_flags = osEventFlagsWait(uart_rx_event_id, UART_RX_HEARTBEAT_TIMEOUT_EVENT | UART_RX_HEARTBEAT_CMD_EVENT | UART_RX_REBOOT_CMD_EVENT, osFlagsWaitAny, osWaitForever);
        if (event_flags > 0)
        {
            if (event_flags & UART_RX_HEARTBEAT_TIMEOUT_EVENT)
            {

            }
            else if (event_flags & UART_RX_HEARTBEAT_CMD_EVENT)
            {

            }
            else if (event_flags & UART_RX_REBOOT_CMD_EVENT)
            {
                HAL_NVIC_SystemReset();
            }
        }
    }

    return 0;
}

static int8_t dose_uart_send_entry(void *argument)
{
    int8_t ret = 0;
    uint8_t buf[DOSE_UART_FRAME_SIZE_MAX] = {0};
    struct uart_data obj = {0};

    for (;;)
    {
        osMessageQueueGet(dose_uart_send_queue, buf, NULL, osWaitForever);

        obj.id = 0;
        obj.cmd = 0;
        obj.len = (buf[2] | buf[3] << 8) + 4;
        obj.data = buf;

#if 0
        LOG_I("send id: %d, cmd: %d, len: %d\r\n", obj.id, obj.cmd, obj.len);
        for (uint8_t i = 0; i < obj.len; i++)
        {
            LOG_I("%02x ", obj.data[i]);
        }
        LOG_I("\r\n");
#endif

        ret = device_dose_uart_data_write(&obj, 100);
        if (ret != 0)
        {
            LOG_E("device dose uart data write err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t dose_uart_thread_init(void)
{
    uart_rx_event_id = osEventFlagsNew(NULL);
    if (uart_rx_event_id == NULL)
    {
        LOG_E("osEventFlagsNew err\r\n");
        return -1;
    }

    osThreadAttr_t attr = {
    .name = "link_status_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityAboveNormal,
    };

    osThreadId_t thread_id = osThreadNew(link_status_entry, NULL, &attr);
    if (thread_id == NULL)
    {
        LOG_E("thread link status create failed\r\n");
        return -2;
    }

    attr.name = "uart_recv_thread";
    attr.priority = osPriorityAboveNormal5;
    thread_id = osThreadNew(dose_uart_recv_entry, NULL, &attr);
    if (thread_id == NULL)
    {
        LOG_E("thread uart recv create failed\r\n");
        return -3;
    }

    dose_uart_send_queue = osMessageQueueNew(16, DOSE_UART_FRAME_SIZE_MAX, NULL);
    if (dose_uart_send_queue == NULL)
    {
        LOG_E("queue dose uart send create failed\r\n");
        return -4;
    }

    attr.name = "uart_send_thread";
    thread_id = osThreadNew(dose_uart_send_entry, NULL, &attr);
    if (thread_id == NULL)
    {
        LOG_E("thread uart send create failed\r\n");
        return -5;
    }

    osMutexAttr_t mutex_attributes = {
    .name = "data_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    control_data_get()->mutex = osMutexNew(&mutex_attributes);
    if (control_data_get()->mutex == NULL)
    {
        LOG_E("mutex create failed\r\n");
        return -6;
    }

    return 0;
}
INIT_APP_EXPORT(dose_uart_thread_init);

#ifdef LOG_OUTPUT_TO_ARM_IO
int8_t log_output_write(uint8_t *buf, uint16_t len)
{
#if 0
    printf("send data: ");
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02x ", buf[i]);
    }
    printf("\r\n");
#endif

    int8_t ret = 0;
    uint8_t msg[128] = {0};
    msg[0] = 0x01;
    msg[1] = LOG_OUTPUT_MSG;
    memcpy(&msg[2], buf, len);
    len += 2;

    struct dose_object cmd = {0};

    cmd.id.bits.cmd_id = DOSE_UART_ID;
    cmd.id.bits.cmd_ack = 0;
    cmd.type = 0x85;
    cmd.len = &len;
    cmd.data = msg;

    ret = dose_uart_cmd_write(&cmd);
    if (ret != 0)
    {
        LOG_E("dose uart cmd write err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
static int8_t log_output_bridge_init(void)
{
    struct ulog_write_func_info info = {
        .func_init = NULL,
        .func_callback = log_output_write,
        .index = 3,
        .level = ULOG_INFO_LEVEL};

    int8_t ret = ulog_write_func_register(&info);
    if (ret != 0)
    {
        printf("console log register err:%d\r\n", ret);
        return ret;
    }

    return 0;
}
INIT_COMPONENT_EXPORT(log_output_bridge_init);
#endif

#ifdef DOSE_UART_TEST
#include "shell.h"
static int8_t dose_uart_cmd_send(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct dose_object cmd = {0};
    uint8_t data[10] = {0};
    uint16_t len = 10;

    cmd.id.bits.cmd_id = DOSE_UART_ID;
    cmd.id.bits.cmd_ack = 1;
    cmd.type = 0x40;
    cmd.len = &len;
    cmd.data = data;

    for (uint8_t i = 0; i < *cmd.len; i++)
    {
        cmd.data[i] = i;
    }

    ret = dose_uart_cmd_write(&cmd);
    if (ret != 0)
    {
        LOG_E("dose uart cmd write err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_uart_cmd_send, dose_uart_cmd_send, send dose uart cmd);
#endif