/**
 * @file app_state_machine.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-05-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef _APP_STATE_MACHINE_H_
#define _APP_STATE_MACHINE_H_

#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif

    enum
    {
        STATE_MACHINE_NULL = 0, /*< Null state */
        STATE_MACHINE_INIT,     /*< Initial state */
        STATE_MACHINE_IDLE,     /*< Idle state */

        STATE_MACHINE_PRELIMINARY, /*< Preliminary state */
        STATE_MACHINE_PREPARE,     /*< Prepare state */
        STATE_MACHINE_READY,       /*< Ready state */
        STATE_MACHINE_WORK,        /*< Work state */
        STATE_MACHINE_MANUAL,      /*< Manual state */
        STATE_MACHINE_COMPLETE,    /*< Complete state */

        STATE_MACHINE_SHUTDOWN,    /*< Shutdown state */
        STATE_MACHINE_POWER_SAVER, /*< Power saver state */
        STATE_MACHINE_TERMINATE,   /*< Terminate state */
        STATE_MACHINE_INTERRUPT,   /*< Interrupt state */

        STATE_MACHINE_KV_PRELIMINARY, /*< KV preliminary state */
        STATE_MACHINE_KV_PREPARE,     /*< KV prepare state */
        STATE_MACHINE_SURVIEW_READY,  /*< Surview ready state */
        STATE_MACHINE_SURVIEW_WORK,   /*< Surview work state */
        STATE_MACHINE_CT_READY,       /*< CT ready state */
        STATE_MACHINE_CT_WORK,        /*< CT work state */
        STATE_MACHINE_KV_COMPLETE,    /*< KV complete state */
        STATE_MACHINE_MAX
    };

    enum RtmSignals
    {
        NULL_SIG = 0,
        INITIALIZATION_SIG,
        SYSTEM_ON_SIG,
        MV_PREPARE_SIG,
        MV_READY_SIG,
        MV_RADIATION_SIG,
        MV_COMPLETE_SIG,
        MV_INTERRUPT_SIG,
        MV_TERMINATE_SIG,

        KV_PRELIMINARY_SIG = 10,
        KV_PREPARE_SIG,
        SURVIEW_READY_SIG,
        SURVIEW_RADIATION_SIG,
        CT_READY_SIG,
        CT_RADIATION_SIG,
        KV_COMPLETE_SIG,
        SHUTDOWN_SIG = 20,
        POWER_SAVER_SIG,

        TIME_SIG,
        ERROR_SIG,
        USER_MAX_SIG
    };
    enum sigs
    {
        ENTER_SIG = USER_MAX_SIG,
        EXIT_SIG,
        MAX_SIG,
    };

    typedef struct Event
    {
        uint8_t sig;
    } Event_t;

    enum StateRet
    {
        RET_SUPER,

        RET_HANDLED,
        RET_IGNORED,
        RET_TRAN,
    };

    typedef enum StateRet State_t;

    typedef State_t (*StateHandler_t)(void *const self, Event_t const *const e);

    typedef struct StateMachine
    {
        StateHandler_t StateHandler;
    } StateMachine_t;

#define HANDLED() RET_HANDLED
#define IGNORED() RET_IGNORED

#define TRAN(target)                                                      \
    (((StateMachine_t *)(self))->StateHandler = (StateHandler_t)(target), \
     (State_t)RET_TRAN)
    /******************************************************************************/
    typedef enum RtmSignals rtm_state_t;

    typedef struct rtm_StateMachine
    {
        StateMachine_t super;
        rtm_state_t current_state;
        void *parameters;
    } rtm_StateMachine_t;

    void rtm_state_machine_ctor(rtm_StateMachine_t *self, void *parameters);
    int32_t rtm_state_dispatch(rtm_StateMachine_t *self, Event_t const *e);
    rtm_state_t rtm_get_state(rtm_StateMachine_t *self);
#ifdef __cplusplus
}
#endif

#endif
