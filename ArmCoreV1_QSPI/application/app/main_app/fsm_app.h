#ifndef __FSM_APP_H__
#define __FSM_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum fsm_state
{
    FSM_STATE_INIT = 0,
    FSM_STATE_IDLE,
    FSM_STATE_DUMMY,
    FSM_STATE_DUMMY_END,
    FSM_STATE_PREPARE,
    FSM_STATE_READY,
    FSM_STATE_RADIATION,
    FSM_STATE_COMPLETE,
    FSM_STATE_FAULT,
    FSM_STATE_MAX
};

enum fsm_state fsm_state_get(void);
int8_t fsm_state_switch(enum fsm_state new_state);

#ifdef __cplusplus
}
#endif

#endif /* __FSM_APP_H__ */