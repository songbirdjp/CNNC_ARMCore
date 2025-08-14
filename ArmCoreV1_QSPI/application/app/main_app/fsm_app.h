#ifndef __FSM_APP_H__
#define __FSM_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


enum fsm_state {
    FSM_STATE_POST,
    FSM_STATE_INIT,
    FSM_STATE_IDLE,
    FSM_STATE_PREPARE,
    FSM_STATE_READY,
    FSM_STATE_WORK,
    FSM_STATE_MOVE, /* reserved */
    FSM_STATE_FAULT,
    FSM_STATE_MAX
};

enum function_index {
    ACTION_1,
    ACTION_2,
    ACTION_3,
    ACTION_4,
    ACTION_5,
    ACTION_6,
    ACTION_7,
    ACTION_8,
    ACTION_9,
    ACTION_10,
    ACTION_MAX
};


int8_t fsm_state_set(enum fsm_state state);
enum fsm_state fsm_state_get(void);
int8_t action_of_function_authority_check(enum function_index function_index);  /* 0: forbidden, 1: allowed */

#ifdef __cplusplus
}
#endif

#endif /* __FSM_APP_H__ */