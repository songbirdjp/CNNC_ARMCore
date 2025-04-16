/**
 * @file app_rtm_main.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-09-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef _APP_RTM_MAIN_H_
#define _APP_RTM_MAIN_H_

#include "dev_base.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct Event
    {
        uint8_t sig;
    } Event_t;

    typedef void (*Tran)(struct StateTable *self, Event_t const *e);

    typedef struct StateTable
    {
        Tran const *state_table; /*!< State table */
        uint8_t n_states;        /*!< Number of states */
        uint8_t n_signals;       /*!< Number of signals */
        uint8_t state;           /*!< Current module state */
        uint8_t signal;          /*!< Current signal */
        Tran initial;            /*!< Initial state */
        void *argument;          /*!< Argument for state table */
    } stateTable_t;

    typedef struct rtm_state_machine
    {
        stateTable_t super;
    } rtm_state_machine_t;

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

    enum 
    {
        SYSTEM_STATE_NULL = 0,
        SYSTEM_STATE_INITIALIZATION = 1,
        SYSTEM_STATE_SYSTEM_ON = 2,
        SYSTEM_STATE_MV_PREPARE,
        SYSTEM_STATE_MV_READY,
        SYSTEM_STATE_MV_RADIATION,
        SYSTEM_STATE_MV_COMPLETE,
        SYSTEM_STATE_MV_INTERRUPT,
        SYSTEM_STATE_MV_TERMINATE,

        SYSTEM_STATE_KV_PRELIMINARY = 10,
        SYSTEM_STATE_KV_PREPARE,
        SYSTEM_STATE_SURVIEW_READY,
        SYSTEM_STATE_SURVIEW_RADIATION,
        SYSTEM_STATE_CT_READY,
        SYSTEM_STATE_CT_RADIATION,
        SYSTEM_STATE_KV_COMPLETE,
        SYSTEM_STATE_SHUTDOWN = 20,
        SYSTEM_STATE_POWER_SAVER,
        SYSTEM_STATE_MAX
    };

    void rtm_state_machine_ctor(stateTable_t *self);
    device_err_t stateTable_init(stateTable_t *self, Event_t const *e);
    device_err_t stateTable_dispatch(stateTable_t *self, Event_t const *e);
    uint8_t stateTable_get_state(stateTable_t *self);
#ifdef __cplusplus
}
#endif

#endif