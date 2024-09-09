#include "fsm_app.h"
#include "cmsis_os2.h"
#include "init_call.h"

static osMutexId_t dose_fsm_mutex = NULL;
static enum dose_fsm_state dose_fsm_state_current = FSM_STATE_INIT;
static osEventFlagsId_t dose_fsm_event = NULL;
#define DOSE_FSM_CHANGE_EVENT   (1 << 0)
int8_t dose_fsm_state_set(enum dose_fsm_state state)
{
    if (state >= FSM_STATE_MAX) 
    {
        return -1;
    }

    osMutexAcquire(dose_fsm_mutex, osWaitForever);

    dose_fsm_state_current = state;

    osMutexRelease(dose_fsm_mutex);

    osEventFlagsSet(dose_fsm_event, DOSE_FSM_CHANGE_EVENT);

    return 0;
}

enum dose_fsm_state dose_fsm_state_get(void)
{
    enum dose_fsm_state state;

    osMutexAcquire(dose_fsm_mutex, osWaitForever);

    state = dose_fsm_state_current;

    osMutexRelease(dose_fsm_mutex);

    return state;
}

static int8_t dose_fsm_process_entry(void *argument)
{
    for (;;)
    {
        osEventFlagsWait(dose_fsm_event, DOSE_FSM_CHANGE_EVENT, osFlagsWaitAny, osWaitForever);

        switch (dose_fsm_state_get())
        {
        case FSM_STATE_INIT:
            break;
        case FSM_STATE_SETTING:
            break;
        case FSM_STATE_DUMMY:
            break;
        case FSM_STATE_READY:
            break;
        case FSM_STATE_RADIATION:
            break;
        case FSM_STATE_STOP:
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

static int8_t dose_fsm_thread_init(void)
{
    osMutexAttr_t dose_fsm_mutex_attr = {
    .name = "dose_fsm_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    dose_fsm_mutex = osMutexNew(&dose_fsm_mutex_attr);
    if (dose_fsm_mutex == NULL)
    {
        printf("dose fsm mutex create failed\r\n");
        return -1;
    }

    dose_fsm_event = osEventFlagsNew(NULL);
    if (dose_fsm_event == NULL)
    {
        printf("dose fsm event create failed\r\n");
        return -2;
    }   

    osThreadAttr_t dose_fsm_thread_attributes = {
    .name = "dose_fsm_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    osThreadId_t dose_fsm_threadHandle = osThreadNew(dose_fsm_process_entry, NULL, &dose_fsm_thread_attributes);
    if (dose_fsm_threadHandle == NULL)
    {
        printf("thread dose fsm create failed\r\n");
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(dose_fsm_thread_init);