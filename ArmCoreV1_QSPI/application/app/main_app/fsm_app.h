#ifndef __FSM_APP_H__
#define __FSM_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum fsm_state
{
    FSM_STATE_INIT = 1,
    FSM_STATE_IDLE,
    FSM_STATE_PRELIMINARY,
    FSM_STATE_PREPARE,
    FSM_STATE_READY,
    FSM_STATE_WORK,
    FSM_STATE_PARK = 10,
    FSM_STATE_MANUAL,
    FSM_STATE_COMPLETE,
    FSM_STATE_SHUTDOWN,
    FSM_STATE_POWERSAVER,
    FSM_STATE_TERMINATE,
    FSM_STATE_INTERRUPT,
    FSM_STATE_PRELIMINARY_BEGIN = 30,
    FSM_STATE_MAX
};

enum fsm_state fsm_state_get(void);
int8_t fsm_state_switch(enum fsm_state new_state);

#ifdef __cplusplus
}
#endif

#endif /* __FSM_APP_H__ */