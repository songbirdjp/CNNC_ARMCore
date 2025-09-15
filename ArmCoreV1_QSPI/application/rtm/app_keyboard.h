/**
 * @file app_keyboard.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-08-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __APP_KEYBOARD_H__
#define __APP_KEYBOARD_H__

#include "stdint.h"
#include "stdbool.h"
#include "cmsis_os2.h"
#ifdef __cplusplus
extern "C"
{
#endif

    enum
    {
        BUTTON_STATE_RELEASE = 0,
        BUTTON_STATE_SETUP,
        BUTTON_STATE_LOAD,
        BUTTON_STATE_PREPARE,
        BUTTON_STATE_KV,
        BUTTON_STATE_MV,
        BUTTON_STATE_UNLOAD,
        BUTTON_STATE_FKP_STOP,
        BUTTON_STATE_RESET,
        BUTTON_STATE_RIGHT,
        BUTTON_STATE_LEFT,
        BUTTON_STATE_FORWARD,
        BUTTON_STATE_BACK,
        BUTTON_STATE_UP,
        BUTTON_STATE_DOWN,
        BUTTON_STATE_LIGHT_ON,
        BUTTON_STATE_LIGHT_OFF,
        BUTTON_STATE_LASER_ON,
        BUTTON_STATE_LASER_OFF,
        BUTTON_STATE_REVERSE1,
        BUTTON_STATE_REVERSE2,
        BUTTON_STATE_GOTO,
        BUTTON_STATE_OVERRIDE_RIGHT,
        BUTTON_STATE_OVERRIDE_LEFT,
        BUTTON_STATE_OVERRIDE_FORWARD,
        BUTTON_STATE_OVERRIDE_BACK,
        BUTTON_STATE_OVERRIDE_UP,
        BUTTON_STATE_OVERRIDE_DOWN,
        BUTTON_STATE_CPG_STOP,
        BUTTON_STATE_TALK_BACK,
    };

    typedef struct button_state
    {
        uint8_t function_state;
        struct
        {
            uint8_t emergency_state_fkp : 1;
            uint8_t emergency_state_cpg_l : 1;
            uint8_t emergency_state_cpg_r : 1;
            uint8_t power_on_state : 1;
            uint8_t auto_power_on_switch_state : 1;
            uint8_t hv_key_state : 1;
            uint8_t y_brake_state : 1;
            uint8_t reserved : 1;
        } special_button;
    } __attribute__((aligned(1), packed)) button_state_t;

    typedef struct prompt_state
    {
        uint32_t led_blink_setup : 1;
        uint32_t led_blink_load : 1;
        uint32_t led_blink_prepare : 1;
        uint32_t led_blink_kv : 1;
        uint32_t led_blink_mv : 1;
        uint32_t led_blink_unload : 1;
        uint32_t setup_vibration : 1;
        uint32_t setup_beep : 1;
        uint32_t load_vibration : 1;
        uint32_t load_beep : 1;
        uint32_t prepare_vibration : 1;
        uint32_t prepare_beep : 1;
        uint32_t unload_vibration : 1;
        uint32_t unload_beep : 1;
        uint32_t reserved : 18;
    } prompt_state_t;

    typedef enum Priority
    {
        PRIORITY_NONE,
        PRIORITY_FKP,
        PRIORITY_CPG,
    } button_priority_t;

    typedef struct original_prompt
    {
        uint32_t Led_blink;
        struct
        {
            uint8_t vibration : 1;
            uint8_t beep : 1;
            uint8_t reserved : 6;
        } userPrompt;
    } __attribute__((aligned(1), packed)) original_prompt_t;

    typedef struct app_keyboard
    {
        button_state_t button_state;
        osMutexId_t mutex;
        prompt_state_t prompt_state;

        button_priority_t button_priority;
#define FKP_BUTTON_STOP_BIT_MASK (1 << 4)
#define FKP_BUTTON_EMERGENCY_BIT_MASK (1 << 7)
#define FKP_BUTTON_POWER_ON_BIT_MASK (1 << 8)
#define FKP_BUTTON_AUTO_POWER_ON_SWITCH_BIT_MASK (1 << 9)
#define FKP_BUTTON_HV_KEY_BIT_MASK (1 << 10)
        uint16_t fkp_Button;
        bool fkp_stop_state;
        uint8_t fkp_state; /*< FKP button state :0 normal, 1 fault */
        original_prompt_t fkp_original_prompt;
#define CPG_BUTTON_STOP_BIT_MASK (1 << 16)
#define CPG_BUTTON_EMERGENCY_BIT_MASK (1 << 4)
#define CPG_BUTTON_Y_BRAKE_BIT_MASK (1 << 15)
        uint32_t cpg_button_l;
        bool cpg_stop_state_l;
        uint8_t cpg_state_l; /*< CPG button state :0 normal, 1 fault */
        uint32_t cpg_button_r;
        bool cpg_stop_state_r;
        uint8_t cpg_state_r; /*< CPG button state :0 normal, 1 fault */
        original_prompt_t cpg_original_prompt;
        bool light_state;
        bool laser_state;
        bool y_brake_state;
        bool y_brake_state_mutex;
    } app_keyboard_t;

    typedef enum
    {
        BUTTON_TYPE_FKP,
        BUTTON_TYPE_CPG_L,
        BUTTON_TYPE_CPG_R,
    } button_type_t;
    typedef enum
    {
        FILTER_TYPE_NONE,
        FILTER_TYPE_RTM_ON_PLC,
        FILTER_TYPE_PSM,
        FILTER_TYPE_GMM,
        FILTER_TYPE_RTM_OFF_ARM,
    } filter_type_t;

    typedef enum
    {
        CPG_ORIGINAL_PROMPT_SET_TYPE_LIGHT,
        CPG_ORIGINAL_PROMPT_SET_TYPE_LASER,
        CPG_ORIGINAL_PROMPT_SET_TYPE_Y_BRAKE,
        CPG_ORIGINAL_PROMPT_SET_TYPE_STOP,
    } cpg_original_prompt_set_type_t;

    int32_t app_keyboard_init(app_keyboard_t *self);
    int32_t app_keyboard_button_state_get(app_keyboard_t *self,
                                          button_type_t button_type,
                                          uint32_t original_button,
                                          button_state_t *button_state);
    int32_t app_keyboard_button_state_filter(app_keyboard_t *self,
                                             filter_type_t filter_type,
                                             button_state_t original_button_state,
                                             button_state_t *filtered_button_state);
    int32_t app_keyboard_prompt_state_get(app_keyboard_t *self,
                                          button_type_t button_type,
                                          prompt_state_t prompt_state,
                                          original_prompt_t *original_prompt);
    int32_t app_keyboard_cpg_original_prompt_set(app_keyboard_t *self,
                                                 cpg_original_prompt_set_type_t cpg_original_prompt_set_type,
                                                 uint8_t led_state);
    int32_t app_keyboard_cpg_original_prompt_check(app_keyboard_t *self,
                                                   cpg_original_prompt_set_type_t cpg_original_prompt_set_type,
                                                   uint8_t led_state);
    uint8_t app_keyboard_fault_get(app_keyboard_t *self, button_type_t button_type);
#ifdef __cplusplus
}
#endif

#endif //__APP_KEYBOARD_H__