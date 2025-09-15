/**
 * @file app_prompt.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-09-04
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __APP_PROMPT_H__
#define __APP_PROMPT_H__

#include <stdint.h>
#include "app_manage.h"
#include "app_keyboard.h"
#include "app_state_machine.h"
#ifdef __cplusplus
extern "C"
{
#endif

    enum
    {
        TREATMENT_RECORD_INIT = 0,
        TREATMENT_RECORD_SEND_SUCCEED,
        TREATMENT_RECORD_SWITCH,
        TREATMENT_RECORD_FINISH,
        TREATMENT_RECORD_CLEAR,
        TREATMENT_RECORD_MAX
    };

    enum
    {
        TRM_REQUIRE_NULL = 0,
        TRM_REQUIRE_SETUP,
        TRM_REQUIRE_LOAD,
        TRM_REQUIRE_LOAD_READY,
        TRM_REQUIRE_UNLOAD,
        TRM_REQUIRE_MAX
    };

    enum promptSignals
    {
        PROMPT_NULL_SIG = 0,
        PROMPT_INITIALIZATION_SIG,
        PROMPT_SYSTEM_ON_SIG,
        PROMPT_MV_PREPARE_SIG,
        PROMPT_MV_READY_SIG,
        PROMPT_MV_RADIATION_SIG,
        PROMPT_MV_COMPLETE_SIG,
        PROMPT_MV_INTERRUPT_SIG,
        PROMPT_MV_TERMINATE_SIG,

        PROMPT_KV_PRELIMINARY_SIG,
        PROMPT_KV_PREPARE_SIG,
        PROMPT_KV_READY_SIG,
        PROMPT_KV_RADIATION_SIG,
        PROMPT_KV_COMPLETE_SIG,

        PROMPT_RT_SEND_SUCCEED_SIG,
        PROMPT_RT_SWITCH_SIG,
        PROMPT_RT_FINISH_SIG,
        PROMPT_RT_CLEAR_SIG,

        PROMPT_BUTTON_SETUP_SIG,
        PROMPT_BUTTON_LOAD_SIG,
        PROMPT_BUTTON_UNLOAD_SIG,
        PROMPT_BUTTON_PREPARE_SIG,

        PROMPT_ARRIVE_SETUP_SIG,
        PROMPT_ARRIVE_LOAD_SIG,
        PROMPT_ARRIVE_UNLOAD_SIG,
        PROMPT_ARRIVE_PREPARE_SIG,
        PROMPT_USER_MAX_SIG
    };

    enum promptMasterState
    {
        PROMPT_INITIALIZATION_STATE = 1,
        PROMPT_SYSTEM_ON_STATE = 2,
        PROMPT_MV_PREPARE_STATE = 3,
        PROMPT_MV_READY_STATE = 4,
        PROMPT_MV_RADIATION_STATE = 5,
        PROMPT_MV_COMPLETE_STATE = 6,
        PROMPT_MV_INTERRUPT_STATE = 7,
        PROMPT_MV_TERMINATE_STATE = 8,

        PROMPT_KV_PRELIMINARY_STATE = 10,
        PROMPT_KV_PREPARE_STATE = 11,
        PROMPT_SURVIEW_READY_STATE = 12,
        PROMPT_SURVIEW_RADIATION_STATE = 13,
        PROMPT_CT_READY_STATE = 14,
        PROMPT_CT_RADIATION_STATE = 15,
        PROMPT_KV_COMPLETE_STATE = 16,
        USER_MAX_STATE
    };
    typedef struct psm_position
    {
        uint16_t data_valid_flag;
        float psm_position_x_cur;
        float psm_position_y_cur;
        float psm_position_z_cur;
        float psm_position_x_r_tar;
        float psm_position_y_r_tar;
        float psm_position_z_r_tar;
    } __attribute__((aligned(1), packed)) psm_position_t;

    typedef struct app_prompt
    {
        StateMachine_t super;

        uint8_t last_system_state;
        uint8_t last_last_system_state;

        psm_position_t position;
        prompt_state_t prompt_state;
        uint8_t trm_require_state;
        void *userdata;
    } app_prompt_t;

    void prompt_state_machine_ctor(app_prompt_t *self, void *userdata);
    int32_t prompt_state_dispatch(app_prompt_t *self, Event_t const *e);
    int32_t prompt_state_trm_require_get(app_prompt_t *self, uint8_t *trm_require_state);
    int32_t prompt_state_load_position_get(app_prompt_t *self, psm_position_t *psm_position);
    int32_t prompt_state_prompt_get(app_prompt_t *self, prompt_state_t *prompt_state);
#ifdef __cplusplus
}
#endif

#endif /* __APP_PROMPT_H__ */