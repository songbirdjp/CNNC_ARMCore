/**
 * @file app_prompt.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-09-04
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "app_prompt.h"
#include "string.h"
#include "rtm_main.h"
#include "ulog.h"
static State_t prompt_initialization(void *self, Event_t const *const e);
static State_t prompt_systemOn(void *self, Event_t const *const e);
static State_t prompt_mv_prepare(void *self, Event_t const *const e);
static State_t prompt_mv_ready(void *self, Event_t const *const e);
static State_t prompt_mv_radiation(void *self, Event_t const *const e);
static State_t prompt_mv_complete(void *self, Event_t const *const e);
static State_t prompt_mv_interrupt(void *self, Event_t const *const e);
static State_t prompt_mv_terminate(void *self, Event_t const *const e);
static State_t prompt_kv_preliminary(void *self, Event_t const *const e);
static State_t prompt_kv_prepare(void *self, Event_t const *const e);
static State_t prompt_kv_ready(void *self, Event_t const *const e);
static State_t prompt_kv_radiation(void *self, Event_t const *const e);
static State_t prompt_kv_complete(void *self, Event_t const *const e);

static State_t prompt_systemOn_s(void *self, Event_t const *const e);
static State_t prompt_setup(void *self, Event_t const *const e);
static State_t prompt_setup_wait(void *self, Event_t const *const e);
static State_t prompt_load(void *self, Event_t const *const e);
static State_t prompt_load_wait(void *self, Event_t const *const e);
static State_t prompt_load_ready(void *self, Event_t const *const e);
static State_t prompt_prepare(void *self, Event_t const *const e);
static State_t prompt_prepare_wait_1(void *self, Event_t const *const e);
static State_t prompt_prepare_wait_2(void *self, Event_t const *const e);
static State_t prompt_prepare_ready(void *self, Event_t const *const e);
static State_t prompt_unload(void *self, Event_t const *const e);
static State_t prompt_unload_wait(void *self, Event_t const *const e);
static State_t prompt_unload_ready(void *self, Event_t const *const e);
static State_t prompt_mv_prepare_s(void *self, Event_t const *const e);
static State_t prompt_mv_ready_s(void *self, Event_t const *const e);
static State_t prompt_mv_radiation_s(void *self, Event_t const *const e);
static State_t prompt_mv_interrupt_s(void *self, Event_t const *const e);
static State_t prompt_mv_terminate_s(void *self, Event_t const *const e);
static State_t prompt_mv_complete_s(void *self, Event_t const *const e);
static State_t prompt_kv_preliminary_s(void *self, Event_t const *const e);
static State_t prompt_kv_prepare_s(void *self, Event_t const *const e);
static State_t prompt_kv_ready_s(void *self, Event_t const *const e);
static State_t prompt_kv_radiation_s(void *self, Event_t const *const e);
static State_t prompt_kv_complete_s(void *self, Event_t const *const e);
void prompt_state_machine_ctor(app_prompt_t *self, void *userdata)
{
    if (self == NULL || userdata == NULL)
    {
        return;
    }
    memset(self, 0, sizeof(app_prompt_t));
    self->userdata = userdata;
    self->last_system_state = PROMPT_INITIALIZATION_STATE;
    self->last_last_system_state = PROMPT_INITIALIZATION_STATE;
    self->trm_require_state = TRM_REQUIRE_NULL;
    stateMachine_ctor(self, prompt_initialization);
    stateMachine_dispatch(self, PROMPT_NULL_SIG);
}
int32_t prompt_state_dispatch(app_prompt_t *self, Event_t const *e)
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
int32_t prompt_state_trm_require_get(app_prompt_t *self, uint8_t *trm_require_state)
{
    if (self == NULL || trm_require_state == NULL)
    {
        return -1;
    }
    *trm_require_state = self->trm_require_state;
    return 0;
}
int32_t prompt_state_load_position_get(app_prompt_t *self, psm_position_t *psm_position)
{
    if (self == NULL || psm_position == NULL)
    {
        return -1;
    }
    memcpy(psm_position, &self->position, sizeof(psm_position_t));
    return 0;
}
int32_t prompt_state_prompt_get(app_prompt_t *self, prompt_state_t *prompt_state)
{
    if (self == NULL || prompt_state == NULL)
    {
        return -1;
    }
    memcpy(prompt_state, &self->prompt_state, sizeof(prompt_state_t));
    return 0;
}
static State_t prompt_initialization(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        status = HANDLED();
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
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
static State_t prompt_systemOn(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_systemOn\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        status = TRAN(&prompt_systemOn_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_systemOn\r\n");
        status = HANDLED();
        prompt_sm->last_system_state = PROMPT_SYSTEM_ON_STATE;
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
static State_t prompt_mv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_prepare\r\n");
        status = TRAN(&prompt_mv_prepare_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_prepare\r\n");
        prompt_sm->last_last_system_state = prompt_sm->last_system_state;
        LOG_I("prompt_sm->last_last_system_state = %d\r\n", prompt_sm->last_last_system_state);
        LOG_I("prompt_sm->last_system_state = %d\r\n", prompt_sm->last_system_state);
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
static State_t prompt_mv_ready(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_ready\r\n");
        LOG_I("prompt_sm->last_last_system_state = %d\r\n", prompt_sm->last_last_system_state);
        LOG_I("prompt_sm->last_system_state = %d\r\n", prompt_sm->last_system_state);
        if ((prompt_sm->last_last_system_state == PROMPT_SYSTEM_ON_STATE) ||
            (prompt_sm->last_system_state == PROMPT_MV_INTERRUPT_STATE))
        {
            prompt_sm->prompt_state.led_blink_mv = 1;
        }
        else
        {
            memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        }
        status = TRAN(&prompt_mv_ready_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_ready\r\n");
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
static State_t prompt_mv_radiation(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_radiation\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        status = TRAN(&prompt_mv_radiation_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_radiation\r\n");
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
static State_t prompt_mv_complete(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_complete\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        status = TRAN(&prompt_mv_complete_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_complete\r\n");
        prompt_sm->last_system_state = PROMPT_MV_COMPLETE_STATE;
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
static State_t prompt_mv_interrupt(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_interrupt\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        status = TRAN(&prompt_mv_interrupt_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_interrupt\r\n");
        prompt_sm->last_system_state = PROMPT_MV_INTERRUPT_STATE;
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
static State_t prompt_mv_terminate(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_terminate\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        status = TRAN(&prompt_mv_terminate_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_terminate\r\n");
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
static State_t prompt_kv_preliminary(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_preliminary\r\n");
        status = TRAN(&prompt_kv_preliminary_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_preliminary\r\n");
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
static State_t prompt_kv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_prepare\r\n");
        status = TRAN(&prompt_kv_prepare_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_prepare\r\n");
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
static State_t prompt_kv_ready(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_ready\r\n");
        prompt_sm->prompt_state.led_blink_kv = 1;
        status = TRAN(&prompt_kv_ready_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_ready\r\n");
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
static State_t prompt_kv_radiation(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_radiation\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        status = TRAN(&prompt_kv_radiation_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_radiation\r\n");
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
static State_t prompt_kv_complete(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_complete\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        status = TRAN(&prompt_kv_complete_s);
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_complete\r\n");
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

static State_t prompt_systemOn_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_systemOn_s\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_systemOn_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_SEND_SUCCEED_SIG:
    {
        status = TRAN(&prompt_setup);
        break;
    }
    case PROMPT_RT_CLEAR_SIG:
    {
        status = TRAN(&prompt_unload);
        break;
    }
    case PROMPT_RT_SWITCH_SIG:
    {
        status = TRAN(&prompt_prepare);
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
static State_t prompt_setup(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_setup\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        prompt_sm->prompt_state.led_blink_setup = 1;
        prompt_sm->trm_require_state = TRM_REQUIRE_SETUP;
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_setup\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_BUTTON_SETUP_SIG:
    {
        status = TRAN(&prompt_setup_wait);
        break;
    }
    case PROMPT_RT_CLEAR_SIG:
    {
        status = TRAN(&prompt_unload);
        break;
    }
    case PROMPT_BUTTON_LOAD_SIG:
    {
        status = TRAN(&prompt_load_wait);
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
static State_t prompt_setup_wait(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_setup_wait\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_setup_wait\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_ARRIVE_SETUP_SIG:
    {
        status = TRAN(&prompt_load);
        break;
    }
    case PROMPT_RT_CLEAR_SIG:
    {
        status = TRAN(&prompt_unload);
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
static State_t prompt_load(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_load\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        prompt_sm->prompt_state.setup_vibration = 1;
        prompt_sm->prompt_state.setup_beep = 1;
        prompt_sm->prompt_state.led_blink_load = 1;
        prompt_sm->trm_require_state = TRM_REQUIRE_LOAD;
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_load\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_CLEAR_SIG:
    {
        status = TRAN(&prompt_unload);
        break;
    }
    case PROMPT_BUTTON_LOAD_SIG:
    {
        status = TRAN(&prompt_load_wait);
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
static State_t prompt_load_wait(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)prompt_sm->userdata;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_load_wait\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        prompt_sm->prompt_state.led_blink_load = 1;
        prompt_sm->prompt_state.led_blink_prepare = 1;
        prompt_sm->position.data_valid_flag = 1;
        memcpy(&prompt_sm->position.psm_position_x_cur, &rtm->psm_info.psm_position_x_cur, sizeof(psm_position_t) - sizeof(uint16_t));
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_load_wait\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_CLEAR_SIG:
    {
        status = TRAN(&prompt_unload);
        break;
    }
    case PROMPT_BUTTON_PREPARE_SIG:
    {
        status = TRAN(&prompt_prepare_wait_2);
        break;
    }
    case PROMPT_ARRIVE_LOAD_SIG:
    {
        status = TRAN(&prompt_load_ready);
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
static State_t prompt_load_ready(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_load_ready\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        prompt_sm->prompt_state.load_vibration = 1;
        prompt_sm->prompt_state.load_beep = 1;
        prompt_sm->prompt_state.led_blink_prepare = 1;
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_load_ready\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_CLEAR_SIG:
    {
        status = TRAN(&prompt_unload);
        break;
    }
    case PROMPT_BUTTON_PREPARE_SIG:
    {
        status = TRAN(&prompt_prepare_wait_1);
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
static State_t prompt_prepare(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_prepare\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        prompt_sm->prompt_state.led_blink_prepare = 1;
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_prepare\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_CLEAR_SIG:
    {
        status = TRAN(&prompt_unload);
        break;
    }
    case PROMPT_BUTTON_PREPARE_SIG:
    {
        status = TRAN(&prompt_prepare_wait_1);
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
static State_t prompt_prepare_wait_1(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_prepare_wait_1\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        prompt_sm->prompt_state.led_blink_prepare = 1;
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_prepare_wait_1\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_CLEAR_SIG:
    {
        status = TRAN(&prompt_unload);
        break;
    }
    // case PROMPT_ARRIVE_PREPARE_SIG:
    // {
    //     status = TRAN(&prompt_prepare_ready);
    //     break;
    // }
    case PROMPT_MV_PREPARE_SIG:
    {
        status = TRAN(&prompt_mv_prepare);
        break;
    }
    case PROMPT_KV_PRELIMINARY_SIG:
    {
        status = TRAN(&prompt_kv_preliminary);
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
static State_t prompt_prepare_wait_2(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_prepare_wait_2\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_prepare_wait_2\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_CLEAR_SIG:
    {
        status = TRAN(&prompt_unload);
        break;
    }
    case PROMPT_MV_PREPARE_SIG:
    {
        status = TRAN(&prompt_mv_prepare);
        break;
    }
    case PROMPT_KV_PRELIMINARY_SIG:
    {
        status = TRAN(&prompt_kv_preliminary);
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
static State_t prompt_prepare_ready(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_prepare_ready\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        prompt_sm->prompt_state.prepare_vibration = 1;
        prompt_sm->prompt_state.prepare_beep = 1;
        prompt_sm->trm_require_state = TRM_REQUIRE_LOAD_READY;
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_prepare_ready\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_MV_READY_SIG:
    {
        status = TRAN(&prompt_mv_ready);
        break;
    }
    case PROMPT_KV_READY_SIG:
    {
        status = TRAN(&prompt_kv_ready);
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
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
static State_t prompt_unload(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_unload\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        prompt_sm->prompt_state.led_blink_unload = 1;
        prompt_sm->position.data_valid_flag = 0;
        prompt_sm->trm_require_state = TRM_REQUIRE_UNLOAD;
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_unload\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_SEND_SUCCEED_SIG:
    {
        status = TRAN(&prompt_setup);
        break;
    }
    case PROMPT_BUTTON_UNLOAD_SIG:
    {
        status = TRAN(&prompt_unload_wait);
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
static State_t prompt_unload_wait(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_unload_wait\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_unload_wait\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_SEND_SUCCEED_SIG:
    {
        status = TRAN(&prompt_setup);
        break;
    }
    case PROMPT_ARRIVE_UNLOAD_SIG:
    {
        status = TRAN(&prompt_unload_ready);
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
static State_t prompt_unload_ready(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_unload_ready\r\n");
        memset(&prompt_sm->prompt_state, 0, sizeof(prompt_state_t));
        prompt_sm->prompt_state.unload_beep = 1;
        prompt_sm->prompt_state.unload_vibration = 1;
        prompt_sm->trm_require_state = TRM_REQUIRE_NULL;
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_unload_ready\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_RT_SEND_SUCCEED_SIG:
    {
        status = TRAN(&prompt_setup);
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
static State_t prompt_mv_prepare_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_prepare_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_prepare_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
        break;
    }
    case PROMPT_ARRIVE_PREPARE_SIG:
    {
        if (prompt_sm->last_system_state == PROMPT_SYSTEM_ON_STATE)
        {
            status = TRAN(&prompt_prepare_ready);
        }
        break;
    }
    case PROMPT_KV_READY_SIG:
    {
        if (prompt_sm->last_system_state == PROMPT_MV_COMPLETE_STATE)
        {
            status = TRAN(&prompt_mv_ready);
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
static State_t prompt_mv_ready_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_ready_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_ready_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_MV_RADIATION_SIG:
    {
        status = TRAN(&prompt_mv_radiation);
        break;
    }
    case PROMPT_MV_INTERRUPT_SIG:
    {
        status = TRAN(&prompt_mv_interrupt);
        break;
    }
    case PROMPT_MV_TERMINATE_SIG:
    {
        status = TRAN(&prompt_mv_terminate);
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
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
static State_t prompt_mv_radiation_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_radiation_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_radiation_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_MV_COMPLETE_SIG:
    {
        status = TRAN(&prompt_mv_complete);
        break;
    }
    case PROMPT_MV_INTERRUPT_SIG:
    {
        status = TRAN(&prompt_mv_interrupt);
        break;
    }
    case PROMPT_MV_TERMINATE_SIG:
    {
        status = TRAN(&prompt_mv_terminate);
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
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
static State_t prompt_mv_interrupt_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_interrupt_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_interrupt_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_MV_READY_SIG:
    {
        status = TRAN(&prompt_mv_ready);
        break;
    }
    case PROMPT_MV_TERMINATE_SIG:
    {
        status = TRAN(&prompt_mv_terminate);
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
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
static State_t prompt_mv_terminate_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_terminate_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_terminate_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
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
static State_t prompt_mv_complete_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_mv_complete_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_mv_complete_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
        break;
    }
    case PROMPT_MV_PREPARE_SIG:
    {
        status = TRAN(&prompt_mv_prepare);
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
static State_t prompt_kv_preliminary_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_preliminary_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_preliminary_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
        break;
    }
    case PROMPT_KV_PREPARE_SIG:
    {
        status = TRAN(&prompt_kv_prepare);
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
static State_t prompt_kv_prepare_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_prepare_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_prepare_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
        break;
    }
    case PROMPT_ARRIVE_PREPARE_SIG:
    {
        status = TRAN(&prompt_prepare_ready);
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
static State_t prompt_kv_ready_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_ready_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_ready_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
        break;
    }
    case PROMPT_KV_RADIATION_SIG:
    {
        status = TRAN(&prompt_kv_radiation);
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
static State_t prompt_kv_radiation_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_radiation_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_radiation_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
        break;
    }
    case PROMPT_KV_COMPLETE_SIG:
    {
        status = TRAN(&prompt_kv_complete);
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
static State_t prompt_kv_complete_s(void *self, Event_t const *const e)
{
    State_t status;
    app_prompt_t *prompt_sm = (app_prompt_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        LOG_I("enter prompt_kv_complete_s\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        LOG_I("exit prompt_kv_complete_s\r\n");
        status = HANDLED();
        break;
    }
    case PROMPT_SYSTEM_ON_SIG:
    {
        status = TRAN(&prompt_systemOn);
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