#include "bgm_app.h"
#include "bgm_def.h"
#include "init_call.h"
#include "tim.h"
#include "ulog.h"

void TriggerConfig_SetARR(TIM_HandleTypeDef *htim, uint32_t arr_value)
{
    __HAL_TIM_SET_AUTORELOAD(htim, arr_value);
}

static struct bgm_data_info bgm_info = 
{
    .fsm_state = BGM_STATE_INIT,
};

struct bgm_data_info *bgm_data_info_get(void)
{
    return &bgm_info;
}

static void PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        AFC_GetADCValueByFrame();   
    }
    else if (htim->Instance == TIM5)
    {
        dose_radiation_data_get(BGM_UART_DOSE1);
        dose_radiation_data_get(BGM_UART_DOSE2);
    }
}

static void trigger_out_distribute_init(void)
{
    MX_TIM1_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_TIM5_Init();
    MX_TIM23_Init();

    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim23, TIM_CHANNEL_3);
    HAL_TIM_Base_Start_IT(&htim5);

    HAL_TIM_RegisterCallback(&htim3, HAL_TIM_PERIOD_ELAPSED_CB_ID, PeriodElapsedCallback);
    HAL_TIM_RegisterCallback(&htim5, HAL_TIM_PERIOD_ELAPSED_CB_ID, PeriodElapsedCallback);
}

enum fsm_source_t
{
    FSM_SOURCE_REMOTE = 0,  /* from master cmd */
    FSM_SOURCE_LOCAL,       /* from slave feedback */
    FSM_SOURCE_MAX
};

static int8_t fsm_switch_check(enum bgm_fsm_state state_request, enum bgm_fsm_state state_current)
{
    if (state_request >= BGM_STATE_MAX)
    {
        LOG_E("invalid state request: %d\r\n", state_request);
        return -1;
    }

    int8_t ret = 0;

    switch (state_request)
    {
    case BGM_STATE_INIT:
        if (state_current != BGM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_IDLE:
        if (state_current != BGM_STATE_INIT && state_current != BGM_STATE_POWERSAVER 
            && state_current != BGM_STATE_TERMINATE && state_current != BGM_STATE_COMPLETE
            && state_current != BGM_STATE_MANUAL && state_current != BGM_STATE_PARK)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_PRELIMINARY:
        if (state_current != BGM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_PREPARE:
        if (state_current != BGM_STATE_PRELIMINARY && state_current != BGM_STATE_COMPLETE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_READY:
        if (state_current != BGM_STATE_PREPARE && state_current != BGM_STATE_INTERRUPT)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_WORK:
        if (state_current != BGM_STATE_READY)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_PARK:
        if (state_current != BGM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_MANUAL:
        if (state_current != BGM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_COMPLETE:
        if (state_current != BGM_STATE_WORK)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_SHUTDOWN:
        if (state_current != BGM_STATE_IDLE && state_current != BGM_STATE_POWERSAVER)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_POWERSAVER:
        if (state_current != BGM_STATE_IDLE)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_TERMINATE:
        if (state_current != BGM_STATE_PRELIMINARY && state_current != BGM_STATE_PREPARE
            && state_current != BGM_STATE_READY && state_current != BGM_STATE_WORK
            && state_current != BGM_STATE_INTERRUPT)
        {
            ret = -1;
        }
        break;
    case BGM_STATE_INTERRUPT:
        if (state_current != BGM_STATE_READY && state_current != BGM_STATE_WORK)
        {
            ret = -1;
        }
        break;
    default:
        break;
    }

    if (ret != 0)
    {
        LOG_E("state_request: %d, state_current: %d\r\n", state_request, state_current);
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
        ret = dose_handshake(BGM_UART_AFC);
        ret |= dose_handshake(BGM_UART_DOSE1);
        ret |= dose_handshake(BGM_UART_DOSE2);
        break;
    case BGM_STATE_IDLE:
        /* 1. wait dose board to idle */
        break;
    case BGM_STATE_PRELIMINARY:
        /* 1. clear beam cumulated */
        ret = dose_beam_cumulated_clear(BGM_UART_DOSE1);
        ret |= dose_beam_cumulated_clear(BGM_UART_DOSE2);
        /* 2. set generate mode to 0 */
        ret |= dose_generate_mode_set(BGM_UART_DOSE1, 0);
        ret |= dose_generate_mode_set(BGM_UART_DOSE2, 0);
        /* 3. set pulse mode to 0 */
        ret |= dose_pulse_mode_set(BGM_UART_DOSE1, 0);
        ret |= dose_pulse_mode_set(BGM_UART_DOSE2, 0);
        /* 4. set dummy dose meter */
        ret |= dose_meter_value_set(BGM_UART_DOSE1, info.dose_meter_dummy);
        ret |= dose_meter_value_set(BGM_UART_DOSE2, info.dose_meter_dummy);
        /* 5. set dose board to dummy */
        ret |= dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_DUMMY);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_DUMMY);
        break;
    case BGM_STATE_PREPARE:
        /* 1. clear beam cumulated */
        ret = dose_beam_cumulated_clear(BGM_UART_DOSE1);
        ret |= dose_beam_cumulated_clear(BGM_UART_DOSE2);
        /* 2. set generate mode to 1 */
        ret |= dose_generate_mode_set(BGM_UART_DOSE1, 1);
        ret |= dose_generate_mode_set(BGM_UART_DOSE2, 1);
        /* 3. set pulse mode */
        ret |= dose_pulse_mode_set(BGM_UART_DOSE1, info.pulse_mode);
        ret |= dose_pulse_mode_set(BGM_UART_DOSE2, info.pulse_mode);
        /* 4. download beam parameters to dose board */
        if (info.cali_mode == 1)
        {
            /* 4.1 set prf */
            ret |= dose_prf_value_set(BGM_UART_DOSE1, info.cali_prf);
            ret |= dose_prf_value_set(BGM_UART_DOSE2, info.cali_prf);
            /* 4.2 set dose meter */
            ret |= dose_meter_value_set(BGM_UART_DOSE1, info.dose_meter);
            ret |= dose_meter_value_set(BGM_UART_DOSE2, info.dose_meter);
        }
        else
        {
            ret |= dose_beam_parameter_set(BGM_UART_DOSE1, info.beam_id);
            ret |= dose_beam_parameter_set(BGM_UART_DOSE2, info.beam_id);
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
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_RADIATION);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_RADIATION);
        break;
    case BGM_STATE_PARK:
        break;
    case BGM_STATE_MANUAL:
        break;
    case BGM_STATE_COMPLETE:
        LOG_E("illegal state request: %d\r\n", state_request);
        ret = -1;
        break;
    case BGM_STATE_SHUTDOWN:
        break;
    case BGM_STATE_POWERSAVER:
        break;
    case BGM_STATE_TERMINATE:
        /* set dose board to fault */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_FAULT);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_FAULT);
        break;
    case BGM_STATE_INTERRUPT:
        /* set dose board to fault */
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_FAULT);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_FAULT);
        break;
    default:
        break;
    }

    if (ret != 0)
    {
        LOG_E("remote state request set err: %d\r\n", state_request);
    }

    return ret;
}

static int8_t fsm_state_set(enum bgm_fsm_state state_request, enum fsm_source_t source)
{
    enum bgm_fsm_state state_current = BGM_STATE_INIT;
    struct bgm_data_info *obj = bgm_data_info_get();
    uint8_t dummy_end_flag = 0;

    osMutexAcquire(obj->mutex, osWaitForever);
    state_current = obj->fsm_state;
    dummy_end_flag = obj->dummy_end_flag;
    osMutexRelease(obj->mutex);

    if (dummy_end_flag == 0)
    {
        if (state_request == state_current)
        {
            return 0;
        }
    }
    else
    {
        if (state_request == state_current && state_request == BGM_STATE_PREPARE)
        {
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->dummy_end_flag = 0;
            osMutexRelease(obj->mutex);
            goto except;
        }
        else if (state_request == state_current)
        {
            return 0;
        }
    }

    int8_t ret = fsm_switch_check(state_request, state_current);
    if (ret != 0)
    {
        LOG_E("fsm_switch_check err: %d\r\n", ret);
        return ret;
    }

except:
    if (source >= FSM_SOURCE_MAX)
    {
        LOG_E("invalid source: %d\r\n", source);
        return -1;
    }

    switch (source)
    {
    case FSM_SOURCE_REMOTE:
        ret = fsm_state_remote_set(state_request);
        break;
    case FSM_SOURCE_LOCAL:
        osMutexAcquire(obj->mutex, osWaitForever);
        obj->fsm_state = state_request;
        osMutexRelease(obj->mutex);
        break;
    default:
        break;
    }

    if (ret != 0)
    {
        LOG_E("fsm_state_remote_set err: %d\r\n", ret);
    }

    return ret;
}

static int8_t fsm_state_update_from_local(void)
{
    int8_t ret = 0;
    enum dose_fsm_state fsm_state_dose1 = 0, fsm_state_dose2 = 0;

    enum bgm_fsm_state state_request = BGM_STATE_INIT;
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    state_request = obj->fsm_state_request;
    osMutexRelease(obj->mutex);

    /* 0. polling dose board fsm */
    ret = dose_fsm_state_polling(BGM_UART_DOSE1);
    ret |= dose_fsm_state_polling(BGM_UART_DOSE2);
    if (ret != 0)
    {
        LOG_E("dose fsm polling err: %d\r\n", ret);
    }

    /* 1. get dose fsm state */
    ret = dose_fsm_state_get(BGM_UART_DOSE1, &fsm_state_dose1);
    ret |= dose_fsm_state_get(BGM_UART_DOSE2, &fsm_state_dose2);
    if (ret != 0)
    {
        LOG_E("dose fsm get err: %d\r\n", ret);
    }

    /* 2. update fsm state current from local */
    if (state_request == BGM_STATE_INTERRUPT || state_request == BGM_STATE_TERMINATE)
    {
        if (fsm_state_dose1 == DOSE_FSM_STATE_FAULT && fsm_state_dose2 == DOSE_FSM_STATE_FAULT)
        {
            ret = fsm_state_set(state_request, FSM_SOURCE_LOCAL);
            if (ret != 0)
            {
                LOG_E("fsm state set err: %d\r\n", ret);
            }
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_FAULT && fsm_state_dose2 == DOSE_FSM_STATE_FAULT)
    {
        ret = fsm_state_set(BGM_STATE_TERMINATE, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_FAULT || fsm_state_dose2 == DOSE_FSM_STATE_FAULT)
    {
        ret = fsm_state_dose1 == DOSE_FSM_STATE_FAULT ? dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_FAULT) : 0;
        ret |= fsm_state_dose2 == DOSE_FSM_STATE_FAULT ? dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_FAULT) : 0;
        if (ret != 0)
        {
            LOG_E("dose fsm set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_IDLE && fsm_state_dose2 == DOSE_FSM_STATE_IDLE)
    {
        ret = fsm_state_set(BGM_STATE_IDLE, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_DUMMY && fsm_state_dose2 == DOSE_FSM_STATE_DUMMY)
    {
        ret = fsm_state_set(BGM_STATE_PRELIMINARY, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_DUMMY_END && fsm_state_dose2 == DOSE_FSM_STATE_DUMMY_END)
    {
        struct bgm_data_info *obj = bgm_data_info_get();

        osMutexAcquire(obj->mutex, osWaitForever);
        obj->dummy_end_flag = 1;
        osMutexRelease(obj->mutex);

        osDelay(3000);
        ret = dose_fsm_state_set(BGM_UART_DOSE1, DOSE_FSM_STATE_PREPARE);
        ret |= dose_fsm_state_set(BGM_UART_DOSE2, DOSE_FSM_STATE_PREPARE);
        if (ret != 0)
        {
            LOG_E("dose fsm set err: %d\r\n", ret);
        }
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
        ret = fsm_state_set(BGM_STATE_READY, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_RADIATION && fsm_state_dose2 == DOSE_FSM_STATE_RADIATION)
    {
        ret = fsm_state_set(BGM_STATE_WORK, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }
    else if (fsm_state_dose1 == DOSE_FSM_STATE_COMPLETE || fsm_state_dose2 == DOSE_FSM_STATE_COMPLETE)
    {
        ret = fsm_state_set(BGM_STATE_COMPLETE, FSM_SOURCE_LOCAL);
        if (ret != 0)
        {
            LOG_E("fsm state set err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t fsm_state_update_from_remote(void)
{
    enum bgm_fsm_state state_request = BGM_STATE_INIT;
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    state_request = obj->fsm_state_request;
    osMutexRelease(obj->mutex);

    return fsm_state_set(state_request, FSM_SOURCE_REMOTE);
}

static void system_fsm_state_entry(void *argument)
{
    int8_t ret = 0;

    trigger_out_distribute_init();
    
    for (;;)
    {
        osDelay(10);

        /* 1. update fsm state current from local */
        // ret = fsm_state_update_from_local();
        if (ret != 0)
        {
            LOG_E("fsm_state_update_from_local err: %d\r\n", ret);
        }

        /* 2. update fsm state current from remote */
        // ret = fsm_state_update_from_remote();
        if (ret != 0)
        {
            LOG_E("fsm_state_update_from_remote err: %d\r\n", ret);
        }
    }
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

    return 0;
}
INIT_APP_EXPORT(fsm_thread_init);

