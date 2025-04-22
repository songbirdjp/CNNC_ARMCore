#include "fsm_app.h"
#include "cmsis_os2.h"
#include "init_call.h"
#include "gpio_app.h"

static osMutexId_t fsm_mutex = NULL;
static enum fsm_state fsm_state_current = FSM_STATE_INIT;
static int8_t fsm_state_set(enum fsm_state state)
{
    if (state >= FSM_STATE_MAX) 
    {
        return -1;
    }

    osMutexAcquire(fsm_mutex, osWaitForever);

    fsm_state_current = state;

    osMutexRelease(fsm_mutex);

    return 0;
}

enum fsm_state fsm_state_get(void)
{
    enum fsm_state state;

    osMutexAcquire(fsm_mutex, osWaitForever);

    state = fsm_state_current;

    osMutexRelease(fsm_mutex);

    return state;
}

int8_t fsm_state_switch(enum fsm_state new_state)
{
    int8_t ret = 0;

    switch (new_state)
    {
    case FSM_STATE_INIT:
        break;
    case FSM_STATE_IDLE:
        ret = dose_hv_enable_set(0);
        break;
    case FSM_STATE_PRELIMINARY:
        ret = dose_hv_enable_set(0);
        break;
    case FSM_STATE_PREPARE:
        ret = dose_hv_enable_set(1);
        break;
    case FSM_STATE_READY:
        break;
    case FSM_STATE_WORK:
        break;
    case FSM_STATE_PARK:
        break;
    case FSM_STATE_MANUAL:
        break;
    case FSM_STATE_COMPLETE:
        break;
    case FSM_STATE_SHUTDOWN:
        break;
    case FSM_STATE_POWERSAVER:
        break;
    case FSM_STATE_TERMINATE:
        ret = dose_hv_enable_set(0);
        ret |= dose_trigger_out_set(1);
        break;
    case FSM_STATE_INTERRUPT:
        break;
    case FSM_STATE_PRELIMINARY_BEGIN:
        ret = dose_hv_enable_set(1);
        break;
    default:
        ret = -1;
        break;
    }

    if (ret == 0)
    {
        ret = fsm_state_set(new_state);
    }
    else
    {
        printf("fsm state switch %d failed: %d\r\n", new_state, ret);
    }

    return ret;
}

static int8_t fsm_thread_init(void)
{
    osMutexAttr_t fsm_mutex_attr = {
    .name = "fsm_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    fsm_mutex = osMutexNew(&fsm_mutex_attr);
    if (fsm_mutex == NULL)
    {
        printf("fsm mutex create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(fsm_thread_init);