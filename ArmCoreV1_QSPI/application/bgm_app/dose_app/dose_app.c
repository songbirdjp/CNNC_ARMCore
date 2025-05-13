#include "dose_app.h"
#include <stddef.h>
#include "cmsis_os2.h"
#include "init_call.h"
#include "ulog.h"
#include "plan_data.h"
#include "rtm_app.h"

struct board_status
{
    uint8_t hw_version;
    uint8_t sw_version[6];
    union
    {
        uint16_t bytes;
        struct
        {
            uint16_t board_power_fault : 1;
            uint16_t hv_limit : 1;
            uint16_t comm_timeout : 1;
            uint16_t wdt_fault : 1;
            uint16_t adcs7476_1_limit_high : 1;
            uint16_t adcs7476_1_limit_low : 1;
            uint16_t adcs7476_2_limit_high :1;
            uint16_t adcs7476_2_limit_low : 1;
            uint16_t illegal_write : 1;
            uint16_t dose_rate_low : 1;
            uint16_t dose_rate_high : 1;
            uint16_t dose_total_low : 1;
            uint16_t dose_total_high : 1;
            uint16_t dose_symmetry_fault : 1;
            uint16_t dose_dummy_timeout : 1;
            uint16_t reserved : 1;
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
    obj->status.sw_version[0] = cmd->data[2];
    obj->status.sw_version[1] = '.';
    obj->status.sw_version[2] = cmd->data[3];
    obj->status.sw_version[3] = '.';
    obj->status.sw_version[4] = cmd->data[4];

    osMutexRelease(obj->mutex);

    // LOG_I("[%d]: hw version: %d\r\n", id, obj->status.hw_version);
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
            cmd->data[2] == 0 ? LOG_I("[%d]: dose calibration data lock opened\r\n", id) : LOG_I("[%d]: dose calibration data lock closed\r\n", id);
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
            LOG_I("[%d]: dose meter set %d ok\r\n", id, cmd->data[3] << 8 | cmd->data[2]);
            break;
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
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
            cmd->data[2] == 0 ? LOG_I("[%d]: beam data lock opened\r\n", id) : LOG_I("[%d]: beam data lock closed\r\n", id);
            obj->treatment.status.bits.lock = cmd->data[2];
            break;
        case 0x01:
            cmd->data[2] == 0 ? LOG_I("[%d]: beam data valid\r\n", id) : LOG_I("[%d]: beam data invalid\r\n", id);
            obj->treatment.status.bits.check = cmd->data[2];
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
        // LOG_I("[%d]: dose interlock get %#.4x\r\n", id, cmd->data[3] << 8 | cmd->data[2]);
        obj->status.interlock.bytes = cmd->data[3] << 8 | cmd->data[2];
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
            LOG_I("[%d]: set radiation index: %d\r\n", id, cmd->data[2] << 8 | cmd->data[1]);
        }
        break;
    case 0x01:
#if 0
        LOG_I("[%d]: dose state: %#.2x\r\n", id, cmd->data[1]);
        LOG_I("[%d]: dose interlock: %#.4x\r\n", id, cmd->data[3] << 8 | cmd->data[2]);
        LOG_I("[%d]: dose current cp: %d\r\n", id, cmd->data[5] << 8 | cmd->data[4]);
        LOG_I("[%d]: dose current radiation index: %d\r\n", id, cmd->data[7] << 8 | cmd->data[6]);
        LOG_I("[%d]: dose current cumulative: %f (MU)\r\n", id, *(float *)&cmd->data[8]);
        LOG_I("[%d]: dose current prf: %d\r\n", id, cmd->data[12]);
        LOG_I("[%d]: dose abnormal pulse count: %d\r\n", id, cmd->data[14] << 8 | cmd->data[13]);
        LOG_I("[%d]: dose one pulse valid flag: %d\r\n", id, cmd->data[15]);
        LOG_I("[%d]: dose one pulse code: %d\r\n", id, *(uint32_t *)&cmd->data[16]);
#endif
        obj->realtime.state.byte = cmd->data[1];
        obj->status.interlock.bytes = cmd->data[3] << 8 | cmd->data[2];
        obj->realtime.control_point = cmd->data[5] << 8 | cmd->data[4];
        obj->realtime.radiation_index = cmd->data[7] << 8 | cmd->data[6];
        obj->realtime.dose_cumulated = *(float *)&cmd->data[8];
        obj->realtime.prf_current = cmd->data[12];
        obj->interlock.one_pulse.count_abnormal= cmd->data[14] << 8 | cmd->data[13];
        obj->realtime.one_pulse_valid_flag = cmd->data[15];
        obj->realtime.one_pulse_dose = *(uint32_t *)&cmd->data[16];
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
                obj->status.interlock.bytes = cmd->data[5] << 8 | cmd->data[4];
                obj->realtime.control_point = cmd->data[7] << 8 | cmd->data[6];
                obj->realtime.radiation_index = cmd->data[9] << 8 | cmd->data[8];
                obj->realtime.dose_cumulated = *(float *)&cmd->data[10];
                obj->realtime.prf_current = cmd->data[14];
                obj->interlock.one_pulse.count_abnormal= cmd->data[16] << 8 | cmd->data[15];
                obj->realtime.one_pulse_valid_flag = cmd->data[17];
                obj->realtime.one_pulse_dose = *(uint32_t *)&cmd->data[18];
                osMutexRelease(obj->mutex);
            }
            break;
        case REAL_TIME_DATA_TYPE_RI:
            {
                struct dose_info_t *obj = dose_info_object_get(id);
                osMutexAcquire(obj->mutex, osWaitForever);
                obj->realtime.radiation_index = cmd->data[3] << 8 | cmd->data[2];
                osMutexRelease(obj->mutex);

                ret = cmd_to_rtm_upload(RS422_BUS_MODULE_ID_BROADCAST, UART_CMD_SEND_RADIATION_INDEX, &cmd->data[2], *cmd->len - 2);
                if (ret != 0)
                {
                    LOG_E("cmd to rtm upload err: %d\r\n", ret);
                }
            }
            break;
        case REAL_TIME_DATA_TYPE_QAM:
            /* 1. to fkp */
            ret = cmd_to_rtm_upload(RS422_BUS_MODULE_ID_FKP, UART_DATA_CMD_SEND_DOSE_INFO, &cmd->data[2], 8);
            /* 2. to rtm */
            ret |= cmd_to_rtm_upload(RS422_BUS_MODULE_ID_QAM, UART_DATA_CMD_SEND_QAM, &cmd->data[2], *cmd->len - 2);
            if (ret != 0)
            {
                LOG_E("cmd to rtm upload err: %d\r\n", ret);
            }
            break;
        default:
            LOG_E("invalid realtime data sub cmd: %x\r\n", cmd->data[1]);
            ret = -1;
            break;
        }
        break;

    default:
        LOG_E("invalid realtime data cmd: %x\r\n", cmd->data[0]);
        ret = -2;
        break;
    }

    return ret;
}

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
    case DOSE_INFO_BEAM_SET:
    {
        struct one_beam_order *beam_info = (struct one_beam_order *)data;
        /* 1. beam unlock */
        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x00;
        ret = dose_cmd_write(id, 0x02, buf, offset);        
        /* 2. beam meter set */
        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x01;
        uint16_t beam_meter = beam_info->info->beamMeterSet * 10.0f;
        buf[offset++] = beam_meter;
        buf[offset++] = beam_meter >> 8;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        LOG_I("[%d]: beam meter set: %d\r\n", id, beam_meter);
        /* 3. beam cp & ri num */
        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x02;
        buf[offset++] = beam_info->info->CPQuantityInBeam;          /* cp num */
        buf[offset++] = beam_info->info->CPQuantityInBeam >> 8;
        buf[offset++] = beam_info->info->RIQuantityInBeam;          /* ri num low */
        buf[offset++] = beam_info->info->RIQuantityInBeam >> 8;     /* ri num high */
        ret = dose_cmd_write(id, 0x02, buf, offset);
        LOG_I("[%d]: beam cp num: %d, ri num: %d\r\n", id, beam_info->info->CPQuantityInBeam, beam_info->info->RIQuantityInBeam);
        /* 4. beam cp & ri map */
        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x04;
        for (uint8_t i = 0; i < beam_info->info->CPQuantityInBeam; i++)    /* cp num */
        {
            buf[offset++] = i + 1;   /* cp index */
            buf[offset++] = (i + 1) >> 8;
            buf[offset++] = beam_info->cp_ri_map[i];        /* ri value low */
            buf[offset++] = beam_info->cp_ri_map[i] >> 8;   /* ri value high */
            ret = dose_cmd_write(id, 0x02, buf, offset);
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
            uint16_t dose = beam_info->ri_data[i].fCumulativeDose * 10.0f;
            buf[offset++] = dose;   /* ri dose value, need *10 */
            buf[offset++] = dose >> 8;
            uint16_t dose_rate = beam_info->ri_data[i].fDoseRate * 10.0f;
            buf[offset++] = dose_rate;   /* ri dose rate value, unit: MU/min */
            buf[offset++] = dose_rate >> 8;
            uint16_t dose_expect_time = beam_info->ri_data[i].DeliveryTime * 1000.0f;
            buf[offset++] = dose_expect_time;   /* ri dose expect time value, unit: ms */
            buf[offset++] = dose_expect_time >> 8;

            ret = dose_cmd_write(id, 0x02, buf, offset);
            offset = 2;
        }
        /* 6. beam info */
        offset = 0;
        buf[offset++] = 0x42;
        buf[offset++] = 0x06;
        buf[offset++] = beam_info->info->beamType;
        buf[offset++] = 0;
        buf[offset++] = beam_info->info->deliveryType;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        /* 7. beam lock and validate */
        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x00;
        buf[offset++] = 0x01;
        ret = dose_cmd_write(id, 0x02, buf, offset);

        offset = 0;
        buf[offset++] = 0x43;
        buf[offset++] = 0x01;
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
        buf[offset++] = *(uint16_t *)data;
        buf[offset++] = *(uint16_t *)data >> 8;
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
        buf[offset++] = 0x00;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    case DOSE_INFO_CUMULATED_CLEAR:
        buf[offset++] = 0xC1;
        buf[offset++] = 0x02;
        ret = dose_cmd_write(id, 0x02, buf, offset);
        break;
    default:
        break;
    }

    return ret;
}
