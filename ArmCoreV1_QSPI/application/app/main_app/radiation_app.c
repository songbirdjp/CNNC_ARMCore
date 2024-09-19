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

#define USING_LPTIM3_FOR_RADIATION_TIMEOUT
#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
#include "lptim.h"
#endif

struct radiation_index_data
{
    struct control_para *control_data;
    struct beam_data *beam_data;
    uint16_t pulse_interval_us;

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
    DOSE_BEAM_METER = 0,
    DOSE_RADIATION_IDX,
    DOSE_RATE_RADIATION_IDX,
    DOSE_TIME_RADIATION_IDX,
    DOSE_INTERPOLATED_RADIATION_IDX,
    DOSE_RATE_INTERPOLATED_RADIATION_IDX,
    DOSE_GENERATION_MODE,
    PULSE_GENERATION_MODE,
    PULSE_INTERVAL,
    PULSE_INTERVAL_MIN,
    PULSE_SYMMETRY,
};
static uint64_t radiation_data_value_get(enum radiation_data_state state)
{
    uint64_t value = 0;
    struct radiation_index_data *obj = radiation_data_get();

    switch (state)
    {
    case DOSE_BEAM_METER:
        {
            osMutexAcquire(obj->control_data->mutex, osWaitForever);
            uint32_t factor = obj->control_data->calibration.adc_factor[0];
            osMutexRelease(obj->control_data->mutex);

            osMutexAcquire(obj->beam_data->mutex, osWaitForever);
            value = obj->beam_data->dose_meter * factor;
            osMutexRelease(obj->beam_data->mutex);
        }
        break;
    case DOSE_RADIATION_IDX:
        {
            osMutexAcquire(obj->control_data->mutex, osWaitForever);
            uint16_t idx = obj->control_data->radiation.index;
            uint32_t factor = obj->control_data->calibration.adc_factor[0];
            osMutexRelease(obj->control_data->mutex);

            osMutexAcquire(obj->beam_data->mutex, osWaitForever);
            value = (uint64_t)obj->beam_data->radiation_data[idx].dose_cumulative * factor;
            osMutexRelease(obj->beam_data->mutex);
        }
        break;
    case DOSE_RATE_RADIATION_IDX:
        {
            osMutexAcquire(obj->control_data->mutex, osWaitForever);
            uint16_t idx = obj->control_data->radiation.index;
            uint32_t factor = obj->control_data->calibration.adc_factor[0];
            osMutexRelease(obj->control_data->mutex);

            osMutexAcquire(obj->beam_data->mutex, osWaitForever);
            value = (uint64_t)obj->beam_data->radiation_data[idx].dose_rate * factor;
            osMutexRelease(obj->beam_data->mutex);            
        }
        break;
    case DOSE_TIME_RADIATION_IDX:
        {
            osMutexAcquire(obj->control_data->mutex, osWaitForever);
            uint16_t idx = obj->control_data->radiation.index;
            osMutexRelease(obj->control_data->mutex);

            osMutexAcquire(obj->beam_data->mutex, osWaitForever);
            value = (uint64_t)obj->beam_data->radiation_data[idx].time_expected;
            osMutexRelease(obj->beam_data->mutex);            
        }
        break;
    case DOSE_INTERPOLATED_RADIATION_IDX:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        value = obj->control_data->radiation.dose_interpolated;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_RATE_INTERPOLATED_RADIATION_IDX:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        value = obj->control_data->radiation.dose_rate_interpolated;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_GENERATION_MODE:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        value = obj->control_data->treatment.dose_mode;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_GENERATION_MODE:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        value = obj->control_data->treatment.pulse_mode;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_INTERVAL:
        {
            osMutexAcquire(obj->control_data->mutex, osWaitForever);
            uint8_t pulse_mode = obj->control_data->treatment.pulse_mode;
            uint8_t prf_hz = obj->control_data->treatment.prf_hz;
            osMutexRelease(obj->control_data->mutex);

            osMutexAcquire(obj->mutex, osWaitForever);
            value = (pulse_mode == 0) ? 1000000 / prf_hz : obj->pulse_interval_us;
            osMutexRelease(obj->mutex);
        }
        break;
    case PULSE_INTERVAL_MIN:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        value = obj->control_data->calibration.trig_interval_min;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_SYMMETRY:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        value = obj->control_data->interlock.threshold_symmetry;
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
    struct radiation_index_data *obj = radiation_data_get();

    switch (state)
    {
    case DOSE_INTERPOLATED_RADIATION_IDX:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        obj->control_data->radiation.dose_interpolated = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case DOSE_RATE_INTERPOLATED_RADIATION_IDX:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        obj->control_data->radiation.dose_rate_interpolated = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_INTERVAL:
        {
            osMutexAcquire(obj->control_data->mutex, osWaitForever);
            uint32_t interval_min = obj->control_data->calibration.trig_interval_min;
            osMutexRelease(obj->control_data->mutex);
            if (value < interval_min)
            {
                ret = -1;
                LOG_E("invalid pulse interval set: %llu\r\n", value);
                break;
            }
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->pulse_interval_us = value;
            osMutexRelease(obj->mutex);
        }
        break;
    default:
        ret = -1;
        LOG_E("invalid radiation data state: %d\r\n", state);
        break;
    }

    return ret;
}

#define ONE_PULSE_TIMEOUT_US    300
#define ADCS7476_SERVO_VALUE    50
#define ADCS7476_VALUE_ACCUMULATE_LIMIT 100

struct dose_value
{
    uint16_t cnt_one_pulse;         /* indicate valid pulse count, here 1cnt = 1us */
    uint32_t dose_one_pulse;
    uint64_t dose_accumulated;

    uint8_t one_pulse_complete;     /* for uart protocol */
    uint8_t one_beam_complete;

    osMutexId_t mutex;
};
static struct dose_value dose_value_object = {0};
static struct dose_value *dose_value_obj_get(void)
{
    return &dose_value_object;
}
static int8_t dose_value_status_update(enum pulse_state state, uint32_t pulse_cnt, uint32_t dose_value)
{
    int8_t ret = 0;
    struct dose_value *obj = dose_value_obj_get();
    osMutexAcquire(obj->mutex, osWaitForever);

    switch (state)
    {
    case ONE_PULSE_START:
        obj->cnt_one_pulse = 0;
        obj->dose_one_pulse = 0;
        obj->one_pulse_complete = 0;
        obj->one_beam_complete = 0;
        break;
    case ONE_PULSE_RUNNING:
        obj->cnt_one_pulse += pulse_cnt;
        obj->dose_one_pulse += dose_value;
        obj->dose_accumulated += dose_value;
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
int8_t dose_value_status_set(enum pulse_state state, uint64_t value)
{
    int8_t ret = 0;
    struct dose_value *obj = dose_value_obj_get();
    osMutexAcquire(obj->mutex, osWaitForever);

    switch (state)
    {
    case ONE_PULSE_COMPLETE:
        obj->one_pulse_complete = value;
        break;
    case ONE_PULSE_COUNT:
        obj->cnt_one_pulse = value;
        break;
    case ONE_PULSE_DOSE:
        obj->dose_one_pulse = value;
        break;
    case DOSE_ACCUMULATED:
        obj->dose_accumulated = value;
        break;
    default:
        ret = -1;
        LOG_E("invalid pulse state: %d\r\n", state);
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}
uint64_t dose_value_status_get(enum pulse_state state)
{
    uint64_t value = 0;
    struct dose_value *obj = dose_value_obj_get();
    osMutexAcquire(obj->mutex, osWaitForever);

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
        value = obj->dose_one_pulse;
        break;
    case DOSE_ACCUMULATED:
        value = obj->dose_accumulated;
        break;
    default:
        LOG_E("invalid dose value state: %d\r\n", state);
        break;
    }

    osMutexRelease(obj->mutex);

    return value;
}

#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
static uint8_t timer_delay_flag = 0;
#define TIM_DELAY_ONE_PULSE_TIMEOUT_US      (1 << 0)
#define TIM_DELAY_PULSE_INTERVAL_TIME_US    (1 << 1)
#define TIM_DELAY_NO_PULSE_INTERVAL_TIME_US (1 << 2)
#define TIM_DELAY_DUMMY_START_US            (1 << 3)
#define TIM_DELAY_RUNNING_FLAG_BIT_7        (1 << 7)
#define DUMMY_START_DELAY_TIME_US           (20000)
static osEventFlagsId_t timer_delay_event = NULL;
static void LPTIM3_Callback(LPTIM_HandleTypeDef *hlptim)
{
    HAL_StatusTypeDef status = HAL_LPTIM_SetOnce_Stop_IT(hlptim);
    if (status != HAL_OK)
    {
        LOG_E("HAL_LPTIM_SetOnce_Stop_IT err: %d\r\n", status);
    }

    osEventFlagsSet(timer_delay_event, timer_delay_flag & ~TIM_DELAY_RUNNING_FLAG_BIT_7);
}
static int8_t adcs7476_value_dummy(uint16_t value, uint8_t channel);
static int8_t time_delay_entry(void *argument)
{
    int8_t ret = 0;
    uint32_t event_flag = 0;

#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
    MX_LPTIM3_Init();
    HAL_StatusTypeDef status = HAL_LPTIM_RegisterCallback(&hlptim3, HAL_LPTIM_AUTORELOAD_MATCH_CB_ID, LPTIM3_Callback);
    if (status != HAL_OK)
    {
        LOG_E("HAL_LPTIM_RegisterCallback err: %d\r\n", status);
        return -1;
    }
#endif

    for (;;)
    {
        event_flag = osEventFlagsWait(timer_delay_event, TIM_DELAY_ONE_PULSE_TIMEOUT_US | TIM_DELAY_PULSE_INTERVAL_TIME_US | TIM_DELAY_NO_PULSE_INTERVAL_TIME_US | TIM_DELAY_DUMMY_START_US, osFlagsWaitAny, osWaitForever);
        if (event_flag & TIM_DELAY_ONE_PULSE_TIMEOUT_US)
        {
            if (dose_value_status_get(ONE_PULSE_COMPLETE) == 0)
            {
                ret = dose_value_status_update(ONE_PULSE_TIMEOUT, 0, 0);
                if (ret != 0)
                {
                    LOG_E("dose value status update err: %d\r\n", ret);
                }
            }
        }
        else if (event_flag & TIM_DELAY_PULSE_INTERVAL_TIME_US)
        {
            ret = dose_value_status_update(ONE_PULSE_START, 0, 0);
            if (ret != 0)
            {
                LOG_E("dose value status update err: %d\r\n", ret);
            }

            ret = dose_trigger_out_set(1);
            if (ret != 0)
            {
                LOG_E("dose_trigger_out_set err: %d\r\n", ret);
            }
        }
        else if (event_flag & TIM_DELAY_NO_PULSE_INTERVAL_TIME_US)
        {
            /* do nothing */
        }
        else if (event_flag & TIM_DELAY_DUMMY_START_US)
        {
            ret = adcs7476_sample_enable(1);
            if (ret != 0)
            {
                LOG_E("adcs7476_sample_enable err: %d\r\n", ret);
            }
        }

        timer_delay_flag = 0;
    }

    return 0;
}

static int8_t timer_delay_start(uint8_t type, uint16_t timeout_us)
{
    if (timeout_us * 1000 / 320 > 0xFFFF)
    {
        LOG_E("parameter err: %u\r\n", timeout_us);
        return -1;
    }

    if (timer_delay_flag & TIM_DELAY_RUNNING_FLAG_BIT_7)
    {
        // LOG_I("timer delay is running\r\n");
        return 0;
    }

    timer_delay_flag = TIM_DELAY_RUNNING_FLAG_BIT_7 | type;

    HAL_StatusTypeDef status = HAL_LPTIM_SetOnce_Start_IT(&hlptim3, timeout_us * 1000 / 320, timeout_us * 1000 / 320);
    if (status != HAL_OK)
    {
        LOG_E("HAL_LPTIM_SetOnce_Start_IT err: %d\r\n", status);
        return -3;
    }

    return 0;
}
#endif
/*****************************************************************/
static uint16_t adcs7476_value_check(uint16_t *buf, uint16_t len, uint64_t *pulse_val, uint32_t *servo_val)
{
    uint16_t valid_cnt = 0;
    int8_t ret = 0;

    for (uint16_t i = 0; i < len; i++)
    {
        if (buf[i] >= ADCS7476_VALUE_ACCUMULATE_LIMIT)
        {
            *pulse_val += buf[i] - ADCS7476_SERVO_VALUE;
#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
            ret = timer_delay_start(TIM_DELAY_ONE_PULSE_TIMEOUT_US, ONE_PULSE_TIMEOUT_US);
            if (ret != 0)
            {
                LOG_E("timer delay start err: %d\r\n", ret);
            }
#else
#endif
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

    return 0;
}

static int8_t adcs7476_value_servo(uint32_t value, uint16_t len, uint8_t channel)
{
    int8_t ret = 0;
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

    return 0;
}

static int8_t adcs7476_value_dose(uint32_t value, uint32_t value_1, uint16_t pulse_cnt)
{
    enum fsm_state fsm_stat = fsm_state_get();

    if (fsm_stat != FSM_STATE_RADIATION && fsm_stat != FSM_STATE_DUMMY)
    {
        return dose_value_status_update(ONE_PULSE_CLEAR, 0, 0);
    }

    /* check dose symmetry */
    uint32_t value_diff = abs(value - value_1);
    uint32_t value_max = (value > value_1) ? value : value_1;
    uint8_t percentage = radiation_data_value_get(PULSE_SYMMETRY);

    if (value_diff > value_max * percentage / 100)
    {
        LOG_I("dose symmetry fault, value_diff: %d, value_max: %d, percentage: %d\r\n", value_diff, value_max, percentage);
    }

    interlock_status_set(INTERLOCK_DOSE_SYMMETRY_FAULT, value_diff > value_max * percentage / 100);

    return dose_value_status_update(ONE_PULSE_RUNNING, pulse_cnt, value + value_1);
}

static int8_t dose_accumulated_check(uint16_t pulse_cnt)
{
    int8_t ret = 0;
    uint8_t dose_mode = radiation_data_value_get(DOSE_GENERATION_MODE);
    uint16_t one_pulse_cnt = dose_value_status_get(ONE_PULSE_COUNT);

    switch (dose_mode)
    {
    case 0: /* dummy */
        break;
    case 1: /* normal */
        if (one_pulse_cnt < ONE_PULSE_TIMEOUT_US && pulse_cnt >= BUF_LEN)
        {
            return 0;
        }

        /* 1. stop trigger out */
        ret = dose_trigger_out_set(0);
        if (ret != 0)
        {
            LOG_E("dose_trigger_out_set err: %d\r\n", ret);
            return ret;
        }

        /* 2. update one pulse status */
        ret = dose_value_status_update(ONE_PULSE_COMPLETE, 0, 0);
        if (ret != 0)
        {
            LOG_E("dose value status update err: %d\r\n", ret);
            return ret;
        }
        break;
    default:
        ret = -1;
        break;
    }

    /* 3. check total dose */
    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED);
    uint64_t dose_accumulated_target = radiation_data_value_get(DOSE_BEAM_METER);

#if 0
    static uint32_t cnt = 0;
    if (cnt++ % 1000 == 0)
    {
        LOG_I("dose accumulated: %llu, target: %llu\r\n", dose_accumulated_cur, dose_accumulated_target);
    }
#endif

    if (dose_accumulated_cur >= dose_accumulated_target)
    {
        LOG_I("end dose accumulated: %llu, target: %llu\r\n", dose_accumulated_cur, dose_accumulated_target);

        ret = dose_value_status_update(ONE_BEAM_COMPLETE, 0, 0);
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
        }

        return fsm_state_switch(FSM_STATE_STOP);
    }

    return ret;
}

static int8_t detect_whether_one_pulse_repeat(void)
{
    int8_t ret = 0;

    if (fsm_state_get() == FSM_STATE_DUMMY)
    {
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

#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
        ret = timer_delay_start(TIM_DELAY_DUMMY_START_US, DUMMY_START_DELAY_TIME_US);
        if (ret != 0)
        {
            LOG_E("timer delay start err: %d\r\n", ret);
            return ret;
        }
#endif
        return 0;
    }

    if (fsm_state_get() != FSM_STATE_RADIATION)
    {
        return 0;
    }

    uint8_t time_delay_type = 0;
    uint8_t pulse_mode = radiation_data_value_get(PULSE_GENERATION_MODE);
    uint64_t dose_radiation_index = radiation_data_value_get(DOSE_RADIATION_IDX);
    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED);
    uint16_t pulse_interval = radiation_data_value_get(PULSE_INTERVAL);
    
    if (pulse_mode == 0)    /* PRF */
    {
        time_delay_type = TIM_DELAY_PULSE_INTERVAL_TIME_US;
    }
    else    /* fixed dose rate */
    {
        if (dose_accumulated_cur < dose_radiation_index)
        {
            /* not reach radiation index dose yet, so judge whether reached interpolated dose */
            uint64_t dose_rate_interpolated = radiation_data_value_get(DOSE_RATE_INTERPOLATED_RADIATION_IDX);
            uint64_t dose_interpolated = radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX);
            if (dose_accumulated_cur < dose_interpolated)
            {
                time_delay_type = TIM_DELAY_PULSE_INTERVAL_TIME_US;
            }
            else
            {
                /* no need to generate a new pulse, just wait a cycle */
                time_delay_type = TIM_DELAY_NO_PULSE_INTERVAL_TIME_US;
            }

            /* calculate next interpolated dose */
            dose_interpolated += dose_rate_interpolated * pulse_interval;
            ret = radiation_data_value_set(DOSE_INTERPOLATED_RADIATION_IDX, dose_interpolated);
            if (ret != 0)
            {
                LOG_E("radiation data value set err: %d\r\n", ret);
                return ret;
            }
        }
        else
        {
            /* wait for next radiation index update from bgm */
            return 0;
        }
    }

    /* delay to prepare a new pulse */
#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
    ret = timer_delay_start(time_delay_type, pulse_interval);
    if (ret != 0)
    {
        LOG_E("timer delay start err: %d\r\n", ret);
        return ret;
    }
#endif

    return 0;
}

int8_t adcs7476_value_process(void)
{
    int8_t ret = 0;
    uint16_t buf[BUF_LEN] = {0}, buf_1[BUF_LEN] = {0}, pulse_cnt = 0, pulse_cnt_1 = 0;
    uint32_t servo_value = 0, servo_value_1 = 0;
    uint32_t pulse_value = 0, pulse_value_1 = 0;

    /* 1. read adcs7476 2 channel data */
    ret = adcs7476_object_data_read(ADCS7476_ID1_NAME, buf, BUF_LEN, osWaitForever);
    if (ret != 0)
    {
        LOG_E("adcs7476_object_data_read err: %d\r\n", ret);
        return ret;
    }

    ret = adcs7476_object_data_read(ADCS7476_ID2_NAME, buf_1, BUF_LEN, osWaitForever);
    if (ret != 0)
    {
        LOG_E("adcs7476_object_data_read err: %d\r\n", ret);
        return ret;
    }

    /* 2. check value */
    pulse_cnt = adcs7476_value_check(buf, BUF_LEN, &pulse_value, &servo_value);
    pulse_cnt_1 = adcs7476_value_check(buf_1, BUF_LEN, &pulse_value_1, &servo_value_1);
    if (pulse_cnt != pulse_cnt_1 && pulse_cnt != 0 && pulse_cnt_1 != 0)
    {
        LOG_E("adcs7476 value pulse count not match: %d, %d\r\n", pulse_cnt, pulse_cnt_1);
#if 0
        LOG_I("pulse_cnt: %d, pulse_cnt_1: %d\r\n", pulse_cnt, pulse_cnt_1);
        LOG_I("pulse_value: %d  pulse_value_1: %d\r\n", pulse_value, pulse_value_1);
        LOG_I("servo_value: %d, servo_value_1: %d\r\n", servo_value, servo_value_1);
#endif
        pulse_cnt = pulse_cnt <= pulse_cnt_1 ? pulse_cnt : pulse_cnt_1;
    }

#if 0
    static uint32_t cnt = 0;
    if (cnt++ % 1000 == 0)
    {
        LOG_I("buf: %d  buf_1: %d\r\n", buf[0], buf_1[0]);
        LOG_I("pulse_cnt: %d, pulse_cnt_1: %d\r\n", pulse_cnt, pulse_cnt_1);
    }
#endif

    uint16_t one_pulse_cnt = dose_value_status_get(ONE_PULSE_COUNT);

    if (one_pulse_cnt == 0)
    {
        if (pulse_cnt == 0 && pulse_cnt_1 == 0)
        {
            /* servo mode */
            ret = adcs7476_value_servo(servo_value, BUF_LEN, LTC2632_CHANNEL_OUTA);
            if (ret != 0)
            {
                LOG_E("adcs7476 value servo err: %d\r\n", ret);
            }
            ret = adcs7476_value_servo(servo_value_1, BUF_LEN, LTC2632_CHANNEL_OUTB);
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
        if (pulse_cnt < BUF_LEN || pulse_cnt_1 < BUF_LEN)
        {
            /* pulse mode end, servo mode begin */
            ret = adcs7476_value_dose(pulse_value, pulse_value_1, pulse_cnt);
            if (ret != 0)
            {
                LOG_E("adcs7476 value dose err: %d\r\n", ret);
            }

            ret = adcs7476_value_servo(servo_value, BUF_LEN - pulse_cnt, LTC2632_CHANNEL_OUTA);
            if (ret != 0)
            {
                LOG_E("adcs7476 value servo err: %d\r\n", ret);
            }
            ret = adcs7476_value_servo(servo_value_1, BUF_LEN - pulse_cnt_1, LTC2632_CHANNEL_OUTB);
            if (ret != 0)
            {
                LOG_E("adcs7476 value servo err: %d\r\n", ret);
            }
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

        ret = dose_accumulated_check(pulse_cnt);
        if (ret != 0)
        {
            LOG_E("dose accumulated check err: %d\r\n", ret);
        }
    }

    return ret;
}

static int8_t dose_interpolation_calculate(void)
{
    int8_t ret = 0;

    uint64_t dose_radiation_index = radiation_data_value_get(DOSE_RADIATION_IDX);
    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED);
    uint16_t time_radiation_index = radiation_data_value_get(DOSE_TIME_RADIATION_IDX);
    uint16_t pulse_interval_min = radiation_data_value_get(PULSE_INTERVAL_MIN);

    uint64_t dose_interpolated = 0;
    uint32_t dose_rate_interpolated = 0;

    time_radiation_index -= 100;    /* here reserve 100ms for safety */
    if (dose_accumulated_cur < dose_radiation_index)
    {
        dose_rate_interpolated = (dose_radiation_index - dose_accumulated_cur) / time_radiation_index / 1000;
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

    return 0;
} 
static int8_t dose_interpolation_init(void)
{
    return radiation_index_update_callback(dose_interpolation_calculate);
}

static int8_t interlock_fault_callback(void)
{
    // int8_t ret = fsm_state_switch(FSM_STATE_FAULT);
    // if (ret != 0)
    // {
    //     LOG_E("fsm state switch err: %d\r\n", ret);
    // }    

    // return ret;
    return 0;
}

static int8_t interlock_fault_process_init(void)
{
    return interlock_fault_register_callback(interlock_fault_callback);
}

static int8_t radiation_thread_init(void)
{
    osMutexAttr_t mutex_attributes = {
    .name = "data_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    dose_value_obj_get()->mutex = osMutexNew(&mutex_attributes);
    if (dose_value_obj_get()->mutex == NULL)
    {
        LOG_E("dose value mutex create failed\r\n");
        return -1;
    }

    osThreadAttr_t time_delay_thread_attributes = {
    .name = "time_delay_thread",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    osThreadId_t time_delay_threadHandle = osThreadNew(time_delay_entry, NULL, &time_delay_thread_attributes);
    if (time_delay_threadHandle == NULL)
    {
        LOG_E("thread time delay create failed\r\n");
        return -2;
    }

    timer_delay_event = osEventFlagsNew(NULL);
    if (timer_delay_event == NULL)
    {
        LOG_E("timer delay event create failed\r\n");
        return -3;
    }

    int8_t ret = radiation_data_init();
    if (ret != 0)
    {
        LOG_E("radiation data init err: %d\r\n", ret);
        return -4;
    }

    ret = dose_interpolation_init();
    if (ret != 0)
    {
        LOG_E("dose interpolation init err: %d\r\n", ret);
        return -5;
    }

    ret = interlock_fault_process_init();
    if (ret != 0)
    {
        LOG_E("interlock fault process init err: %d\r\n", ret);
        return -6;
    }

    return 0;
}
INIT_APP_EXPORT(radiation_thread_init);


#ifndef RADIATION_TEST
#include "shell.h"
static int8_t dose_dummy_mode_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;

    /* clear dose meter value */
    ret = dose_value_status_set(DOSE_ACCUMULATED, 0);
    if (ret != 0)
    {
        printf("dose value status set err: %d\r\n", ret);
        return ret;
    }

    /* 2. update dose meter value */
    ret = beam_data_value_set(0, BEAM_DOSE_METER, 0, 13000 * 10000);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }
    /* 3. update fsm state */
    ret = fsm_state_switch(FSM_STATE_DUMMY);
    if (ret != 0)
    {
        LOG_E("fsm state switch err: %d\r\n", ret);
        return ret;
    }   

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_dummy_mode_test, dose_dummy_mode_test, dose dummy mode test);

#endif
