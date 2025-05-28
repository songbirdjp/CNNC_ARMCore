#include "bgm_app.h"
#include "bgm_def.h"
#include "init_call.h"
#include "tim.h"
#include "ulog.h"
#include "adc_app.h"

// #define BGM_FSM_STATE_SIMULATION
#ifdef BGM_FSM_STATE_SIMULATION
static enum dose_fsm_state dose1_fsm_state = DOSE_FSM_STATE_MAX, dose2_fsm_state = DOSE_FSM_STATE_MAX;
#endif

#define DOSE_NONE_FLAG          (0 << 0)
#define DOSE_PRELIMINARY_FLAG   (1 << 0)
#define DOSE_COMPLETE_FLAG      (1 << 1)

void TriggerConfig_SetARR(TIM_HandleTypeDef *htim, uint32_t arr_value)
{
    __HAL_TIM_SET_AUTORELOAD(htim, arr_value);
}

static struct bgm_data_info bgm_info = 
{
    .fsm_state = BGM_STATE_INIT,
    .cali_mode = 0,
    .cali_prf = 10,
    .cali_dose1_adc = 1213875,
    .cali_dose1_dac = 30,
    .cali_dose2_adc = 1398015,
    .cali_dose2_dac = 30,
    .dose_meter = 100.0,
    .dose_meter_dummy = 300.0
};

struct bgm_data_info *bgm_data_info_get(void)
{
    return &bgm_info;
}

static osEventFlagsId_t trigger_out_event_flag = NULL;
#define TRIGGER_OUT_EVENT_FLAG  (1 << 0)
static uint32_t trigger_out_cnt = 0;
#include "shell.h"
static int8_t trigger_out_cnt_get(void)
{
    LOG_I("trigger_out_cnt: %d\r\n", trigger_out_cnt);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(trigger_out_cnt_get, trigger_out_cnt_get, get trigger out count);
static void trigger_out_cnt_clear(void)
{
    trigger_out_cnt = 0;
}
MSH_CMD_EXPORT_ALIAS(trigger_out_cnt_clear, trigger_out_cnt_clear, clear trigger out count);

static void PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    osEventFlagsSet(trigger_out_event_flag, TRIGGER_OUT_EVENT_FLAG);
    trigger_out_cnt++;
}
static void trigger_out_distribute_init(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    MX_TIM1_Init(); /* PA8 -> TIM1 */
    MX_TIM3_Init(); /* PC7 -> QAM */
    MX_TIM4_Init(); /* PD13 -> MOD */
    MX_TIM5_Init(); /* get dose accumulated */
    MX_TIM23_Init();/* PG14 -> AFC */

    status = HAL_TIM_RegisterCallback(&htim5, HAL_TIM_PERIOD_ELAPSED_CB_ID, PeriodElapsedCallback);
    if (status != HAL_OK)
    {
        LOG_E("HAL_TIM_RegisterCallback err: %d\r\n", status);
    }

    status |= HAL_TIM_Base_Stop(&htim1);
    status |= HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
    status |= HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
    status |= HAL_TIM_PWM_Stop(&htim23, TIM_CHANNEL_3);
    status |= HAL_TIM_Base_Stop_IT(&htim5);
    if (status != HAL_OK)
    {
        LOG_E("trigger out distribute init err: %d\r\n", status);
    }
}
static void trigger_out_enable(uint8_t en)
{
    en == 0 ? HAL_TIM_Base_Stop(&htim1) : HAL_TIM_Base_Start(&htim1);
    en == 0 ? HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2) : HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    en == 0 ? HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2) : HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    en == 0 ? HAL_TIM_PWM_Stop(&htim23, TIM_CHANNEL_3) : HAL_TIM_PWM_Start(&htim23, TIM_CHANNEL_3);
    // en == 0 ? HAL_TIM_Base_Stop_IT(&htim5) : HAL_TIM_Base_Start_IT(&htim5);
}
static int8_t trigger_out_entry(void *argument)
{
    int8_t ret = 0;

    trigger_out_distribute_init();

    for (;;)
    {
        osEventFlagsWait(trigger_out_event_flag, TRIGGER_OUT_EVENT_FLAG, osFlagsWaitAny, osWaitForever);

        ret = dose_radiation_data_get(BGM_UART_DOSE1);
        ret |= dose_radiation_data_get(BGM_UART_DOSE2);
        if (ret != 0)
        {
            LOG_E("dose_radiation_data_get err: %d\r\n", ret);
        }
    }

    return 0;
}

enum fsm_source_t
{
    FSM_SOURCE_REMOTE = 0,  /* from master cmd */
    FSM_SOURCE_LOCAL,       /* from slave feedback */
    FSM_SOURCE_MAX
};

static int8_t fsm_switch_check(enum bgm_fsm_state state_current, enum bgm_fsm_state state_request)
{
    if (state_request >= BGM_STATE_MAX)
    {
        LOG_E("invalid state request: %d\r\n", state_request);
        return -1;
    }

    if (state_request == state_current)
    {
        return 0;
    }

    int8_t ret = 0;

    switch (state_current)
    {
    case BGM_STATE_INIT:
        if (state_request != BGM_STATE_IDLE && state_request != BGM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_IDLE:
        if (state_request != BGM_STATE_INIT && state_request != BGM_STATE_PRELIMINARY && 
            state_request != BGM_STATE_PARK && state_request != BGM_STATE_MANUAL && 
            state_request != BGM_STATE_POWERSAVER && state_request != BGM_STATE_SHUTDOWN && 
            state_request != BGM_STATE_TERMINATE)
        {
                ret = -1;
        }
        break;
    case BGM_STATE_PRELIMINARY:
        if (state_request != BGM_STATE_PREPARE && state_request != BGM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_PREPARE:
        if (state_request != BGM_STATE_READY && state_request != BGM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_READY:
        if (state_request != BGM_STATE_WORK && state_request != BGM_STATE_INTERRUPT && state_request != BGM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_WORK:
        if (state_request != BGM_STATE_COMPLETE && state_request != BGM_STATE_INTERRUPT && state_request != BGM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_PARK:
        if (state_request != BGM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_MANUAL:
        if (state_request != BGM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_COMPLETE:
        if (state_request != BGM_STATE_IDLE && state_request != BGM_STATE_PREPARE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_SHUTDOWN:
        ret = -1;
        break;
    case BGM_STATE_POWERSAVER:
        if (state_request != BGM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_TERMINATE:
        if (state_request != BGM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_INTERRUPT:
        if (state_request != BGM_STATE_READY && state_request != BGM_STATE_TERMINATE)
        {
            ret = -1;
        }
        break;
    default:
        ret = -1;
        break;
    }

    if (ret != 0)
    {
        // LOG_E("state_request: %d, state_current: %d\r\n", state_request, state_current);
    }

    return ret;
}

static int8_t fsm_state_remote_set(enum bgm_fsm_state state_request)
{
    int8_t ret = 0;
    struct bgm_data_info info = {0};
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    memcpy(&info, obj, sizeof(struct bgm_data_info) - sizeof(osMutexId_t));
    osMutexRelease(obj->mutex);

    switch (state_request)
    {
    case BGM_STATE_INIT:
        /* 1. handshake with dose、afc board and so on */
        // ret = dose_handshake(BGM_UART_AFC);
        ret |= dose_handshake(BGM_UART_DOSE1);
        ret |= dose_handshake(BGM_UART_DOSE2);
        break;
    case BGM_STATE_IDLE:
        /* 1. wait dose board to idle */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_IDLE);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_IDLE);
        break;
    case BGM_STATE_PRELIMINARY:
        LOG_I("---remote set to preliminary---\r\n");
#if 0
        LOG_I("dose_mode: %d\r\n", info.dose_mode);
        LOG_I("pulse_mode: %d\r\n", info.pulse_mode);
        LOG_I("cali_mode: %d\r\n", info.cali_mode);
        LOG_I("cali_prf: %d\r\n", info.cali_prf);
        LOG_I("cali_dose1_adc: %d\r\n", info.cali_dose1_adc);
        LOG_I("cali_dose1_dac: %d\r\n", info.cali_dose1_dac);
        LOG_I("cali_dose2_adc: %d\r\n", info.cali_dose2_adc);
        LOG_I("cali_dose2_dac: %d\r\n", info.cali_dose2_dac);
        LOG_I("dose_meter_dummy: %f\r\n", info.dose_meter_dummy);
#endif
        /* 1. set cali adc & dac value */
        ret = dose_adc_value_set(BGM_UART_DOSE1, &info.cali_dose1_adc);
        ret |= dose_adc_value_set(BGM_UART_DOSE2, &info.cali_dose2_adc);
        ret |= dose_dac_value_set(BGM_UART_DOSE1, &info.cali_dose1_dac);
        ret |= dose_dac_value_set(BGM_UART_DOSE2, &info.cali_dose2_dac);
        /* 2. clear beam cumulated */
        ret |= dose_beam_cumulated_clear(BGM_UART_DOSE1);
        ret |= dose_beam_cumulated_clear(BGM_UART_DOSE2);
        /* 3. set generate mode to 0 */
        info.dose_mode = 0;
        ret |= dose_generate_mode_set(BGM_UART_DOSE1, &info.dose_mode);
        ret |= dose_generate_mode_set(BGM_UART_DOSE2, &info.dose_mode);
        /* 4. set pulse mode to 0 */
        ret |= dose_pulse_mode_set(BGM_UART_DOSE1, &info.pulse_mode);
        ret |= dose_pulse_mode_set(BGM_UART_DOSE2, &info.pulse_mode);
        /* 5. set dummy dose meter */
        if (info.dose_meter_dummy < 0.1f)
        {
            info.dose_meter_dummy = 300.0f;
        }
        ret |= dose_meter_value_set(BGM_UART_DOSE1, &info.dose_meter_dummy);
        ret |= dose_meter_value_set(BGM_UART_DOSE2, &info.dose_meter_dummy);
        /* 6. set dose board to dummy */
        ret |= dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_PRELIMINARY_BEGIN);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_PRELIMINARY_BEGIN);
        /* 7. update beam deliver type */
        uint8_t deliver_type = 0;
        ret |= beam_deliver_type_get(info.beam_id, &deliver_type);
        osMutexAcquire(obj->mutex, osWaitForever);
        obj->deliver_type = deliver_type;
        osMutexRelease(obj->mutex);
        break;
    case BGM_STATE_PREPARE:
        LOG_I("---remote set to prepare---\r\n");
#if 0
        LOG_I("dose_mode: %d\r\n", info.dose_mode);
        LOG_I("pulse_mode: %d\r\n", info.pulse_mode);
        LOG_I("cali_mode: %d\r\n", info.cali_mode);
        LOG_I("cali_prf: %d\r\n", info.cali_prf);
        LOG_I("dose_meter: %f\r\n", info.dose_meter);
#endif
        /* 1. clear beam cumulated */
        ret = dose_beam_cumulated_clear(BGM_UART_DOSE1);
        ret |= dose_beam_cumulated_clear(BGM_UART_DOSE2);
        ret |= dose_radiation_data_get(BGM_UART_DOSE1);
        ret |= dose_radiation_data_get(BGM_UART_DOSE2);
        /* 2. set generate mode to 1 */
        info.dose_mode = 1;
        ret |= dose_generate_mode_set(BGM_UART_DOSE1, &info.dose_mode);
        ret |= dose_generate_mode_set(BGM_UART_DOSE2, &info.dose_mode);
        /* 3. set pulse mode */
        ret |= dose_pulse_mode_set(BGM_UART_DOSE1, &info.pulse_mode);
        ret |= dose_pulse_mode_set(BGM_UART_DOSE2, &info.pulse_mode);
        /* 4. download beam parameters to dose board */
        if (info.cali_mode == 1)
        {
            /* 4.0 set beam data */
            ret |= dose_beam_parameter_set(BGM_UART_DOSE1, info.beam_id);
            ret |= dose_beam_parameter_set(BGM_UART_DOSE2, info.beam_id);
            // ret |= dose_radiation_index_set(BGM_UART_DOSE1, info.radiation_index, 0);
            // ret |= dose_radiation_index_set(BGM_UART_DOSE2, info.radiation_index, 0);
            /* 4.1 set prf */
            ret |= dose_prf_value_set(BGM_UART_DOSE1, &info.cali_prf);
            ret |= dose_prf_value_set(BGM_UART_DOSE2, &info.cali_prf);
            /* 4.2 set dose meter */
            ret |= dose_meter_value_set(BGM_UART_DOSE1, &info.dose_meter);
            ret |= dose_meter_value_set(BGM_UART_DOSE2, &info.dose_meter);
            /* 4.3 set beam info */
            // osMutexAcquire(obj->mutex, osWaitForever);
            // uint8_t deliver_type = obj->deliver_type;
            // osMutexRelease(obj->mutex);
            // ret |= dose_beam_info_set(BGM_UART_DOSE1, &deliver_type);
            // ret |= dose_beam_info_set(BGM_UART_DOSE2, &deliver_type);
        }
        else
        {
            ret |= dose_beam_parameter_set(BGM_UART_DOSE1, info.beam_id);
            ret |= dose_beam_parameter_set(BGM_UART_DOSE2, info.beam_id);
            // ret |= dose_radiation_index_set(BGM_UART_DOSE1, info.radiation_index, 0);
            // ret |= dose_radiation_index_set(BGM_UART_DOSE2, info.radiation_index, 0);
        }
        /* 5. set dose board to prepare */
        ret |= dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_PREPARE);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_PREPARE);
        break;
    case BGM_STATE_READY:
        /* 1. set dose board to ready */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_READY);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_READY);
        break;
    case BGM_STATE_WORK:
        /* 1. set dose board to radiation */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_WORK);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_WORK);
        break;
    case BGM_STATE_PARK:
        /* 1. set dose board to park */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_PARK);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_PARK);
        break;
    case BGM_STATE_MANUAL:
        /* 1. set dose board to manual */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_MANUAL);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_MANUAL);
        break;
    case BGM_STATE_COMPLETE:
        LOG_E("illegal state request: %d\r\n", state_request);
        ret = -1;
        break;
    case BGM_STATE_SHUTDOWN:
        /* set dose board to shutdown */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_SHUTDOWN);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_SHUTDOWN);
        break;
    case BGM_STATE_POWERSAVER:
        /* set dose board to power saver */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_POWERSAVER);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_POWERSAVER);
        break;
    case BGM_STATE_TERMINATE:
        /* set dose board to terminate */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_TERMINATE);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_TERMINATE);
        break;
    case BGM_STATE_INTERRUPT:
        /* set dose board to interrupt */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_INTERRUPT);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_INTERRUPT);
        break;
    default:
        ret = -1;
        break;
    }

    if (ret != 0)
    {
        LOG_E("remote state request set err: %d\r\n", state_request);
    }
    else
    {
        osMutexAcquire(obj->mutex, osWaitForever);
        obj->fsm_state_request_already = state_request;
        osMutexRelease(obj->mutex);
    }

    return ret;
}

static int8_t fsm_state_set(enum bgm_fsm_state state_request, enum fsm_source_t source)
{
    int8_t ret = 0;

    if (source >= FSM_SOURCE_MAX)
    {
        LOG_E("invalid source: %d\r\n", source);
        return -1;
    }

    uint8_t dose_fsm_state_flag = 0;
    enum bgm_fsm_state state_current = BGM_STATE_MAX, state_request_already = BGM_STATE_MAX, state_request_pre = BGM_STATE_MAX;
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    state_current = obj->fsm_state;
    state_request_pre = obj->fsm_state_request_pre;
    state_request_already = obj->fsm_state_request_already;
    dose_fsm_state_flag = obj->dose_fsm_state_flag;
    osMutexRelease(obj->mutex);

    ret = fsm_switch_check(state_current, state_request);
    if (ret != 0)
    {
        // LOG_E("fsm_switch_check err: %d\r\n", ret);
        return ret;
    }

    switch (source)
    {
    case FSM_SOURCE_REMOTE:
        if (state_request == BGM_STATE_PREPARE)
        {
            switch (dose_fsm_state_flag)
            {
            case DOSE_NONE_FLAG:    /* just wait for dummy or beam end */
                return 0;
                break;
            case DOSE_PRELIMINARY_FLAG:
                osMutexAcquire(obj->mutex, osWaitForever);
                obj->dose_fsm_state_flag &= ~DOSE_PRELIMINARY_FLAG;
                osMutexRelease(obj->mutex);
                break;
            case DOSE_COMPLETE_FLAG:
                osMutexAcquire(obj->mutex, osWaitForever);
                obj->dose_fsm_state_flag &= ~DOSE_COMPLETE_FLAG;
                osMutexRelease(obj->mutex);
                break;
            default:
                LOG_E("dose fsm flag err: %d\r\n", dose_fsm_state_flag);
                return 0;
                break;
            }
        }
        else
        {
            if (state_request == state_current)
            {
                break;
            }
        }

        if (state_request != state_request_already)
        {
            ret = fsm_state_remote_set(state_request);
        }
        break;
    case FSM_SOURCE_LOCAL:
        if (state_request == state_current)
        {
            break;
        }
        osMutexAcquire(obj->mutex, osWaitForever);
        obj->fsm_state = state_request;
        osMutexRelease(obj->mutex);

        switch (state_request)
        {
        case BGM_STATE_IDLE:
            trigger_out_enable(0);
            break;
        case BGM_STATE_READY:
            trigger_out_enable(1);
            break;
        case BGM_STATE_WORK:
            break;
        case BGM_STATE_COMPLETE:
            trigger_out_enable(0);
            break;
        case BGM_STATE_TERMINATE:
            trigger_out_enable(0);
            break;
        default:
            break;
        }
        break;
    default:
        LOG_E("invalid source: %d\r\n", source);
        ret = -2;
        break;
    }

    if (ret != 0)
    {
        LOG_E("fsm state set err: %d\r\n", ret);
    }

    return ret;
}

static int8_t fsm_state_update_from_local(void)
{
    int8_t ret = 0;
    static enum dose_fsm_state dose_state_set = DOSE_FSM_STATE_MAX;
    enum dose_fsm_state fsm_state_dose1 = DOSE_FSM_STATE_MAX, fsm_state_dose2 = DOSE_FSM_STATE_MAX;
    struct bgm_data_info *obj = bgm_data_info_get();
    enum bgm_fsm_state state_current = BGM_STATE_MAX, state_request = BGM_STATE_MAX;

    osMutexAcquire(obj->mutex, osWaitForever);
    state_current = obj->fsm_state;
    state_request = obj->fsm_state_request;
    osMutexRelease(obj->mutex);


    /* 0. polling dose board fsm */
#ifdef BGM_FSM_STATE_SIMULATION
    fsm_state_dose1 = dose1_fsm_state;
    fsm_state_dose2 = dose2_fsm_state;
#else
    ret = dose_state_polling(BGM_UART_DOSE1);
    ret |= dose_state_polling(BGM_UART_DOSE2);
    if (ret != 0)
    {
        LOG_E("dose fsm polling err: %d\r\n", ret);
    }

    /* 1. get dose fsm state */
    fsm_state_dose1 = dose_fsm_state_get(BGM_UART_DOSE1);
    fsm_state_dose2 = dose_fsm_state_get(BGM_UART_DOSE2);

    if (fsm_state_dose1 == DOSE_FSM_STATE_INIT || fsm_state_dose2 == DOSE_FSM_STATE_INIT)
    {
        switch (state_current)
        {
        case BGM_STATE_INIT:
            ret = fsm_state_dose1 == DOSE_FSM_STATE_INIT ? dose_handshake(BGM_UART_DOSE1) : 0;
            ret |= fsm_state_dose2 == DOSE_FSM_STATE_INIT ? dose_handshake(BGM_UART_DOSE2) : 0;
            if (ret != 0)
            {
                LOG_E("dose handshake err: %d\r\n", ret);
            }
            break;
        default:
            fsm_state_dose1 == DOSE_FSM_STATE_INIT ? LOG_E("dose1 abnormal reboot\r\n"), dose_handshake(BGM_UART_DOSE1) : NULL;
            fsm_state_dose2 == DOSE_FSM_STATE_INIT ? LOG_E("dose2 abnormal reboot\r\n"), dose_handshake(BGM_UART_DOSE2) : NULL;
            ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_TERMINATE);
            ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_TERMINATE);
            if (ret != 0)
            {
                LOG_E("dose fsm set err: %d\r\n", ret);
            }
            break;
        }
    }
#endif

    /* 2. update fsm state current from local */
    if (fsm_state_dose1 == DOSE_FSM_STATE_TERMINATE && fsm_state_dose2 == DOSE_FSM_STATE_TERMINATE)
    {
        ret = fsm_state_set(BGM_STATE_TERMINATE, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_TERMINATE || fsm_state_dose2 == DOSE_FSM_STATE_TERMINATE)
    {
        if (state_request == BGM_STATE_IDLE)
        {
            return 0;
        }

        if (dose_state_set == DOSE_FSM_STATE_TERMINATE)
        {
            return 0;
        }
        dose_state_set = DOSE_FSM_STATE_TERMINATE;

        ret = fsm_state_dose1 == DOSE_FSM_STATE_TERMINATE ? dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_TERMINATE) : 0;
        ret |= fsm_state_dose2 == DOSE_FSM_STATE_TERMINATE ? dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_TERMINATE) : 0;
        if (ret != 0)
        {
            LOG_E("dose fsm set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_IDLE && fsm_state_dose2 == DOSE_FSM_STATE_IDLE)
    {
        dose_state_set = DOSE_FSM_STATE_IDLE;

        ret = fsm_state_set(BGM_STATE_IDLE, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_PRELIMINARY_BEGIN && fsm_state_dose2 == DOSE_FSM_STATE_PRELIMINARY_BEGIN)
    {
        // ret = fsm_state_set(BGM_STATE_PRELIMINARY, FSM_SOURCE_LOCAL);
        // if (ret != 0)
        // {
        //     LOG_E("fsm state set err: %d\r\n", ret);
        // }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_PRELIMINARY && fsm_state_dose2 == DOSE_FSM_STATE_PRELIMINARY)
    {
        if (dose_state_set == DOSE_FSM_STATE_PRELIMINARY)
        {
            return 0;
        }
        dose_state_set = DOSE_FSM_STATE_PRELIMINARY;

        ret = fsm_state_set(BGM_STATE_PRELIMINARY, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }

        osMutexAcquire(obj->mutex, osWaitForever);
        obj->dose_fsm_state_flag = DOSE_PRELIMINARY_FLAG;
        osMutexRelease(obj->mutex);

        LOG_I("---dummy end---\r\n");
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_PREPARE && fsm_state_dose2 == DOSE_FSM_STATE_PREPARE)
    {
        ret = fsm_state_set(BGM_STATE_PREPARE, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_READY && fsm_state_dose2 == DOSE_FSM_STATE_READY)
    {
        dose_state_set = DOSE_FSM_STATE_READY;

        ret = fsm_state_set(BGM_STATE_READY, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_WORK && fsm_state_dose2 == DOSE_FSM_STATE_WORK)
    {
        ret = fsm_state_set(BGM_STATE_WORK, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_COMPLETE || fsm_state_dose2 == DOSE_FSM_STATE_COMPLETE)
    {
        if (dose_state_set == DOSE_FSM_STATE_COMPLETE)
        {
            return 0;
        }
        dose_state_set = DOSE_FSM_STATE_COMPLETE;

        osMutexAcquire(obj->mutex, osWaitForever);
        obj->dose_fsm_state_flag = DOSE_COMPLETE_FLAG;
        osMutexRelease(obj->mutex);

        ret = fsm_state_set(BGM_STATE_COMPLETE, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_INTERRUPT && fsm_state_dose2 == DOSE_FSM_STATE_INTERRUPT)
    {
        ret = fsm_state_set(BGM_STATE_INTERRUPT, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_PARK && fsm_state_dose2 == DOSE_FSM_STATE_PARK)
    {
        ret = fsm_state_set(BGM_STATE_PARK, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_MANUAL && fsm_state_dose2 == DOSE_FSM_STATE_MANUAL)
    {
        ret = fsm_state_set(BGM_STATE_MANUAL, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_SHUTDOWN && fsm_state_dose2 == DOSE_FSM_STATE_SHUTDOWN)
    {
        ret = fsm_state_set(BGM_STATE_SHUTDOWN, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_POWERSAVER && fsm_state_dose2 == DOSE_FSM_STATE_POWERSAVER)
    {
        ret = fsm_state_set(BGM_STATE_POWERSAVER, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t fsm_state_update_from_remote(void)
{
    enum bgm_fsm_state state_request = BGM_STATE_MAX;
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    state_request = obj->fsm_state_request;
    osMutexRelease(obj->mutex);

    return fsm_state_set(state_request, FSM_SOURCE_REMOTE);
}

static void system_fsm_state_entry(void *argument)
{
    int8_t ret = 0;
  
    for (;;)
    {
        osDelay(100);

        /* 1. update fsm state current from local */
        ret = fsm_state_update_from_local();
        if (ret != 0)
        {
            LOG_E("fsm_state_update_from_local err: %d\r\n", ret);
        }

        /* 2. update fsm state current from remote */
        ret = fsm_state_update_from_remote();
        if (ret != 0)
        {
            // LOG_E("fsm_state_update_from_remote err: %d\r\n", ret);
        }
    }
}

static int8_t dose_rate_calculate(void *argument)
{
    int8_t ret = 0;
    float dose1_meter_cur = 0, dose2_meter_cur = 0;
    enum bgm_fsm_state state_current = BGM_STATE_MAX;
    float dose1_rate = 0, dose2_rate = 0;
    struct bgm_data_info *obj = bgm_data_info_get();

    for (;;)
    {
        osDelay(500);

        osMutexAcquire(obj->mutex, osWaitForever);
        state_current = obj->fsm_state;
        osMutexRelease(obj->mutex);

        if (state_current != BGM_STATE_WORK)
        {
            ret = dose_rate_value_set(BGM_UART_DOSE1, &dose1_rate);
            ret |= dose_rate_value_set(BGM_UART_DOSE2, &dose2_rate);
            if (ret != 0)
            {
                LOG_E("dose rate set err: %d\r\n", ret);
            }
        }
    
        /* TODO: */
        /* 1. 周期性核对dose1和dose2的剂量偏差，控制在10%以内？ */
        dose1_meter_cur = dose_meter_value_get(BGM_UART_DOSE1);
        dose2_meter_cur = dose_meter_value_get(BGM_UART_DOSE2);
        if (fabs(dose1_meter_cur - dose2_meter_cur) / dose1_meter_cur > 0.1)
        {
            LOG_E("dose meter difference exceed 10\% limit\r\n");
        }

        /* 2.  */


    }

    return 0;
}

static int8_t fsm_thread_init(void)
{
    osThreadAttr_t BGMFSM_attributes = {
    .name = "bgm_fsm_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t)osPriorityNormal,
    };

    osThreadId_t BGMFSMHandle = osThreadNew(system_fsm_state_entry, NULL, &BGMFSM_attributes);
    if (BGMFSMHandle == NULL)
    {
        printf("thread BGMFSM create failed\r\n");
        return -1;
    }

    osMutexAttr_t mutex_attributes = {
    .name = "bgm_info_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    bgm_data_info_get()->mutex = osMutexNew(&mutex_attributes);
    if (bgm_data_info_get()->mutex == NULL)
    {
        printf("mutex create failed\r\n");
        return -2;
    }

    trigger_out_event_flag = osEventFlagsNew(NULL);
    if (trigger_out_event_flag == NULL)
    {
        printf("event flag create failed\r\n");
        return -3;
    }

    osThreadAttr_t trigger_out_attributes = {
    .name = "trigger_out_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t)osPriorityNormal,
    };

    osThreadId_t trigger_outHandle = osThreadNew(trigger_out_entry, NULL, &trigger_out_attributes);
    if (trigger_outHandle == NULL)
    {
        printf("thread trigger out create failed\r\n");
        return -4;
    }

    osThreadAttr_t dose_rate_calculate_attributes = {
    .name = "dose_rate_calculate_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t)osPriorityNormal,
    };

    osThreadId_t dose_rate_calculateHandle = osThreadNew(dose_rate_calculate, NULL, &dose_rate_calculate_attributes);
    if (dose_rate_calculateHandle == NULL)
    {
        printf("thread dose rate calculate create failed\r\n");
        return -5;
    }

    return 0;
}
INIT_APP_EXPORT(fsm_thread_init);

static int8_t SF6_analog_value_get(float *value)
{
#define SF6_FACTOR  0.25f
#define SF6_OFFSET  -0.25f

    *value = mcu_adc_value_get(MCU_ADC_CHANNEL_SF6) / 1000 * SF6_FACTOR + SF6_OFFSET;

    return 0;
}

#ifdef BGM_FSM_STATE_SIMULATION
#include "shell.h"
static int8_t dose_fsm_state_update(uint8_t argc, char **argv)
{
    if (argc != 3)
    {
        LOG_E("invalid args\r\n");
        return -1;
    }

    uint8_t uart_id = atoi(argv[1]);
    uint8_t state = atoi(argv[2]);

    switch (uart_id)
    {
    case 1:
        dose1_fsm_state = state;
        break;
    case 2:
        dose2_fsm_state = state;
        break;
    default:
        break;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_fsm_state_update, dose_fsm_state_update, update dose fsm state);
#endif


#ifndef BGM_STATE_TEST
#include "shell.h"
static int8_t bgm_info_get(uint8_t argc, char **argv)
{
    struct bgm_data_info info = {0};
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    memcpy(&info, obj, sizeof(struct bgm_data_info));
    osMutexRelease(obj->mutex);

    LOG_I("beam_id: %d\r\n", info.beam_id);
    LOG_I("radiation_index: %d\r\n", info.radiation_index);
    LOG_I("dose_mode: %d\r\n", info.dose_mode);
    LOG_I("pulse_mode: %d\r\n", info.pulse_mode);
    LOG_I("cali_mode: %d\r\n", info.cali_mode);
    LOG_I("cali_prf: %d\r\n", info.cali_prf);
    LOG_I("cali_dose1_dac: %d\r\n", info.cali_dose1_dac);
    LOG_I("cali_dose1_adc: %d\r\n", info.cali_dose1_adc);
    LOG_I("cali_dose2_dac: %d\r\n", info.cali_dose2_dac);
    LOG_I("cali_dose2_adc: %d\r\n", info.cali_dose2_adc);
    LOG_I("dose_meter: %f\r\n", info.dose_meter);
    LOG_I("dose_meter_dummy: %f\r\n", info.dose_meter_dummy);
    LOG_I("dose_fsm_state_flag: %d\r\n", info.dose_fsm_state_flag);

    LOG_I("fsm_state: %d\r\n", info.fsm_state);
    LOG_I("fsm_state_request: %d\r\n", info.fsm_state_request);
    LOG_I("fsm_state_request_pre: %d\r\n", info.fsm_state_request_pre);
    LOG_I("fsm_state_request_already: %d\r\n", info.fsm_state_request_already);

#ifdef BGM_FSM_STATE_SIMULATION
    LOG_I("state_dose1: %d\r\n", dose1_fsm_state);
    LOG_I("state_dose2: %d\r\n", dose2_fsm_state);
#else
    LOG_I("state_dose1: %d\r\n", dose_fsm_state_get(BGM_UART_DOSE1));
    LOG_I("state_dose2: %d\r\n", dose_fsm_state_get(BGM_UART_DOSE2));
#endif

    LOG_I("interlock_dose1: %d\r\n", dose_interlock_get(BGM_UART_DOSE1));
    LOG_I("interlock_dose2: %d\r\n", dose_interlock_get(BGM_UART_DOSE2));

    LOG_I("meter_dose1: %f\r\n", dose_meter_value_get(BGM_UART_DOSE1));
    LOG_I("meter_dose2: %f\r\n", dose_meter_value_get(BGM_UART_DOSE2));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_info_get, bgm_info_get, get bgm info);

static int8_t bgm_fsm_state_current_set(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    obj->fsm_state = atoi(argv[1]);
    osMutexRelease(obj->mutex);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_fsm_state_current_set, bgm_fsm_state_current_set, set bgm fsm state current);

static int8_t bgm_sf6_analog_value_get(uint8_t argc, char **argv)
{
    float value = 0;
    int8_t ret = 0;

    ret = SF6_analog_value_get(&value);
    if (ret != 0)
    {
        LOG_E("SF6 analog value get err: %d\r\n", ret);
        return -1;
    }

    LOG_I("SF6 analog value: %f\r\n", value);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_sf6_analog_value_get, bgm_sf6_analog_value_get, get bgm sf6 analog value);
#endif