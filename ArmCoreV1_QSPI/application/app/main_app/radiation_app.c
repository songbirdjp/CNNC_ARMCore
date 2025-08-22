#include "radiation_app.h"
#include "init_call.h"
#include "cmsis_os2.h"
#include "adcs7476.h"
#include "ltc2632.h"
#include "fsm_app.h"
#include "interlock_app.h"
#include "dose_uart.h"
#include "plan_data.h"
#include "gpio_app.h"
#include "ulog.h"
#include "timestamp.h"
#include "os_tool.h"

// #define RADIATION_SIMULATION_MODE    /* used for different plan data test */
#ifdef RADIATION_SIMULATION_MODE
/* NOTE: modify pulse interval for simulation mode if needed */
static uint8_t radiation_simulation_trigger_out_flag = 0;
static uint16_t dose_simulated[BUF_LEN] = {0}, dose_simulated_1[BUF_LEN] = {0};
static int8_t radiation_simulation_dose_deal(uint16_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0)
    {
        return -1;
    }

    if (dose_simulated[0] == 0)
    {
        for (uint16_t i = 0; i < BUF_LEN; i++)
        {
            dose_simulated[i] = i * 2 + 800;
            dose_simulated_1[i] =  (BUF_LEN - i - 1) * 2 + 800;
        }
    }

#if 0
    for (uint16_t i = 0; i < BUF_LEN; i++)
    {
        LOG_I("%d ", dose_simulated_1[i]);
    }
    LOG_I("\r\n");
#endif

    static uint8_t cnt = 0;
    if (cnt < 2)
    {
        memcpy(buf, dose_simulated, len * 2);
    }
    else
    {
        memcpy(buf, dose_simulated_1, len * 2);
    }

    cnt++;
    cnt %= 4;

    return 0;
}
#endif

// #define RADIATION_FIX_RATE_SIMULATE
#ifdef RADIATION_FIX_RATE_SIMULATE
static int8_t adcs7476_value_dose(uint32_t value, uint32_t value_1, uint16_t pulse_cnt);
#endif

struct radiation_index_time
{
    uint32_t ri_start_time;
    uint32_t ri_end_time;
    uint32_t ri_elapse_time;
};

struct radiation_index_data
{
    struct control_para *control_data;
    struct beam_data *beam_data;
    uint16_t pulse_interval_us;

    struct radiation_index_time ri_time;

    osMutexId_t mutex;
};
static struct radiation_index_data radiation_data = {0};
static struct radiation_index_data *radiation_data_get(void)
{
    return &radiation_data;
}

static int8_t radiation_data_init(void)
{
    int8_t ret = 0;
    struct radiation_index_data *obj = radiation_data_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    ret = control_data_pointer_get(&obj->control_data);
    if (ret != 0)
    {
        LOG_E("control data pointer get err: %d\r\n", ret);
        ret = -1;
    }

    ret = beam_data_pointer_get(0, &obj->beam_data);
    if (ret != 0)
    {
        LOG_E("beam data pointer get err: %d\r\n", ret);
        ret = -2;
    }

    osMutexRelease(obj->mutex);

    return ret;
}

enum radiation_data_state
{
    DOSE_BOARD_ID = 0,
    DOSE_BEAM_METER,
    DOSE_BEAM_DELIVER_TYPE,
    DOSE_RADIATION_IDX_CURRENT,
    DOSE_CONTROL_POINT_CURRENT,
    DOSE_CONTROL_POINT_PREV_RADIATION_IDX,
    DOSE_CONTROL_POINT_UPDATE,
    DOSE_PREV_RADIATION_IDX,
    DOSE_RADIATION_IDX,
    DOSE_RATE_RADIATION_IDX,
    DOSE_TIME_RADIATION_IDX,
    DOSE_INTERPOLATED_RADIATION_IDX,
    DOSE_RATE_INTERPOLATED_RADIATION_IDX,
    DOSE_CALIBRATION_FACTOR,
    DOSE_GENERATION_MODE,
    PULSE_GENERATION_MODE,
    PULSE_INTERVAL,
    PULSE_INTERVAL_MIN,
    PULSE_SYMMETRY,
    DOSE_LIMIT,
    DOSE_RATE_LIMIT,
    TIME_RADIATION_IDX_BEGIN,
    TIME_RADIATION_IDX_END,
    TIME_RADIATION_IDX_ELAPSE,
    RADIATION_ENABLE,
};
static uint64_t radiation_data_value_get(enum radiation_data_state state)
{
    uint64_t value = 0;
    osStatus_t stat = osOK;
    struct radiation_index_data *obj = radiation_data_get();

    switch (state)
    {
    case DOSE_BOARD_ID:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->board_id;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_BEAM_METER:
        {
            stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->control_data->mutex);
            }
            uint32_t factor = obj->control_data->calibration.adc_factor[0][0] + obj->control_data->calibration.adc_factor[0][1];
            osMutexRelease(obj->control_data->mutex);

            stat = osMutexAcquire(obj->beam_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->beam_data->mutex);
            }
            value = (uint64_t)((double)obj->beam_data->dose_meter * factor / 2.0f + 0.5f);
            osMutexRelease(obj->beam_data->mutex);
        }
        break;
    case DOSE_BEAM_DELIVER_TYPE:
        stat = osMutexAcquire(obj->beam_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->beam_data->mutex, __FILE__, __LINE__, __FUNCTION__);
        }
        value = obj->beam_data->deliver_type;
        osMutexRelease(obj->beam_data->mutex);
        break;
    case DOSE_RADIATION_IDX_CURRENT:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->radiation.index;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_CONTROL_POINT_CURRENT:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->radiation.cp;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_CONTROL_POINT_PREV_RADIATION_IDX:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->radiation.cp_prev;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_PREV_RADIATION_IDX:
        {
            stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->control_data->mutex);
            }
            uint16_t idx = obj->control_data->radiation.index;
            uint32_t factor = obj->control_data->calibration.adc_factor[0][0] + obj->control_data->calibration.adc_factor[0][1];
            osMutexRelease(obj->control_data->mutex);

            stat = osMutexAcquire(obj->beam_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->beam_data->mutex);
            }
            if (idx <= obj->beam_data->total_ri)
            {
                value = (uint64_t)((double)obj->beam_data->radiation_data[idx < 1 ? 0 : idx - 1].dose_cumulative * factor / 2.0f + 0.5f);
            }
            else
            {
                LOG_E("invalid radiation index: %d\r\n", idx);
            }
            osMutexRelease(obj->beam_data->mutex);
        }
        break;
    case DOSE_RADIATION_IDX:
        {
            stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->control_data->mutex);
            }
            uint16_t idx = obj->control_data->radiation.index;
            uint32_t factor = obj->control_data->calibration.adc_factor[0][0] + obj->control_data->calibration.adc_factor[0][1];
            osMutexRelease(obj->control_data->mutex);

            stat = osMutexAcquire(obj->beam_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->beam_data->mutex);
            }
            if (idx <= obj->beam_data->total_ri)
            {
                value = (uint64_t)((double)obj->beam_data->radiation_data[idx].dose_cumulative * factor / 2.0f + 0.5f);
            }
            else
            {
                LOG_E("invalid radiation index: %d\r\n", idx);
            }
            osMutexRelease(obj->beam_data->mutex);
        }
        break;
    case DOSE_RATE_RADIATION_IDX:
        {
            stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->control_data->mutex);
            }
            uint16_t idx = obj->control_data->radiation.index;
            uint32_t factor = obj->control_data->calibration.adc_factor[0][0] + obj->control_data->calibration.adc_factor[0][1];
            osMutexRelease(obj->control_data->mutex);

            stat = osMutexAcquire(obj->beam_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->beam_data->mutex);
            }
            if (idx <= obj->beam_data->total_ri)
            {
                value = (uint64_t)((double)obj->beam_data->radiation_data[idx < 1 ? 0 : idx - 1].dose_rate * factor / 2.0f + 0.5f);
            }
            else
            {
                LOG_E("invalid radiation index: %d\r\n", idx);
            }
            osMutexRelease(obj->beam_data->mutex);
        }
        break;
    case DOSE_TIME_RADIATION_IDX:
        {
            stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->control_data->mutex);
            }
            uint16_t idx = obj->control_data->radiation.index;
            osMutexRelease(obj->control_data->mutex);

            stat = osMutexAcquire(obj->beam_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->beam_data->mutex);
            }
            if (idx <= obj->beam_data->total_ri)
            {
                value = (uint64_t)obj->beam_data->radiation_data[idx < 1 ? 0 : idx - 1].time_expected;
            }
            else
            {
                LOG_E("invalid radiation index: %d\r\n", idx);
            }
            osMutexRelease(obj->beam_data->mutex);
        }
        break;
    case DOSE_INTERPOLATED_RADIATION_IDX:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->radiation.dose_interpolated;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_RATE_INTERPOLATED_RADIATION_IDX:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->radiation.dose_rate_interpolated;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_CALIBRATION_FACTOR:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = (uint64_t)obj->control_data->calibration.adc_factor[0][0] | (uint64_t)obj->control_data->calibration.adc_factor[0][1] << 32;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_GENERATION_MODE:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->treatment.dose_mode;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_GENERATION_MODE:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->treatment.pulse_mode;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_INTERVAL:
        {
            stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->control_data->mutex);
            }
            uint8_t pulse_mode = obj->control_data->treatment.pulse_mode;
            uint8_t prf_hz = obj->control_data->treatment.prf_hz;
            osMutexRelease(obj->control_data->mutex);

            stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->mutex);
            }
            value = (pulse_mode == 0) ? 1000000 / prf_hz : obj->pulse_interval_us;
            osMutexRelease(obj->mutex);
        }
        break;
    case PULSE_INTERVAL_MIN:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->calibration.trig_interval_min;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_SYMMETRY:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->interlock.threshold_symmetry;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_LIMIT:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->interlock.threshold_dose_cp.high << 8 | obj->control_data->interlock.threshold_dose_cp.low;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_RATE_LIMIT:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->interlock.threshold_dose_rate.high << 8 | obj->control_data->interlock.threshold_dose_rate.low;
        osMutexRelease(obj->control_data->mutex);
        break;
    case TIME_RADIATION_IDX_BEGIN:
        stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->mutex);
        }
        value = obj->ri_time.ri_start_time;
        osMutexRelease(obj->mutex);
        break;
    case TIME_RADIATION_IDX_END:
        stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->mutex);
        }
        value = obj->ri_time.ri_end_time;
        osMutexRelease(obj->mutex);
        break;
    case TIME_RADIATION_IDX_ELAPSE:
        stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->mutex);
        }
        value = obj->ri_time.ri_elapse_time;
        osMutexRelease(obj->mutex);
        break;
    case RADIATION_ENABLE:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        value = obj->control_data->radiation_ctrl.radiation_enable;
        osMutexRelease(obj->control_data->mutex);
        break;
    default:
        LOG_E("invalid radiation data state: %d\r\n", state);
        break;
    }

    return value;
}
static int8_t radiation_data_value_set(enum radiation_data_state state, uint64_t value)
{
    int8_t ret = 0;
    osStatus_t stat = osOK;
    struct radiation_index_data *obj = radiation_data_get();

    switch (state)
    {
    case DOSE_BEAM_DELIVER_TYPE:
        stat = osMutexAcquire(obj->beam_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->beam_data->mutex);
        }
        obj->beam_data->deliver_type = value;
        osMutexRelease(obj->beam_data->mutex);
        break;
    case DOSE_RADIATION_IDX_CURRENT:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        obj->control_data->radiation.index = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_CONTROL_POINT_CURRENT:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        obj->control_data->radiation.cp = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_CONTROL_POINT_PREV_RADIATION_IDX:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        obj->control_data->radiation.cp_prev = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_CONTROL_POINT_UPDATE:
        {
            stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->control_data->mutex);
            }
            uint16_t idx = obj->control_data->radiation.index;
            obj->control_data->radiation.cp_prev = obj->control_data->radiation.cp;
            obj->control_data->radiation.cp = beam_data_value_get(0, BEAM_RI_IN_CP, idx);
            obj->control_data->radiation.index_max_in_cp = beam_data_value_get(0, BEAM_RI_IN_CP_MAX, idx);
            osMutexRelease(obj->control_data->mutex);
        }
        break;
    case DOSE_INTERPOLATED_RADIATION_IDX:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        obj->control_data->radiation.dose_interpolated = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_RATE_INTERPOLATED_RADIATION_IDX:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        obj->control_data->radiation.dose_rate_interpolated = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_GENERATION_MODE:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        obj->control_data->treatment.dose_mode = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_GENERATION_MODE:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        obj->control_data->treatment.pulse_mode = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_INTERVAL:
        {
            stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->control_data->mutex);
            }
            uint32_t interval_min = obj->control_data->calibration.trig_interval_min;
            osMutexRelease(obj->control_data->mutex);
            if (value < interval_min)
            {
                ret = -1;
                LOG_E("invalid pulse interval set: %llu\r\n", value);
                break;
            }
            stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
            if (stat != osOK)
            {
                os_tool_mutex_holder_get(obj->mutex);
            }
            obj->pulse_interval_us = value;
            osMutexRelease(obj->mutex);
        }
        break;
    case TIME_RADIATION_IDX_BEGIN:
        stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->mutex);
        }
        obj->ri_time.ri_start_time = value;
        osMutexRelease(obj->mutex);
        break;
    case TIME_RADIATION_IDX_END:
        stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->mutex);
        }
        obj->ri_time.ri_end_time = value;
        osMutexRelease(obj->mutex);
        break;
    case TIME_RADIATION_IDX_ELAPSE:
        stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->mutex);
        }
        obj->ri_time.ri_elapse_time = value;
        osMutexRelease(obj->mutex);
        break;
    case RADIATION_ENABLE:
        stat = osMutexAcquire(obj->control_data->mutex, MUTEX_TIMEOUT_MS);
        if (stat != osOK)
        {
            os_tool_mutex_holder_get(obj->control_data->mutex);
        }
        obj->control_data->radiation_ctrl.radiation_enable = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    default:
        ret = -1;
        LOG_E("invalid radiation data state: %d\r\n", state);
        break;
    }

    return ret;
}

#define ONE_PULSE_TIMEOUT_US    300
struct dose_value
{
    uint16_t cnt_one_pulse;         /* indicate valid pulse count, here 1cnt = 1us */
    uint32_t dose_one_pulse[DOSE_CHANNEL_MAX];
    uint64_t dose_accumulated[DOSE_CHANNEL_MAX];
    uint64_t dose_rate_current;
    uint16_t prf_current;

    uint8_t one_pulse_complete;     /* for uart protocol */
    uint8_t one_beam_complete;

    osMutexId_t mutex;
};
static struct dose_value dose_value_object = {0};
static struct dose_value *dose_value_obj_get(void)
{
    return &dose_value_object;
}
static int8_t dose_value_status_update(enum pulse_state state, uint32_t pulse_cnt, uint32_t dose_value, uint32_t dose_value_1)
{
    int8_t ret = 0;
    osStatus_t stat = osOK;
    struct dose_value *obj = dose_value_obj_get();
    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }

    switch (state)
    {
    case ONE_PULSE_START:
        obj->cnt_one_pulse = 0;
        obj->dose_one_pulse[DOSE_CHANNEL_0] = 0;
        obj->dose_one_pulse[DOSE_CHANNEL_1] = 0;
        obj->one_pulse_complete = 0;
        obj->one_beam_complete = 0;
        break;
    case ONE_PULSE_RUNNING:
        obj->cnt_one_pulse += pulse_cnt;
        obj->dose_one_pulse[DOSE_CHANNEL_0] += dose_value;
        obj->dose_one_pulse[DOSE_CHANNEL_1] += dose_value_1;
        obj->dose_accumulated[DOSE_CHANNEL_0] += dose_value;
        obj->dose_accumulated[DOSE_CHANNEL_1] += dose_value_1;
        break;
    case ONE_PULSE_COMPLETE:
        obj->cnt_one_pulse = 0;
        obj->one_pulse_complete = 1;
        break;
    case ONE_PULSE_TIMEOUT:
        obj->cnt_one_pulse = ONE_PULSE_TIMEOUT_US;
        break;
    case ONE_PULSE_CLEAR:
        obj->cnt_one_pulse = 0;
        break;
    case ONE_BEAM_COMPLETE:
        obj->cnt_one_pulse = 0;
        obj->one_beam_complete = 1;
        break;
    default:
        ret = -1;
        LOG_E("invalid pulse state: %d\r\n", state);
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}
int8_t dose_value_status_set(enum pulse_state state, uint64_t value, uint64_t value_1)
{
    int8_t ret = 0;
    osStatus_t stat = osOK;
    struct dose_value *obj = dose_value_obj_get();
    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }

    switch (state)
    {
    case ONE_PULSE_COMPLETE:
        obj->one_pulse_complete = value;
        break;
    case ONE_PULSE_COUNT:
        obj->cnt_one_pulse = value;
        break;
    case ONE_PULSE_DOSE:
        obj->dose_one_pulse[DOSE_CHANNEL_0] = value;
        obj->dose_one_pulse[DOSE_CHANNEL_1] = value_1;
        break;
    case DOSE_ACCUMULATED:
        obj->dose_accumulated[DOSE_CHANNEL_0] = value;
        obj->dose_accumulated[DOSE_CHANNEL_1] = value_1;
        break;
    case DOSE_RATE_CURRENT:
        obj->dose_rate_current = value;
        break;
    case PRF_CURRENT:
        obj->prf_current = value;
        break;
    default:
        ret = -1;
        LOG_E("invalid pulse state: %d\r\n", state);
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}
uint64_t dose_value_status_get(enum pulse_state state, enum dose_channel channel)
{
    uint64_t value = 0;
    osStatus_t stat = osOK;
    struct dose_value *obj = dose_value_obj_get();
    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }

    switch (state)
    {
    case ONE_PULSE_COMPLETE:
        value = obj->one_pulse_complete;
        break;
    case ONE_BEAM_COMPLETE:
        value = obj->one_beam_complete;
        break;
    case ONE_PULSE_COUNT:
        value = obj->cnt_one_pulse;
        break;
    case ONE_PULSE_DOSE:
        if (channel < DOSE_CHANNEL_0 || channel > DOSE_CHANNEL_1)
        {
            LOG_E("invalid dose channel: %d\r\n", channel);
            break;
        }
        value = obj->dose_one_pulse[channel];
        break;
    case DOSE_ACCUMULATED:
        if (channel < DOSE_CHANNEL_0 || channel > DOSE_CHANNEL_1)
        {
            LOG_E("invalid dose channel: %d\r\n", channel);
            break;
        }
        value = obj->dose_accumulated[channel];
        break;
    case DOSE_RATE_CURRENT:
        value = obj->dose_rate_current;
        break;
    case PRF_CURRENT:
        value = obj->prf_current;
        break;
    default:
        LOG_E("invalid dose value state: %d\r\n", state);
        break;
    }

    osMutexRelease(obj->mutex);

    return value;
}

struct trigger_out
{
    uint8_t type;
    uint8_t flag;
    uint32_t interval_us;
    uint32_t count;

    osMutexId_t mutex;
};
static struct trigger_out trigger_out_object = {0};
static struct trigger_out *trigger_out_obj_get(void)
{
    return &trigger_out_object;
}
enum trigger_out_state
{
    TRIGGER_OUT_TYPE = 0,
    TRIGGER_OUT_FLAG,
    TRIGGER_OUT_INTERVAL,
    TRIGGER_OUT_COUNT,
    TRIGGER_OUT_MAX,
};
static int8_t trigger_out_info_set(enum trigger_out_state state, uint32_t value)
{
    int8_t ret = 0;
    osStatus_t stat = osOK;
    struct trigger_out *obj = trigger_out_obj_get();
    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }

    switch (state)
    {
    case TRIGGER_OUT_TYPE:
        obj->type = value;
        break;
    case TRIGGER_OUT_FLAG:
        obj->flag = value;
        break;
    case TRIGGER_OUT_INTERVAL:
        obj->interval_us = value;
        break;
    case TRIGGER_OUT_COUNT:
        obj->count = value;
        break;
    default:
        LOG_E("invalid trigger out state: %d\r\n", state);
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}
static uint32_t trigger_out_info_get(enum trigger_out_state state)
{
    uint32_t value = 0;
    osStatus_t stat = osOK;
    struct trigger_out *obj = trigger_out_obj_get();
    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }

    switch (state)
    {
    case TRIGGER_OUT_TYPE:
        value = obj->type;
        break;
    case TRIGGER_OUT_FLAG:
        value = obj->flag;
        break;
    case TRIGGER_OUT_INTERVAL:
        value = obj->interval_us;
        break;
    case TRIGGER_OUT_COUNT:
        value = obj->count;
        break;
    default:
        LOG_E("invalid trigger out state: %d\r\n", state);
        break;
    }

    osMutexRelease(obj->mutex);

    return value;
}

// #define DETECT_RADIATION_TIME_FROM_TRIGGER_OUT
#ifdef DETECT_RADIATION_TIME_FROM_TRIGGER_OUT
#include "utilities.h"
static struct system_time begin_time = {0}, end_time = {0};
#endif

// #define TRIGGER_OUT_STATISTIC
#ifdef TRIGGER_OUT_STATISTIC
static uint32_t trigger_out_cnt = 0;
static uint16_t pulse_end_detect = 0;
static uint16_t one_pulse_count[1024] = {0};
#endif

#define USING_TIM23_FOR_TRIGGER_TIME_COMPENSATION
#ifdef USING_TIM23_FOR_TRIGGER_TIME_COMPENSATION
#include "tim.h"
/**
  * @brief This function handles TIM23 global interrupt.
  */
void TIM23_IRQHandler(void)
{
  /* USER CODE BEGIN TIM23_IRQn 0 */

  /* USER CODE END TIM23_IRQn 0 */
  HAL_TIM_IRQHandler(&htim23);
  /* USER CODE BEGIN TIM23_IRQn 1 */

  /* USER CODE END TIM23_IRQn 1 */
}
static uint64_t tim23_timestamp = 0;
static void tim23_period_elapsed_callback(TIM_HandleTypeDef *htim)
{
    __disable_irq();
    tim23_timestamp += __HAL_TIM_GET_AUTORELOAD(&htim23) + 1;
    __enable_irq();
}
static int8_t trigger_time_compensation_init(void)
{
    MX_TIM23_Init();

    HAL_StatusTypeDef status = HAL_TIM_RegisterCallback(&htim23, HAL_TIM_PERIOD_ELAPSED_CB_ID, tim23_period_elapsed_callback);
    if (status != HAL_OK)
    {
        printf("tim23 register callback err: %d\r\n", status);
        return -1;
    }

    status = HAL_TIM_Base_Start_IT(&htim23);
    if (status != HAL_OK)
    {
        printf("tim23 start err: %d\r\n", status);
        return -2;
    }

    return 0;
}
static uint64_t trigger_timestamp_us_get(void)
{
    return (tim23_timestamp + __HAL_TIM_GET_COUNTER(&htim23));
}
#endif

#define USING_TIM5_FOR_RADIATION_TIMEOUT
#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
#include "tim.h"
#define TIM_DELAY_PULSE_INTERVAL            (1 << 0)
#define TIM_DELAY_NO_PULSE_INTERVAL         (1 << 1)
#define TIM_DELAY_DUMMY_START               (1 << 2)
#define TIM_DELAY_PULSE_LEVEL_RESET         (1 << 3)
#define TIM_DELAY_RUNNING_FLAG_BIT_7        (1 << 7)
#define NORMAL_PULSE_RESET_DELAY_TIME_US    (2000)
#define DUMMY_START_DELAY_TIME_US           (500000)
static uint8_t timer_delay_flag = 0;
static osEventFlagsId_t timer_delay_event = NULL;
static uint64_t timestamp_start = 0, timestamp_mid = 0, timestamp_end = 0;
static void PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    HAL_StatusTypeDef status = HAL_TIM_Base_Stop_IT(htim);
    if (status != HAL_OK)
    {
        LOG_E("HAL_TIM_Base_Stop_IT err: %d\r\n", status);
    }

    if ((timer_delay_flag & ~TIM_DELAY_RUNNING_FLAG_BIT_7) == TIM_DELAY_PULSE_INTERVAL)
    {
        dose_trigger_out_set(0);
        timestamp_start = trigger_timestamp_us_get();
    }
    else if ((timer_delay_flag & ~TIM_DELAY_RUNNING_FLAG_BIT_7) == TIM_DELAY_PULSE_LEVEL_RESET)
    {
        dose_trigger_out_set(1);
        timestamp_mid = trigger_timestamp_us_get();
    }

    osEventFlagsSet(timer_delay_event, timer_delay_flag & ~TIM_DELAY_RUNNING_FLAG_BIT_7);
}
static int8_t timer_delay_start(uint8_t type, uint32_t timeout_us)
{
    if (timer_delay_flag & TIM_DELAY_RUNNING_FLAG_BIT_7)
    {
        LOG_I("timer delay is running\r\n");
        return 0;
    }

    timer_delay_flag = TIM_DELAY_RUNNING_FLAG_BIT_7 | type;

    __HAL_TIM_SET_AUTORELOAD(&htim5, timeout_us);

    HAL_StatusTypeDef status = HAL_TIM_Base_Start_IT(&htim5);
    if (status != HAL_OK)
    {
        LOG_E("HAL_TIM_Base_Start_IT err: %d\r\n", status);
        return -1;
    }

    return 0;
}

static int8_t timer_delay_stop(void)
{
    HAL_StatusTypeDef status = HAL_TIM_Base_Stop_IT(&htim5);
    if (status != HAL_OK)
    {
        LOG_E("HAL_TIM_Base_Stop_IT err: %d\r\n", status);
        return -1;
    }

    timer_delay_flag = 0;

    return 0;
}

static enum deliver_type plan_data_deliver_type_get(void);
static int8_t dose_interpolation_check(uint8_t *time_delay_type, uint32_t *interval_us, uint32_t time_compensation_us);
static int8_t time_delay_entry(void *argument)
{
    int8_t ret = 0;
    uint8_t type = 0;
    uint32_t event_flag = 0, interval_us = 0, time_diff = 0;
    enum fsm_state fsm_state_cur = FSM_STATE_MAX;
    uint8_t trigger_out_flag = 0;
    enum dose_board board_id = DOSE_BOARD_MAX;

    MX_TIM5_Init();
    HAL_StatusTypeDef status = HAL_TIM_RegisterCallback(&htim5, HAL_TIM_PERIOD_ELAPSED_CB_ID, PeriodElapsedCallback);
    if (status != HAL_OK)
    {
        LOG_E("HAL_TIM_RegisterCallback err: %d\r\n", status);
        return -1;
    }

    for (;;)
    {
        event_flag = osEventFlagsWait(timer_delay_event, TIM_DELAY_PULSE_INTERVAL | TIM_DELAY_NO_PULSE_INTERVAL | TIM_DELAY_DUMMY_START | TIM_DELAY_PULSE_LEVEL_RESET, osFlagsWaitAny, osWaitForever);
        timer_delay_flag = 0;
        fsm_state_cur = fsm_state_get();
        trigger_out_flag = trigger_out_info_get(TRIGGER_OUT_FLAG);
        board_id = radiation_data_value_get(DOSE_BOARD_ID);
        interval_us = trigger_out_info_get(TRIGGER_OUT_INTERVAL);

        if (event_flag & TIM_DELAY_PULSE_INTERVAL)
        {
            if (fsm_state_cur == FSM_STATE_WORK && trigger_out_flag == 1)
            {
                ret = dose_value_status_update(ONE_PULSE_START, 0, 0, 0);
                if (ret != 0)
                {
                    LOG_E("dose value status update err: %d\r\n", ret);
                }
                if (board_id == DOSE_BOARD_TRIGGER_OUT)
                {
                    // ret = dose_trigger_out_set(0);
                    // if (ret != 0)
                    // {
                    //     LOG_E("dose_trigger_out_set err: %d\r\n", ret);
                    // }

                    ret = trigger_out_info_set(TRIGGER_OUT_COUNT, trigger_out_info_get(TRIGGER_OUT_COUNT) + 1);
                    if (ret != 0)
                    {
                        LOG_E("trigger out info set err: %d\r\n", ret);
                    }

#ifdef DETECT_RADIATION_TIME_FROM_TRIGGER_OUT
                    system_time_get(&begin_time);
#endif

#ifdef RADIATION_SIMULATION_MODE
                    if (radiation_simulation_trigger_out_flag != 0)
                    {
                        LOG_E("trigger out error\r\n");
                    }
                    radiation_simulation_trigger_out_flag = 1;
#endif
                }

                time_diff = trigger_timestamp_us_get() - timestamp_start;
                if (NORMAL_PULSE_RESET_DELAY_TIME_US - time_diff < 0)
                {
                    LOG_E("pulse interval time diff err: %d\r\n", NORMAL_PULSE_RESET_DELAY_TIME_US - time_diff);
                }
                /* reset trigger out level */
                ret = timer_delay_start(TIM_DELAY_PULSE_LEVEL_RESET, NORMAL_PULSE_RESET_DELAY_TIME_US - time_diff);
                if (ret != 0)
                {
                    LOG_E("timer delay start err: %d\r\n", ret);
                }
            }
            else if ((fsm_state_cur == FSM_STATE_INTERRUPT || fsm_state_cur == FSM_STATE_READY) && trigger_out_flag == 1)
            {
                ret = timer_delay_start(TIM_DELAY_NO_PULSE_INTERVAL, trigger_out_info_get(TRIGGER_OUT_INTERVAL));
                if (ret != 0)
                {
                    LOG_E("timer delay start err: %d\r\n", ret);
                }
            }
        }
        else if (event_flag & TIM_DELAY_PULSE_LEVEL_RESET)
        {
#ifdef RADIATION_FIX_RATE_SIMULATE
            ret = adcs7476_value_dose(100000, 100000, 100);
            ret = dose_interpolation_check(&type, &interval_us, NORMAL_PULSE_RESET_DELAY_TIME_US);
#endif
            /* prepare for next pulse */
            if (fsm_state_cur == FSM_STATE_WORK && trigger_out_flag == 1)
            {
#ifdef RADIATION_FIX_RATE_SIMULATE
                // uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) + dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1);
                // uint64_t dose_interpolated = radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX);
                // LOG_I("dose_accumulated_cur: %llu, dose_interpolated: %llu\r\n", dose_accumulated_cur, dose_interpolated);
#endif
                if (board_id == DOSE_BOARD_TRIGGER_OUT)
                {
                    // ret = dose_trigger_out_set(1);
                    // if (ret != 0)
                    // {
                    //     LOG_E("dose_trigger_out_set err: %d\r\n", ret);
                    // }
                }

                time_diff = trigger_timestamp_us_get() - timestamp_mid;

                if (radiation_data_value_get(PULSE_GENERATION_MODE) == 0)    /* PRF */
                {
                    type = TIM_DELAY_PULSE_INTERVAL;
                }
                else
                {
                    if (board_id == DOSE_BOARD_TRIGGER_OUT)
                    {
                        ret = dose_interpolation_check(&type, &interval_us, NORMAL_PULSE_RESET_DELAY_TIME_US);
                        if (ret != 0)
                        {
                            LOG_E("dose interpolation check err: %d\r\n", ret);
                        }
// #ifdef RADIATION_SIMULATION_MODE
//                         enum deliver_type deliver_type = plan_data_deliver_type_get();
//                         if (deliver_type == DELIVER_TYPE_SWIMRT || deliver_type == DELIVER_TYPE_CRT)
//                         {
//                             type = TIM_DELAY_PULSE_INTERVAL;
//                         }
// #endif
                    }
                }
                // type == 1 ? LOG_I("next: trigger a new pulse\r\n") : LOG_I("next: trigger no pulse\r\n");
                // ret = trigger_out_info_set(TRIGGER_OUT_TYPE, type);
                // if (ret != 0)
                // {
                //     LOG_E("trigger out info set err: %d\r\n", ret);
                // }

                // time_diff = trigger_timestamp_us_get() - timestamp_mid;
                if (interval_us - NORMAL_PULSE_RESET_DELAY_TIME_US - time_diff < 0)
                {
                    LOG_E("pulse reset level time diff error: %d\r\n", interval_us - NORMAL_PULSE_RESET_DELAY_TIME_US - time_diff);
                }
                ret = timer_delay_start(type, interval_us - NORMAL_PULSE_RESET_DELAY_TIME_US - time_diff);
                if (ret != 0)
                {
                    LOG_E("timer delay start err: %d\r\n", ret);
                }
            }
            else if ((fsm_state_cur == FSM_STATE_INTERRUPT || fsm_state_cur == FSM_STATE_READY) && trigger_out_flag == 1)
            {
                ret = timer_delay_start(TIM_DELAY_NO_PULSE_INTERVAL, trigger_out_info_get(TRIGGER_OUT_INTERVAL));
                if (ret != 0)
                {
                    LOG_E("timer delay start err: %d\r\n", ret);
                }
            }
        }
        else if (event_flag & TIM_DELAY_NO_PULSE_INTERVAL)
        {
            /* prepare for next pulse */
            if (fsm_state_cur == FSM_STATE_WORK && trigger_out_flag == 1)
            {
#ifdef RADIATION_FIX_RATE_SIMULATE
                // uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) + dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1);
                // uint64_t dose_interpolated = radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX);
                // LOG_I("dose_accumulated_cur: %llu, dose_interpolated: %llu\r\n", dose_accumulated_cur, dose_interpolated);
                ret = dose_interpolation_check(&type, &interval_us, 0);
#endif
                if (radiation_data_value_get(PULSE_GENERATION_MODE) == 0)    /* PRF */
                {
                    type = TIM_DELAY_PULSE_INTERVAL;
                }
                else
                {
                    if (board_id == DOSE_BOARD_TRIGGER_OUT)
                    {
                        ret = dose_interpolation_check(&type, &interval_us, 0);
                        if (ret != 0)
                        {
                            LOG_E("dose interpolation check err: %d\r\n", ret);
                        }
                    }
                }

                // type == 1 ? LOG_I("next: trigger a new pulse\r\n") : LOG_I("next: trigger no pulse\r\n");
                // ret = trigger_out_info_set(TRIGGER_OUT_TYPE, type);
                // if (ret != 0)
                // {
                //     LOG_E("trigger out info set err: %d\r\n", ret);
                // }

                ret = timer_delay_start(type, interval_us);
                if (ret != 0)
                {
                    LOG_E("timer delay start err: %d\r\n", ret);
                }
            }
            else if ((fsm_state_cur == FSM_STATE_INTERRUPT || fsm_state_cur == FSM_STATE_READY) && trigger_out_flag == 1)
            {
                ret = timer_delay_start(TIM_DELAY_NO_PULSE_INTERVAL, trigger_out_info_get(TRIGGER_OUT_INTERVAL));
                if (ret != 0)
                {
                    LOG_E("timer delay start err: %d\r\n", ret);
                }
            }
        }
        else if (event_flag & TIM_DELAY_DUMMY_START)
        {
            LOG_I("---resume adcs7476 sampling---\r\n");
            ret = adcs7476_sample_enable(1);
            if (ret != 0)
            {
                LOG_E("adcs7476_sample_enable err: %d\r\n", ret);
            }
        }
        else
        {
            /* do nothing */
        }
    }

    return 0;
}
#endif

#define USING_LPTIM3_FOR_RADIATION_TIMEOUT
#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
#include "lptim.h"
static uint8_t lptim3_delay_flag = 0;
static osEventFlagsId_t lptim3_delay_event = NULL;
#define LPTIM3_DELAY_ONE_PULSE_TIMEOUT_US      (1 << 0)
#define LPTIM3_DELAY_RUNNING_FLAG_BIT_7        (1 << 7)
static void AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
    HAL_StatusTypeDef status = HAL_LPTIM_SetOnce_Stop_IT(hlptim);
    if (status != HAL_OK)
    {
        LOG_E("HAL_LPTIM_SetOnce_Stop_IT err: %d\r\n", status);
    }

    osEventFlagsSet(lptim3_delay_event, lptim3_delay_flag & ~LPTIM3_DELAY_RUNNING_FLAG_BIT_7);
}
static int8_t lptim3_delay_start(uint8_t type, uint32_t timeout_us)
{
    if (timeout_us * 1000 / 40 > 0xFFFF)
    {
        LOG_E("timeout_us too large: %d\r\n", timeout_us);
        return -1;
    }

    if (lptim3_delay_flag & LPTIM3_DELAY_RUNNING_FLAG_BIT_7)
    {
        // LOG_I("timer delay is running\r\n");
        return 0;
    }

    lptim3_delay_flag = LPTIM3_DELAY_RUNNING_FLAG_BIT_7 | type;

    HAL_StatusTypeDef status = HAL_LPTIM_SetOnce_Start_IT(&hlptim3, timeout_us * 1000 / 40, timeout_us * 1000 / 40);
    if (status != HAL_OK)
    {
        LOG_E("HAL_LPTIM_SetOnce_Start_IT err: %d\r\n", status);
        return -2;
    }

    return 0;
}
static int8_t lptim3_delay_entry(void *argument)
{
    int8_t ret = 0;
    uint32_t event_flag = 0;

    MX_LPTIM3_Init();
    HAL_StatusTypeDef status = HAL_LPTIM_RegisterCallback(&hlptim3, HAL_LPTIM_AUTORELOAD_MATCH_CB_ID, AutoReloadMatchCallback);
    if (status != HAL_OK)
    {
        LOG_E("HAL_TIM_RegisterCallback err: %d\r\n", status);
        return -1;
    }

    for (;;)
    {
        event_flag = osEventFlagsWait(lptim3_delay_event, LPTIM3_DELAY_ONE_PULSE_TIMEOUT_US, osFlagsWaitAny, osWaitForever);
        if (event_flag & LPTIM3_DELAY_ONE_PULSE_TIMEOUT_US)
        {
            if (dose_value_status_get(ONE_PULSE_COMPLETE, DOSE_CHANNEL_NONE) == 0)
            {
                ret = dose_value_status_update(ONE_PULSE_TIMEOUT, 0, 0, 0);
                if (ret != 0)
                {
                    LOG_E("dose value status update err: %d\r\n", ret);
                }
                LOG_I("one pulse timeout\r\n");
            }
        }

        lptim3_delay_flag = 0;
    }
    return 0;
}
#endif
/*****************************************************************/
#define ADCS7476_SERVO_VALUE    50
#define ADCS7476_VALUE_ACCUMULATE_LIMIT 100
#define ADCS7476_VALUE_MAX_LIMIT    4096
static uint16_t adcs7476_value_check(uint16_t *buf, uint16_t len, uint32_t *pulse_val, uint32_t *servo_val)
{
    uint16_t valid_cnt = 0;
    int8_t ret = 0;

    for (uint16_t i = 0; i < len; i++)
    {
        if(buf[i] >= ADCS7476_VALUE_MAX_LIMIT)
        {
            LOG_E("adcs7476 value exceed max limit: %d (max: %d)\r\n", buf[i], ADCS7476_VALUE_MAX_LIMIT);
            continue;
        }

        if (buf[i] >= ADCS7476_VALUE_ACCUMULATE_LIMIT)
        {
            *pulse_val += buf[i] - ADCS7476_SERVO_VALUE;

// #ifdef LPTIM3_DELAY_ONE_PULSE_TIMEOUT_US
//             ret = lptim3_delay_start(LPTIM3_DELAY_ONE_PULSE_TIMEOUT_US, ONE_PULSE_TIMEOUT_US);
//             if (ret != 0)
//             {
//                 LOG_E("lptim3 delay start err: %d\r\n", ret);
//             }
// #else
// #endif
            valid_cnt++;
        }
        else
        {
            *servo_val += buf[i];
        }
    }

    return valid_cnt;
}

static int8_t adcs7476_value_dummy(uint16_t value, uint8_t channel)
{
    int8_t ret = 0;
    struct ltc2632_object *ltc2632 = ltc2632_object_data_get();

    ltc2632->value.bits.data = value;
    ltc2632->value.bits.channel = channel;

    ret = ltc2632_data_write(ltc2632);
    if (ret != 0)
    {
        LOG_E("ltc2632_data_write err: %d\r\n", ret);
        return ret;
    }

    return 0;
}

static int8_t adcs7476_value_restore(void)
{
    int8_t ret = 0;
    struct ltc2632_object *ltc2632 = ltc2632_object_data_get();

#if 1
    ltc2632->value.bits.data = ltc2632->out_a_value;
    ltc2632->value.bits.channel = LTC2632_CHANNEL_OUTA;

    ret = ltc2632_data_write(ltc2632);
    if (ret != 0)
    {
        LOG_E("ltc2632_data_write err: %d\r\n", ret);
        return ret;
    }

    ltc2632->value.bits.data = ltc2632->out_b_value;
    ltc2632->value.bits.channel = LTC2632_CHANNEL_OUTB;

    ret = ltc2632_data_write(ltc2632);
    if (ret != 0)
    {
        LOG_E("ltc2632_data_write err: %d\r\n", ret);
        return ret;
    }
#else
    ltc2632->value.bits.data = 1500;
    ltc2632->value.bits.channel = LTC2632_CHANNEL_ALL;

    ret = ltc2632_data_write(ltc2632);
    if (ret != 0)
    {
        LOG_E("ltc2632_data_write err: %d\r\n", ret);
        return ret;
    }
#endif

    return 0;
}

static int8_t adcs7476_servo_state_set(uint8_t channel, uint8_t value)
{
    struct ltc2632_object *ltc2632 = ltc2632_object_data_get();

    switch (channel)
    {
    case LTC2632_CHANNEL_OUTA:
        ltc2632->a_servo_state = value;
        break;
    case LTC2632_CHANNEL_OUTB:
        ltc2632->b_servo_state = value;
        break;
    case LTC2632_CHANNEL_ALL:
        break;
    default:
        LOG_E("channel invalid: %d\r\n", channel);
        return -1;
        break;
    }

    return 0;
}
static int8_t adcs7476_servo_state_get(uint8_t channel)
{
    struct ltc2632_object *ltc2632 = ltc2632_object_data_get();

    switch (channel)
    {
    case LTC2632_CHANNEL_OUTA:
        return ltc2632->a_servo_state;
    case LTC2632_CHANNEL_OUTB:
        return ltc2632->b_servo_state;
    case LTC2632_CHANNEL_ALL:
        return ltc2632->a_servo_state | ltc2632->b_servo_state << 1;
    default:
        LOG_E("channel invalid: %d\r\n", channel);
        return -1;
        break;
    }

    return 0;
}

static int8_t adcs7476_value_servo(uint32_t value, uint16_t len, uint8_t channel)
{
    int8_t ret = 0, flag = 0;
    uint16_t avg = value / len;
    uint16_t pre_val = 0;
    struct ltc2632_object *ltc2632 = ltc2632_object_data_get();

    switch (channel)
    {
    case LTC2632_CHANNEL_OUTA:
        pre_val = ltc2632->out_a_value;
        avg < ADCS7476_SERVO_VALUE ? ltc2632->out_a_value++ : avg > ADCS7476_SERVO_VALUE ? ltc2632->out_a_value-- : 0;
        ltc2632->value.bits.data = ltc2632->out_a_value;
        ltc2632->value.bits.channel = LTC2632_CHANNEL_OUTA;
        break;
    case LTC2632_CHANNEL_OUTB:
        pre_val = ltc2632->out_b_value;
        avg < ADCS7476_SERVO_VALUE ? ltc2632->out_b_value++ : avg > ADCS7476_SERVO_VALUE ? ltc2632->out_b_value-- : 0;
        ltc2632->value.bits.data = ltc2632->out_b_value;
        ltc2632->value.bits.channel = LTC2632_CHANNEL_OUTB;
        break;
    case LTC2632_CHANNEL_ALL:
        break;
    default:
        LOG_E("channel invalid: %d\r\n", channel);
        return -1;
        break;
    }

#if 0
    LOG_I("channel: %d, value: %d, avg: %d\r\n", channel, value, avg);
    LOG_I("out_a_value: %d, out_b_value: %d\r\n", ltc2632->out_a_value, ltc2632->out_b_value);
#endif

    if (pre_val != ltc2632->value.bits.data)
    {
        ret = ltc2632_data_write(ltc2632);
        if (ret != 0)
        {
            LOG_E("ltc2632_data_write err: %d\r\n", ret);
            return ret;
        }
    }

    flag = abs(pre_val - ltc2632->value.bits.data) > 2 ? 1 : 0;

    return adcs7476_servo_state_set(channel, flag);
}

static int8_t adcs7476_value_dose(uint32_t value, uint32_t value_1, uint16_t pulse_cnt)
{
    enum fsm_state fsm_stat = fsm_state_get();

    if (fsm_stat != FSM_STATE_WORK && fsm_stat != FSM_STATE_PRELIMINARY_BEGIN)
    {
        return dose_value_status_update(ONE_PULSE_CLEAR, 0, 0, 0);
    }

    /* check dose symmetry */
    uint32_t value_diff = abs(value - value_1);
    uint32_t value_max = (value > value_1) ? value : value_1;
    uint8_t percentage = radiation_data_value_get(PULSE_SYMMETRY);

    if (value_diff > value_max * percentage / 100)
    {
        // LOG_I("dose symmetry fault, value_diff: %d, value_max: %d, percentage: %d\r\n", value_diff, value_max, percentage);
        // LOG_E("dose symmetry fault, ch1: %d, ch2: %d, percentage: %d\r\n", value, value_1, percentage);
        int8_t ret = interlock_fault_info_set(INTERLOCK_FAULT_DOSE_SYMMETRY_FAULT, 1);
        if (ret != 0)
        {
            LOG_E("interlock fault info set err: %d\r\n", ret);
        }
    }


    return dose_value_status_update(ONE_PULSE_RUNNING, pulse_cnt, value, value_1);
}

/* used for qam */
static uint32_t trigger_interval_calculate_ms(void)
{
    int8_t ret = 0;
    int32_t trigger_interval = 0;
    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) + dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1);
    uint64_t dose_radiation_index = radiation_data_value_get(DOSE_RADIATION_IDX);

    if (dose_accumulated_cur < dose_radiation_index)
    {
        /* not reach radiation index dose yet, so judge whether reached interpolated dose */
        uint32_t pulse_interval = radiation_data_value_get(PULSE_INTERVAL);
        uint64_t dose_rate_interpolated = radiation_data_value_get(DOSE_RATE_INTERPOLATED_RADIATION_IDX);
        uint64_t dose_interpolated = radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX);

        trigger_interval = pulse_interval;

        if (dose_accumulated_cur < dose_interpolated)
        {

        }
        else
        {
            /* no need to generate a new pulse, calculate next interpolated dose which need to generate a new pulse */
#if 0
            LOG_I("dose_radiation_index: %llu\r\n", dose_radiation_index);
            LOG_I("dose_accumulated_cur: %llu\r\n", dose_accumulated_cur);
            LOG_I("dose_interpolated: %llu\r\n", dose_interpolated);
            LOG_I("dose_rate_interpolated: %llu\r\n", dose_rate_interpolated);
            LOG_I("pulse_interval: %u\r\n", pulse_interval);
#endif
            while (1)
            {
                dose_interpolated += dose_rate_interpolated * pulse_interval;
                dose_interpolated = (dose_interpolated <= dose_radiation_index) ? dose_interpolated : dose_radiation_index;

                if (dose_accumulated_cur >= dose_interpolated)
                {
                    trigger_interval += pulse_interval;
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {
        /* time left in current radiation index */
        uint32_t time_current_ms = osKernelGetTickCount() * 1000 / osKernelGetTickFreq();
        trigger_interval = (radiation_data_value_get(TIME_RADIATION_IDX_END) + UINT32_MAX - time_current_ms) % UINT32_MAX;

        trigger_interval = trigger_interval < 0 ? 0 : trigger_interval;
    }

    // LOG_I("trigger_interval: %d ms\r\n", trigger_interval);

    return trigger_interval;
}
enum real_time_data_type
{
    REAL_TIME_DATA_TYPE_RADIATION = 0,
    REAL_TIME_DATA_TYPE_RI,
    REAL_TIME_DATA_TYPE_QAM,
};
static int8_t dose_data_upload(enum real_time_data_type type)
{
    int8_t ret = 0;
    uint8_t buf[64] = {0};
    uint16_t offset = 0;

    buf[offset++] = 0x01;
    buf[offset++] = type;

    switch (type)
    {
    case REAL_TIME_DATA_TYPE_RADIATION:
        {
            uint16_t len = 0;

            buf[offset++] = 0x01;
            uint32_t trigger_interval = 0;//fsm_state_get() == FSM_STATE_WORK ? trigger_interval_calculate_ms() : 0;    /* TODO: 影响VMAT过程 */
            ret = radiation_status_get(&buf[offset], &len, trigger_interval);
            if (ret != 0)
            {
                LOG_E("radiation status get err: %d\r\n", ret);
                return ret;
            }
            offset += len;
        }
        break;
    case REAL_TIME_DATA_TYPE_RI:
        {
            uint16_t ri = radiation_data_value_get(DOSE_RADIATION_IDX_CURRENT);
            memcpy(&buf[offset], &ri, sizeof(uint16_t));
            offset += sizeof(uint16_t);
        }
        break;
    case REAL_TIME_DATA_TYPE_QAM:
        {
            uint64_t factor = radiation_data_value_get(DOSE_CALIBRATION_FACTOR);
            uint32_t factor_0 = factor, factor_1 = factor >> 32;

            uint64_t dose_accumulated_target = radiation_data_value_get(DOSE_BEAM_METER);
            float dose_total = (double)dose_accumulated_target * 2.0f / (factor_0 + factor_1);
            float dose_cumulated = (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) / factor_0 + (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1) / factor_1;
            uint32_t trigger_interval_ms = trigger_interval_calculate_ms();
            uint64_t timestamp = timestamp_ns_get();

            memcpy(&buf[offset], &dose_total, sizeof(float));
            offset += sizeof(float);
            memcpy(&buf[offset], &dose_cumulated, sizeof(float));
            offset += sizeof(float);
            memcpy(&buf[offset], &trigger_interval_ms, sizeof(uint32_t));
            offset += sizeof(uint32_t);
            memcpy(&buf[offset], &timestamp, sizeof(uint64_t));
            offset += sizeof(uint64_t);
        }
        break;
    default:
        LOG_E("invalid data type: %d\r\n", type);
        return -1;
        break;
    }

    struct dose_object cmd = {0};
    cmd.id.bits.cmd_id = DOSE_UART_ID;
    cmd.id.bits.cmd_ack = 1;
    cmd.type = 0x04;
    cmd.len = &offset;
    cmd.data = buf;
    ret = dose_uart_cmd_write(&cmd);
    if (ret != 0)
    {
        LOG_E("dose uart cmd write err: %d\r\n", ret);
        return ret;
    }

    return 0;
}
static int8_t dose_data_upload_once(enum fsm_state state)
{
    int8_t ret = 0;
    static uint8_t state_flag = 0;
    uint8_t state_flag_cur = 0;

    switch (state)
    {
    case FSM_STATE_PRELIMINARY_BEGIN:
        state_flag_cur = 1;
        break;
    case FSM_STATE_PREPARE:
        state_flag_cur = 2;
        break;
    case FSM_STATE_READY:
    case FSM_STATE_COMPLETE:
    case FSM_STATE_INTERRUPT:
    case FSM_STATE_TERMINATE:
        state_flag_cur = 3;
        break;
    default:
        state_flag_cur = 0xFF;
        break;
    }

    if (state_flag != state_flag_cur)
    {
        state_flag = state_flag_cur;

        if (state_flag_cur == 1 || state_flag_cur == 2)
        {
            /* clear dose meter value */
            ret = dose_value_status_set(DOSE_ACCUMULATED, 0, 0);
            if (ret != 0)
            {
                LOG_E("dose accumulated set err: %d\r\n", ret);
                return ret;
            }

            /* clear radiation index */
            ret = radiation_data_value_set(DOSE_RADIATION_IDX_CURRENT, 0);
            if (ret != 0)
            {
                LOG_E("radiation data value set err: %d\r\n", ret);
                return ret;
            }
        }

        /* clear dose rate */
        ret = dose_value_status_set(DOSE_RATE_CURRENT, 0, 0);
        if (ret != 0)
        {
            LOG_E("dose rate set err: %d\r\n", ret);
            return ret;
        }

        /* clear prf */
        ret = dose_value_status_set(PRF_CURRENT, 0, 0);
        if (ret != 0)
        {
            LOG_E("prf set err: %d\r\n", ret);
            return ret;
        }

        // ret = dose_data_upload(REAL_TIME_DATA_TYPE_RADIATION);
        // ret |= dose_data_upload(REAL_TIME_DATA_TYPE_QAM);
        if (ret != 0)
        {
            LOG_E("dose data upload err: %d\r\n", ret);
            return ret;
        }
    }

    return 0;
}

static int8_t dose_accumulated_check(uint16_t pulse_cnt, uint16_t len)
{
    int8_t ret = 0;
    uint8_t dose_mode = radiation_data_value_get(DOSE_GENERATION_MODE);
    uint16_t one_pulse_cnt = dose_value_status_get(ONE_PULSE_COUNT, DOSE_CHANNEL_NONE);

    switch (dose_mode)
    {
    case 0: /* dummy */
        break;
    case 1: /* normal */
        if (one_pulse_cnt < ONE_PULSE_TIMEOUT_US && pulse_cnt >= len)
        {
            return 0;
        }

#ifdef TRIGGER_OUT_STATISTIC
        /* TODO: 需要测试下 one_pulse_cnt 值在什么范围，实测大部分情况下在180~190左右 */
        LOG_I("[%d]one_pulse_cnt: %d\r\n", pulse_end_detect, one_pulse_cnt);
        one_pulse_count[pulse_end_detect % 1024] = one_pulse_cnt;
        pulse_end_detect++;
#endif

#ifdef RADIATION_SIMULATION_MODE
        radiation_simulation_trigger_out_flag = 0;
#endif

        if (fsm_state_get() == FSM_STATE_WORK)
        {
            /* 1. update one pulse status */
            ret = dose_value_status_update(ONE_PULSE_COMPLETE, 0, 0, 0);
            if (ret != 0)
            {
                LOG_E("dose value status update err: %d\r\n", ret);
                return ret;
            }
            // LOG_I("one pulse complete, pulse dose: %llu, %llu\r\n", dose_value_status_get(ONE_PULSE_DOSE, DOSE_CHANNEL_0), dose_value_status_get(ONE_PULSE_DOSE, DOSE_CHANNEL_1));

            /* 2. upload data -> arm io -> afc */
#ifdef RADIATION_FIX_RATE_SIMULATE
            static uint32_t cnt = 0;
            if (cnt++ % 10 == 0)
#endif
            {
                ret = dose_data_upload(REAL_TIME_DATA_TYPE_RADIATION);
                if (ret != 0)
                {
                    LOG_E("dose radiation data upload err: %d\r\n", ret);
                    return ret;
                }

                // /* 3. update trigger interval for qam */
                // if (radiation_data_value_get(DOSE_BOARD_ID) == DOSE_BOARD_TRIGGER_OUT)
                // {
                //     ret = dose_data_upload(REAL_TIME_DATA_TYPE_QAM);
                //     if (ret != 0)
                //     {
                //         LOG_E("dose data upload err: %d\r\n", ret);
                //         return ret;
                //     }
                // }
            }
        }
        break;
    default:
        ret = -1;
        break;
    }

    /* 3. check total dose */
    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) + dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1);
    uint64_t dose_accumulated_target = radiation_data_value_get(DOSE_BEAM_METER);

    uint64_t board_id = radiation_data_value_get(DOSE_BOARD_ID);

    if (board_id == DOSE_BOARD_NO_TRIGGER_OUT)
    {
        dose_accumulated_target *= DOSE_BOARD_NO_TRIGGER_OUT_SCALE;
    }

#if 0
    static uint32_t cnt = 0;
#ifndef RADIATION_SIMULATION_MODE
    if (cnt++ % 100 == 0)
#else
    if (radiation_simulation_trigger_out_flag == 0)
#endif
    {
        LOG_I("dose accumulated: %llu, target: %llu\r\n", dose_accumulated_cur, dose_accumulated_target);
    }
#endif

    if (dose_accumulated_cur >= dose_accumulated_target)
    {
        LOG_I("end dose accumulated: %llu, target: %llu\r\n", dose_accumulated_cur, dose_accumulated_target);

        ret = trigger_out_info_set(TRIGGER_OUT_FLAG, 0);
        if (ret != 0)
        {
            LOG_E("trigger out info set err: %d\r\n", ret);
            return ret;
        }

        ret = dose_value_status_update(ONE_BEAM_COMPLETE, 0, 0, 0);
        if (ret != 0)
        {
            LOG_E("dose value status update err: %d\r\n", ret);
            return ret;
        }

        if (dose_mode == 0) /* dummy mode */
        {
            ret = adcs7476_value_restore();
            if (ret != 0)
            {
                LOG_E("adcs7476_value_restore err: %d\r\n", ret);
                return ret;
            }

            ret = fsm_state_get() == FSM_STATE_PRELIMINARY_BEGIN ? fsm_state_switch(FSM_STATE_PRELIMINARY) : fsm_state_switch(FSM_STATE_TERMINATE);
        }
        else
        {
            if (board_id == DOSE_BOARD_NO_TRIGGER_OUT)
            {
                // LOG_E("detected dose reached upper limit\r\n");
                ret = interlock_fault_info_set(INTERLOCK_FAULT_DOSE_REACH_UPPER_LIMIT, 1);
                ret |= fsm_state_switch(FSM_STATE_TERMINATE);
            }
            else
            {
                ret = fsm_state_switch(FSM_STATE_COMPLETE);
            }
        }
    }

    return ret;
}

static enum deliver_type plan_data_deliver_type_get(void)
{
    return (enum deliver_type)radiation_data_value_get(DOSE_BEAM_DELIVER_TYPE);
}

static int8_t control_point_limit_check(void)
{
    int8_t ret = 0;
    uint16_t cp_cur = radiation_data_value_get(DOSE_CONTROL_POINT_CURRENT);
    uint16_t cp_prev = radiation_data_value_get(DOSE_CONTROL_POINT_PREV_RADIATION_IDX);

    // LOG_I("cp_cur: %d, cp_prev: %d\r\n", cp_cur, cp_prev);

    if (cp_prev + 1 == cp_cur)
    {
        uint64_t factor = radiation_data_value_get(DOSE_CALIBRATION_FACTOR);
        uint32_t factor_0 = factor, factor_1 = factor >> 32;
        float dose_accumulated_cur = (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) / factor_0 + (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1) / factor_1;
        float dose_prev_radiation_index = (double)radiation_data_value_get(DOSE_PREV_RADIATION_IDX) * 2.0f / (factor_0 + factor_1);
        uint16_t dose_limit = radiation_data_value_get(DOSE_LIMIT);
        uint8_t dose_low_percentage = dose_limit & 0xFF;
        uint8_t dose_high_percentage = dose_limit >> 8;

        /* 1. check dose limit */
        if (dose_accumulated_cur - dose_prev_radiation_index * (1 + (float)dose_high_percentage / 100.0f) > 1e-6)
        {
            /* dose high limit reached */
            // LOG_E("dose accumulated reached upper limit, dose_accumulated_cur: %f, dose_prev_radiation_index: %f\r\n", dose_accumulated_cur, dose_prev_radiation_index);
            ret = interlock_fault_info_set(INTERLOCK_FAULT_DOSE_CP_HIGH, 1);
            if (ret != 0)
            {
                LOG_E("interlock fault info set err: %d\r\n", ret);
                return ret;
            }
        }
        else if (dose_accumulated_cur - dose_prev_radiation_index * (1 - (float)dose_low_percentage / 100.0f) < -1e-6)
        {
            /* dose low limit reached */
            // LOG_E("dose accumulated reached lower limit, dose_accumulated_cur: %f, dose_prev_radiation_index: %f\r\n", dose_accumulated_cur, dose_prev_radiation_index);
            ret = interlock_fault_info_set(INTERLOCK_FAULT_DOSE_CP_LOW, 1);
            if (ret != 0)
            {
                LOG_E("interlock fault info set err: %d\r\n", ret);
                return ret;
            }
        }

        /* 2. check dose rate limit */
        // uint16_t dose_rate_limit = radiation_data_value_get(DOSE_RATE_LIMIT);
        // uint8_t dose_rate_low_percentage = dose_rate_limit & 0xFF;
        // uint8_t dose_rate_high_percentage = dose_rate_limit >> 8;
    }
    else
    {
        /* do nothing */
    }

    return ret;
}

static int8_t dose_interpolation_calculate(void)
{
    int8_t ret = 0;

    uint64_t dose_radiation_index = radiation_data_value_get(DOSE_RADIATION_IDX);
    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) + dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1);
    uint32_t time_radiation_index = radiation_data_value_get(DOSE_TIME_RADIATION_IDX);
    uint64_t dose_rate_radiation_index = radiation_data_value_get(DOSE_RATE_RADIATION_IDX);
    uint16_t pulse_interval_min = radiation_data_value_get(PULSE_INTERVAL_MIN);

    float dose_rate_interpolated = 0;
    uint64_t dose_interpolated = 0;

    /* record timestamp */
    uint32_t time_current_ms = osKernelGetTickCount() * 1000 / osKernelGetTickFreq();
    ret = radiation_data_value_set(TIME_RADIATION_IDX_BEGIN, time_current_ms);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }
    ret = radiation_data_value_set(TIME_RADIATION_IDX_END, time_current_ms + time_radiation_index);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }

#if 0
    LOG_I("current ri: %llu\r\n", radiation_data_value_get(DOSE_RADIATION_IDX_CURRENT));
    LOG_I("dose_radiation_index: %llu\r\n", dose_radiation_index);
    LOG_I("dose_accumulated_cur: %llu\r\n", dose_accumulated_cur);
    LOG_I("time_radiation_index: %u ms\r\n", time_radiation_index);
    LOG_I("dose_rate_radiation_index: %llu\r\n", dose_rate_radiation_index);
    LOG_I("pulse_interval_min: %u us\r\n", pulse_interval_min);
#endif

    time_radiation_index = time_radiation_index * 1000;   /* convert to us */
    if (dose_radiation_index == 0)  /* first radiation index */
    {
        dose_rate_interpolated = (double)dose_rate_radiation_index / 1e6 / 60.0f;
        ret = radiation_data_value_set(DOSE_RATE_INTERPOLATED_RADIATION_IDX, dose_rate_interpolated);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
    }
    if (dose_accumulated_cur < dose_radiation_index)
    {
        if (time_radiation_index == 0)  /* last radiation index, but not enter here */
        {
            dose_rate_interpolated = radiation_data_value_get(DOSE_RATE_INTERPOLATED_RADIATION_IDX);
        }
        else
        {
            dose_rate_interpolated = (double)(dose_radiation_index - dose_accumulated_cur) / time_radiation_index;
        }

        dose_interpolated = dose_accumulated_cur + dose_rate_interpolated * pulse_interval_min;
        ret = radiation_data_value_set(DOSE_RATE_INTERPOLATED_RADIATION_IDX, dose_rate_interpolated);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
        ret = radiation_data_value_set(DOSE_INTERPOLATED_RADIATION_IDX, dose_interpolated);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
        ret = radiation_data_value_set(PULSE_INTERVAL, pulse_interval_min);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
    }

#if 0
    LOG_I("dose_rate_interpolated: %f\r\n", dose_rate_interpolated);
    LOG_I("dose_interpolated: %llu\r\n", dose_interpolated);
#endif

    return 0;
}

static int8_t dose_radiation_index_update(uint32_t time_excess_ms)
{
    int8_t ret = 0;
    enum deliver_type deliver_type = plan_data_deliver_type_get();

    if (fsm_state_get() == FSM_STATE_WORK && (deliver_type == DELIVER_TYPE_SWIMRT || deliver_type == DELIVER_TYPE_CRT))
    {
        /* 1. update radiation index、control point and upload to arm io */
        ret = radiation_data_value_set(DOSE_RADIATION_IDX_CURRENT, radiation_data_value_get(DOSE_RADIATION_IDX_CURRENT) + 1);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
        ret = radiation_data_value_set(DOSE_CONTROL_POINT_UPDATE, 0);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }

        if (radiation_data_value_get(DOSE_BOARD_ID) == DOSE_BOARD_TRIGGER_OUT)
        {
            ret = dose_data_upload(REAL_TIME_DATA_TYPE_RI);
            if (ret != 0)
            {
                LOG_E("dose radiation data upload err: %d\r\n", ret);
                return ret;
            }
        }
    }

    /* 2. interpolation calculate */
    ret = dose_interpolation_calculate();
    if (ret != 0)
    {
        LOG_E("dose interpolation calculate err: %d\r\n", ret);
        return ret;
    }

    /* 3. update radiation index end time and record current time */
    // ret = radiation_data_value_set(TIME_RADIATION_IDX_BEGIN, 0);
    // if (ret != 0)
    // {
    //     LOG_E("radiation data value set err: %d\r\n", ret);
    //     return ret;
    // }
    // ret = radiation_data_value_set(TIME_RADIATION_IDX_END, radiation_data_value_get(DOSE_TIME_RADIATION_IDX));
    // if (ret != 0)
    // {
    //     LOG_E("radiation data value set err: %d\r\n", ret);
    //     return ret;
    // }
    ret = radiation_data_value_set(TIME_RADIATION_IDX_ELAPSE, time_excess_ms/*  + radiation_data_value_get(PULSE_INTERVAL_MIN) / 1000 */);    /* ms */
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }

    /* 4. check cp limit */
    ret = control_point_limit_check();
    if (ret != 0)
    {
        LOG_E("dose cp limit check err: %d\r\n", ret);
        return ret;
    }

    return ret;
}

static float dose_rate_average_calculate(float dose_cur, uint32_t time_interval_ms)
{
    #define DOSE_RATE_AVERAGE_COUNT 10
    static uint8_t count = 0;
    static float dose_diff_array[DOSE_RATE_AVERAGE_COUNT + 1] = {0}, dose_pre = 0;

    dose_pre = dose_cur < 1e-6 ? dose_cur : dose_pre;
    dose_diff_array[count++] = dose_cur - dose_pre;
    dose_pre = dose_cur;
    count %= DOSE_RATE_AVERAGE_COUNT;
    dose_diff_array[DOSE_RATE_AVERAGE_COUNT] = 0;
    for (uint8_t i = 0; i < DOSE_RATE_AVERAGE_COUNT; i++)
    {
        dose_diff_array[DOSE_RATE_AVERAGE_COUNT] += dose_diff_array[i];
    }
    dose_diff_array[DOSE_RATE_AVERAGE_COUNT] /= DOSE_RATE_AVERAGE_COUNT;

    return (dose_diff_array[DOSE_RATE_AVERAGE_COUNT] * 1000.0f * 60.0f / time_interval_ms);
}

static int8_t dose_rate_calculate_and_check(uint64_t *dose_accumulated, uint64_t *dose_target, uint32_t *time_expected_ms, uint32_t *time_elapse_ms)
{
    int8_t ret = 0;
    uint64_t factor = radiation_data_value_get(DOSE_CALIBRATION_FACTOR);
    uint32_t factor_0 = factor, factor_1 = factor >> 32;
    float dose_prev_radiation_index = (double)radiation_data_value_get(DOSE_PREV_RADIATION_IDX) * 2.0f / (factor_0 + factor_1);
    float dose_rate_radiation_index = (double)radiation_data_value_get(DOSE_RATE_RADIATION_IDX) * 2.0f / (factor_0 + factor_1);
    float dose_cur = 0, dose_rate_cur = 0;

#if 0
    /* 1. calculate dose rate */
    if (*dose_accumulated < *dose_target)
    {
        dose_cur = (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) / factor_0 + (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1) / factor_1;
        dose_rate_cur = (dose_cur - dose_prev_radiation_index) * 1000.0f * 60.0f / *time_elapse_ms;
    }
    else
    {
        if (*time_elapse_ms <= *time_expected_ms)
        {
            dose_rate_cur = dose_rate_radiation_index;
        }
        else
        {
            dose_rate_cur = dose_rate_radiation_index * *time_expected_ms / *time_elapse_ms;
        }
    }
#else
    // dose_cur = (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) / factor_0 + (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1) / factor_1;
    // dose_rate_cur = dose_rate_average_calculate(dose_cur, *time_elapse_ms);
#endif

    // ret = dose_value_status_set(DOSE_RATE_CURRENT, dose_rate_cur * 100.0f, 0);
    // if (ret != 0)
    // {
    //     LOG_E("dose rate set err: %d\r\n", ret);
    // }

    dose_rate_cur = dose_value_status_get(DOSE_RATE_CURRENT, DOSE_CHANNEL_NONE) / 100.0f;

    /* 2. check dose rate limit */
    uint16_t dose_rate_limit = radiation_data_value_get(DOSE_RATE_LIMIT);
    uint8_t dose_rate_low_percentage = dose_rate_limit & 0xFF;
    uint8_t dose_rate_high_percentage = dose_rate_limit >> 8;

    if (dose_rate_cur - dose_rate_radiation_index * (1 - (float)dose_rate_low_percentage / 100.0f) < -1e-6)
    {
        // LOG_E("dose rate low, current: %f  target: %f\r\n", dose_rate_cur, dose_rate_radiation_index);
        ret = interlock_fault_info_set(INTERLOCK_FAULT_DOSE_RATE_LOW, 1);
        if (ret != 0)
        {
            LOG_E("interlock fault info set err: %d\r\n", ret);
            return ret;
        }
    }
    else if (dose_rate_cur - dose_rate_radiation_index * (1 + (float)dose_rate_high_percentage / 100.0f) > 1e-6)
    {
        // LOG_E("dose rate high, current: %f  target: %f\r\n", dose_rate_cur, dose_rate_radiation_index);
        ret = interlock_fault_info_set(INTERLOCK_FAULT_DOSE_RATE_HIGH, 1);
        if (ret != 0)
        {
            LOG_E("interlock fault info set err: %d\r\n", ret);
            return ret;
        }
    }

    return 0;
}

static int8_t dose_interpolation_check(uint8_t *time_delay_type, uint32_t *interval_us, uint32_t time_compensation_us)
{
    int8_t ret = 0;
    enum deliver_type deliver_type = plan_data_deliver_type_get();

    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) + dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1);
    uint64_t dose_radiation_index = radiation_data_value_get(DOSE_RADIATION_IDX);

    *interval_us = trigger_out_info_get(TRIGGER_OUT_INTERVAL);
    uint32_t time_elapse_ms = radiation_data_value_get(TIME_RADIATION_IDX_ELAPSE) + *interval_us / 1000;
    ret = radiation_data_value_set(TIME_RADIATION_IDX_ELAPSE, time_elapse_ms);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }

    uint32_t time_expected_ms = (radiation_data_value_get(TIME_RADIATION_IDX_END) + UINT32_MAX - radiation_data_value_get(TIME_RADIATION_IDX_BEGIN)) % UINT32_MAX;
    time_elapse_ms += time_compensation_us / 1000;

    if (dose_accumulated_cur < dose_radiation_index)
    {
        /* not reach radiation index dose yet, so judge whether reached interpolated dose */
        uint32_t pulse_interval = radiation_data_value_get(PULSE_INTERVAL);
        uint64_t dose_rate_interpolated = radiation_data_value_get(DOSE_RATE_INTERPOLATED_RADIATION_IDX);
        uint64_t dose_interpolated = radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX);

        if (dose_accumulated_cur < dose_interpolated)
        {
            if (adcs7476_servo_state_get(LTC2632_CHANNEL_ALL) == 0)
            {
                *time_delay_type = TIM_DELAY_PULSE_INTERVAL;
            }
            else
            {
                *time_delay_type = TIM_DELAY_NO_PULSE_INTERVAL;
                LOG_E("[work]: wait for adcs7476 servo ok: %d\r\n", adcs7476_servo_state_get(LTC2632_CHANNEL_ALL));
            }
        }
        else
        {
            /* no need to generate a new pulse, just wait a cycle */
            *time_delay_type = TIM_DELAY_NO_PULSE_INTERVAL;
        }

        /* calculate next interpolated dose */
        dose_interpolated += dose_rate_interpolated * pulse_interval;
        dose_interpolated = (dose_interpolated <= dose_radiation_index) ? dose_interpolated : dose_radiation_index;
        ret = radiation_data_value_set(DOSE_INTERPOLATED_RADIATION_IDX, dose_interpolated);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }

        // if (time_elapse_ms > time_expected_ms)
        // {
        //     ret = interlock_fault_info_set(INTERLOCK_FAULT_DOSE_RATE_LOW, 1);
        //     if (ret != 0)
        //     {
        //         LOG_E("interlock fault info set err: %d\r\n", ret);
        //         return ret;
        //     }
        // }
    }
    else
    {
        if (deliver_type == DELIVER_TYPE_SWIMRT || deliver_type == DELIVER_TYPE_CRT) /* 主动轴 */
        {
            /* 0. just delay to consume surplus time */
            time_elapse_ms < time_expected_ms ? osDelay(time_expected_ms - time_elapse_ms) : NULL;

            /* 1. update radiation index */
            // LOG_I("----update radiation index----\r\n");
            ret = dose_radiation_index_update(0);
            if (ret != 0)
            {
                LOG_E("dose radiation index update err: %d\r\n", ret);
                return ret;
            }
            *time_delay_type = TIM_DELAY_NO_PULSE_INTERVAL;
        }
        else
        {
            *time_delay_type = TIM_DELAY_NO_PULSE_INTERVAL;
            // *interval_us = 1000;    /* TODO: if needed, delay 1ms to wait for next ri update */
        }
    }
#if 0
    ret = dose_rate_calculate_and_check(&dose_accumulated_cur, &dose_radiation_index, &time_expected_ms, &time_elapse_ms);
#else
    time_elapse_ms = (*interval_us + time_compensation_us) / 1000;
    ret = dose_rate_calculate_and_check(&dose_accumulated_cur, &dose_radiation_index, &time_expected_ms, &time_elapse_ms);
#endif
    if (ret != 0)
    {
        LOG_E("dose rate calculate and check err: %d\r\n", ret);
        return ret;
    }

    return ret;
}

static int8_t detect_whether_one_pulse_repeat(void)
{
    int8_t ret = 0;
    enum fsm_state fsm_state_cur = fsm_state_get();
    enum deliver_type deliver_type = plan_data_deliver_type_get();

    if (fsm_state_cur == FSM_STATE_PRELIMINARY_BEGIN)
    {
        if (trigger_out_info_get(TRIGGER_OUT_FLAG) == 1)
        {
            return 0;
        }

        ret = dose_data_upload_once(fsm_state_cur);
        if (ret != 0)
        {
            LOG_E("dose data upload once err: %d\r\n", ret);
            return ret;
        }

        ret = adcs7476_sample_enable(0);
        if (ret != 0)
        {
            LOG_E("adcs7476_sample_enable err: %d\r\n", ret);
            return ret;
        }

        #define DUMMY_VALUE 3500
        ret = adcs7476_value_dummy(DUMMY_VALUE, LTC2632_CHANNEL_ALL);
        if (ret != 0)
        {
            LOG_E("adcs7476_value_dummy err: %d\r\n", ret);
            return ret;
        }

        ret = trigger_out_info_set(TRIGGER_OUT_FLAG, 1);
        if (ret != 0)
        {
            LOG_E("trigger out info set err: %d\r\n", ret);
            return ret;
        }

        /* calculate dummy time */
        #define DUMMY_VALUE_INCREASE_PER_US (DUMMY_VALUE / 30 - ADCS7476_SERVO_VALUE)
        uint32_t dummy_time_ms = radiation_data_value_get(DOSE_BEAM_METER) / (DUMMY_VALUE_INCREASE_PER_US * 2) / 1000;
        uint32_t timestamp_dummy_end = osKernelGetTickCount() * 1000 / osKernelGetTickFreq() + dummy_time_ms + DUMMY_START_DELAY_TIME_US / 1000;
        timestamp_dummy_end *= DOSE_BOARD_NO_TRIGGER_OUT_SCALE;
        if (radiation_data_value_get(DOSE_BOARD_ID) == DOSE_BOARD_NO_TRIGGER_OUT)
        {
            timestamp_dummy_end *= DOSE_BOARD_NO_TRIGGER_OUT_SCALE;
        }
        // LOG_I("dummy end time: %u\r\n", timestamp_dummy_end);

        ret = interlock_fault_info_set(INTERLOCK_FAULT_DOSE_DUMMY_END_TIME, timestamp_dummy_end);
        if (ret != 0)
        {
            LOG_E("interlock fault info set err: %d\r\n", ret);
            return ret;
        }

#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
        ret = timer_delay_start(TIM_DELAY_DUMMY_START, DUMMY_START_DELAY_TIME_US);
        if (ret != 0)
        {
            LOG_E("timer delay start err: %d\r\n", ret);
            return ret;
        }
#endif
        LOG_I("---dummy start---\r\n");
    }
    else if (fsm_state_cur == FSM_STATE_PREPARE || fsm_state_cur == FSM_STATE_INTERRUPT || fsm_state_cur == FSM_STATE_READY)
    {
        ret = dose_data_upload_once(fsm_state_cur);
        if (ret != 0)
        {
            LOG_E("dose data upload once err: %d\r\n", ret);
            return ret;
        }

        if (fsm_state_cur == FSM_STATE_PREPARE)
        {
            ret = interlock_fault_info_clear();
            if (ret != 0)
            {
                LOG_E("interlock fault info clear err: %d\r\n", ret);
                return ret;
            }
        }

    }
    else if (fsm_state_cur == FSM_STATE_WORK)
    {
        /* delay to prepare a new pulse */
        if (trigger_out_info_get(TRIGGER_OUT_FLAG) == 0)
        {
            /* 0. check adcs7476 servo state */
            if (adcs7476_servo_state_get(LTC2632_CHANNEL_ALL) != 0)
            {
                return 0;
            }

            /* 1. check beam deliver type */
            switch (deliver_type)
            {
            case DELIVER_TYPE_VMAT:
            case DELIVER_TYPE_SSIMRT:
            case DELIVER_TYPE_HiMAT:
#ifdef RADIATION_SIMULATION_MODE
                dose_radiation_index_update(0);
#endif
                break;
            case DELIVER_TYPE_SWIMRT:
            case DELIVER_TYPE_CRT:
                if (radiation_data_value_get(RADIATION_ENABLE) != 1)
                {
                    return 0;
                }
                // LOG_I("---start radiation index---\r\n");
                ret = dose_radiation_index_update(0);
                if (ret != 0)
                {
                    LOG_E("dose radiation index update err: %d\r\n", ret);
                    return ret;
                }
                break;
            case DELIVER_TYPE_SURVIEW:
            case DELIVER_TYPE_CT:
                return 0;
                break;
            default:
                LOG_E("invalid beam deliver type: %d\r\n", deliver_type);
                return -1;
                break;
            }

            /* 2.  */
            uint8_t time_delay_type = 0;
            uint8_t pulse_mode = radiation_data_value_get(PULSE_GENERATION_MODE);
            ret = radiation_data_value_set(PULSE_INTERVAL, radiation_data_value_get(PULSE_INTERVAL_MIN));
            if (ret != 0)
            {
                LOG_E("radiation data value set err: %d\r\n", ret);
                return ret;
            }

            uint32_t pulse_interval = radiation_data_value_get(PULSE_INTERVAL);

            if (pulse_mode == 0)    /* PRF */
            {
                time_delay_type = TIM_DELAY_PULSE_INTERVAL;
            }
            else    /* fixed dose rate */
            {
                // ret = dose_interpolation_check(&time_delay_type, &pulse_interval, 0);
                // if (ret != 0)
                // {
                //     LOG_E("dose interpolation check err: %d\r\n", ret);
                //     return ret;
                // }
                time_delay_type = TIM_DELAY_NO_PULSE_INTERVAL;  /* just wait radiation index update */
            }

            ret = trigger_out_info_set(TRIGGER_OUT_TYPE, time_delay_type);
            if (ret != 0)
            {
                LOG_E("trigger out info set err: %d\r\n", ret);
                return ret;
            }
            ret = trigger_out_info_set(TRIGGER_OUT_FLAG, 1);
            if (ret != 0)
            {
                LOG_E("trigger out info set err: %d\r\n", ret);
                return ret;
            }
            ret = trigger_out_info_set(TRIGGER_OUT_INTERVAL, pulse_interval);
            if (ret != 0)
            {
                LOG_E("trigger out info set err: %d\r\n", ret);
                return ret;
            }
#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
            ret = timer_delay_start(time_delay_type, pulse_interval);
            if (ret != 0)
            {
                LOG_E("timer delay start err: %d\r\n", ret);
                return ret;
            }
        }
#endif
    }
    else
    {
#if 0
        timer_delay_flag = 0;
        lptim3_delay_flag = 0;
#endif
        ret = dose_data_upload_once(fsm_state_cur);
        if (ret != 0)
        {
            LOG_E("dose data upload once err: %d\r\n", ret);
            return ret;
        }

        ret = trigger_out_info_set(TRIGGER_OUT_FLAG, 0);
        if (ret != 0)
        {
            LOG_E("trigger out info set err: %d\r\n", ret);
            return ret;
        }
        ret = trigger_out_info_set(TRIGGER_OUT_COUNT, 0);
        if (ret != 0)
        {
            LOG_E("trigger out info set err: %d\r\n", ret);
            return ret;
        }
        ret = radiation_data_value_set(DOSE_INTERPOLATED_RADIATION_IDX, 0);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
        ret = radiation_data_value_set(DOSE_RATE_INTERPOLATED_RADIATION_IDX, 0);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
        ret = radiation_data_value_set(DOSE_CONTROL_POINT_CURRENT, 0);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
        ret = radiation_data_value_set(DOSE_CONTROL_POINT_PREV_RADIATION_IDX, 0);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
        ret = radiation_data_value_set(RADIATION_ENABLE, 0);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
        ret = timer_delay_stop();
        if (ret != 0)
        {
            LOG_E("timer delay stop err: %d\r\n", ret);
            return ret;
        }
#endif
    }

    return 0;
}

// #define ADCS7476_DATA_DUMP
#ifdef ADCS7476_DATA_DUMP
#include "shell.h"
static uint16_t adcs7476_data_buf[2][BUF_LEN * 10] = {0};
static uint16_t adcs7476_data_len[2] = {0};
static int8_t adcs7476_data_array_clear(uint8_t index)
{
    if (index >= 2)
    {
        return -1;
    }

    memset(adcs7476_data_len, 0, sizeof(adcs7476_data_len));
    memset(adcs7476_data_buf, 0, sizeof(adcs7476_data_buf));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(adcs7476_data_array_clear, adcs7476_data_array_clear, adcs7476 data array clear);
static int8_t adcs7476_data_save(uint8_t index, uint16_t *buf, uint16_t len)
{
    if (index >= 2)
    {
        return -1;
    }

    memcpy(&adcs7476_data_buf[index][adcs7476_data_len[index]], buf, len * sizeof(uint16_t));
    adcs7476_data_len[index] += len;
    if (adcs7476_data_len[index] >= sizeof(adcs7476_data_buf[index]) / sizeof(uint16_t))
    {
        adcs7476_data_len[index] = 0;
    }

    return 0;
}
static int8_t adcs7476_data_dump(uint8_t argc, char *argv[])
{
    if (argc != 2)
    {
        return -1;
    }

    // uint8_t index = atoi(argv[1]);

    // if (index >= 2)
    // {
    //     return -2;
    // }

    // uint16_t *buf = adcs7476_data_buf[index];
    // uint16_t len = adcs7476_data_len[index];

    // LOG_I("adcs7476 data dump: %d\r\n", len);
    // for (uint16_t i = 0; i < len; i++)
    // {
    //     LOG_I("%.4d ", buf[i]);
    // }
    // LOG_I("\r\n");

    uint16_t *buf = NULL;
    uint16_t len = 0;

    switch (atoi(argv[1]))
    {
    case 0:
        buf = adcs7476_data_buf[0];
        len = adcs7476_data_len[0];
        break;
    case 1:
        buf = adcs7476_data_buf[1];
        len = adcs7476_data_len[1];
        break;
    case 2: /* sum of two channel */
        for (uint16_t i = 0; i < adcs7476_data_len[0]; i++)
        {
            adcs7476_data_buf[0][i] += adcs7476_data_buf[1][i];
        }
        buf = adcs7476_data_buf[0];
        len = adcs7476_data_len[0];
        break;
    default:
        return -2;
        break;
    }

    LOG_I("adcs7476 data dump: %d\r\n", len);
    for (uint16_t i = 0; i < len; i++)
    {
        LOG_I("%.4d ", buf[i]);
    }
    LOG_I("\r\n");

    return 0;
}
MSH_CMD_EXPORT_ALIAS(adcs7476_data_dump, adcs7476_data_dump, adcs7476 data dump);
#endif

static int8_t dose_channel_offset_check(uint16_t *buf, uint16_t *buf_1, uint16_t len)
{
    int8_t ret = 0;
    enum fsm_state fsm_state_cur = fsm_state_get();

    if (fsm_state_cur == FSM_STATE_PRELIMINARY_BEGIN || fsm_state_cur == FSM_STATE_WORK)
    {
        return 0;
    }

    /* check adc offset limit */
    ret = adcs7476_object_offset_limit_check(buf, buf_1, len);
    if (ret < 0)
    {
        LOG_E("adcs7476 offset limit check err: %d\r\n", ret);
    }
    else if (ret > 0)
    {
        ret = interlock_fault_info_set(INTERLOCK_FAULT_ADCS7476_1_OFFSET_LIMIT_LOW, (ret >> 0) & 0x01);
        ret |= interlock_fault_info_set(INTERLOCK_FAULT_ADCS7476_1_OFFSET_LIMIT_HIGH, (ret >> 1) & 0x01);
        ret |= interlock_fault_info_set(INTERLOCK_FAULT_ADCS7476_2_OFFSET_LIMIT_LOW, (ret >> 2) & 0x01);
        ret |= interlock_fault_info_set(INTERLOCK_FAULT_ADCS7476_2_OFFSET_LIMIT_HIGH, (ret >> 3) & 0x01);
        if (ret != 0)
        {
            LOG_E("interlock fault info set err: %d\r\n", ret);
        }
    }

    return ret;
}

int8_t adcs7476_value_process(uint16_t *buf, uint16_t *buf_1, uint16_t len)
{
    int8_t ret = 0;
    uint16_t pulse_cnt = 0, pulse_cnt_1 = 0;
    uint32_t servo_value = 0, servo_value_1 = 0;
    uint32_t pulse_value = 0, pulse_value_1 = 0;

    /* 1. read adcs7476 2 channel data */
    // ret = adcs7476_object_data_read(ADCS7476_ID1_NAME, buf, BUF_LEN, osWaitForever);
    // if (ret != 0)
    // {
    //     LOG_E("adcs7476_object_data_read err: %d\r\n", ret);
    //     return ret;
    // }

    // ret = adcs7476_object_data_read(ADCS7476_ID2_NAME, buf_1, BUF_LEN, osWaitForever);
    // if (ret != 0)
    // {
    //     LOG_E("adcs7476_object_data_read err: %d\r\n", ret);
    //     return ret;
    // }

    ret = dose_channel_offset_check(buf, buf_1, len);
    if (ret != 0)
    {
        LOG_E("dose channel offset check err: %d\r\n", ret);
    }

#ifdef RADIATION_SIMULATION_MODE
    if (radiation_simulation_trigger_out_flag)
    {
        radiation_simulation_dose_deal(buf, len);
        radiation_simulation_dose_deal(buf_1, len);
    }
#endif

    /* 2. check value */
    pulse_cnt = adcs7476_value_check(buf, len, &pulse_value, &servo_value);
    pulse_cnt_1 = adcs7476_value_check(buf_1, len, &pulse_value_1, &servo_value_1);
    if (pulse_cnt != pulse_cnt_1 && pulse_cnt != 0 && pulse_cnt_1 != 0)
    {
        // LOG_E("adcs7476 value pulse count not match: %d, %d\r\n", pulse_cnt, pulse_cnt_1);
#if 0
        LOG_I("pulse_cnt: %d, pulse_cnt_1: %d\r\n", pulse_cnt, pulse_cnt_1);
        LOG_I("pulse_value: %d  pulse_value_1: %d\r\n", pulse_value, pulse_value_1);
        LOG_I("servo_value: %d, servo_value_1: %d\r\n", servo_value, servo_value_1);
#endif
        pulse_cnt = pulse_cnt <= pulse_cnt_1 ? pulse_cnt : pulse_cnt_1;
    }

#ifdef ADCS7476_DATA_DUMP
    if (pulse_cnt != 0 && pulse_cnt <= len)
    {
        ret = adcs7476_data_save(0, buf, len);
        ret = adcs7476_data_save(1, buf_1, len);
        if (ret != 0)
        {
            LOG_E("adcs7476_data_save err: %d\r\n", ret);
            return ret;
        }
    }
#endif

    uint16_t one_pulse_cnt = dose_value_status_get(ONE_PULSE_COUNT, DOSE_CHANNEL_NONE);

#ifdef DETECT_RADIATION_TIME_FROM_TRIGGER_OUT
    if (one_pulse_cnt == 0 && pulse_cnt != 0)
    {
        system_time_get(&end_time);
        // LOG_I("radiation delayed from trigger out: %u us\r\n", time_diff_us(&begin_time, &end_time));
    }
#endif

#if 0
    static uint32_t cnt = 0;
    if (cnt++ % 1000 == 0)
    {
        // LOG_I("buf: %d  buf_1: %d\r\n", buf[0], buf_1[0]);
        LOG_I("one_pulse_cnt: %d\r\n", one_pulse_cnt);
        LOG_I("pulse_cnt: %d, pulse_cnt_1: %d\r\n", pulse_cnt, pulse_cnt_1);
    }
#endif

    if (one_pulse_cnt == 0)
    {
        if (pulse_cnt == 0 && pulse_cnt_1 == 0)
        {
            /* servo mode */
            ret = adcs7476_value_servo(servo_value, len, LTC2632_CHANNEL_OUTA);
            if (ret != 0)
            {
                LOG_E("adcs7476 value servo err: %d\r\n", ret);
            }
            ret = adcs7476_value_servo(servo_value_1, len, LTC2632_CHANNEL_OUTB);
            if (ret != 0)
            {
                LOG_E("adcs7476 value servo err: %d\r\n", ret);
            }

            /* check whether generate a new pulse */
            ret = detect_whether_one_pulse_repeat();
            if (ret != 0)
            {
                LOG_E("detect whether one pulse repeat err: %d\r\n", ret);
            }
        }
        else
        {
            // LOG_I("---one pulse begin---\r\n");
#ifdef TRIGGER_OUT_STATISTIC
            trigger_out_cnt++;
#endif
            /* pulse mode begin */
            ret = adcs7476_value_dose(pulse_value, pulse_value_1, pulse_cnt);
            if (ret != 0)
            {
                LOG_E("adcs7476 value dose err: %d\r\n", ret);
            }
        }
    }
    else
    {
        if (pulse_cnt < len || pulse_cnt_1 < len)
        {
            // LOG_I("---one pulse end---\r\n");
            /* pulse mode end, servo mode begin */
            ret = adcs7476_value_dose(pulse_value, pulse_value_1, pulse_cnt);
            if (ret != 0)
            {
                LOG_E("adcs7476 value dose err: %d\r\n", ret);
            }

#if 0       /* reduce mcu work load, so value servo delayed maximum is 100 sample cycles */
            ret = adcs7476_value_servo(servo_value, len - pulse_cnt, LTC2632_CHANNEL_OUTA);
            if (ret != 0)
            {
                LOG_E("adcs7476 value servo err: %d\r\n", ret);
            }
            ret = adcs7476_value_servo(servo_value_1, len - pulse_cnt_1, LTC2632_CHANNEL_OUTB);
            if (ret != 0)
            {
                LOG_E("adcs7476 value servo err: %d\r\n", ret);
            }
#endif
        }
        else
        {
            /* pulse mode running */
            ret = adcs7476_value_dose(pulse_value, pulse_value_1, pulse_cnt);
            if (ret != 0)
            {
                LOG_E("adcs7476 value dose err: %d\r\n", ret);
            }
        }

        ret = dose_accumulated_check(pulse_cnt, len);
        if (ret != 0)
        {
            LOG_E("dose accumulated check err: %d\r\n", ret);
        }
    }

    return ret;
}

static int8_t dose_rate_calculate(void *argument)
{
    int8_t ret = 0;
    uint32_t cnt = 0;
    uint32_t trigger_out_count_pre = 0, trigger_out_count = 0, prf_cur = 0;
    uint64_t factor = 0;
    float dose_cur = 0, dose_rate_cur = 0;

    for (;;)
    {
        osDelay(100);

        if (cnt++ % 5 == 0)
        {
            /* 1. upload data to arm io periodically */
            // ret = dose_data_upload(REAL_TIME_DATA_TYPE_RADIATION);
            // if (ret != 0)
            // {
            //     LOG_E("dose data upload err: %d\r\n", ret);
            // }

            /* 2. calculate prf */
            trigger_out_count = trigger_out_info_get(TRIGGER_OUT_COUNT);
            trigger_out_count_pre = trigger_out_count == 0 ? trigger_out_count : trigger_out_count_pre;
            prf_cur = (trigger_out_count - trigger_out_count_pre) * 2;  /* unit: hz/s */
            ret = dose_value_status_set(PRF_CURRENT, prf_cur, 0);
            if (ret != 0)
            {
                LOG_E("prf set err: %d\r\n", ret);
            }
            trigger_out_count_pre = trigger_out_count;
        }

        /* 3. calculate dose rate */
        if (fsm_state_get() == FSM_STATE_WORK/*  && radiation_data_value_get(PULSE_GENERATION_MODE) == 0 */)    /* PRF */
        {
            factor = radiation_data_value_get(DOSE_CALIBRATION_FACTOR);
            dose_cur = (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) / (factor & 0xffffffff) + (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1) / (factor >> 32);
            dose_rate_cur = dose_rate_average_calculate(dose_cur, 100);

            ret = dose_value_status_set(DOSE_RATE_CURRENT, dose_rate_cur * 100.0f, 0);
            if (ret != 0)
            {
                LOG_E("dose rate set err: %d\r\n", ret);
            }
        }
    }

    return 0;
}

// #define RADIATION_EFFICIENCY_TEST
#ifdef RADIATION_EFFICIENCY_TEST
struct radiation_ri_data
{
    uint32_t timestamp; /* unit: ms, record timestamp of ri received */
    uint64_t dose_cumulated;    /* record dose cumulated when ri update */
};
static struct radiation_ri_data ri_data_array[4096] __attribute__((section(".ram_itcm"))) = {0};
static int8_t dose_radiation_index_update_dump(uint32_t time_excess_ms)
{
    uint16_t ri = radiation_data_value_get(DOSE_RADIATION_IDX_CURRENT);
    uint16_t ri_max = sizeof(ri_data_array) / sizeof(ri_data_array[0]);
    if (ri < ri_max)
    {
        ri_data_array[ri].timestamp = osKernelGetTickCount() * 1000 / osKernelGetTickFreq();
        ri_data_array[ri].dose_cumulated = dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) + dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1);   /* channel value is in proportion to the calibration factor */
    }

    return dose_radiation_index_update(time_excess_ms);
}
#endif

static int8_t dose_interpolation_init(void)
{
#ifdef RADIATION_EFFICIENCY_TEST
    return radiation_index_update_callback(dose_radiation_index_update_dump);
#else
    return radiation_index_update_callback(dose_radiation_index_update);
#endif

}

static int8_t interlock_fault_callback(uint32_t interlock)
{
    int8_t ret = 0;
    enum fsm_state state = fsm_state_get();

    switch (state)
    {
    case FSM_STATE_INIT:
        break;
    case FSM_STATE_IDLE:
        break;
    case FSM_STATE_PRELIMINARY_BEGIN:
    case FSM_STATE_PRELIMINARY:
    case FSM_STATE_PREPARE:
        break;
    case FSM_STATE_READY:
        ret = interlock_status_value_locked_set(interlock);
        // ret |= fsm_state_switch(FSM_STATE_INTERRUPT);
        LOG_I("set fsm state to interrupt: %#.8x\r\n", interlock);
        break;
    case FSM_STATE_WORK:
        ret = interlock_status_value_locked_set(interlock);
        // ret |= fsm_state_switch(FSM_STATE_TERMINATE);
        LOG_I("set fsm state to terminate: %#.8x\r\n", interlock);
        break;
    case FSM_STATE_INTERRUPT:
        break;
    case FSM_STATE_COMPLETE:
        break;
    case FSM_STATE_TERMINATE:
        break;
    case FSM_STATE_PARK:
    case FSM_STATE_MANUAL:
    case FSM_STATE_SHUTDOWN:
    case FSM_STATE_POWERSAVER:
        break;
    default:
        ret = -1;
        LOG_E("invalid fsm state: %d\r\n", state);
        break;
    }

    return ret;
}

static int8_t interlock_fault_process_init(void)
{
    return interlock_fault_register_callback(interlock_fault_callback);
}

static osEventFlagsId_t fsm_state_change_event = NULL;
static int8_t fsm_state_change(void)
{
    osEventFlagsSet(fsm_state_change_event, 1);

    return 0;
}
static int8_t fsm_state_change_entry(void *argument)
{
    int8_t ret = 0;

    for (;;)
    {
        osEventFlagsWait(fsm_state_change_event, 1, osFlagsWaitAny, osWaitForever);

        ret = dose_data_upload(REAL_TIME_DATA_TYPE_RADIATION);
        if (ret != 0)
        {
            LOG_E("dose data upload err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t radiation_thread_init(void)
{
    osMutexAttr_t dose_value_mutex_attributes = {
    .name = "dose_value_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    dose_value_obj_get()->mutex = osMutexNew(&dose_value_mutex_attributes);
    if (dose_value_obj_get()->mutex == NULL)
    {
        LOG_E("dose value mutex create failed\r\n");
        return -1;
    }

    osMutexAttr_t radiation_data_mutex_attributes = {
    .name = "radiation_data_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    radiation_data_get()->mutex = osMutexNew(&radiation_data_mutex_attributes);
    if (radiation_data_get()->mutex == NULL)
    {
        LOG_E("radiation data mutex create failed\r\n");
        return -2;
    }

    osMutexAttr_t trigger_out_mutex_attributes = {
    .name = "trigger_out_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    trigger_out_obj_get()->mutex = osMutexNew(&trigger_out_mutex_attributes);
    if (trigger_out_obj_get()->mutex == NULL)
    {
        LOG_E("trigger out mutex create failed\r\n");
        return -3;
    }

#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
    timer_delay_event = osEventFlagsNew(NULL);
    if (timer_delay_event == NULL)
    {
        LOG_E("timer delay event create failed\r\n");
        return -4;
    }

    osThreadAttr_t time_delay_thread_attributes = {
    .name = "time_delay_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal6,
    };

    osThreadId_t time_delay_threadHandle = osThreadNew(time_delay_entry, NULL, &time_delay_thread_attributes);
    if (time_delay_threadHandle == NULL)
    {
        LOG_E("thread time delay create failed\r\n");
        return -5;
    }
#endif

#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
    lptim3_delay_event = osEventFlagsNew(NULL);
    if (lptim3_delay_event == NULL)
    {
        LOG_E("lptim3 delay event create failed\r\n");
        return -6;
    }

    osThreadAttr_t lptim3_delay_thread_attributes = {
    .name = "lptim3_delay_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    osThreadId_t lptim3_delay_threadHandle = osThreadNew(lptim3_delay_entry, NULL, &lptim3_delay_thread_attributes);
    if (lptim3_delay_threadHandle == NULL)
    {
        LOG_E("thread lptim3 delay create failed\r\n");
        return -7;
    }
#endif

#ifdef USING_TIM23_FOR_TRIGGER_TIME_COMPENSATION
    int8_t ret = trigger_time_compensation_init();
    if (ret != 0)
    {
        LOG_E("trigger time compensation init err: %d\r\n", ret);
        return -8;
    }
#endif

    osThreadAttr_t dose_rate_calculate_attributes = {
    .name = "dose_rate_calculate_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t)osPriorityNormal,
    };

    osThreadId_t dose_rate_calculateHandle = osThreadNew(dose_rate_calculate, NULL, &dose_rate_calculate_attributes);
    if (dose_rate_calculateHandle == NULL)
    {
        printf("thread dose rate calculate create failed\r\n");
        return -9;
    }

    ret = radiation_data_init();
    if (ret != 0)
    {
        LOG_E("radiation data init err: %d\r\n", ret);
        return -10;
    }

    ret = dose_interpolation_init();
    if (ret != 0)
    {
        LOG_E("dose interpolation init err: %d\r\n", ret);
        return -11;
    }

    ret = interlock_fault_process_init();
    if (ret != 0)
    {
        LOG_E("interlock fault process init err: %d\r\n", ret);
        return -12;
    }

    ret = adcs7476_object_data_callback_register(adcs7476_value_process);
    if (ret != 0)
    {
        LOG_E("adcs7476 callback register err: %d\r\n", ret);
        return -13;
    }

    fsm_state_change_event = osEventFlagsNew(NULL);
    if (fsm_state_change_event == NULL)
    {
        LOG_E("fsm state change event create failed\r\n");
        return -14;
    }

    ret = fsm_state_change_event_callback_register(fsm_state_change);
    if (ret != 0)
    {
        LOG_E("fsm state change event callback register err: %d\r\n", ret);
        return -15;
    }

    osThreadAttr_t attr = {
        .name = "fsm_state_change_thread",
       .stack_size = 1024 * 4,
       .priority = (osPriority_t)osPriorityAboveNormal,
    };

    osThreadId_t fsm_state_change_threadHandle = osThreadNew(fsm_state_change_entry, NULL, &attr);
    if (fsm_state_change_threadHandle == NULL)
    {
        LOG_E("thread fsm state change create failed\r\n");
        return -16;
    }

    return 0;
}
INIT_APP_EXPORT(radiation_thread_init);

#ifndef RADIATION_TEST
#include "shell.h"
static int8_t dose_dummy_mode_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;

    /* 1. clear dose meter value */
    ret = dose_value_status_set(DOSE_ACCUMULATED, 0, 0);
    if (ret != 0)
    {
        LOG_E("dose value status set err: %d\r\n", ret);
        return ret;
    }

    /* 2. update dose meter value */
    ret = beam_data_value_set(0, BEAM_DOSE_METER, 0, 100);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }

    /* 3. set dose mode and pulse mode */
    ret = radiation_data_value_set(DOSE_GENERATION_MODE, 0);
    ret |= radiation_data_value_set(PULSE_GENERATION_MODE, 0);

    return ret;
}
MSH_CMD_EXPORT_ALIAS(dose_dummy_mode_test, dose_dummy_mode_test, dose dummy mode test);

static int8_t dose_radiation_mode_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;

    /* 1. clear dose meter value */
    ret = dose_value_status_set(DOSE_ACCUMULATED, 0, 0);
    if (ret != 0)
    {
        LOG_E("dose value status set err: %d\r\n", ret);
        return ret;
    }

    /* 2. update dose meter value */
    ret = beam_data_value_set(0, BEAM_DOSE_METER, 0, 100);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }

    /* 3. set dose mode and pulse mode */
    ret = radiation_data_value_set(DOSE_GENERATION_MODE, 1);
    ret |= radiation_data_value_set(PULSE_GENERATION_MODE, 0);  /* fix prf */

    return ret;
}
MSH_CMD_EXPORT_ALIAS(dose_radiation_mode_test, dose_radiation_mode_test, dose radiation mode test);


static int8_t dose_plan_data_deliver_type_set(uint8_t argc, char **argv)
{
    return radiation_data_value_set(DOSE_BEAM_DELIVER_TYPE, atoi(argv[1]));
}
MSH_CMD_EXPORT_ALIAS(dose_plan_data_deliver_type_set, dose_plan_data_deliver_type_set, dose plan data deliver type set);

static int8_t dose_plan_data_deliver_type_get(uint8_t argc, char **argv)
{
    LOG_I("deliver type: %d\r\n", radiation_data_value_get(DOSE_BEAM_DELIVER_TYPE));
}
MSH_CMD_EXPORT_ALIAS(dose_plan_data_deliver_type_get, dose_plan_data_deliver_type_get, dose plan data deliver type get);

static int8_t dose_interpolation_init_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;

    /* 1. clear dose meter value */
    ret = dose_value_status_set(DOSE_ACCUMULATED, 0, 0);
    if (ret != 0)
    {
        LOG_E("dose value status set err: %d\r\n", ret);
        return ret;
    }

    /* 2. set dose meter value */
    ret = beam_data_value_set(0, BEAM_DOSE_METER, 0, 10240);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }

    /* 3. set plan data */
    #define TOTAL_CP    1024
    #define TOTAL_RI    10240
    ret = beam_data_value_set(0, BEAM_TOTAL_CP, 0, TOTAL_CP);
    if (ret != 0)
    {
        LOG_E("beam data total cp set err: %d\r\n", ret);
    }
    ret |= beam_data_value_set(0, BEAM_TOTAL_RI, 0, TOTAL_RI);
    if (ret != 0)
    {
        LOG_E("beam data total ri set err: %d\r\n", ret);
    }

    for (uint16_t i = 1; i <= TOTAL_CP; i++)
    {
        ret = beam_data_value_set(0, BEAM_CP_RI_MAP, i, 10 * i);
        if (ret != 0)
        {
            LOG_E("beam data cp ri map set err: %d\r\n", ret);
        }
    }

    for (uint16_t i = 1; i <= TOTAL_RI; i++)
    {
        ret = beam_data_value_set(0, BEAM_RI_CUMULATIVE, i, i);
        if (ret != 0)
        {
            LOG_E("beam data ri cumulative set err: %d\r\n", ret);
        }
        ret = beam_data_value_set(0, BEAM_RI_DOSE_RATE, i, 5);
        if (ret != 0)
        {
            LOG_E("beam data ri dose rate set err: %d\r\n", ret);
        }
        ret = beam_data_value_set(0, BEAM_RI_TIME_EXPECTED, i, 4); /* unit: ms, can modify here to test for vmat or crt */
        if (ret != 0)
        {
            LOG_E("beam data ri time expected set err: %d\r\n", ret);
        }
    }

    ret = beam_data_value_set(0, BEAM_RI_CUMULATIVE, 1, 0);
    if (ret != 0)
    {
        LOG_E("beam data ri cumulative set err: %d\r\n", ret);
    }
    ret = beam_data_value_set(0, BEAM_RI_DOSE_RATE, TOTAL_RI, 0);
    if (ret != 0)
    {
        LOG_E("beam data ri dose rate set err: %d\r\n", ret);
    }
    ret = beam_data_value_set(0, BEAM_RI_TIME_EXPECTED, TOTAL_RI, 0);
    if (ret != 0)
    {
        LOG_E("beam data ri time expected set err: %d\r\n", ret);
    }

    /* 4. set mode */
    ret = radiation_data_value_set(DOSE_GENERATION_MODE, 1);
    ret |= radiation_data_value_set(PULSE_GENERATION_MODE, 1);  /* fix dose rate */

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_interpolation_init_test, dose_interpolation_init_test, dose interpolation init test);

static int8_t dose_interpolation_data_get(uint8_t argc, char **argv)
{
    int8_t ret = 0;

    LOG_I("beam dose meter: %f\r\n", beam_data_value_get(0, BEAM_DOSE_METER, 0));
    LOG_I("beam dose rate: %f\r\n", beam_data_value_get(0, BEAM_DOSE_RATE, 0));
    LOG_I("beam total cp: %d\r\n", (uint16_t)beam_data_value_get(0, BEAM_TOTAL_CP, 0));
    LOG_I("beam total ri: %d\r\n", (uint16_t)beam_data_value_get(0, BEAM_TOTAL_RI, 0));

    for (uint16_t i = 1; i <= beam_data_value_get(0, BEAM_TOTAL_CP, 0); i++)
    {
        LOG_I("beam cp[%d]: %d\r\n", i, (uint16_t)beam_data_value_get(0, BEAM_CP_RI_MAP, i));
    }

    for (uint16_t i = 1; i <= beam_data_value_get(0, BEAM_TOTAL_RI, 0); i++)
    {
        LOG_I("ri[%d]: dose cumulative: %f, dose rate: %f, time expected: %f\r\n", 
                i, beam_data_value_get(0, BEAM_RI_CUMULATIVE, i), beam_data_value_get(0, BEAM_RI_DOSE_RATE, i), beam_data_value_get(0, BEAM_RI_TIME_EXPECTED, i));
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_interpolation_data_get, dose_interpolation_data_get, dose interpolation data get);

static int8_t dose_interpolation_radiation_index_set(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct radiation_index_data *obj = radiation_data_get();

    uint16_t index = atoi(argv[1]);

    osMutexAcquire(obj->control_data->mutex, osWaitForever);
    obj->control_data->radiation.index = index;
    obj->control_data->radiation.cp_prev = obj->control_data->radiation.cp;
    obj->control_data->radiation.cp = beam_data_value_get(0, BEAM_RI_IN_CP, obj->control_data->radiation.index);
    obj->control_data->radiation.index_max_in_cp = beam_data_value_get(0, BEAM_RI_IN_CP_MAX, obj->control_data->radiation.index);
    osMutexRelease(obj->control_data->mutex);

    ret = dose_radiation_index_update(0);
    if (ret!= 0)
    {
        LOG_E("dose interpolation calculate err: %d\r\n", ret);
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_interpolation_radiation_index_set, dose_interpolation_radiation_index_set, dose interpolation radiation index set);

static int8_t dose_interpolation_index_data_get(uint8_t argc, char **argv)
{
    LOG_I("dose_accumulated_cur[0]: %llu\r\n", dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0));
    LOG_I("dose_accumulated_cur[1]: %llu\r\n", dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1));
    LOG_I("dose_radiation_index: %llu\r\n", radiation_data_value_get(DOSE_RADIATION_IDX));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_interpolation_index_data_get, dose_interpolation_index_data_get, dose interpolation index data get);

static int8_t dose_radiation_enable_set(uint8_t argc, char **argv)
{
    return radiation_data_value_set(RADIATION_ENABLE, atoi(argv[1]));
}
MSH_CMD_EXPORT_ALIAS(dose_radiation_enable_set, dose_radiation_enable_set, dose radiation enable set);

static int8_t fsm_state_current_get(uint8_t argc, char **argv)
{
    struct radiation_index_data *obj = radiation_data_get();
    osMutexAcquire(obj->control_data->mutex, osWaitForever);
    LOG_I("fsm state request: %d\r\n", obj->control_data->fsm.state_request);
    LOG_I("fsm state cur: %d, realtime state: %d\r\n", obj->control_data->fsm.state_cur, fsm_state_get());
    LOG_I("fsm state prev: %d\r\n", obj->control_data->fsm.state_prev);
    osMutexRelease(obj->control_data->mutex);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(fsm_state_current_get, fsm_state_current_get, fsm state current get);

static int8_t fsm_state_current_set(uint8_t argc, char **argv)
{
    enum fsm_state state = atoi(argv[1]);
    /*
    1：FSM_STATE_INIT
    2：FSM_STATE_IDLE
    3：FSM_STATE_PRELIMINARY
    4：FSM_STATE_PREPARE
    5：FSM_STATE_READY
    6：FSM_STATE_WORK
    12：FSM_STATE_COMPLETE
    15：FSM_STATE_TERMINATE
    16: FSM_STATE_INTERRUPT
    30: FSM_STATE_PRELIMINARY_BEGIN
    */

    int8_t ret = fsm_state_switch(state);

    if (ret != 0)
    {
        LOG_E("fsm state switch err: %d\r\n", ret);
        return ret;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(fsm_state_current_set, fsm_state_current_set, fsm state current set);

static int8_t tim5_delay_test(uint8_t argc, char **argv)
{
    return timer_delay_start(TIM_DELAY_PULSE_INTERVAL, atoi(argv[1]));
}
MSH_CMD_EXPORT_ALIAS(tim5_delay_test, tim5_delay_test, tim5 delay test);

static int8_t lptim3_delay_test(uint8_t argc, char **argv)
{
    return lptim3_delay_start(LPTIM3_DELAY_ONE_PULSE_TIMEOUT_US, atoi(argv[1]));
}
MSH_CMD_EXPORT_ALIAS(lptim3_delay_test, lptim3_delay_test, lptim3 delay test);

static int8_t trigger_out_stop(void)
{
    int8_t ret = 0;
    ret = trigger_out_info_set(TRIGGER_OUT_FLAG, 0);
    if (ret != 0)
    {
        LOG_E("trigger out info set err: %d\r\n", ret);
        return ret;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(trigger_out_stop, trigger_out_stop, trigger out stop);

static int8_t beam_data_info_get(uint8_t argc, char **argv)
{
    LOG_I("------------------- control info ----------------\r\n");
    LOG_I("dose board id: %llu\r\n", radiation_data_value_get(DOSE_BOARD_ID));
    LOG_I("dose generation mode: %llu\r\n", radiation_data_value_get(DOSE_GENERATION_MODE));
    LOG_I("pulse generation mode: %llu\r\n", radiation_data_value_get(PULSE_GENERATION_MODE));
    LOG_I("pulse interval: %llu us\r\n", radiation_data_value_get(PULSE_INTERVAL));
    LOG_I("pulse interval min: %llu us\r\n", radiation_data_value_get(PULSE_INTERVAL_MIN));
    LOG_I("pulse symmetry: %llu\r\n", radiation_data_value_get(PULSE_SYMMETRY));
    LOG_I("dose limit: (%llu, %llu)\r\n", radiation_data_value_get(DOSE_LIMIT) & 0xFF, radiation_data_value_get(DOSE_LIMIT) >> 8);
    LOG_I("dose rate limit: (%llu, %llu)\r\n", radiation_data_value_get(DOSE_RATE_LIMIT) & 0xFF, radiation_data_value_get(DOSE_RATE_LIMIT) >> 8);

    LOG_I("------------------- beam data info --------------\r\n");
    uint64_t factor = radiation_data_value_get(DOSE_CALIBRATION_FACTOR);
    uint32_t factor_0 = factor, factor_1 = factor >> 32;
    LOG_I("radiation index current: %llu\r\n", radiation_data_value_get(DOSE_RADIATION_IDX_CURRENT));
    LOG_I("dose meter target: %llu, %lf\r\n", radiation_data_value_get(DOSE_BEAM_METER), (double)radiation_data_value_get(DOSE_BEAM_METER) * 2.0f / (factor_0 + factor_1));
    LOG_I("dose radiation index: %llu, %lf\r\n", radiation_data_value_get(DOSE_RADIATION_IDX), (double)radiation_data_value_get(DOSE_RADIATION_IDX) * 2.0f / (factor_0 + factor_1));
    LOG_I("dose rate radiation index: %llu, %lf\r\n", radiation_data_value_get(DOSE_RATE_RADIATION_IDX), (double)radiation_data_value_get(DOSE_RATE_RADIATION_IDX) * 2.0f / (factor_0 + factor_1));
    LOG_I("dose time radiation index: %llu\r\n", radiation_data_value_get(DOSE_TIME_RADIATION_IDX));
    LOG_I("dose interpolated radiation index: %llu\r\n", radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX));
    LOG_I("dose rate interpolated radiation index: %llu\r\n", radiation_data_value_get(DOSE_RATE_INTERPOLATED_RADIATION_IDX));

    LOG_I("------------------- realtime info ---------------\r\n");
    double dose_cumulated = (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) / factor_0;
    double dose_cumulated_1 = (double)dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1) / factor_1;
    LOG_I("dose cumulated[0]: %llu, %lf\r\n",  dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0), dose_cumulated);
    LOG_I("dose cumulated[1]: %llu, %lf\r\n",  dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1), dose_cumulated_1);
    LOG_I("dose cumulated[0+1]: %llu, %lf, %f\r\n",  dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_0) + dose_value_status_get(DOSE_ACCUMULATED, DOSE_CHANNEL_1), dose_cumulated + dose_cumulated_1, (float)(dose_cumulated + dose_cumulated_1));

    LOG_I("prf current: %llu\r\n", dose_value_status_get(PRF_CURRENT, DOSE_CHANNEL_NONE));
    LOG_I("dose rate: %f\r\n",  (float)dose_value_status_get(DOSE_RATE_CURRENT, DOSE_CHANNEL_NONE) / 100.0f);

    LOG_I("control point current: %llu\r\n", radiation_data_value_get(DOSE_CONTROL_POINT_CURRENT));
    LOG_I("control point prev radiation index: %llu\r\n", radiation_data_value_get(DOSE_CONTROL_POINT_PREV_RADIATION_IDX));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(beam_data_info_get, beam_data_info_get, beam data info get);

static int8_t one_pulse_dose_get(uint8_t argc, char **argv)
{
    LOG_I("one pulse timeout: %llu\r\n", dose_value_status_get(ONE_PULSE_COUNT, DOSE_CHANNEL_NONE));
    LOG_I("one pulse flag: %llu\r\n", dose_value_status_get(ONE_PULSE_COMPLETE, DOSE_CHANNEL_NONE));
    LOG_I("one pulse dose[0]: %llu\r\n", dose_value_status_get(ONE_PULSE_DOSE, DOSE_CHANNEL_0));
    LOG_I("one pulse dose[1]: %llu\r\n", dose_value_status_get(ONE_PULSE_DOSE, DOSE_CHANNEL_1));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(one_pulse_dose_get, one_pulse_dose_get, one pulse dose get);

static int8_t radiation_data_info_get(uint8_t argc, char **argv)
{
    LOG_I("timer_delay_flag: %d\r\n", timer_delay_flag);
    LOG_I("lptim3_delay_flag: %d\r\n", lptim3_delay_flag);
    LOG_I("trigger_out_flag: %d\r\n", trigger_out_info_get(TRIGGER_OUT_FLAG));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(radiation_data_info_get, radiation_data_info_get, radiation data info get);

#ifdef TRIGGER_OUT_STATISTIC
static int8_t radiation_trigger_out_count_get(uint8_t argc, char **argv)
{
    LOG_I("trigger out: %u\r\n", trigger_out_info_get(TRIGGER_OUT_COUNT));
    LOG_I("pulse begin detect: %u\r\n", trigger_out_cnt);

    LOG_I("pulse end detect: %u\r\n", pulse_end_detect);
    for (uint16_t i = 0; i < 1024; i++)
    {
        LOG_I("%u ", one_pulse_count[i]);
    }
    LOG_I("\r\n");

    return 0;
}
MSH_CMD_EXPORT_ALIAS(radiation_trigger_out_count_get, radiation_trigger_out_count_get, radiation trigger out count get);

static int8_t radiation_trigger_out_count_clear(uint8_t argc, char **argv)
{
    trigger_out_cnt = 0;
    trigger_out_info_set(TRIGGER_OUT_COUNT, 0);

    pulse_end_detect = 0;
    memset(one_pulse_count, 0, sizeof(one_pulse_count));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(radiation_trigger_out_count_clear, radiation_trigger_out_count_clear, radiation trigger out count clear);
#endif

#ifdef RADIATION_EFFICIENCY_TEST
static int8_t radiation_efficiency_array_dump(uint8_t argc, char **argv)
{
    switch (atoi(argv[1]))
    {
    case 0:
        {
            adcs7476_sample_enable(0);

            struct radiation_index_data *obj = radiation_data_get();
            uint32_t time_expected = 0, time_actual = 0;

            for (uint16_t i = 1; i < 4095/* obj->beam_data->total_ri */; i++)
            {
                osMutexAcquire(obj->beam_data->mutex, osWaitForever);
                time_expected = obj->beam_data->radiation_data[i].time_expected;
                time_actual = ri_data_array[i + 1].timestamp - ri_data_array[i].timestamp;
                osMutexRelease(obj->beam_data->mutex);

                LOG_I("[ri: %4d]: time_expected: %d, time_actual: %d, diff: %d\r\n", i, time_expected, time_actual, time_expected - time_actual);
                osDelay(10);
            }

            adcs7476_sample_enable(1);
        }
        break;
    case 1:
        {
            adcs7476_sample_enable(0);

            struct radiation_index_data *obj = radiation_data_get();
            float dose_expected = 0.0f, dose_actual = 0.0f;
            uint64_t factor = radiation_data_value_get(DOSE_CALIBRATION_FACTOR);

            for (uint16_t i = 1; i < 4095/* obj->beam_data->total_ri */; i++)
            {
                osMutexAcquire(obj->beam_data->mutex, osWaitForever);
                dose_expected = obj->beam_data->radiation_data[i].dose_cumulative;
                dose_actual = (float)(ri_data_array[i + 1].dose_cumulated / (factor & 0xffffffff));
                osMutexRelease(obj->beam_data->mutex);

                LOG_I("[ri: %4d]: dose_expected: %11.6f, dose_actual: %11.6f, diff: %11.6f\r\n", i, dose_expected, dose_actual, dose_expected - dose_actual);
                osDelay(10);
            }

            adcs7476_sample_enable(1);
        }
        break;
    default:
        LOG_E("invalid argument: %d\r\n", atoi(argv[1]));
        break;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(radiation_efficiency_array_dump, radiation_efficiency_array_dump, dump radiation efficiency array);
static int8_t radiation_efficiency_array_clear(uint8_t argc, char **argv)
{
    memset(ri_data_array, 0, sizeof(ri_data_array));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(radiation_efficiency_array_clear, radiation_efficiency_array_clear, clear radiation efficiency array);
#endif
#endif
