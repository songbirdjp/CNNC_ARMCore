#include "dose_app.h"
#include <stddef.h>
#include "cmsis_os2.h"
#include "init_call.h"
#include "ulog.h"
#include "plan_data.h"
#include "rtm_app.h"
#include "bgm_app.h"

struct board_status
{
    uint8_t hw_version;
    uint8_t sw_version[6];
    union
    {
        uint32_t bytes;
        struct
        {
            uint32_t board_power_fault : 1;
            uint32_t hv_limit : 1;
            uint32_t comm_timeout : 1;
            uint32_t wdt_fault : 1;
            uint32_t adcs7476_1_limit_high : 1;
            uint32_t adcs7476_1_limit_low : 1;
            uint32_t adcs7476_2_limit_high : 1;
            uint32_t adcs7476_2_limit_low : 1;
            uint32_t illegal_write : 1;
            uint32_t dose_rate_low : 1;
            uint32_t dose_rate_high : 1;
            uint32_t dose_total_low : 1;
            uint32_t dose_total_high : 1;
            uint32_t dose_symmetry_fault : 1;
            uint32_t dose_dummy_timeout : 1;
            uint32_t reserved : 17;
        } bits;
    }interlock;

    float voltage_ionization_chamber;
    float voltage_p5v;
    float voltage_n5v;
    uint16_t dac_ch1_offset;
    uint16_t dac_ch2_offset;
};

struct calibration_para
{
    union
    {
        struct
        {
            uint8_t lock : 1;   /* 0: unlock  1: lock */
            uint8_t valid : 1;  /* 0: invalid 1: valid */
            uint8_t reserved : 6;
        }bits;

        uint8_t byte;
    }status;

    uint32_t adc_factor[5];     /* Kadc <=> 1MU */
    uint32_t dac_factor;        /* dummy dose dac factor, Kdac <=> 1MU */
    uint32_t trig_interval_min; /* trigger interval minimal value */
};

struct treatment_para
{
    union
    {
        struct
        {
            uint8_t lock : 1;   /* 0: unlock  1: lock */
            uint8_t check : 1;  /* 0: fail  1: pass */
            uint8_t reserved : 6;
        }bits;

        uint8_t byte;
    }status;

    uint8_t dose_mode;  /* 0: dummy 1: normal */
    uint8_t pulse_mode; /* 0: PRF   1: fixed dose rate */
    uint8_t prf_hz;     /* PRF fixed */
    uint8_t ri_src;     /* radiation index source, 0: bgm  1: internal */
};

struct dose_rate_threshold
{
    uint8_t low;    /* percentage */
    uint8_t high;   /* percentage */
};

struct dose_cp_threshold
{
    uint8_t low;    /* percentage */
    uint8_t high;   /* percentage */
};

struct dose_one_pulse
{
    uint8_t threshold_low;  /* percentage */
    uint8_t threshold_high; /* percentage */

    uint16_t count_low;     /* count of low threshold pulse */
    uint16_t count_high;    /* count of high threshold pulse */
    uint32_t count_abnormal;/* count of abnormal pulse */
};

struct interlock_para
{
    struct dose_rate_threshold threshold_dose_rate;
    struct dose_cp_threshold threshold_dose_cp;
    struct dose_one_pulse one_pulse;
    uint8_t threshold_symmetry;     /* percentage */
    uint16_t communication_timeout; /* Unit: ms */
};

struct realtime_data
{
    union 
    {
        uint8_t byte;
        struct
        {
            uint8_t interlock : 1;
            uint8_t complete : 1;
            uint8_t radiation : 1;
            uint8_t ready : 1;
            uint8_t local_ri : 1;
            uint8_t beam_lock : 1;
            uint8_t cali_lock : 1;
            uint8_t normal : 1;
        }bits;
    }state;

    uint16_t control_point;
    uint16_t radiation_index;
    float dose_cumulated;   /* unit: MU */
    float dose_rate;        /* unit: MU/min */
    uint8_t prf_current;
    uint8_t one_pulse_valid_flag;
    uint32_t one_pulse_dose;
};

struct dose_info_t
{
    /* board status */
    struct board_status status;

    /* fsm state */
    enum dose_fsm_state fsm_state;

    /* calibration data */
    struct calibration_para calibration;

    /* treatment data */
    struct treatment_para treatment;

    /* interlock data */
    struct interlock_para interlock;

    /* realtime data */
    struct realtime_data realtime;

    osMutexId_t mutex;
};

static struct dose_info_t dose_info[BGM_UART_DOSE2] = {0};
static struct dose_info_t *dose_info_object_get(enum uart_id id)
{
    if (id > BGM_UART_DOSE2 || id < BGM_UART_DOSE1)
    {
        return NULL;
    }

    return &dose_info[id - 1];
}

static int8_t dose_handshake_frame_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;
    struct dose_info_t *obj = dose_info_object_get(id);

    osMutexAcquire(obj->mutex, osWaitForever);

    obj->status.hw_version = cmd->data[1];
    obj->status.sw_version[0] = cmd->data[2] + 0x30;
    obj->status.sw_version[1] = '.';
    obj->status.sw_version[2] = cmd->data[3] + 0x30;
    obj->status.sw_version[3] = '.';
    obj->status.sw_version[4] = cmd->data[4] + 0x30;

    osMutexRelease(obj->mutex);

    // LOG_I("[%d]: hw version: %#.2x\r\n", id, obj->status.hw_version);
    // LOG_I("[%d]: sw version: %s\r\n", id, obj->status.sw_version);

    return ret;
}

static int8_t dose_calibration_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;

    struct dose_info_t *obj = dose_info_object_get(id);

    osMutexAcquire(obj->mutex, osWaitForever);

    switch (cmd->data[0])
    {
    case 0x01:
        switch (cmd->data[1])
        {
        case 0x00:
            // cmd->data[2] == 0 ? LOG_I("[%d]: dose calibration data lock opened\r\n", id) : LOG_I("[%d]: dose calibration data lock closed\r\n", id);
            obj->calibration.status.bits.lock = cmd->data[2];
            break;
        case 0x01:
            cmd->data[2] == 0 ? LOG_I("[%d]: dose calibration data invalid\r\n", id) : LOG_I("[%d]: dose calibration data valid\r\n", id);
            obj->calibration.status.bits.valid = cmd->data[2];
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x02:
        switch (cmd->data[1])
        {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
            LOG_I("[%d]: dose adc factor set (1MU == %d code)\r\n", id, cmd->data[4] << 16 | cmd->data[3] << 8 | cmd->data[2]);
            obj->calibration.adc_factor[cmd->data[1]] = cmd->data[4] << 16 | cmd->data[3] << 8 | cmd->data[2];
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x03:
        LOG_I("[%d]: dose dac factor set : %d\r\n", id, cmd->data[3] << 8 | cmd->data[2]);
        obj->calibration.dac_factor = cmd->data[3] << 8 | cmd->data[2];
        break;
    case 0x04:
        LOG_I("[%d]: dose trigger interval set: %d\r\n", id, cmd->data[3] << 8 | cmd->data[2]);
        obj->calibration.trig_interval_min = cmd->data[3] << 8 | cmd->data[2];
        break;
    default:
        LOG_E("[%d]: invalid calibration cmd type: %x\r\n", id, cmd->data[0]);
        ret = -1;
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}
static int8_t dose_treatment_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;

    struct dose_info_t *obj = dose_info_object_get(id);

    osMutexAcquire(obj->mutex, osWaitForever);

    switch (cmd->data[0])
    {
    case 0x40:
        cmd->data[2] == 0 ? LOG_I("[%d]: dose dummy mode set\r\n", id) : LOG_I("[%d]: dose normal mode set\r\n", id);
        obj->treatment.dose_mode = cmd->data[2];
        break;
    case 0x41:
        switch (cmd->data[1])
        {
        case 0x00:
            LOG_I("[%d]: pulse generation mode set %d\r\n", id, cmd->data[2]);
            obj->treatment.pulse_mode = cmd->data[2];
            break;
            break;
        case 0x01:
            LOG_I("[%d]: dose prf set %u ok\r\n", id, cmd->data[2]);
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
            obj->treatment.ri_src = cmd->data[2];
            break;
        case 0x01:
            uint32_t meter = cmd->data[2] | cmd->data[3] << 8 | cmd->data[4] << 16 | cmd->data[5] << 24;
            LOG_I("[%d]: dose meter set %f ok\r\n", id, *(float *)&meter);
            break;
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
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
            // cmd->data[2] == 0 ? LOG_I("[%d]: beam data lock opened\r\n", id) : LOG_I("[%d]: beam data lock closed\r\n", id);
            obj->treatment.status.bits.lock = cmd->data[2];
            break;
        case 0x01:
            cmd->data[2] == 0 ? LOG_I("[%d]: treatment para set valid\r\n", id) : LOG_I("[%d]: treatment para set invalid\r\n", id);
            obj->treatment.status.bits.check = cmd->data[2];
            break;
        case 0x02:
            LOG_I("[%d]: beam valid set: %d\r\n", id, cmd->data[2]);
            break;
        default:
            ret = -1;
            break;
        }
        break;
    default:
        LOG_E("[%d]: invalid treatment cmd type: %x\r\n", id, cmd->data[0]);
        ret = -1;
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}
static int8_t dose_interlock_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;

    struct dose_info_t *obj = dose_info_object_get(id);

    osMutexAcquire(obj->mutex, osWaitForever);

    switch (cmd->data[0])
    {
    case 0x80:
        switch (cmd->data[1])
        {
        case 0x00:
            obj->interlock.threshold_dose_rate.low = cmd->data[2];
            break;
        case 0x01:
            obj->interlock.threshold_dose_rate.high = cmd->data[2];
            break;
        case 0x02:
            obj->interlock.threshold_dose_cp.low = cmd->data[2];
            break;
        case 0x03:
            obj->interlock.threshold_dose_cp.high = cmd->data[2];
            break;
        case 0x04:
            obj->interlock.threshold_symmetry = cmd->data[2];
            break;
        case 0x05:
            obj->interlock.one_pulse.threshold_high = cmd->data[1];
            break;
        case 0x06:
            obj->interlock.one_pulse.threshold_low = cmd->data[1];
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
            obj->interlock.one_pulse.count_high = cmd->data[3] << 8 | cmd->data[2];
            break;
        case 0x01:
            obj->interlock.one_pulse.count_low = cmd->data[3] << 8 | cmd->data[2];
            break;
        case 0x02:
            obj->interlock.one_pulse.count_abnormal = cmd->data[3] << 8 | cmd->data[2];
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
            LOG_I("[%d]: ionization chamber voltage get: %d\r\n", id, (cmd->data[3] << 8 | cmd->data[2]) / 100);
            obj->status.voltage_ionization_chamber = (float)(cmd->data[3] << 8 | cmd->data[2]) / 100.0f;
            break;
        case 0x01:
            LOG_I("[%d]: P5V voltage get: %d\r\n", id, (cmd->data[3] << 8 | cmd->data[2]) / 100);
            obj->status.voltage_p5v = (float)(cmd->data[3] << 8 | cmd->data[2]) / 100.0f;
            break;
        case 0x02:
            LOG_I("[%d]: N5V voltage get: %d\r\n", id, (cmd->data[3] << 8 | cmd->data[2]) / 100);
            obj->status.voltage_n5v = (float)(cmd->data[3] << 8 | cmd->data[2]) / 100.0f;
            break;
        case 0x03:
            LOG_I("[%d]: dac1 channelA offset code get: %d\r\n", id, cmd->data[3] << 8 | cmd->data[2]);
            obj->status.dac_ch1_offset = cmd->data[3] << 8 | cmd->data[2];
            break;
        case 0x04:
            LOG_I("[%d]: dac1 channelB offset code get: %d\r\n", id, cmd->data[3] << 8 | cmd->data[2]);
            obj->status.dac_ch2_offset = cmd->data[3] << 8 | cmd->data[2];
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0xB0:
        switch (cmd->data[1])
        {
        case 0x00:
            LOG_I("[%d]: dose interlock override set: %#.x\r\n", id, cmd->data[5] << 24 | cmd->data[4] << 16 | cmd->data[3] << 8 | cmd->data[2]);
            break;
        case 0x01:
            LOG_I("[%d]: dose unready override set: %#.x\r\n", id, cmd->data[5] << 24 | cmd->data[4] << 16 | cmd->data[3] << 8 | cmd->data[2]);
            break;
        case 0x02:
            obj->status.interlock.bytes = cmd->data[5] << 24 | cmd->data[4] << 16 | cmd->data[3] << 8 | cmd->data[2];
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0xB1:
        break;
    default:
        LOG_E("[%d]: invalid interlock cmd type: %x\r\n", id, cmd->data[0]);
        ret = -1;
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}
static int8_t dose_state_control_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;

    struct dose_info_t *obj = dose_info_object_get(id);

    switch (cmd->data[0])
    {
    case 0xC0:
        switch (cmd->data[1])
        {
        case 0x00:  /* dose state switch result */
            cmd->data[2] == 0 ? LOG_I("[%d]: dose state switch success\r\n", id) : LOG_I("[%d]: dose state switch fail\r\n", id);
            break;
        case 0x01:  /* dose current state */
            // LOG_I("[%d]: dose current state: %d\r\n", id, cmd->data[2]);
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->fsm_state = cmd->data[2];
            osMutexRelease(obj->mutex);
            break;
        case 0x02:  /* enable dose radiation */
            cmd->data[2] == 0 ? LOG_I("[%d]: dose radiation disable\r\n", id) : LOG_I("[%d]: dose radiation enable\r\n", id);
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0xC1:
        switch (cmd->data[1])
        {
        case 0x00:
            LOG_I("[%d]: pulse abnormal cleanup ok\r\n", id);
            break;
        case 0x01:
            LOG_I("[%d]: beam data cleanup ok\r\n", id);
            break;
        case 0x02:
            LOG_I("[%d]: dose cumulative data cleanup ok\r\n", id);
            break;
        case 0x03:
            LOG_I("[%d]: interlock cleanup ok\r\n", id);
            break;
        case 0x04:
            LOG_I("[%d]: one pulse valid flag cleanup ok\r\n", id);
            break;
        case 0x05:
            LOG_I("[%d]: dose fault cleanup ok\r\n", id);
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
            LOG_I("[%d]: dose reset wdt ok\r\n", id);
            break;
        case 0x01:
            LOG_I("[%d]: dose reset ok\r\n", id);
            break;
        default:
            ret = -1;
            break;
        }
        break;
    default:
        LOG_E("[%d]: invalid state control cmd type: %x\r\n", id, cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}

static int8_t dose_command_frame_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])   /* first cmd */
    {
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
        ret = dose_calibration_parse(id, cmd);
        break;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
        ret = dose_treatment_parse(id, cmd);
        break;
    case 0x80:
    case 0x81:
    case 0x82:
    case 0xB0:
    case 0xB1:
        ret = dose_interlock_parse(id, cmd);
        break;
    case 0xC0:
    case 0xC1:
    case 0xC2:
        ret = dose_state_control_parse(id, cmd);
        break;
    default:
        LOG_E("[%d]: invalid cmd type: %x\r\n", id, cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}

static int8_t dose_realtime_frame_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;

    struct dose_info_t *obj = dose_info_object_get(id);

    osMutexAcquire(obj->mutex, osWaitForever);

    switch (cmd->data[0])
    {
    case 0x00:
        if (cmd->data[3] == 0x01)
        {
            LOG_I("[%d]: set emergency stop\r\n", id);
        }
        else
        {
            // LOG_I("[%d]: set radiation index: %d\r\n", id, cmd->data[2] << 8 | cmd->data[1]);
        }
        break;
    case 0x01:
#if 0
        LOG_I("[%d]: dose state: %#.2x\r\n", id, cmd->data[1]);
        LOG_I("[%d]: dose fsm state: %d\r\n", id, cmd->data[2]);
        LOG_I("[%d]: dose interlock: %#.4x\r\n", id, cmd->data[4] << 8 | cmd->data[3]);
        LOG_I("[%d]: dose current cp: %d\r\n", id, cmd->data[6] << 8 | cmd->data[5]);
        LOG_I("[%d]: dose current radiation index: %d\r\n", id, cmd->data[8] << 8 | cmd->data[7]);
        LOG_I("[%d]: dose meter: %f (MU)\r\n", id, *(float *)&cmd->data[9]);
        LOG_I("[%d]: dose current cumulative: %f (MU)\r\n", id, *(float *)&cmd->data[13]);
        LOG_I("[%d]: dose current dose rate: %f (MU/min)\r\n", id, *(float *)&cmd->data[17]);
        LOG_I("[%d]: dose current prf: %d\r\n", id, cmd->data[21]);
        LOG_I("[%d]: dose abnormal pulse count: %d\r\n", id, cmd->data[23] << 8 | cmd->data[22]);
        LOG_I("[%d]: dose one pulse valid flag: %d\r\n", id, cmd->data[24]);
        LOG_I("[%d]: dose one pulse code: %d\r\n", id, *(uint32_t *)&cmd->data[25]);
#endif
        obj->realtime.state.byte = cmd->data[1];
        obj->fsm_state = cmd->data[2];
        obj->status.interlock.bytes = cmd->data[6] << 24 | cmd->data[5] << 16 | cmd->data[4] << 8 | cmd->data[3];
        obj->realtime.control_point = cmd->data[8] << 8 | cmd->data[7];
        obj->realtime.radiation_index = cmd->data[10] << 8 | cmd->data[9];
        uint32_t dose_cumulated = cmd->data[15] | cmd->data[16] << 8 | cmd->data[17] << 16 | cmd->data[18] << 24;
        obj->realtime.dose_cumulated = *(float *)&dose_cumulated;
        uint32_t dose_rate = cmd->data[19] | cmd->data[20] << 8 | cmd->data[21] << 16 | cmd->data[22] << 24;
        obj->realtime.dose_rate = *(float *)&dose_rate;
        obj->realtime.prf_current = cmd->data[23];
        obj->interlock.one_pulse.count_abnormal= cmd->data[25] << 8 | cmd->data[24];
        obj->realtime.one_pulse_valid_flag = cmd->data[26];
        obj->realtime.one_pulse_dose = cmd->data[27] | cmd->data[28] << 8 | cmd->data[29] << 16 | cmd->data[30] << 24;;
        // LOG_I("[%d]: recv dose realtime frame: %f\r\n", id, obj->realtime.dose_cumulated);

#if 0
        if (id == BGM_UART_DOSE1)
        {
            /* to fkp and qam */
            uint8_t buf[32] = {0};
            memcpy(&buf[0], &cmd->data[11], 8); /* dose meter、dose cumulated */
            memcpy(&buf[8], &cmd->data[31], 12);/* trigger interval、timestamp */
            ret = cmd_to_rtm_upload(RS422_BUS_MODULE_ID_QAM | RS422_BUS_MODULE_ID_FKP, UART_DATA_CMD_SEND_DOSE_INFO, buf, 20);
            if (ret != 0)
            {
                LOG_E("[%d]: cmd to rtm upload err: %d\r\n", id, ret);
            }
        }
#endif
        break;
    default:
        LOG_E("[%d]: invalid realtime cmd type: %x\r\n", id, cmd->data[0]);
        ret = -1;
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}

enum real_time_data_type
{
    REAL_TIME_DATA_TYPE_RADIATION = 0,
    REAL_TIME_DATA_TYPE_RI,
    REAL_TIME_DATA_TYPE_QAM,
};
static int8_t dose_realtime_data_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0x01:
        switch (cmd->data[1])
        {
        case REAL_TIME_DATA_TYPE_RADIATION:
            {
                struct dose_info_t *obj = dose_info_object_get(id);

                osMutexAcquire(obj->mutex, osWaitForever);
                obj->realtime.state.byte = cmd->data[3];
                obj->fsm_state = cmd->data[4];
                obj->status.interlock.bytes = cmd->data[8] << 24 | cmd->data[7] << 16 | cmd->data[6] << 8 | cmd->data[5];
                obj->realtime.control_point = cmd->data[10] << 8 | cmd->data[9];
                obj->realtime.radiation_index = cmd->data[12] << 8 | cmd->data[11];
                // uint32_t dose_meter = cmd->data[13] | cmd->data[14] << 8 | cmd->data[15] << 16 | cmd->data[16] << 24;
                uint32_t dose_cumulated = cmd->data[17] | cmd->data[18] << 8 | cmd->data[19] << 16 | cmd->data[20] << 24;

                obj->realtime.dose_cumulated = *(float *)&dose_cumulated;
                uint32_t dose_rate = cmd->data[21] | cmd->data[22] << 8 | cmd->data[23] << 16 | cmd->data[24] << 24;
                obj->realtime.dose_rate = *(float *)&dose_rate;
                obj->realtime.prf_current = cmd->data[25];
                obj->interlock.one_pulse.count_abnormal= cmd->data[27] << 8 | cmd->data[26];
                obj->realtime.one_pulse_valid_flag = cmd->data[28];
                obj->realtime.one_pulse_dose = cmd->data[29] | cmd->data[30] << 8 | cmd->data[31] << 16 | cmd->data[32] << 24;
                osMutexRelease(obj->mutex);

                // LOG_I("[%d]: recv dose realtime data: %f\r\n", id, obj->realtime.dose_cumulated);

                if (id == BGM_UART_DOSE1)
                {
                    /* to fkp and qam */
                    uint8_t buf[32] = {0};
                    memcpy(&buf[0], &cmd->data[13], 8); /* dose meter、dose cumulated */
                    memcpy(&buf[8], &cmd->data[33], 12);/* trigger interval、timestamp */

                    ret = cmd_to_rtm_upload(RS422_BUS_MODULE_ID_QAM | RS422_BUS_MODULE_ID_FKP, UART_DATA_CMD_SEND_DOSE_INFO, buf, 20);
                    if (ret != 0)
                    {
                        LOG_E("[%d]: cmd to rtm upload err: %d\r\n", id, ret);
                    }
                }
            }
            break;
        case REAL_TIME_DATA_TYPE_RI:
            {
                struct dose_info_t *obj = dose_info_object_get(id);
                osMutexAcquire(obj->mutex, osWaitForever);
                obj->realtime.radiation_index = cmd->data[3] << 8 | cmd->data[2];
                osMutexRelease(obj->mutex);

                struct bgm_data_info *obj_bgm = bgm_data_info_get();
                osMutexAcquire(obj->mutex, osWaitForever);
                obj_bgm->radiation_index = cmd->data[3] << 8 | cmd->data[2];
                osMutexRelease(obj->mutex);

                // LOG_I("[%d]: recv dose radiation index: %d\r\n", id, obj->realtime.radiation_index);

                ret = cmd_to_rtm_upload(RS422_BUS_MODULE_ID_BROADCAST, UART_DATA_CMD_SEND_RADIATION_INDEX, &cmd->data[2], *cmd->len - 2);
                if (ret != 0)
                {
                    LOG_E("[%d]: cmd to rtm upload err: %d\r\n", id, ret);
                }
            }
            break;
        case REAL_TIME_DATA_TYPE_QAM:
            /* 1. to fkp */
            // ret = cmd_to_rtm_upload(RS422_BUS_MODULE_ID_FKP, UART_DATA_CMD_SEND_DOSE_INFO, &cmd->data[2], 8);
            /* 2. to qam */
            // ret |= cmd_to_rtm_upload(RS422_BUS_MODULE_ID_QAM, UART_DATA_CMD_SEND_QAM, &cmd->data[2], *cmd->len - 2);
            // if (ret != 0)
            // {
            //     LOG_E("[%d]: cmd to rtm upload err: %d\r\n", id, ret);
            // }
            break;
        default:
            LOG_E("[%d]: invalid realtime data sub cmd: %x\r\n", id, cmd->data[1]);
            ret = -1;
            break;
        }
        break;

    default:
        LOG_E("[%d]: invalid realtime data cmd: %x\r\n", id, cmd->data[0]);
        ret = -2;
        break;
    }

    return ret;
}

#define LOG_OUTPUT_TO_ARM_IO
#ifdef LOG_OUTPUT_TO_ARM_IO
enum log_output_type
{
    LOG_OUTPUT_MSG = 0,
};
static int8_t dose_log_frame_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0x01:
        switch (cmd->data[1])
        {
        case LOG_OUTPUT_MSG:
            LOG_E("[%d]: dose msg: %s", id, (char *)&cmd->data[2]);
            break;
        default:
            LOG_E("[%d]: invalid log data sub cmd: %x\r\n", id, cmd->data[1]);
            break;
        }
        break;

    default:
        LOG_E("[%d]: invalid log data cmd: %x\r\n", id, cmd->data[0]);
        return -2;
        break;
    }

    return 0;
}
#endif

static int8_t dose_cmd_parse(enum uart_id id, struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        LOG_E("[%d]: cmd is NULL\r\n", id);
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.bits.cmd_id != BGM_UART_ID)
    {
        LOG_E("[%d]: cmd id err: %d\r\n", id, cmd->id.bits.cmd_id);
        return 0;
    }

    int8_t ret = 0;

    /* 2. parse cmd type */
    switch (cmd->type)
    {
    case 0x04:  /* realtime data upload by dose */
        ret = dose_realtime_data_parse(id, cmd);
        if (ret != 0)
        {
            LOG_E("[%d]: dose_realtime_data_parse err: %d\r\n", id, ret);
            return -2;
        }
        break;
#ifdef LOG_OUTPUT_TO_ARM_IO
    case 0x85:  /* dose log frame */
        ret = dose_log_frame_parse(id, cmd);
        if (ret != 0)
        {
            LOG_E("[%d]: dose_log_frame_parse err: %d\r\n", id, ret);
            return -2;
        }
        break;
#endif
    case 0x81:  /* handshake frame */
        ret = dose_handshake_frame_parse(id, cmd);
        if (ret != 0)
        {
            LOG_E("[%d]: dose_handshake_frame_parse err: %d\r\n", id, ret);
            return -2;
        }
        break;
    case 0x82:  /* command frame */
        ret = dose_command_frame_parse(id, cmd);
        if (ret != 0)
        {
            LOG_E("[%d]: dose_command_frame_parse err: %d\r\n", id, ret);
            return -2;
        }
        break;
    case 0x83:  /* realtime frame */
        ret = dose_realtime_frame_parse(id, cmd);
        if (ret != 0)
        {
            LOG_E("[%d]: dose_realtime_frame_parse err: %d\r\n", id, ret);
            return -2;
        }
        break;
    default:
        LOG_E("[%d]: invalid cmd type: %x\r\n", id, cmd->type);
        return -2;
        break;
    }

    return ret;
}

static int8_t dose_functions_init(void)
{
    int8_t ret = 0;

    struct dose_info_t *obj = NULL;

    osMutexAttr_t mutex_attributes = {
    .name = "dose_data_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    for (int i = 0; i < BGM_UART_DOSE2; i++)
    {
        obj = dose_info_object_get(i);
        obj->mutex = osMutexNew(&mutex_attributes);
        if (obj->mutex == NULL)
        {
            LOG_E("[%d]: dose mutex create failed\r\n", i);
            return -1;
        }
    }

    ret = uart_cmd_parse_callback_register(BGM_UART_DOSE1, dose_cmd_parse);
    if (ret != 0)
    {
        LOG_E("dose cmd parse callback register err: %d\r\n", ret);
        return -2;
    }

    ret = uart_cmd_parse_callback_register(BGM_UART_DOSE2, dose_cmd_parse);
    if (ret != 0)
    {
        LOG_E("dose cmd parse callback register err: %d\r\n", ret);
        return -3;
    }

    ret = uart_init_callback_register(BGM_UART_DOSE1, NULL);
    if (ret != 0)
    {
        LOG_E("dose init callback register err: %d\r\n", ret);
        return -4;
    }

    ret = uart_init_callback_register(BGM_UART_DOSE2, NULL);
    if (ret != 0)
    {
        LOG_E("dose init callback register err: %d\r\n", ret);
        return -5;
    }

    return 0;
}
INIT_ENV_EXPORT(dose_functions_init);

float dose_data_info_get(enum uart_id id, enum dose_info_index index, void *data)
{
    if (id != BGM_UART_DOSE1 && id != BGM_UART_DOSE2)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    float value = 0.0f;
    struct dose_info_t *obj = dose_info_object_get(id);

    osMutexAcquire(obj->mutex, osWaitForever);
    switch (index)
    {
    case DOSE_INFO_VERSION:
        memcpy(data, &obj->status.hw_version, 6);
        value = 6.0f;
        break;
    case DOSE_INFO_FSM_STATE_GET:
        value = (float)obj->fsm_state;
        break;
    case DOSE_INFO_METER_GET:
        value = obj->realtime.dose_cumulated;
        break;
    case DOSE_INFO_DOSE_RATE_SET:
        obj->realtime.dose_rate = *(float *)data;
        break;
    case DOSE_INFO_DOSE_RATE_GET:
        value = obj->realtime.dose_rate;
        break;
    case DOSE_INFO_PRF_GET:
        value = obj->realtime.prf_current;
        break;
    case DOSE_INFO_PULSE_MODE_GET:
        value = obj->treatment.pulse_mode;
        break;
    case DOSE_INFO_RADIATION_GET:
        value = obj->realtime.one_pulse_dose | obj->realtime.one_pulse_valid_flag << 24;
        break;
    case DOSE_INFO_INTERLOCK_GET:
        value = obj->status.interlock.bytes;
        break;
    case DOSE_INFO_RADIATION_INDEX_GET:
        value = obj->realtime.radiation_index;
        break;
    default:
        LOG_E("[%d]: invalid dose info index: %d\r\n", id, index);
        break;
    }
    osMutexRelease(obj->mutex);

    return value;
}

static int8_t dose_cmd_write(enum uart_id id, uint8_t type, void *data, uint16_t len)
{
    int8_t ret = 0;
    struct cmd_object cmd = {0};

    cmd.id.bits.cmd_id = 0;
    cmd.id.bits.cmd_ack = 1;
    cmd.type = type;
    cmd.len = &len;
    cmd.data = data;

#if 0
    for (uint8_t i = 0; i < cmd.len; i++)
    {
        cmd.data[i] = i;
    }
#endif

    ret = uart_cmd_write(id, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -4;
    }

    return 0;
}

int8_t dose_data_info_set(enum uart_id id, enum dose_info_index index, void *data, uint16_t len)
{
    if (id != BGM_UART_DOSE1 && id != BGM_UART_DOSE2)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    if (index >= DOSE_INFO_MAX)
    {
        LOG_E("[%d]: invalid dose index: %d\r\n", id, index);
        return -2;
    }

    // if (data == NULL)
    // {
    //     LOG_E("[%d]: data is NULL\r\n", id);
    //     return -3;
    // }

    // if (len == 0)
    // {
    //     LOG_E("[%d]: len is 0\r\n", id);
    //     return -4;
    // }

    int8_t ret = 0;
    uint8_t buf[10] = {0};
    uint16_t offset = 0;

    switch (index)
    {
    case DOSE_INFO_VERSION:
        ret = dose_cmd_write(id, 0x01, data, len);
        break;
    case DOSE_INFO_FSM_STATE_SET:
        buf[offset++] = 0xC0;
        buf[offset++] = 0x00;
        buf[offset++] = *(uint8_t *)data;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_FSM_STATE_GET:
        buf[offset++] = 0xC0;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_ADC_CALI:
        buf[offset++] = 0x01;
        buf[offset++] = 0x00;
        buf[offset++] = 0x00;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x02;
        buf[offset++] = len;
        buf[offset++] = *(uint32_t *)data;
        buf[offset++] = *(uint32_t *)data >> 8;
        buf[offset++] = *(uint32_t *)data >> 16;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x01;
        buf[offset++] = 0x00;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_DAC_CALI:
        buf[offset++] = 0x01;
        buf[offset++] = 0x00;
        buf[offset++] = 0x00;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x03;
        buf[offset++] = 0x00;
        buf[offset++] = *(uint16_t *)data;
        buf[offset++] = *(uint16_t *)data >> 8;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x01;
        buf[offset++] = 0x00;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_BEAM_TYPE_SET:
    {
        buf[offset++] = 0x42;
        buf[offset++] = 0x06;
        buf[offset++] = 0;
        buf[offset++] = RADIATION_TYPE_MV;
        buf[offset++] = *(uint8_t *)data;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    }
    case DOSE_INFO_BEAM_SET:
    {
        struct one_beam_order *beam_info = (struct one_beam_order *)data;
        /* 0. beam invalid set */
        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x02;
        buf[offset++] = 0x00;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        /* 1. treatment para unlock */
        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x00;
        ret |= dose_cmd_write(id, 0x02, buf, offset);
        /* 2. beam meter set */
        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x01;
        memcpy(&buf[offset], &beam_info->info->beamMeterSet, sizeof(float));
        offset += sizeof(float);
        ret |= dose_cmd_write(id, 0x02, buf, offset);
        LOG_I("[%d]: beam meter set: %f\r\n", id, beam_info->info->beamMeterSet);
        /* 3. beam cp & ri num */
        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x02;
        buf[offset++] = beam_info->info->CPQuantityInBeam;          /* cp num */
        buf[offset++] = beam_info->info->CPQuantityInBeam >> 8;
        buf[offset++] = beam_info->info->RIQuantityInBeam;          /* ri num low */
        buf[offset++] = beam_info->info->RIQuantityInBeam >> 8;     /* ri num high */
        ret |= dose_cmd_write(id, 0x02, buf, offset);
        LOG_I("[%d]: beam cp num: %d, ri num: %d\r\n", id, beam_info->info->CPQuantityInBeam, beam_info->info->RIQuantityInBeam);
        /* 4. beam cp & ri map */
        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x04;
        for (uint16_t i = 0; i < beam_info->info->CPQuantityInBeam; i++)    /* cp num */
        {
            buf[offset++] = i + 1;   /* cp index */
            buf[offset++] = (i + 1) >> 8;
            buf[offset++] = beam_info->cp_ri_map[i];        /* ri value low */
            buf[offset++] = beam_info->cp_ri_map[i] >> 8;   /* ri value high */
            ret |= dose_cmd_write(id, 0x02, buf, offset);
            offset = 2;
        }
        /* 5. beam ri value */
        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x05;
        for (uint16_t i = 0; i < beam_info->info->RIQuantityInBeam; i++)    /* ri num */
        {
            buf[offset++] = i + 1;
            buf[offset++] = (i + 1) >> 8;
            memcpy(&buf[offset], &beam_info->ri_data[i].fCumulativeDose, sizeof(float));    /* ri dose value, unit: MU */
            offset += sizeof(float);
            memcpy(&buf[offset], &beam_info->ri_data[i].fDoseRate, sizeof(float));  /* ri dose rate value, unit: MU/min */
            offset += sizeof(float);
            float dose_expect_time = beam_info->ri_data[i].DeliveryTime * 1000.0f;  /* ri dose expect time value, unit: ms */
            memcpy(&buf[offset], &dose_expect_time, sizeof(float));
            offset += sizeof(float);

            ret |= dose_cmd_write(id, 0x02, buf, offset);
            offset = 2;
        }
        /* 6. beam info */
        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x06;
        buf[offset++] = 0;
        buf[offset++] = beam_info->info->radiationType;
        buf[offset++] = beam_info->info->deliveryType;
        ret |= dose_cmd_write(id, 0x02, buf, offset);
        /* 7. treatment para lock and validate */
        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x01;
        ret |= dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x01;
        buf[offset++] = 0x01;
        ret |= dose_cmd_write(id, 0x02, buf, offset);

        if (ret != 0)
        {
            LOG_E("[%d]: beam set failed\r\n", id);
            return -5;
        }
        /* 8. beam valid set */
        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x02;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    }
    case DOSE_INFO_METER_SET:
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x00;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x01;
        memcpy(&buf[offset], (float *)data, sizeof(float));
        offset += sizeof(float);
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_PRF_SET:
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x00;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x41;
        buf[offset++] = 0x01;
        buf[offset++] = *(uint8_t *)data;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_GENERATE_MODE_SET:
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x00;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x40;
        buf[offset++] = 0x00;
        buf[offset++] = *(uint8_t *)data;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_PULSE_MODE_SET:
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x00;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x41;
        buf[offset++] = 0x00;
        buf[offset++] = *(uint8_t *)data;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_RADIATION_SET:
        buf[offset++] = 0x00;
        buf[offset++] = ((uint8_t *)data)[0];
        buf[offset++] = ((uint8_t *)data)[1];
        buf[offset++] = ((uint8_t *)data)[2];
        ret = dose_cmd_write(id, 0x03, buf, offset);
        break;
    case DOSE_INFO_RADIATION_GET:
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x03, buf, offset);
        break;
    case DOSE_INFO_INTERLOCK_SET:
        buf[offset++] = 0xC1;
        buf[offset++] = 0x03;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_INTERLOCK_GET:
        buf[offset++] = 0xB0;
        buf[offset++] = 0x02;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_CUMULATED_CLEAR:
        buf[offset++] = 0xC1;
        buf[offset++] = 0x02;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_RADIATION_ENABLE_SET:
        buf[offset++] = 0xC0;
        buf[offset++] = 0x02;
        buf[offset++] = *(uint8_t *)data;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_PLAN_DATA_CLEAR:
        buf[offset++] = 0xC1;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_FAULT_ALL_CLEAR:
        buf[offset++] = 0xC1;
        buf[offset++] = 0x05;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}


#ifdef LOG_OUTPUT_TO_ARM_IO
#include "shell.h"
static int8_t dose_cmd_send(uint8_t argc, uint8_t **argv)
{
    uint8_t buf[128] = {0};
    uint8_t offset = 0;

    /* 
     * argv[1]: uart id
     * argv[2]: dose cmd
     * argv[3-n]: dose cmd args
    */

    buf[offset++] = 0x01;
    buf[offset++] = LOG_OUTPUT_MSG;

    enum uart_id id = atoi(argv[1]);
    memcpy(buf + offset, argv[2], strlen(argv[2]));
    offset += strlen(argv[2]);

    for (uint8_t i = 3; i < argc; i++)
    {
        buf[offset++] = ' ';
        memcpy(buf + offset, argv[i], strlen(argv[i]));
        offset += strlen(argv[i]);
    }

    // LOG_I("uart id: %d, cmd: %s\r\n", id, &buf[2]);

    return dose_cmd_write(id, 0x05, buf, offset);
}
MSH_CMD_EXPORT_ALIAS(dose_cmd_send, dose_cmd_send, dose cmd send);
#endif