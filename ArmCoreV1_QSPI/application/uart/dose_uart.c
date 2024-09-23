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

static struct control_para control_data = 
{
    .calibration = {.adc_factor = {1}, 
                    .dac_factor = 30,
                    .trig_interval_min = 4000},
    .treatment = {.prf_hz = 250},
    .interlock = {.threshold_dose_rate = {10}, 
                  .threshold_dose_cp = {10}, 
                  .one_pulse = {.threshold_low = 10, .threshold_high = 10}, 
                  .threshold_symmetry = 10, 
                  .communication_timeout = 5000},
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

static int8_t (*callback)(void) = NULL;
int8_t radiation_index_update_callback(int8_t (*cb)(void))
{
    callback = cb;
    return 0;
}

static int8_t dose_handshake_frame_parse(struct dose_object *cmd)
{
    int8_t ret = 0;

    struct control_para *obj = control_data_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    obj->board_id = cmd->data[4];
    osMutexRelease(obj->mutex);

#if 1
    printf("bgm arm core handshake frame parse: %d\r\n", cmd->len);
    printf("hardware version: %#.2x\r\n", cmd->data[0]);
    printf("software version: %u.%u.%u\r\n", cmd->data[1], cmd->data[2], cmd->data[3]);
    printf("dose id: %u\r\n", cmd->data[4]);
#endif

    uint8_t *fw_ver = system_info_get()->fw_version;

    cmd->len = 5;
    cmd->data[0] = 0x01;    /* 0: dose1     1: dose2 */
    cmd->data[1] = 0x00;    /* hardware version */
    cmd->data[2] = strtoul(&fw_ver[0], NULL, 10);  /* software version */
    cmd->data[3] = strtoul(&fw_ver[3], NULL, 10);
    cmd->data[4] = strtoul(&fw_ver[6], NULL, 10);

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
        }
        else
        {
            ret = -1;
        }
        break;
    case 0x03:
        obj->calibration.dac_factor = cmd->data[3] << 8 | cmd->data[2];
        break;
    case 0x04:
        obj->calibration.trig_interval_min = cmd->data[3] << 8 | cmd->data[2];
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
        break;
    case 0x41:
        switch (cmd->data[1])
        {
        case 0x00:
            obj->treatment.pulse_mode = (cmd->data[2] == 0) ? 0 : 1;
            break;
        case 0x01:
            if (1000000 / cmd->data[2] < obj->calibration.trig_interval_min)
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
            ret = beam_data_value_set(0, BEAM_DOSE_METER, 0, cmd->data[3] << 8 | cmd->data[2]);
            if (ret != 0)
            {
                printf("beam data dose meter set err: %d\r\n", ret);
            }
            break;
        case 0x02:
            ret = beam_data_value_set(0, BEAM_TOTAL_CP, 0, cmd->data[2]);
            if (ret != 0)
            {
                printf("beam data total cp set err: %d\r\n", ret);
            }
            ret |= beam_data_value_set(0, BEAM_TOTAL_RI, 0, cmd->data[4] << 8 | cmd->data[3]);
            if (ret != 0)
            {
                printf("beam data total ri set err: %d\r\n", ret);
            }
            break;
        case 0x03:
            break;
        case 0x04:
            ret = beam_data_value_set(0, BEAM_CP_RI_MAP, cmd->data[2], cmd->data[4] << 8 | cmd->data[3]);
            if (ret != 0)
            {
                printf("beam data cp ri map set err: %d\r\n", ret);
            }
            break;
        case 0x05:
            ret = beam_data_value_set(0, BEAM_RI_CUMULATIVE, cmd->data[3] << 8 | cmd->data[2], cmd->data[5] << 8 | cmd->data[4]);
            if (ret != 0)
            {
                printf("beam data ri cumulative set err: %d\r\n", ret);
            }
            ret = beam_data_value_set(0, BEAM_RI_DOSE_RATE, cmd->data[3] << 8 | cmd->data[2], cmd->data[7] << 8 | cmd->data[6]);
            if (ret != 0)
            {
                printf("beam data ri dose rate set err: %d\r\n", ret);
            }
            ret = beam_data_value_set(0, BEAM_RI_TIME_EXPECTED, cmd->data[3] << 8 | cmd->data[2], cmd->data[9] << 8 | cmd->data[8]);
            if (ret != 0)
            {
                printf("beam data ri time expected set err: %d\r\n", ret);
            }
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
            obj->treatment.status.bits.lock = 0;
            cmd->data[2] = 1;
            cmd->len = 3;
            break;
        case 0x01:
            obj->treatment.status.bits.check = 1;
            obj->treatment.status.bits.lock = 1;
            cmd->data[2] = 1;
            cmd->len = 3;
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
            cmd->len = 4;
            break;
        case 0x01:
            cmd->data[2] = obj->interlock.one_pulse.count_low;
            cmd->data[3] = obj->interlock.one_pulse.count_low >> 8;
            cmd->len = 4;
            break;
        case 0x02:
            cmd->data[2] = obj->interlock.one_pulse.count_abnormal;
            cmd->data[3] = obj->interlock.one_pulse.count_abnormal >> 8;
            cmd->len = 4;
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
            value = mcu_adc_value_get(MCU_ADC_CHANNEL_N500V) * 100;
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            cmd->len = 4;
            break;
        case 0x01:
            value = mcu_adc_value_get(MCU_ADC_CHANNEL_P5V) * 100;
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            cmd->len = 4;
            break;
        case 0x02:
            value = mcu_adc_value_get(MCU_ADC_CHANNEL_N5V) * 100;
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            cmd->len = 4;
            break;
        case 0x03:
            value = ltc2632_data_value_get(LTC2632_CHANNEL_OUTA);
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            cmd->len = 4;
            break;
        case 0x04:
            value = ltc2632_data_value_get(LTC2632_CHANNEL_OUTB);
            cmd->data[2] = value;
            cmd->data[3] = value >> 8;
            cmd->len = 4;
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
        cmd->len = 4;
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
        printf("invalid fsm state: %d\r\n", new_state);
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
        if (new_state != FSM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_IDLE:
        if (new_state == FSM_STATE_INIT || new_state == FSM_STATE_PREPARE)
        {
            /* do nothing */
        }
        else if (new_state == FSM_STATE_DUMMY)
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
    case FSM_STATE_DUMMY:
        if (new_state != FSM_STATE_IDLE && new_state != FSM_STATE_FAULT)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_PREPARE:
        if (new_state == FSM_STATE_IDLE || new_state == FSM_STATE_FAULT)
        {

        }
        else if (new_state == FSM_STATE_READY)
        {
            /* check lock status */
            struct control_para *obj = control_data_get();
            osMutexAcquire(obj->mutex, osWaitForever);
            if (obj->calibration.status.bits.lock == 0 || obj->treatment.status.bits.lock == 0)
            {
                ret = -1;
            }
            if (obj->treatment.dose_mode != 1)
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
    case FSM_STATE_READY:
        if (new_state != FSM_STATE_RADIATION && new_state != FSM_STATE_FAULT)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_RADIATION:
        struct control_para *obj = control_data_get();
        osMutexAcquire(obj->mutex, osWaitForever);
        enum dose_board board = obj->board_id;
        osMutexRelease(obj->mutex);
        switch (board)
        {
        case DOSE_BOARD_TRIGGER_OUT:
            if (new_state != FSM_STATE_FAULT)
            {
                ret = -1;
            }
            break;
        case DOSE_BOARD_NO_TRIGGER_OUT:
            if (new_state != FSM_STATE_FAULT && new_state != FSM_STATE_IDLE && new_state != FSM_STATE_PREPARE)
            {
                ret = -1;
            }
            break;
        default:
            break;
        }
        break;
    case FSM_STATE_COMPLETE:
        if (new_state != FSM_STATE_IDLE && new_state != FSM_STATE_PREPARE)
        {
            ret = -1;
        }
        break;
    case FSM_STATE_FAULT:
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
            cmd->data[2] = ret = fsm_state_switch_check(cmd->data[2]);
            if (ret != 0)
            {
                printf("fsm state switch check err: %d\r\n", ret);
                break;
            }
            cmd->data[2] = ret = fsm_state_switch(cmd->data[2]);
            if (ret != 0)
            {
                printf("fsm state switch err: %d\r\n", ret);
                break;
            }
            cmd->len = 3;
            break;
        case 0x01:
            cmd->data[2] = fsm_state_get();
            cmd->len = 3;
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
                printf("beam data clean err: %d\r\n", ret);
            }
            break;
        case 0x02:
            ret = dose_value_status_set(DOSE_ACCUMULATED, 0);
            if (ret != 0)
            {
                printf("dose value status set err: %d\r\n", ret);
            }
            break;
        case 0x03:
            ret = interlock_status_cleanup();
            if (ret != 0)
            {
                printf("interlock status cleanup err: %d\r\n", ret);
            }
            break;
        case 0x04:
            ret = dose_value_status_set(ONE_PULSE_COMPLETE, 0);
            if (ret != 0)
            {
                printf("dose value status set err: %d\r\n", ret);
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
            HAL_NVIC_SystemReset();
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
        printf("dose uart command [%d, %d] parse err: %d\r\n", cmd->data[0], cmd->data[1], ret);
    }

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
            printf("emergency stop\r\n");
            ret = fsm_state_switch(FSM_STATE_FAULT);
            if (ret != 0)
            {
                printf("fsm state switch err: %d\r\n", ret);
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
            obj->radiation.cp = beam_data_value_get(0, BEAM_RI_IN_CP, obj->radiation.index);
            obj->radiation.index_max_in_cp = beam_data_value_get(0, BEAM_RI_IN_CP_MAX, obj->radiation.index);
            osMutexRelease(obj->mutex);

            if (callback != NULL)
            {
                ret = callback();
                if (ret != 0)
                {
                    printf("dose_uart_radiation_index_update_callback err: %d\r\n", ret);
                }
            }            
            printf("dose uart radiation index: %d\r\n", obj->radiation.index);
        }
        break;
    case 0x01:  /* get radiation status */
        {
            uint16_t value = interlock_status_get();
            struct control_para *data = control_data_get();
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

            osMutexAcquire(data->mutex, osWaitForever);
            stat.interlock = (value == 0) ? 0 : 1;
            stat.complete = dose_value_status_get(ONE_BEAM_COMPLETE);
            stat.radiation = fsm_state_get() == FSM_STATE_RADIATION;
            stat.ready = fsm_state_get() == FSM_STATE_READY;
            stat.local_ri = data->treatment.ri_src == 1;
            stat.beam_lock = data->treatment.status.bits.lock;
            stat.cali_lock = data->calibration.status.bits.lock;
            stat.normal = data->treatment.dose_mode == 1;

            cmd->data[1] = *(uint8_t *)&stat;               /* status */
            cmd->data[2] = value;                           /* interlock */
            cmd->data[3] = value >> 8;                      /* interlock */
            cmd->data[4] = data->radiation.cp;              /* cp */
            cmd->data[5] = data->radiation.index;           /* radiation index */
            cmd->data[6] = data->radiation.index >> 8;      /* radiation index */
            uint16_t dose_cumulated = dose_value_status_get(DOSE_ACCUMULATED);
            cmd->data[7] = dose_cumulated;                  /* dose cumulated */
            cmd->data[8] = dose_cumulated >> 8;             /* dose cumulated */
            cmd->data[9] = data->treatment.prf_hz;          /* PRF */
            cmd->data[10] = 0x00;                           /* pulse abnormal */
            cmd->data[11] = 0x00;                           /* pulse abnormal */
            uint8_t one_pulse_valid = dose_value_status_get(ONE_PULSE_COMPLETE);
            uint16_t one_pulse_dose = dose_value_status_get(ONE_PULSE_DOSE);
            ret = dose_value_status_set(ONE_PULSE_COMPLETE, 0);
            if (ret != 0)
            {
                printf("dose value status set err: %d\r\n", ret);
            }
            cmd->data[12] = one_pulse_valid;                /* one pulse valid */
            cmd->data[13] = one_pulse_dose;                 /* dose one pulse */
            cmd->data[14] = one_pulse_dose >> 8;            /* dose one pulse */

            osMutexRelease(data->mutex);

            cmd->len = 15;
        }
        break;
    default:
        printf("invalid dose uart realtime frame type: %d\r\n", cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}

static int8_t dose_uart_cmd_write(struct dose_object *cmd);

static int8_t dose_cmd_parse(struct dose_object *cmd)
{
    if (cmd == NULL)
    {
        printf("cmd is NULL\r\n");
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.bits.cmd_id != DOSE_UART_ID)
    {
        return 0;
    }

    int8_t ret = 0;

    /* 2. parse cmd type */
    switch (cmd->type)
    {
    case 0x01:  /* handshake frame */
        ret = dose_handshake_frame_parse(cmd);
        if (ret != 0)
        {
            printf("dose_uart_handshake_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x02:  /* command frame */
        ret = dose_command_frame_parse(cmd);
        if (ret != 0)
        {
            printf("dose_command_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x03:  /* realtime frame */
        ret = dose_realtime_frame_parse(cmd);
        if (ret != 0)
        {
            printf("dose_uart_realtime_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    default:
        printf("invalid cmd type: %d\r\n", cmd->type);
        return -2;
        break;
    }

    if (cmd->id.bits.cmd_ack != 0)  /* need ack */
    {
        cmd->id.bits.cmd_ack = 0;

        cmd->type |= 0x80;

        ret = dose_uart_cmd_write(cmd);
        if (ret != 0)
        {
            printf("dose_uart_cmd_write err: %d\r\n", ret);
        }
    }

    return ret;
}

osMessageQueueId_t dose_uart_send_queue = NULL;
static int8_t dose_uart_cmd_write(struct dose_object *cmd)
{
    osStatus_t stat = osOK;
    struct dose_uart send_buf = {0};
    uint8_t offset = sizeof(struct dose_object) - sizeof(uint8_t *);

    memcpy(send_buf.buf, cmd, sizeof(struct dose_object));
    memcpy(&send_buf.buf[offset], cmd->data, cmd->len);
    send_buf.len = offset + cmd->len;

    stat = osMessageQueuePut(dose_uart_send_queue, &send_buf, 0, 0);
    if (stat != osOK)
    {
        printf("dose uart send queue put err: %d\r\n", stat);
        return -1;
    }

    return 0;
}

static int8_t dose_uart_send_entry(void *argument)
{
    int8_t ret = 0;
    struct dose_uart send_buf = {0};

    for (;;)
    {
        osMessageQueueGet(dose_uart_send_queue, &send_buf, NULL, osWaitForever);

#if 1
        printf("send_buf len: %d\r\n", send_buf.len);
        for (uint8_t i = 0; i < send_buf.len; i++)
        {
            printf("%02x ", send_buf.buf[i]);
        }
        printf("\r\n");
#endif

        ret = device_dose_uart_data_write(&send_buf, send_buf.len, 1000);
        if (ret != 0)
        {
            printf("device_dose_uart_data_write err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t dose_uart_cmd_process(struct dose_uart *buf)
{
    if (buf == NULL)
    {
        return -1;
    }

#if 0
    printf("recv_buf len: %d\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        printf("%02x ", buf->buf[i]);
    }
    printf("\r\n");
#endif

    struct dose_object cmd = {0};
    memcpy(&cmd, buf->buf, sizeof(struct dose_object));
    cmd.data = &buf->buf[sizeof(struct dose_object) - sizeof(uint8_t *)];

    return dose_cmd_parse(&cmd);
}

static int8_t dose_uart_init(void)
{
    int8_t ret = device_dose_uart_init(DEVICE_DOSE_UART_NAME_DEFAULT);
    if (ret != 0)
    {
        printf("device_dose_uart_init err: %d\r\n", ret);
        return -1;
    }

    ret = device_dose_uart_open();
    if (ret != 0)
    {
        printf("device_dose_uart_open err: %d\r\n", ret);
        return -2;
    }

    return 0;
}

static int8_t dose_uart_recv_entry(void *argument)
{
    int8_t ret = 0;
    struct dose_uart recv_buf = {0};

    ret = dose_uart_init();
    if (ret != 0)
    {
        printf("dose_uart_init err: %d\r\n", ret);
        return -1;
    }

    for (;;)
    {
        ret = device_dose_uart_data_read(&recv_buf, osWaitForever);
        if (ret != 0)
        {
            printf("device_dose_uart_data_read err: %d\r\n", ret);
            continue;
        }

        ret = dose_uart_cmd_process(&recv_buf);
        if (ret!= 0)
        {
            printf("dose_uart_cmd_process err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t dose_uart_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "dose_uart_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityAboveNormal7,
    };

    osThreadId_t thread_id = osThreadNew(dose_uart_recv_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread dose uart create failed\r\n");
        return -1;
    }

    dose_uart_send_queue = osMessageQueueNew(5, sizeof(struct dose_uart), NULL);
    if (dose_uart_send_queue == NULL)
    {
        printf("message queue create failed\r\n");
        return -2;
    }

    thread_id = osThreadNew(dose_uart_send_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread dose uart create failed\r\n");
        return -3;
    }

    osMutexAttr_t mutex_attributes = {
    .name = "data_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    control_data_get()->mutex = osMutexNew(&mutex_attributes);
    if (control_data_get()->mutex == NULL)
    {
        printf("mutex create failed\r\n");
        return -4;
    }

    return 0;
}
INIT_APP_EXPORT(dose_uart_thread_init);


#ifdef dose_uart_TEST
#include "shell.h"

static int8_t dose_uart_cmd_send(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct dose_object cmd = {0};
    uint8_t data[10] = {0};

    cmd.id.bits.cmd_id = DOSE_UART_ID;
    cmd.id.bits.cmd_ack = 1;
    cmd.type = 0x40;
    cmd.len = 10;
    cmd.data = data;

    for (uint8_t i = 0; i < cmd.len; i++)
    {
        cmd.data[i] = i;
    }

    ret = dose_uart_cmd_write(&cmd);
    if (ret != 0)
    {
        printf("dose_uart_cmd_write err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_uart_cmd_send, dose_uart_cmd_send, send dose uart cmd);
#endif