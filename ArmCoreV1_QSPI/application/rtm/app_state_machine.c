/**
 * @file app_state_machine.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-05-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "app_state_machine.h"
#include "ulog.h"
#include "rtm_main.h"

#define RTM_ERROR_WAIT_TIME (25)

static int32_t stateMachine_ctor(StateMachine_t *self,
                                 StateHandler_t initial)
{
    if (self == NULL || initial == NULL)
    {
        return -1;
    }
    memset(self, 0, sizeof(StateMachine_t));
    self->StateHandler = initial;
    return 0;
}
static int32_t stateMachine_init(StateMachine_t *self, Event_t const *e)
{
    if (self == NULL)
    {
        return -1;
    }
    self->StateHandler(self, e);
    return 0;
}
static int32_t stateMachine_dispatch(StateMachine_t *self, Event_t const *e)
{
    if (self == NULL || e == NULL)
    {
        return -1;
    }
    if (e->sig >= MAX_SIG)
    {
        return -2;
    }
    if (self->StateHandler == NULL)
    {
        return -3;
    }
    StateHandler_t handler = self->StateHandler;
    Event_t event = {0};
    State_t status = handler(self, e);

    while (status == RET_TRAN)
    {
        event.sig = EXIT_SIG;
        handler(self, &event);

        handler = self->StateHandler;

        event.sig = ENTER_SIG;
        status = self->StateHandler(self, &event);
    }
    return status;
}

/*************************************state machine start**********************/

static State_t system_initialization(void *self, Event_t const *const e);
static State_t system_systemOn(void *self, Event_t const *const e);
static State_t system_shutdown(void *self, Event_t const *const e);
static State_t system_powerSaver(void *self, Event_t const *const e);
static State_t system_mv_preliminary(void *self, Event_t const *const e);
static State_t system_mv_prepare(void *self, Event_t const *const e);
static State_t system_mv_ready(void *self, Event_t const *const e);
static State_t system_mv_radiation(void *self, Event_t const *const e);
static State_t system_mv_complete(void *self, Event_t const *const e);
static State_t system_mv_interrupt(void *self, Event_t const *const e);
static State_t system_mv_terminate(void *self, Event_t const *const e);

static State_t system_kv_preliminary(void *self, Event_t const *const e);
static State_t system_kv_prepare(void *self, Event_t const *const e);
static State_t system_surview_ready(void *self, Event_t const *const e);
static State_t system_surview_radiation(void *self, Event_t const *const e);
static State_t system_ct_ready(void *self, Event_t const *const e);
static State_t system_ct_radiation(void *self, Event_t const *const e);
static State_t system_kv_complete(void *self, Event_t const *const e);
static State_t system_kv_terminate(void *self, Event_t const *const e);

static State_t module_init(void *self, Event_t const *const e);
static State_t module_idle(void *self, Event_t const *const e);
static State_t module_shutdown(void *self, Event_t const *const e);
static State_t module_powerSaver(void *self, Event_t const *const e);
static State_t module_mv_preliminary(void *self, Event_t const *const e);
static State_t module_mv_prepare(void *self, Event_t const *const e);
static State_t module_mv_ready(void *self, Event_t const *const e);
static State_t module_mv_work(void *self, Event_t const *const e);
static State_t module_mv_complete(void *self, Event_t const *const e);
static State_t module_mv_interrupt(void *self, Event_t const *const e);
static State_t module_mv_terminate(void *self, Event_t const *const e);
static State_t module_kv_preliminary(void *self, Event_t const *const e);
static State_t module_kv_prepare(void *self, Event_t const *const e);
static State_t module_surview_ready(void *self, Event_t const *const e);
static State_t module_surview_work(void *self, Event_t const *const e);
static State_t module_ct_ready(void *self, Event_t const *const e);
static State_t module_ct_work(void *self, Event_t const *const e);
static State_t module_kv_complete(void *self, Event_t const *const e);
static State_t module_kv_terminate(void *self, Event_t const *const e);

static int32_t fault_check(app_rtm_main_t *self, uint8_t state)
{
    int32_t retval = 0;
    dido_structure_t dido_structure = {0};
    app_do_get(&(self->app_dido), &dido_structure);
    app_di_get(&(self->app_dido), &dido_structure);

    if ((0 == dido_structure.tca9535_0x01_u.tca9535_0x01_bit.DI_CITB_EMERGENCY4) ||
        (0 == dido_structure.tca9535_0x02_u.tca9535_0x02_bit.DI_CITB_EMERGENCY2) ||
        (0 == dido_structure.tca9535_0x02_u.tca9535_0x02_bit.DI_CITB_EMERGENCY3) ||
        (0 == dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_CITB_EMERGENCY1) ||
        (0 == dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_CITB_EMERGENCY5))
    {
        self->serious_interlock.emergency_stop = 1;
    }

    if (((dido_structure.tca9535_0x01_u.tca9535_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR2 != 1) ||
         (dido_structure.tca9535_0x02_u.tca9535_0x02_bit.DI_CITB_TREATMENT_ROOM_DOOR1 != 1)))
    {
        if ((state == STATE_MACHINE_READY) ||
            (state == STATE_MACHINE_WORK) ||
            (state == STATE_MACHINE_INTERRUPT) ||
            (state == STATE_MACHINE_SURVIEW_READY) ||
            (state == STATE_MACHINE_SURVIEW_WORK) ||
            (state == STATE_MACHINE_CT_READY) ||
            (state == STATE_MACHINE_CT_WORK))
        {
            self->serious_interlock.door_open = 1;
        }
    }
    // HvEn check
    if (dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn ^ dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_HvEn)
    {
        self->serious_interlock.HvEN = 1;
    }
    // kv_treatment_en check
    if (dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn ^ dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_KV_TreatmentEN)
    {
        self->serious_interlock.KVTreatmentEn = 1;
    }
    // mv_treatment_en check
    if (dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn ^ dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_MV_TreatmentEN)
    {
        self->serious_interlock.MVTreatmentEn = 1;
    }
    *(uint32_t *)&(self->serious_interlock) &= ~(self->interlock_override);
    if (*((uint32_t *)&self->serious_interlock) != 0)
    {
        retval = -1;
    }
    return retval;
}
static int32_t fault_clear(app_rtm_main_t *self, uint8_t state)
{
    int32_t retval = 0;
    dido_structure_t dido_structure = {0};
    app_do_get(&(self->app_dido), &dido_structure);
    app_di_get(&(self->app_dido), &dido_structure);

    if ((0 == dido_structure.tca9535_0x01_u.tca9535_0x01_bit.DI_CITB_EMERGENCY4) ||
        (0 == dido_structure.tca9535_0x02_u.tca9535_0x02_bit.DI_CITB_EMERGENCY2) ||
        (0 == dido_structure.tca9535_0x02_u.tca9535_0x02_bit.DI_CITB_EMERGENCY3) ||
        (0 == dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_CITB_EMERGENCY1) ||
        (0 == dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_CITB_EMERGENCY5))
    {
        self->serious_interlock.emergency_stop = 1;
    }
    else
    {
        self->serious_interlock.emergency_stop = 0;
    }
    if (((dido_structure.tca9535_0x01_u.tca9535_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR2 != 1) ||
         (dido_structure.tca9535_0x02_u.tca9535_0x02_bit.DI_CITB_TREATMENT_ROOM_DOOR1 != 1)))
    {
        if ((state == STATE_MACHINE_READY) ||
            (state == STATE_MACHINE_WORK) ||
            (state == STATE_MACHINE_INTERRUPT) ||
            (state == STATE_MACHINE_SURVIEW_READY) ||
            (state == STATE_MACHINE_SURVIEW_WORK) ||
            (state == STATE_MACHINE_CT_READY) ||
            (state == STATE_MACHINE_CT_WORK))
        {
            self->serious_interlock.door_open = 1;
        }
        else
        {
            self->serious_interlock.door_open = 0;
        }
    }
    // HvEn check
    if (dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn ^ dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_HvEn)
    {
        self->serious_interlock.HvEN = 1;
    }
    else
    {
        self->serious_interlock.HvEN = 0;
    }
    // kv_treatment_en check
    if (dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn ^ dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_KV_TreatmentEN)
    {
        self->serious_interlock.KVTreatmentEn = 1;
    }
    else
    {
        self->serious_interlock.KVTreatmentEn = 0;
    }
    // mv_treatment_en check
    if (dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn ^ dido_structure.tca9535_0x03_u.tca9535_0x03_bit.DI_MV_TreatmentEN)
    {
        self->serious_interlock.MVTreatmentEn = 1;
    }
    else
    {
        self->serious_interlock.MVTreatmentEn = 0;
    }
    *(uint32_t *)&(self->serious_interlock) &= ~(self->interlock_override);
    if (*((uint32_t *)&self->serious_interlock) != 0)
    {
        retval = -1;
    }
    return retval;
}
static State_t system_initialization(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_initialization exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        rtm_sm->current_state = INITIALIZATION_SIG;
        status = TRAN(&module_init);
        // LOG_I("system_initialization enter\r\n");
        break;
    }
    }
    return status;
}

static State_t module_init(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.tca9535_0x04_u.tca9535_0x04_bit.DO_STAND_RESERVE = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_init enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        time = 0;
        // LOG_I("module_init exit\r\n");
        status = HANDLED();
        break;
    }
    case TIME_SIG:
    {
        time++;
        fault_check(rtm, STATE_MACHINE_INIT);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            status = TRAN(&system_systemOn);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}

static State_t system_systemOn(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        // LOG_I("system_systemOn enter\r\n");
        rtm_sm->current_state = SYSTEM_ON_SIG;
        status = TRAN(&module_idle);
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_systemOn exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_idle(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_idle enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        time = 0;
        fault_flag = 0;
        // LOG_I("module_idle exit\r\n");
        status = HANDLED();
        break;
    }
    case INITIALIZATION_SIG:
    {
        HAL_NVIC_SystemReset(); // reset system
        break;
    }
    case SHUTDOWN_SIG:
    {
        status = TRAN(&system_shutdown);
        break;
    }
    case POWER_SAVER_SIG:
    {
        status = TRAN(&system_powerSaver);
        break;
    }
    case MV_PREPARE_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_mv_preliminary);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case KV_PRELIMINARY_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_kv_preliminary);
        }
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_IDLE);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_shutdown(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = SHUTDOWN_SIG;
        status = TRAN(&module_shutdown);
        // LOG_I("system_shutdown enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        status = HANDLED();
        // LOG_I("system_shutdown exit\r\n");
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_shutdown(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_shutdown enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_shutdown exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_powerSaver(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = POWER_SAVER_SIG;
        status = TRAN(&module_powerSaver);
        // LOG_I("system_powerSaver enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_powerSaver exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_powerSaver(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_powerSaver enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_powerSaver exit\r\n");
        status = HANDLED();
        break;
    }
    case SHUTDOWN_SIG:
    {
        status = TRAN(&system_shutdown);
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_systemOn);
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_preliminary(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_PREPARE_SIG;
        status = TRAN(&module_mv_preliminary);
        // LOG_I("system_mv_preliminary enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_preliminary exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_preliminary(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_mv_preliminary enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_preliminary exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate); // TODO change to system_terminate
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_PRELIMINARY);
        if (error == 0)
        {
            status = TRAN(&system_mv_prepare);
        }
        else if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
            status = HANDLED();
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_PREPARE_SIG;
        status = TRAN(&module_mv_prepare);
        // LOG_I("system_mv_prepare enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_prepare exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_mv_prepare enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_prepare exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate); // TODO change to system_terminate
        break;
    }
    case MV_READY_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_mv_ready);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_PREPARE);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
        }
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_READY_SIG;
        status = TRAN(&module_mv_ready);
        // LOG_I("system_mv_ready enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_ready exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        // LOG_I("module_mv_ready enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_ready exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case MV_RADIATION_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_mv_radiation);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case MV_INTERRUPT_SIG:
    {
        status = TRAN(&system_mv_interrupt);
        break;
    }
    case MV_TERMINATE_SIG:
    {
        status = TRAN(&system_mv_terminate);
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_READY);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
            if (error != 0)
            {
                status = TRAN(&system_mv_interrupt);
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_radiation(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_RADIATION_SIG;
        status = TRAN(&module_mv_work);
        // LOG_I("system_mv_work enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_work exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_work(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.tca9535_0x04_u.tca9535_0x04_bit.DO_RadiationIndicator = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_mv_work enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_work exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case MV_COMPLETE_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_mv_complete);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case MV_INTERRUPT_SIG:
    {
        status = TRAN(&system_mv_interrupt);
        break;
    }
    case MV_TERMINATE_SIG:
    {
        status = TRAN(&system_mv_terminate);
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_WORK);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
            if (error != 0)
            {
                status = TRAN(&system_mv_interrupt);
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_complete(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_COMPLETE_SIG;
        status = TRAN(&module_mv_complete);
        // LOG_I("system_mv_complete enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_complete exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_complete(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.tca9535_0x04_u.tca9535_0x04_bit.DO_RadiationIndicator = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_mv_complete enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_complete exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_systemOn);
        break;
    }
    case MV_PREPARE_SIG:
    {
        status = TRAN(&system_mv_prepare);
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_COMPLETE);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
        }
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_interrupt(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_INTERRUPT_SIG;
        status = TRAN(&module_mv_interrupt);
        // LOG_I("system_mv_interrupt enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_interrupt exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_interrupt(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    static uint8_t fault_clear_flag = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.tca9535_0x04_u.tca9535_0x04_bit.DO_RadiationIndicator = 0;

        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_mv_interrupt enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_interrupt exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        fault_clear_flag = 0;
        status = HANDLED();
        break;
    }
    case MV_READY_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_mv_ready);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case MV_TERMINATE_SIG:
    {
        status = TRAN(&system_mv_terminate);
        break;
    }
    case TIME_SIG:
    {
        time++;
        if (fault_clear_flag == 0)
        {
            error = fault_check(rtm, STATE_MACHINE_INTERRUPT);
        }
        else
        {
            error = fault_clear(rtm, STATE_MACHINE_INTERRUPT);
        }
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            fault_clear_flag = 0;
            time = 0;
            fault_flag = 1;
        }

        status = HANDLED();
        break;
    }
    case ERROR_SIG:
    {
        fault_clear_flag = 1;
        time = 0;
        fault_flag = 0;
        error = 0;
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_terminate(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_TERMINATE_SIG;
        status = TRAN(&module_mv_terminate);
        // LOG_I("system_mv_terminate enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_terminate exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_terminate(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.tca9535_0x04_u.tca9535_0x04_bit.DO_RadiationIndicator = 0;

        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_mv_terminate enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_terminate exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_systemOn);
        // TODO:高级故障清除
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_TERMINATE);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_kv_terminate(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = SYSTEM_ON_SIG;
        status = TRAN(&module_kv_terminate);
        // LOG_I("system_kv_terminate enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_kv_terminate exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_kv_terminate(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.tca9535_0x04_u.tca9535_0x04_bit.DO_RadiationIndicator = 0;

        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_kv_terminate enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_kv_terminate exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_TERMINATE);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
            status = TRAN(&system_systemOn);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_kv_preliminary(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = KV_PRELIMINARY_SIG;
        status = TRAN(&module_kv_preliminary);
        // LOG_I("system_kv_preliminary enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_kv_preliminary exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_kv_preliminary(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_kv_preliminary enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_kv_preliminary exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case KV_PREPARE_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_kv_prepare);
        }
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_KV_PRELIMINARY);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
        }
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_kv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = KV_PREPARE_SIG;
        status = TRAN(&module_kv_prepare);
        // LOG_I("system_kv_prepare enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_kv_prepare exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_kv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_kv_prepare enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_kv_prepare exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case SURVIEW_READY_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_surview_ready);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case CT_READY_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_ct_ready);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_KV_PREPARE);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
        }
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_surview_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = SURVIEW_READY_SIG;
        status = TRAN(&module_surview_ready);
        // LOG_I("system_surview_ready enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_surview_ready exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_surview_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        // LOG_I("module_surview_ready enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_surview_ready exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case SURVIEW_RADIATION_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_surview_radiation);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_SURVIEW_READY);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
            if (error != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_surview_radiation(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = SURVIEW_RADIATION_SIG;
        status = TRAN(&module_surview_work);
        // LOG_I("system_surview_radiation enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_surview_radiation exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_surview_work(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.tca9535_0x04_u.tca9535_0x04_bit.DO_RadiationIndicator = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_surview_work enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_surview_work exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case KV_COMPLETE_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_kv_complete);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_SURVIEW_WORK);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
            if (error != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_ct_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = CT_READY_SIG;
        status = TRAN(&module_ct_ready);
        // LOG_I("system_ct_ready enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_ct_ready exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_ct_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        // LOG_I("module_ct_ready enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_ct_ready exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case CT_RADIATION_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_ct_radiation);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_CT_READY);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
            if (error != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_ct_radiation(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = CT_RADIATION_SIG;
        status = TRAN(&module_ct_work);
        // LOG_I("system_ct_radiation enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_ct_radiation exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_ct_work(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.tca9535_0x04_u.tca9535_0x04_bit.DO_RadiationIndicator = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_ct_work enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_ct_work exit\r\n");
        time = 0;
        fault_flag = 0;
        error = 0;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case KV_COMPLETE_SIG:
    {
        if ((error == 0) && (fault_flag == 1))
        {
            status = TRAN(&system_kv_complete);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_CT_WORK);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
            if (error != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_kv_complete(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = KV_COMPLETE_SIG;
        status = TRAN(&module_kv_complete);
        // LOG_I("system_kv_complete enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_kv_complete exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_kv_complete(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint32_t time = 0;
    static uint8_t fault_flag = 0;
    static int32_t error = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.tca9535_0x04_u.tca9535_0x04_bit.DO_RadiationIndicator = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_kv_complete enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_kv_complete exit\r\n");
        time = 0;
        fault_flag = 0;
        status = HANDLED();
        error = 0;
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_systemOn);
        break;
    }
    case TIME_SIG:
    {
        time++;
        error = fault_check(rtm, STATE_MACHINE_KV_COMPLETE);
        if (time >= RTM_ERROR_WAIT_TIME)
        {
            time = 0;
            fault_flag = 1;
        }
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
void rtm_state_machine_ctor(rtm_StateMachine_t *self, void *parameters)
{
    if (self == NULL)
    {
        return;
    }
    memset(self, 0, sizeof(rtm_StateMachine_t));
    self->parameters = parameters;
    self->current_state = NULL_SIG;
    stateMachine_ctor(self, system_initialization);
    stateMachine_dispatch(self, NULL_SIG);
}

int32_t rtm_state_dispatch(rtm_StateMachine_t *self, Event_t const *e)
{
    if (self == NULL || e == NULL)
    {
        return -1;
    }

    int32_t ret = stateMachine_dispatch(self, e);
    if (ret != 0)
    {
        return -2;
    }

    return 0;
}
rtm_state_t rtm_get_state(rtm_StateMachine_t *self)
{
    if (self == NULL)
    {
        return 0;
    }
    return self->current_state;
}

/*************************************state machine end************************/
