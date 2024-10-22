#include "fsm_app.h"
#include "cmsis_os2.h"
#include "init_call.h"
#include "gpio_app.h"

static osMutexId_t fsm_mutex = NULL;
static enum fsm_state fsm_state_current = FSM_STATE_IDLE;
static osEventFlagsId_t fsm_event = NULL;
#define FSM_CHANGE_EVENT   (1 << 0)
static int8_t fsm_state_set(enum fsm_state state)
{
    if (state >= FSM_STATE_MAX) 
    {
        return -1;
    }

    osMutexAcquire(fsm_mutex, osWaitForever);

    fsm_state_current = state;

    osMutexRelease(fsm_mutex);

    osEventFlagsSet(fsm_event, FSM_CHANGE_EVENT);

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
    case FSM_STATE_DUMMY:
        ret = dose_hv_enable_set(1);
        break;
    case FSM_STATE_PREPARE:
        ret = dose_hv_enable_set(1);
        break;
    case FSM_STATE_READY:
        break;
    case FSM_STATE_RADIATION:
        break;
    case FSM_STATE_COMPLETE:
        break;
    case FSM_STATE_FAULT:
        ret = dose_hv_enable_set(0);
        ret |= dose_trigger_out_set(1);
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

static int8_t fsm_process_entry(void *argument)
{
    int8_t ret = 0;

    for (;;)
    {
        osEventFlagsWait(fsm_event, FSM_CHANGE_EVENT, osFlagsWaitAny, osWaitForever);

        switch (fsm_state_get())
        {
        case FSM_STATE_INIT:
            break;
        case FSM_STATE_IDLE:
            break;
        case FSM_STATE_DUMMY:
            break;
        case FSM_STATE_PREPARE:
            break;
        case FSM_STATE_READY:
            break;
        case FSM_STATE_RADIATION:
            break;
        case FSM_STATE_COMPLETE:
            break;
        case FSM_STATE_FAULT:
            break;
        default:
            printf("invalid fsm state\r\n");
            return -2;
            break;
        }
    }

    return 0;
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

    fsm_event = osEventFlagsNew(NULL);
    if (fsm_event == NULL)
    {
        printf("fsm event create failed\r\n");
        return -2;
    }   

    osThreadAttr_t fsm_thread_attributes = {
    .name = "fsm_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    osThreadId_t fsm_threadHandle = osThreadNew(fsm_process_entry, NULL, &fsm_thread_attributes);
    if (fsm_threadHandle == NULL)
    {
        printf("thread fsm create failed\r\n");
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(fsm_thread_init);