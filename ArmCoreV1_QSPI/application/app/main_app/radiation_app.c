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

// #define RADIATION_SIMULATION_MODE
#ifdef RADIATION_SIMULATION_MODE
static uint8_t radiation_simulation_trigger_out_flag = 0;
uint16_t dose_simulated[BUF_LEN] = {0}, dose_simulated_1[BUF_LEN] = {0};
static int8_t radiation_simulation_dose_deal(uint16_t *buf, uint8_t len)
{
    if (buf == NULL || len == 0)
    {
        return -1;
    }

    if (dose_simulated[0] == 0)
    {
        for (uint16_t i = 0; i < BUF_LEN; i++)
        {
            dose_simulated[i] = i * 2 + 50;
            dose_simulated_1[i] =  (BUF_LEN - i - 1) * 2 + 50;
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
    DOSE_BOARD_ID = 0,
    DOSE_BEAM_METER,
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
    case DOSE_BOARD_ID:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        value = obj->control_data->board_id;
        osMutexRelease(obj->control_data->mutex);
        break;
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
    case DOSE_GENERATION_MODE:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        obj->control_data->treatment.dose_mode = value;
        osMutexRelease(obj->control_data->mutex);
        break;
    case PULSE_GENERATION_MODE:
        osMutexAcquire(obj->control_data->mutex, osWaitForever);
        obj->control_data->treatment.pulse_mode = value;
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

struct trigger_out
{
    uint8_t type;
    uint8_t flag;
    uint32_t interval_us;

    osMutexId_t mutex;
};
struct trigger_out trigger_out_object = {0};
static struct trigger_out *trigger_out_obj_get(void)
{
    return &trigger_out_object;
}
enum trigger_out_state
{
    TRIGGER_OUT_TYPE = 0,
    TRIGGER_OUT_FLAG,
    TRIGGER_OUT_INTERVAL,
    TRIGGER_OUT_MAX,
};
static int8_t trigger_out_info_set(enum trigger_out_state state, uint32_t value)
{
    int8_t ret = 0;
    struct trigger_out *obj = trigger_out_obj_get();
    osMutexAcquire(obj->mutex, osWaitForever);

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
    struct trigger_out *obj = trigger_out_obj_get();
    osMutexAcquire(obj->mutex, osWaitForever);

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

#define TRIGGER_OUT_COUNT_STATISTIC
#ifdef TRIGGER_OUT_COUNT_STATISTIC
uint32_t trigger_out_count = 0;
uint32_t trigger_out_cnt = 0;
#endif


#define USING_TIM5_FOR_RADIATION_TIMEOUT
#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
#include "tim.h"
#define TIM_DELAY_PULSE_INTERVAL_TIME_US    (1 << 0)
#define TIM_DELAY_NO_PULSE_INTERVAL_TIME_US (1 << 1)
#define TIM_DELAY_DUMMY_START_US            (1 << 2)
#define TIM_DELAY_PULSE_LEVEL_RESET_TIME_US (1 << 3)
#define TIM_DELAY_RUNNING_FLAG_BIT_7        (1 << 7)
#define NORMAL_PULSE_RESET_DELAY_TIME_US    (2000)
#define DUMMY_START_DELAY_TIME_US           (20000)
static uint8_t timer_delay_flag = 0;
static osEventFlagsId_t timer_delay_event = NULL;
static void PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    HAL_StatusTypeDef status = HAL_TIM_Base_Stop_IT(htim);
    if (status != HAL_OK)
    {
        LOG_E("HAL_TIM_Base_Stop_IT err: %d\r\n", status);
    }

    osEventFlagsSet(timer_delay_event, timer_delay_flag & ~TIM_DELAY_RUNNING_FLAG_BIT_7);
}
static int8_t timer_delay_start(uint8_t type, uint32_t timeout_us)
{
    if (timer_delay_flag & TIM_DELAY_RUNNING_FLAG_BIT_7)
    {
        // LOG_I("timer delay is running\r\n");
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
static int8_t dose_interpolation_check(uint8_t *time_delay_type);
static int8_t time_delay_entry(void *argument)
{
    int8_t ret = 0;
    uint8_t type = 0;
    uint32_t event_flag = 0;

#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
    MX_TIM5_Init();
    HAL_StatusTypeDef status = HAL_TIM_RegisterCallback(&htim5, HAL_TIM_PERIOD_ELAPSED_CB_ID, PeriodElapsedCallback);
    if (status != HAL_OK)
    {
        LOG_E("HAL_TIM_RegisterCallback err: %d\r\n", status);
        return -1;
    }
#endif

    for (;;)
    {
        event_flag = osEventFlagsWait(timer_delay_event, TIM_DELAY_PULSE_INTERVAL_TIME_US | TIM_DELAY_NO_PULSE_INTERVAL_TIME_US | TIM_DELAY_DUMMY_START_US | TIM_DELAY_PULSE_LEVEL_RESET_TIME_US, osFlagsWaitAny, osWaitForever);
        timer_delay_flag = 0;
        if (event_flag & TIM_DELAY_PULSE_INTERVAL_TIME_US)
        {
            if (fsm_state_get() != FSM_STATE_RADIATION || trigger_out_info_get(TRIGGER_OUT_FLAG) == 0)
            {
                continue;
            }

            ret = dose_value_status_update(ONE_PULSE_START, 0, 0);
            if (ret != 0)
            {
                LOG_E("dose value status update err: %d\r\n", ret);
            }
            if (radiation_data_value_get(DOSE_BOARD_ID) == DOSE_BOARD_TRIGGER_OUT)
            {
                ret = dose_trigger_out_set(0);
                if (ret != 0)
                {
                    LOG_E("dose_trigger_out_set err: %d\r\n", ret);
                }
#ifdef TRIGGER_OUT_COUNT_STATISTIC
                trigger_out_count++;
#endif

#ifdef DETECT_RADIATION_TIME_FROM_TRIGGER_OUT
                system_time_get(&begin_time);
#endif
                /* reset trigger out level */                
                ret = timer_delay_start(TIM_DELAY_PULSE_LEVEL_RESET_TIME_US, NORMAL_PULSE_RESET_DELAY_TIME_US);
                if (ret != 0)
                {
                    LOG_E("timer delay start err: %d\r\n", ret);
                }

#ifdef RADIATION_SIMULATION_MODE
                radiation_simulation_trigger_out_flag = 1;
#endif
            }
        }
        else if (event_flag & TIM_DELAY_PULSE_LEVEL_RESET_TIME_US)
        {
            if (radiation_data_value_get(DOSE_BOARD_ID) == DOSE_BOARD_TRIGGER_OUT)
            {
#ifdef RADIATION_FIX_RATE_SIMULATE
                ret = adcs7476_value_dose(10000, 10000, 100);
#endif
                ret = dose_trigger_out_set(1);
                if (ret != 0)
                {
                    LOG_E("dose_trigger_out_set err: %d\r\n", ret);
                }
                /* prepare for next pulse */
                if (fsm_state_get() == FSM_STATE_RADIATION && trigger_out_info_get(TRIGGER_OUT_FLAG) == 1)
                {
#ifdef RADIATION_FIX_RATE_SIMULATE
                    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED);
                    uint64_t dose_interpolated = radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX);
                    LOG_I("dose_accumulated_cur: %llu, dose_interpolated: %llu\r\n", dose_accumulated_cur, dose_interpolated);
#endif
                    ret = dose_interpolation_check(&type);
                    if (ret != 0)
                    {
                        LOG_E("dose interpolation check err: %d\r\n", ret);
                    }

                    // type == 1 ? LOG_I("next: trigger a new pulse\r\n") : LOG_I("next: trigger no pulse\r\n");
                    ret = trigger_out_info_set(TRIGGER_OUT_TYPE, type);
                    if (ret != 0)
                    {
                        LOG_E("trigger out info set err: %d\r\n", ret);
                    }

                    ret = timer_delay_start(trigger_out_info_get(TRIGGER_OUT_TYPE), trigger_out_info_get(TRIGGER_OUT_INTERVAL) - NORMAL_PULSE_RESET_DELAY_TIME_US);
                    if (ret != 0)
                    {
                        LOG_E("timer delay start err: %d\r\n", ret);
                    }
                }
            }
        }
        else if (event_flag & TIM_DELAY_NO_PULSE_INTERVAL_TIME_US)
        {
            /* prepare for next pulse */
            if (fsm_state_get() == FSM_STATE_RADIATION && trigger_out_info_get(TRIGGER_OUT_FLAG) == 1)
            {
#ifdef RADIATION_FIX_RATE_SIMULATE
                uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED);
                uint64_t dose_interpolated = radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX);
                LOG_I("dose_accumulated_cur: %llu, dose_interpolated: %llu\r\n", dose_accumulated_cur, dose_interpolated);
#endif
                ret = dose_interpolation_check(&type);
                if (ret != 0)
                {
                    LOG_E("dose interpolation check err: %d\r\n", ret);
                }

                // type == 1 ? LOG_I("next: trigger a new pulse\r\n") : LOG_I("next: trigger no pulse\r\n");
                ret = trigger_out_info_set(TRIGGER_OUT_TYPE, type);
                if (ret != 0)
                {
                    LOG_E("trigger out info set err: %d\r\n", ret);
                }

                ret = timer_delay_start(trigger_out_info_get(TRIGGER_OUT_TYPE), trigger_out_info_get(TRIGGER_OUT_INTERVAL));
                if (ret != 0)
                {
                    LOG_E("timer delay start err: %d\r\n", ret);
                }
            }
        }
        else if (event_flag & TIM_DELAY_DUMMY_START_US)
        {
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
            if (dose_value_status_get(ONE_PULSE_COMPLETE) == 0)
            {
                ret = dose_value_status_update(ONE_PULSE_TIMEOUT, 0, 0);
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
static uint16_t adcs7476_value_check(uint16_t *buf, uint16_t len, uint64_t *pulse_val, uint32_t *servo_val)
{
    uint16_t valid_cnt = 0;
    int8_t ret = 0;

    for (uint16_t i = 0; i < len; i++)
    {
        if (buf[i] >= ADCS7476_VALUE_ACCUMULATE_LIMIT)
        {
            *pulse_val += buf[i] - ADCS7476_SERVO_VALUE;

            if(buf[i] >= 4096)
            {
                LOG_I("adcs7476 value err: %d\r\n", buf[i]);
            }

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
        // LOG_I("dose symmetry fault, value_diff: %d, value_max: %d, percentage: %d\r\n", value_diff, value_max, percentage);
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

#ifdef RADIATION_SIMULATION_MODE
        radiation_simulation_trigger_out_flag = 0;
#endif

        /* 1. update one pulse status */
        ret = dose_value_status_update(ONE_PULSE_COMPLETE, 0, 0);
        if (ret != 0)
        {
            LOG_E("dose value status update err: %d\r\n", ret);
            return ret;
        }
        // LOG_I("one pulse complete, pulse dose: %llu\r\n", dose_value_status_get(ONE_PULSE_DOSE));
        break;
    default:
        ret = -1;
        break;
    }

    /* 2. check total dose */
    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED);
    uint64_t dose_accumulated_target = radiation_data_value_get(DOSE_BEAM_METER);

    uint64_t board_id = radiation_data_value_get(DOSE_BOARD_ID);

    #define DOSE_BOARD_NO_TRIGGER_OUT_SCALE 1.1
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

#ifdef NONE_DOSE_ACCELERATION_MODE
    return 0;
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

            ret = fsm_state_switch(FSM_STATE_DUMMY_END);
        }
        else
        {
            ret = (board_id == DOSE_BOARD_NO_TRIGGER_OUT) ? LOG_E("detected dose reached upper limit\r\n"), fsm_state_switch(FSM_STATE_FAULT) : fsm_state_switch(FSM_STATE_COMPLETE);
        }
    }

    return ret;
}

static int8_t dose_interpolation_check(uint8_t *time_delay_type)
{
    int8_t ret = 0;
    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED);
    uint64_t dose_radiation_index = radiation_data_value_get(DOSE_RADIATION_IDX);

    if (dose_accumulated_cur < dose_radiation_index)
    {
        /* not reach radiation index dose yet, so judge whether reached interpolated dose */
        uint32_t pulse_interval = radiation_data_value_get(PULSE_INTERVAL);
        uint64_t dose_rate_interpolated = radiation_data_value_get(DOSE_RATE_INTERPOLATED_RADIATION_IDX);
        uint64_t dose_interpolated = radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX);

        if (dose_accumulated_cur < dose_interpolated)
        {
            *time_delay_type = TIM_DELAY_PULSE_INTERVAL_TIME_US;
        }
        else
        {
            /* no need to generate a new pulse, just wait a cycle */
            *time_delay_type = TIM_DELAY_NO_PULSE_INTERVAL_TIME_US;
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
        *time_delay_type = TIM_DELAY_NO_PULSE_INTERVAL_TIME_US;
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

#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
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
#if 0
        timer_delay_flag = 0;
        lptim3_delay_flag = 0;
#endif
        ret = trigger_out_info_set(TRIGGER_OUT_FLAG, 0);
        if (ret != 0)
        {
            LOG_E("trigger out info set err: %d\r\n", ret);
            return ret;
        }
        return 0;
    }

    /* delay to prepare a new pulse */
#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
    if (trigger_out_info_get(TRIGGER_OUT_FLAG) == 0)
    {
        uint8_t time_delay_type = 0;
        uint8_t pulse_mode = radiation_data_value_get(PULSE_GENERATION_MODE);
        uint32_t pulse_interval = radiation_data_value_get(PULSE_INTERVAL);
        
        if (pulse_mode == 0)    /* PRF */
        {
            time_delay_type = TIM_DELAY_PULSE_INTERVAL_TIME_US;
        }
        else    /* fixed dose rate */
        {
            ret = dose_interpolation_check(&time_delay_type);
            if (ret != 0)
            {
                LOG_E("dose interpolation check err: %d\r\n", ret);
                return ret;
            }
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
        ret = timer_delay_start(time_delay_type, pulse_interval);
        if (ret != 0)
        {
            LOG_E("timer delay start err: %d\r\n", ret);
            return ret;
        }
    }
#endif

    return 0;
}

#define ADCS7476_DATA_DUMP
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

#ifdef RADIATION_SIMULATION_MODE
    if (radiation_simulation_trigger_out_flag)
    {
        radiation_simulation_dose_deal(buf, BUF_LEN);
        radiation_simulation_dose_deal(buf_1, BUF_LEN);
    }
#endif

    /* 2. check value */
    pulse_cnt = adcs7476_value_check(buf, BUF_LEN, &pulse_value, &servo_value);
    pulse_cnt_1 = adcs7476_value_check(buf_1, BUF_LEN, &pulse_value_1, &servo_value_1);
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
    if (pulse_cnt != 0 && pulse_cnt <= BUF_LEN)
    {
        ret = adcs7476_data_save(0, buf, BUF_LEN);
        ret = adcs7476_data_save(1, buf_1, BUF_LEN);
        if (ret != 0)
        {
            LOG_E("adcs7476_data_save err: %d\r\n", ret);
            return ret;
        }
    }
#endif

#if 0
    static uint32_t cnt = 0;
    if (cnt++ % 1000 == 0)
    {
        LOG_I("buf: %d  buf_1: %d\r\n", buf[0], buf_1[0]);
        LOG_I("pulse_cnt: %d, pulse_cnt_1: %d\r\n", pulse_cnt, pulse_cnt_1);
    }
#endif

    uint16_t one_pulse_cnt = dose_value_status_get(ONE_PULSE_COUNT);

#ifdef DETECT_RADIATION_TIME_FROM_TRIGGER_OUT
    if (one_pulse_cnt == 0 && pulse_cnt != 0)
    {
        system_time_get(&end_time);
        // LOG_I("radiation delayed from trigger out: %u us\r\n", time_diff_us(&begin_time, &end_time));
    }
#endif

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
            // LOG_I("---one pulse begin---\r\n");
#ifdef TRIGGER_OUT_COUNT_STATISTIC
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
        if (pulse_cnt < BUF_LEN || pulse_cnt_1 < BUF_LEN)
        {
            // LOG_I("---one pulse end---\r\n");
            /* pulse mode end, servo mode begin */
            ret = adcs7476_value_dose(pulse_value, pulse_value_1, pulse_cnt);
            if (ret != 0)
            {
                LOG_E("adcs7476 value dose err: %d\r\n", ret);
            }

#if 0       /* reduce mcu work load, so value servo delayed maximum is 100 sample cycles */
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
    uint32_t time_radiation_index = radiation_data_value_get(DOSE_TIME_RADIATION_IDX);
    uint16_t pulse_interval_min = radiation_data_value_get(PULSE_INTERVAL_MIN);

    uint32_t dose_rate_interpolated = 0;
    // uint64_t dose_interpolated = 0;

#if 0
    LOG_I("dose_radiation_index: %llu\r\n", dose_radiation_index);
    LOG_I("dose_accumulated_cur: %llu\r\n", dose_accumulated_cur);
    LOG_I("time_radiation_index: %u\r\n", time_radiation_index);
    LOG_I("pulse_interval_min: %u\r\n", pulse_interval_min);
#endif

    time_radiation_index -= 100;    /* here reserve 100ms for safety */
    if (dose_accumulated_cur < dose_radiation_index)
    {
        dose_rate_interpolated = (dose_radiation_index - dose_accumulated_cur) / time_radiation_index;
        // dose_interpolated = dose_accumulated_cur + dose_rate_interpolated * pulse_interval_min;
        ret = radiation_data_value_set(DOSE_RATE_INTERPOLATED_RADIATION_IDX, dose_rate_interpolated);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }
        // ret = radiation_data_value_set(DOSE_INTERPOLATED_RADIATION_IDX, dose_interpolated);
        // if (ret != 0)
        // {
        //     LOG_E("radiation data value set err: %d\r\n", ret);
        //     return ret;
        // }
        ret = radiation_data_value_set(PULSE_INTERVAL, pulse_interval_min);
        if (ret != 0)
        {
            LOG_E("radiation data value set err: %d\r\n", ret);
            return ret;
        }

#if 0
        LOG_I("dose_rate_interpolated: %u\r\n", dose_rate_interpolated);
        // LOG_I("dose_interpolated: %llu\r\n", dose_interpolated);
#endif

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
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityHigh7,
    };

#ifdef USING_TIM5_FOR_RADIATION_TIMEOUT
    timer_delay_event = osEventFlagsNew(NULL);
    if (timer_delay_event == NULL)
    {
        LOG_E("timer delay event create failed\r\n");
        return -3;
    }

    osThreadId_t time_delay_threadHandle = osThreadNew(time_delay_entry, NULL, &time_delay_thread_attributes);
    if (time_delay_threadHandle == NULL)
    {
        LOG_E("thread time delay create failed\r\n");
        return -2;
    }
#endif

#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
    lptim3_delay_event = osEventFlagsNew(NULL);
    if (lptim3_delay_event == NULL)
    {
        LOG_E("lptim3 delay event create failed\r\n");
        return -3;
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
        return -2;
    }
#endif

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
    ret = beam_data_value_set(0, BEAM_DOSE_METER, 0, 100);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }
    /* 3. update fsm state */
    enum fsm_state state = atoi(argv[1]);   /* FSM_STATE_DUMMY: 2   FSM_STATE_RADIATION: 6 */

    if (state == FSM_STATE_DUMMY)
    {
        ret = radiation_data_value_set(DOSE_GENERATION_MODE, 0);
        ret |= radiation_data_value_set(PULSE_GENERATION_MODE, 0);
    }
    else if (state == FSM_STATE_RADIATION)
    {
        ret = radiation_data_value_set(DOSE_GENERATION_MODE, 1);
        ret |= radiation_data_value_set(PULSE_GENERATION_MODE, 0);
    }

    // ret = fsm_state_switch(state);
    // if (ret != 0)
    // {
    //     LOG_E("fsm state switch err: %d\r\n", ret);
    //     return ret;
    // }   

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_dummy_mode_test, dose_dummy_mode_test, dose dummy mode test);

static int8_t dose_radiation_mode_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;

    /* 1. clear dose meter value */
    ret = dose_value_status_set(DOSE_ACCUMULATED, 0);
    if (ret != 0)
    {
        printf("dose value status set err: %d\r\n", ret);
        return ret;
    }

    /* 2. update dose meter value */
    ret = beam_data_value_set(0, BEAM_DOSE_METER, 0, 100);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }

    /* 3. update fsm state */
    enum fsm_state state = atoi(argv[1]);   /* FSM_STATE_DUMMY: 2   FSM_STATE_RADIATION: 6 */

    if (state == FSM_STATE_DUMMY)
    {
        ret = radiation_data_value_set(DOSE_GENERATION_MODE, 0);
        ret |= radiation_data_value_set(PULSE_GENERATION_MODE, 0);
    }
    else if (state == FSM_STATE_RADIATION)
    {
        ret = radiation_data_value_set(DOSE_GENERATION_MODE, 1);
        ret |= radiation_data_value_set(PULSE_GENERATION_MODE, 0);
    }

    return ret;
}
MSH_CMD_EXPORT_ALIAS(dose_radiation_mode_test, dose_radiation_mode_test, dose radiation mode test);



static int8_t dose_interpolation_index_data_get(uint8_t argc, char **argv)
{
    uint64_t dose_accumulated_cur = dose_value_status_get(DOSE_ACCUMULATED);
    uint64_t dose_radiation_index = radiation_data_value_get(DOSE_RADIATION_IDX);

    LOG_I("dose_accumulated_cur: %llu\r\n", dose_accumulated_cur);
    LOG_I("dose_radiation_index: %llu\r\n", dose_radiation_index);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_interpolation_index_data_get, dose_interpolation_index_data_get, dose interpolation index data get);

static int8_t dose_interpolation_data_get(uint8_t argc, char **argv)
{
    int8_t ret = 0;

    LOG_I("beam dose meter: %f\r\n", beam_data_value_get(0, BEAM_DOSE_METER, 0));
    LOG_I("beam dose rate: %f\r\n", beam_data_value_get(0, BEAM_DOSE_RATE, 0));
    LOG_I("beam total cp: %f\r\n", beam_data_value_get(0, BEAM_TOTAL_CP, 0));
    LOG_I("beam total ri: %f\r\n", beam_data_value_get(0, BEAM_TOTAL_RI, 0));

    for (uint8_t i = 0; i < beam_data_value_get(0, BEAM_TOTAL_RI, 0); i++)
    {
        LOG_I("beam ri[%d] dose rate: %f\r\n", i, beam_data_value_get(0, BEAM_RI_DOSE_RATE, i));
        LOG_I("beam ri[%d] dose cumulative: %f\r\n", i, beam_data_value_get(0, BEAM_RI_CUMULATIVE, i));
        LOG_I("beam ri[%d] time expected: %f\r\n", i, beam_data_value_get(0, BEAM_RI_TIME_EXPECTED, i));
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_interpolation_data_get, dose_interpolation_data_get, dose interpolation data get);

static int8_t dose_interpolation_init_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;

    /* 1. clear dose meter value */
    ret = dose_value_status_set(DOSE_ACCUMULATED, 0);
    if (ret != 0)
    {
        printf("dose value status set err: %d\r\n", ret);
        return ret;
    }    

    /* 2. set dose meter value */
    ret = beam_data_value_set(0, BEAM_DOSE_METER, 0, 10);
    if (ret != 0)
    {
        LOG_E("radiation data value set err: %d\r\n", ret);
        return ret;
    }

    /* 3. set plan data */
    #define TOTAL_CP    5
    #define TOTAL_RI    10
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

    for (uint8_t i = 0; i < TOTAL_RI; i++)
    {
        ret = beam_data_value_set(0, BEAM_RI_CUMULATIVE, i, i + 1);
        if (ret != 0)
        {
            LOG_E("beam data ri cumulative set err: %d\r\n", ret);
        }
        ret = beam_data_value_set(0, BEAM_RI_DOSE_RATE, i, 5);
        if (ret != 0)
        {
            LOG_E("beam data ri dose rate set err: %d\r\n", ret);
        }
        ret = beam_data_value_set(0, BEAM_RI_TIME_EXPECTED, i, 1000); /* unit: second */
        if (ret != 0)
        {
            LOG_E("beam data ri time expected set err: %d\r\n", ret);
        }
    }

    ret = dose_interpolation_calculate();
    if (ret!= 0)
    {
        LOG_E("dose interpolation calculate err: %d\r\n", ret);
    }

    /* 4. set mode */
    ret = radiation_data_value_set(DOSE_GENERATION_MODE, 1);
    ret |= radiation_data_value_set(PULSE_GENERATION_MODE, 1);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_interpolation_init_test, dose_interpolation_init_test, dose interpolation init test);

static int8_t dose_interpolation_radiation_index_set(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct radiation_index_data *obj = radiation_data_get();

    uint8_t index = atoi(argv[1]);

    osMutexAcquire(obj->control_data->mutex, osWaitForever);
    obj->control_data->radiation.index = index;
    obj->control_data->radiation.cp = beam_data_value_get(0, BEAM_RI_IN_CP, obj->control_data->radiation.index);
    obj->control_data->radiation.index_max_in_cp = beam_data_value_get(0, BEAM_RI_IN_CP_MAX, obj->control_data->radiation.index);
    osMutexRelease(obj->control_data->mutex);

    ret = dose_interpolation_calculate();
    if (ret!= 0)
    {
        LOG_E("dose interpolation calculate err: %d\r\n", ret);
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_interpolation_radiation_index_set, dose_interpolation_radiation_index_set, dose interpolation radiation index set);



static int8_t fsm_state_current_get(uint8_t argc, char **argv)
{
    LOG_I("fsm state current: %d\r\n", fsm_state_get());

    return 0;
}
MSH_CMD_EXPORT_ALIAS(fsm_state_current_get, fsm_state_current_get, fsm state current get);

static int8_t fsm_state_current_set(uint8_t argc, char **argv)
{
    enum fsm_state state = atoi(argv[1]);
    /*     
    0：FSM_STATE_INIT,
    1：FSM_STATE_IDLE,
    2：FSM_STATE_DUMMY,
    3: FSM_STATE_DUMMY_END,
    4：FSM_STATE_PREPARE,
    5：FSM_STATE_READY,
    6：FSM_STATE_RADIATION,
    7：FSM_STATE_COMPLETE,
    8：FSM_STATE_FAULT, */
    
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
    return timer_delay_start(TIM_DELAY_PULSE_INTERVAL_TIME_US, atoi(argv[1]));
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
    LOG_I("dose meter target: %llu\r\n", radiation_data_value_get(DOSE_BEAM_METER));
    LOG_I("dose radiation index: %llu\r\n", radiation_data_value_get(DOSE_RADIATION_IDX));
    LOG_I("dose rate radiation index: %llu\r\n", radiation_data_value_get(DOSE_RATE_RADIATION_IDX));
    LOG_I("dose time radiation index: %llu\r\n", radiation_data_value_get(DOSE_TIME_RADIATION_IDX));
    LOG_I("dose interpolated radiation index: %llu\r\n", radiation_data_value_get(DOSE_INTERPOLATED_RADIATION_IDX));
    LOG_I("dose rate interpolated radiation index: %llu\r\n", radiation_data_value_get(DOSE_RATE_INTERPOLATED_RADIATION_IDX));
    LOG_I("dose generation mode: %llu\r\n", radiation_data_value_get(DOSE_GENERATION_MODE));
    LOG_I("pulse generation mode: %llu\r\n", radiation_data_value_get(PULSE_GENERATION_MODE));
    LOG_I("pulse interval: %llu us\r\n", radiation_data_value_get(PULSE_INTERVAL));
    LOG_I("pulse interval min: %llu us\r\n", radiation_data_value_get(PULSE_INTERVAL_MIN));
    LOG_I("pulse symmetry: %llu\r\n", radiation_data_value_get(PULSE_SYMMETRY));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(beam_data_info_get, beam_data_info_get, beam data info get);

static int8_t one_pulse_dose_get(uint8_t argc, char **argv)
{
    LOG_I("one pulse timeout: %llu\r\n", dose_value_status_get(ONE_PULSE_COUNT));
    LOG_I("one pulse flag: %llu\r\n", dose_value_status_get(ONE_PULSE_COMPLETE));
    LOG_I("one pulse dose: %llu\r\n", dose_value_status_get(ONE_PULSE_DOSE));
    LOG_I("dose cumulated: %llu\r\n", dose_value_status_get(DOSE_ACCUMULATED));

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

static int8_t radiation_trigger_out_time_get(uint8_t argc, char **argv)
{
    LOG_I("trigger_out_time: %u %u\r\n", trigger_out_cnt, trigger_out_count);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(radiation_trigger_out_time_get, radiation_trigger_out_time_get, radiation trigger out time get);

static int8_t radiation_trigger_out_time_clear(uint8_t argc, char **argv)
{
    trigger_out_cnt = 0;
    trigger_out_count = 0;

    return 0;
}
MSH_CMD_EXPORT_ALIAS(radiation_trigger_out_time_clear, radiation_trigger_out_time_clear, radiation trigger out time clear);
#endif
