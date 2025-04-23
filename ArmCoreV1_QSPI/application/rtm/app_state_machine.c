/**
 * @file rtm_on_main.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "app_state_machine.h"
#include "ulog.h"
#include "rtm_main.h"

#define TRAN(x, target_state) (((stateTable_t *)self)->x = (uint8_t)(target_state))

device_err_t stateTable_ctor(stateTable_t *self,
                             Tran const *table,
                             uint8_t n_states,
                             uint8_t n_signals,
                             Tran initial)
{
    if (self == NULL || table == NULL || n_states == 0 || n_signals == 0 || initial == NULL)
    {
        return DEV_EINVAL;
    }
    self->state_table = table;
    self->n_states = n_states;
    self->n_signals = n_signals;
    self->state = 0;
    self->signal = 0;
    self->initial = initial;
    return DEV_EOK;
}
device_err_t stateTable_init(stateTable_t *self, Event_t const *e)
{
    if (self == NULL)
    {
        return DEV_EINVAL;
    }
    self->state = 0;
    self->signal = 0;
    self->initial(self, e);
    return DEV_EOK;
}
device_err_t stateTable_dispatch(stateTable_t *self, Event_t const *e)
{
    Tran func;
    if (self == NULL || e == NULL)
    {
        return DEV_EINVAL;
    }
    if (e->sig >= self->n_signals)
    {
        return DEV_EINVAL;
    }

    func = self->state_table[self->state * self->n_signals + e->sig];

    if (func == NULL)
    {
        return DEV_EINVAL;
    }
    func(self, e);

    return DEV_EOK;
}
uint8_t stateTable_get_state(stateTable_t *self)
{
    if (self == NULL)
    {
        return 0;
    }
    return self->signal;
}
/*************************************state machine start**********************/
static void rtm_state_machine_init(stateTable_t *self, Event_t const *e)
{
    rtm_event_t *rtm_event = (rtm_event_t *)e;
    TRAN(state, e->sig);
    LOG_I("rtm state machine init\r\n");
    HAL_NVIC_SystemReset();
}

static void rtm_state_machine_idle(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;

    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;

    TRAN(signal, SYSTEM_STATE_SYSTEM_ON);
    TRAN(state, STATE_MACHINE_IDLE);
}
static void rtm_state_machine_power_saver(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;


    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_POWER_SAVER);
}
static void rtm_state_machine_shutdown(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;

    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;

    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_SHUTDOWN);
}
static void rtm_state_machine_manual(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;

    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;

    TRAN(signal, SYSTEM_STATE_SYSTEM_ON);
    TRAN(state, STATE_MACHINE_MANUAL);
}

static void rtm_state_machine_preliminary(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;

    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_PRELIMINARY);

    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;

    rtm_event->super.sig = SYSTEM_STATE_MV_PREPARE;
    stateTable_dispatch(self, (Event_t *)rtm_event);
}
static void rtm_state_machine_prepare(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_PREPARE);
}
static void rtm_state_machine_ready(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;


    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_READY);
}
static void rtm_state_machine_work(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;

    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_WORK);
}
static void rtm_state_machine_complete(stateTable_t *self, Event_t const *e)
{
    rtm_event_t *rtm_event = (rtm_event_t *)e;

    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_COMPLETE);
}
static void rtm_state_machine_interrupt(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;

    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_INTERRUPT);

    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;

    if( e->sig == SYSTEM_STATE_MV_READY)
    {
        //TODO:无故障，自发跳转到STATE_MACHINE_INTERRUPT状态，需要处理
        rtm_event->super.sig = SYSTEM_STATE_MV_READY;
        TRAN(state, STATE_MACHINE_READY);
        stateTable_dispatch(self, (Event_t *)rtm_event);
    }
}
static void rtm_state_machine_terminate(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;
    
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;

    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_TERMINATE);
}
static void rtm_state_machine_kv_preliminary(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;

    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;

    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_KV_PRELIMINARY);
}
static void rtm_state_machine_kv_prepare(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;
    
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;

    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_KV_PREPARE);
}
static void rtm_state_machine_surview_ready(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;



    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_SURVIEW_READY);
}
static void rtm_state_machine_surview_work(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;
    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_SURVIEW_WORK);
}
static void rtm_state_machine_ct_ready(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;


    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_CT_READY);
}
static void rtm_state_machine_ct_work(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;
    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_CT_WORK);
}
static void rtm_state_machine_kv_complete(stateTable_t *self, Event_t const *e)
{
    int32_t retval = 0;
    rtm_event_t *rtm_event = (rtm_event_t *)e;
    TRAN(signal, e->sig);
    TRAN(state, STATE_MACHINE_KV_COMPLETE);
}

static void rtm_state_machine_initial(stateTable_t *self, Event_t const *e)
{
    rtm_state_machine_t *rtm_state_machine = (rtm_state_machine_t *)self;
    rtm_event_t *rtm_event = (rtm_event_t *)e;
    int32_t retval = 0;

    TRAN(state, STATE_MACHINE_IDLE);
    TRAN(signal, SYSTEM_STATE_INITIALIZATION);

    rtm_event->dido_structure->tca9535_0x04_u.tca9535_0x04_bit.DO_STAND_RESERVE = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
    rtm_event->dido_structure->gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
    // retval = osThreadFlagsWait(APP_RTM_THREAD_FLAG_ALL, osFlagsWaitAll | osFlagsNoClear, 3000);
    // if (retval < 0)
    // {
    //     LOG_I("rtm thread flags wait fail, retval:%d\r\n", retval);
    //     goto exit;
    // }
exit:
    rtm_event->super.sig = SYSTEM_STATE_SYSTEM_ON;
    stateTable_dispatch(self, (Event_t *)rtm_event);
}
void rtm_state_machine_ctor(stateTable_t *self)
{
    device_err_t device_err = DEV_EIO;

    static Tran state_table[STATE_MACHINE_MAX][SYSTEM_STATE_MAX] = {0};
    for (uint8_t i = 0; i < STATE_MACHINE_MAX; i++)
    {
        for (uint8_t j = 0; j < SYSTEM_STATE_MAX; j++)
        {
            state_table[i][j] = NULL;
        }
    }
    /* 1 STATE_MACHINE_INIT */
    state_table[STATE_MACHINE_NULL][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_idle;

    /* 2 STATE_MACHINE_IDLE */
    state_table[STATE_MACHINE_IDLE][SYSTEM_STATE_INITIALIZATION] = rtm_state_machine_init;
    state_table[STATE_MACHINE_IDLE][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_idle;
    state_table[STATE_MACHINE_IDLE][SYSTEM_STATE_MV_PREPARE] = rtm_state_machine_preliminary;
    state_table[STATE_MACHINE_IDLE][SYSTEM_STATE_SHUTDOWN] = rtm_state_machine_shutdown;
    state_table[STATE_MACHINE_IDLE][SYSTEM_STATE_POWER_SAVER] = rtm_state_machine_power_saver;
    state_table[STATE_MACHINE_IDLE][SYSTEM_STATE_KV_PRELIMINARY] = rtm_state_machine_kv_preliminary;
    /* 3 STATE_MACHINE_PRELIMINARY */
    state_table[STATE_MACHINE_PRELIMINARY][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_PRELIMINARY][SYSTEM_STATE_MV_PREPARE] = rtm_state_machine_prepare;

    /* 4 STATE_MACHINE_PREPARE */
    state_table[STATE_MACHINE_PREPARE][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_PREPARE][SYSTEM_STATE_MV_READY] = rtm_state_machine_ready;
    state_table[STATE_MACHINE_PREPARE][SYSTEM_STATE_MV_PREPARE] = rtm_state_machine_prepare;


    /* 5 STATE_MACHINE_READY */
    state_table[STATE_MACHINE_READY][SYSTEM_STATE_MV_READY] = rtm_state_machine_ready;
    state_table[STATE_MACHINE_READY][SYSTEM_STATE_MV_RADIATION] = rtm_state_machine_work;
    state_table[STATE_MACHINE_READY][SYSTEM_STATE_MV_TERMINATE] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_READY][SYSTEM_STATE_MV_INTERRUPT] = rtm_state_machine_interrupt;

    /* 6 STATE_MACHINE_WORK */
    state_table[STATE_MACHINE_WORK][SYSTEM_STATE_MV_RADIATION] = rtm_state_machine_work;
    state_table[STATE_MACHINE_WORK][SYSTEM_STATE_MV_COMPLETE] = rtm_state_machine_complete;
    state_table[STATE_MACHINE_WORK][SYSTEM_STATE_MV_TERMINATE] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_WORK][SYSTEM_STATE_MV_INTERRUPT] = rtm_state_machine_interrupt;

    /* 11 STATE_MACHINE_MANUAL */
    state_table[STATE_MACHINE_MANUAL][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_idle;

    /* 12 STATE_MACHINE_COMPLETE */
    state_table[STATE_MACHINE_COMPLETE][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_idle;
    state_table[STATE_MACHINE_COMPLETE][SYSTEM_STATE_MV_COMPLETE] = rtm_state_machine_complete;
    state_table[STATE_MACHINE_COMPLETE][SYSTEM_STATE_MV_PREPARE] = rtm_state_machine_prepare;

    /* 13 STATE_MACHINE_SHUTDOWN */

    /* 14 STATE_MACHINE_POWER_SAVER */
    state_table[STATE_MACHINE_POWER_SAVER][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_idle;
    state_table[STATE_MACHINE_POWER_SAVER][SYSTEM_STATE_SHUTDOWN] = rtm_state_machine_shutdown;

    /* 15 STATE_MACHINE_TERMINATE */
    state_table[STATE_MACHINE_TERMINATE][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_idle;
    state_table[STATE_MACHINE_TERMINATE][SYSTEM_STATE_MV_TERMINATE] = rtm_state_machine_terminate;

    /* 16 STATE_MACHINE_INTERRUPT */
    state_table[STATE_MACHINE_INTERRUPT][SYSTEM_STATE_MV_READY] = rtm_state_machine_interrupt;
    state_table[STATE_MACHINE_INTERRUPT][SYSTEM_STATE_MV_TERMINATE] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_INTERRUPT][SYSTEM_STATE_MV_INTERRUPT] = rtm_state_machine_interrupt;

    /* 17 STATE_MACHINE_KV_PRELIMINARY */
    state_table[STATE_MACHINE_KV_PRELIMINARY][SYSTEM_STATE_KV_PRELIMINARY] = rtm_state_machine_kv_preliminary;
    state_table[STATE_MACHINE_KV_PRELIMINARY][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_KV_PRELIMINARY][SYSTEM_STATE_KV_PREPARE] = rtm_state_machine_kv_prepare;
    /* 18 STATE_MACHINE_KV_PREPARE */
    state_table[STATE_MACHINE_KV_PREPARE][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_KV_PREPARE][SYSTEM_STATE_KV_PREPARE] = rtm_state_machine_kv_prepare;
    state_table[STATE_MACHINE_KV_PREPARE][SYSTEM_STATE_SURVIEW_READY] = rtm_state_machine_surview_ready;
    state_table[STATE_MACHINE_KV_PREPARE][SYSTEM_STATE_CT_READY] = rtm_state_machine_ct_ready;
    /* 19 STATE_MACHINE_SURVIEW_READY */
    state_table[STATE_MACHINE_SURVIEW_READY][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_SURVIEW_READY][SYSTEM_STATE_SURVIEW_READY] = rtm_state_machine_surview_ready;
    state_table[STATE_MACHINE_SURVIEW_READY][SYSTEM_STATE_SURVIEW_RADIATION] = rtm_state_machine_surview_work;
    /* 20 STATE_MACHINE_CT_READY */
    state_table[STATE_MACHINE_CT_READY][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_CT_READY][SYSTEM_STATE_CT_READY] = rtm_state_machine_ct_ready;
    state_table[STATE_MACHINE_CT_READY][SYSTEM_STATE_CT_RADIATION] = rtm_state_machine_ct_work;
    /* 21 STATE_MACHINE_SURVIEW_WORK */
    state_table[STATE_MACHINE_SURVIEW_WORK][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_SURVIEW_WORK][SYSTEM_STATE_SURVIEW_RADIATION] = rtm_state_machine_surview_work;
    state_table[STATE_MACHINE_SURVIEW_WORK][SYSTEM_STATE_KV_COMPLETE] = rtm_state_machine_kv_complete;
    /* 22 STATE_MACHINE_CT_WORK */
    state_table[STATE_MACHINE_CT_WORK][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_terminate;
    state_table[STATE_MACHINE_CT_WORK][SYSTEM_STATE_CT_RADIATION] = rtm_state_machine_ct_work;
    state_table[STATE_MACHINE_CT_WORK][SYSTEM_STATE_KV_COMPLETE] = rtm_state_machine_kv_complete;
    /* 23 STATE_MACHINE_KV_COMPLETE */
    state_table[STATE_MACHINE_KV_COMPLETE][SYSTEM_STATE_SYSTEM_ON] = rtm_state_machine_idle;
    state_table[STATE_MACHINE_KV_COMPLETE][SYSTEM_STATE_KV_COMPLETE] = rtm_state_machine_kv_complete;

    device_err = stateTable_ctor(self,
                                 (Tran const *)state_table,
                                 STATE_MACHINE_MAX,
                                 SYSTEM_STATE_MAX,
                                 rtm_state_machine_initial);
    if (device_err != DEV_EOK)
    {
        LOG_I("rtm state machine ctor fail\r\n");
    }
}
/*************************************state machine end************************/