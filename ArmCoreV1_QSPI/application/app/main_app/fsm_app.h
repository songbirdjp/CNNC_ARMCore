#ifndef __FSM_APP_H__
#define __FSM_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum dose_fsm_state
{
    FSM_STATE_INIT,
    FSM_STATE_SETTING,
    FSM_STATE_DUMMY,
    FSM_STATE_READY,
    FSM_STATE_RADIATION,
    FSM_STATE_STOP,
    FSM_STATE_FAULT,
    FSM_STATE_MAX
};

int8_t dose_fsm_state_set(enum dose_fsm_state state);
enum dose_fsm_state dose_fsm_state_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __FSM_APP_H__ */