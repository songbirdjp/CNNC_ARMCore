/**
 * @file app_keyboard.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-08-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "app_keyboard.h"
#include "string.h"
int32_t app_keyboard_init(app_keyboard_t *self)
{
    if (self == NULL)
    {
        return -1;
    }
    memset(self, 0, sizeof(app_keyboard_t));
    self->button_state.special_button.emergency_state_fkp = 1;
    self->button_state.special_button.emergency_state_cpg_l = 1;
    self->button_state.special_button.emergency_state_cpg_r = 1;
    self->mutex = osMutexNew(NULL);
    if (self->mutex == NULL)
    {
        return -2;
    }
    return 0;
}

#define FKP_BUTTON_MASK (0x86F)
#define FKP_BUTTON_PREPARE_MASK (0x03)
#define FKP_BUTTON_KV_MASK (0x04)
#define FKP_BUTTON_MV_MASK (0x08)
#define FKP_BUTTON_UNLOAD_MASK (0x21)
#define FKP_BUTTON_RESET_MASK (0x40)
#define FKP_BUTTON_TALK_BACK_MASK (0x800)
#define FKP_BUTTON_IGNORE_MASK (FKP_BUTTON_EMERGENCY_BIT_MASK |            \
                                FKP_BUTTON_AUTO_POWER_ON_SWITCH_BIT_MASK | \
                                FKP_BUTTON_POWER_ON_BIT_MASK |             \
                                FKP_BUTTON_HV_KEY_BIT_MASK)
static uint8_t fkp_button_state_get(app_keyboard_t *self, uint32_t original_button)
{
    uint8_t state = 0;
    if (original_button & FKP_BUTTON_MASK)
    {
        if (((original_button & FKP_BUTTON_PREPARE_MASK) == FKP_BUTTON_PREPARE_MASK) &&
            ((original_button & ~(FKP_BUTTON_PREPARE_MASK | FKP_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_PREPARE;
        }
        else if (((original_button & FKP_BUTTON_KV_MASK) == FKP_BUTTON_KV_MASK) &&
                 ((original_button & ~(FKP_BUTTON_KV_MASK | FKP_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_KV;
        }
        else if (((original_button & FKP_BUTTON_MV_MASK) == FKP_BUTTON_MV_MASK) &&
                 ((original_button & ~(FKP_BUTTON_MV_MASK | FKP_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_MV;
        }
        else if (((original_button & FKP_BUTTON_UNLOAD_MASK) == FKP_BUTTON_UNLOAD_MASK) &&
                 ((original_button & ~(FKP_BUTTON_UNLOAD_MASK | FKP_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_UNLOAD;
        }
        else if (((original_button & FKP_BUTTON_RESET_MASK) == FKP_BUTTON_RESET_MASK) &&
                 ((original_button & ~(FKP_BUTTON_RESET_MASK | FKP_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_RESET;
        }
        else if (((original_button & FKP_BUTTON_TALK_BACK_MASK) == FKP_BUTTON_TALK_BACK_MASK) &&
                 ((original_button & ~(FKP_BUTTON_TALK_BACK_MASK | FKP_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_TALK_BACK;
        }
        else
        {
            state = BUTTON_STATE_RELEASE;
        }
    }
    else
    {
        state = BUTTON_STATE_RELEASE;
    }
    return state;
}
#define CPG_BUTTON_MASK (0x2FFEF)
#define CPG_BUTTON_SETUP_C_MASK (0x3)
#define CPG_BUTTON_SETUP_S_MASK (0x42)
#define CPG_BUTTON_LOAD_C_MASK (0x5)
#define CPG_BUTTON_LOAD_S_MASK (0x44)
#define CPG_BUTTON_UNLOAD_C_MASK (0x9)
#define CPG_BUTTON_UNLOAD_S_MASK (0x48)
#define CPG_BUTTON_GOTO_C_MASK (0x21)
#define CPG_BUTTON_GOTO_S_MASK (0x60)
#define CPG_BUTTON_RIGHT_C_MASK (0x81)
#define CPG_BUTTON_RIGHT_S_MASK (0xC0)
#define CPG_BUTTON_LEFT_C_MASK (0x101)
#define CPG_BUTTON_LEFT_S_MASK (0x140)
#define CPG_BUTTON_FORWARD_C_MASK (0x201)
#define CPG_BUTTON_FORWARD_S_MASK (0x240)
#define CPG_BUTTON_BACK_C_MASK (0x401)
#define CPG_BUTTON_BACK_S_MASK (0x440)
#define CPG_BUTTON_UP_C_MASK (0x801)
#define CPG_BUTTON_UP_S_MASK (0x840)
#define CPG_BUTTON_DOWN_C_MASK (0x1001)
#define CPG_BUTTON_DOWN_S_MASK (0x1040)
#define CPG_BUTTON_LIGHT_MASK (0x2000)
#define CPG_BUTTON_LASER_MASK (0x4000)
#define CPG_BUTTON_OVERRIDE_RIGHT_MASK (0x20080)
#define CPG_BUTTON_OVERRIDE_LEFT_MASK (0x20100)
#define CPG_BUTTON_OVERRIDE_FORWARD_MASK (0x20200)
#define CPG_BUTTON_OVERRIDE_BACK_MASK (0x20400)
#define CPG_BUTTON_OVERRIDE_UP_MASK (0x20800)
#define CPG_BUTTON_OVERRIDE_DOWN_MASK (0x21000)

#define CPG_BUTTON_IGNORE_MASK (CPG_BUTTON_EMERGENCY_BIT_MASK)
static uint8_t cpg_button_state_get(app_keyboard_t *self, uint32_t original_button)
{
    uint8_t state = 0;
    if (original_button & CPG_BUTTON_MASK)
    {
        if (((original_button & CPG_BUTTON_SETUP_C_MASK) == CPG_BUTTON_SETUP_C_MASK) &&
            ((original_button & ~(CPG_BUTTON_SETUP_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_SETUP;
        }
        else if (((original_button & CPG_BUTTON_SETUP_S_MASK) == CPG_BUTTON_SETUP_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_SETUP_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_SETUP;
        }
        else if (((original_button & CPG_BUTTON_LOAD_C_MASK) == CPG_BUTTON_LOAD_C_MASK) &&
                 ((original_button & ~(CPG_BUTTON_LOAD_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_LOAD;
        }
        else if (((original_button & CPG_BUTTON_LOAD_S_MASK) == CPG_BUTTON_LOAD_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_LOAD_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_LOAD;
        }
        else if (((original_button & CPG_BUTTON_UNLOAD_C_MASK) == CPG_BUTTON_UNLOAD_C_MASK) &&
                 ((original_button & ~(CPG_BUTTON_UNLOAD_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_UNLOAD;
        }
        else if (((original_button & CPG_BUTTON_UNLOAD_S_MASK) == CPG_BUTTON_UNLOAD_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_UNLOAD_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_UNLOAD;
        }
        else if (((original_button & CPG_BUTTON_GOTO_C_MASK) == CPG_BUTTON_GOTO_C_MASK) &&
                 ((original_button & ~(CPG_BUTTON_GOTO_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_GOTO;
        }
        else if (((original_button & CPG_BUTTON_GOTO_S_MASK) == CPG_BUTTON_GOTO_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_GOTO_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_GOTO;
        }
        else if (((original_button & CPG_BUTTON_RIGHT_C_MASK) == CPG_BUTTON_RIGHT_C_MASK) &&
                 ((original_button & ~(CPG_BUTTON_RIGHT_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_RIGHT;
        }
        else if (((original_button & CPG_BUTTON_RIGHT_S_MASK) == CPG_BUTTON_RIGHT_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_RIGHT_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_RIGHT;
        }
        else if (((original_button & CPG_BUTTON_LEFT_C_MASK) == CPG_BUTTON_LEFT_C_MASK) &&
                 ((original_button & ~(CPG_BUTTON_LEFT_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_LEFT;
        }
        else if (((original_button & CPG_BUTTON_LEFT_S_MASK) == CPG_BUTTON_LEFT_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_LEFT_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_LEFT;
        }
        else if (((original_button & CPG_BUTTON_FORWARD_C_MASK) == CPG_BUTTON_FORWARD_C_MASK) &&
                 ((original_button & ~(CPG_BUTTON_FORWARD_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_FORWARD;
        }
        else if (((original_button & CPG_BUTTON_FORWARD_S_MASK) == CPG_BUTTON_FORWARD_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_FORWARD_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_FORWARD;
        }
        else if (((original_button & CPG_BUTTON_BACK_C_MASK) == CPG_BUTTON_BACK_C_MASK) &&
                 ((original_button & ~(CPG_BUTTON_BACK_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_BACK;
        }
        else if (((original_button & CPG_BUTTON_BACK_S_MASK) == CPG_BUTTON_BACK_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_BACK_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_BACK;
        }
        else if (((original_button & CPG_BUTTON_UP_C_MASK) == CPG_BUTTON_UP_C_MASK) &&
                 ((original_button & ~(CPG_BUTTON_UP_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_UP;
        }
        else if (((original_button & CPG_BUTTON_UP_S_MASK) == CPG_BUTTON_UP_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_UP_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_UP;
        }
        else if (((original_button & CPG_BUTTON_DOWN_C_MASK) == CPG_BUTTON_DOWN_C_MASK) &&
                 ((original_button & ~(CPG_BUTTON_DOWN_C_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_DOWN;
        }
        else if (((original_button & CPG_BUTTON_DOWN_S_MASK) == CPG_BUTTON_DOWN_S_MASK) &&
                 ((original_button & ~(CPG_BUTTON_DOWN_S_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_DOWN;
        }
        else if (((original_button & CPG_BUTTON_LIGHT_MASK) == CPG_BUTTON_LIGHT_MASK) &&
                 ((original_button & ~(CPG_BUTTON_LIGHT_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            if ((self->button_state.function_state == BUTTON_STATE_LIGHT_ON) ||
                (self->button_state.function_state == BUTTON_STATE_LIGHT_OFF))
            {
                state = self->button_state.function_state;
            }
            else
            {
                if (self->light_state == false)
                {
                    state = BUTTON_STATE_LIGHT_ON;
                    self->light_state = true;
                }
                else
                {
                    state = BUTTON_STATE_LIGHT_OFF;
                    self->light_state = false;
                }
            }
        }
        else if (((original_button & CPG_BUTTON_LASER_MASK) == CPG_BUTTON_LASER_MASK) &&
                 ((original_button & ~(CPG_BUTTON_LASER_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            if ((self->button_state.function_state == BUTTON_STATE_LASER_ON) ||
                (self->button_state.function_state == BUTTON_STATE_LASER_OFF))
            {
                state = self->button_state.function_state;
            }
            else
            {
                if (self->laser_state == false)
                {
                    state = BUTTON_STATE_LASER_ON;
                    self->laser_state = true;
                }
                else
                {
                    state = BUTTON_STATE_LASER_OFF;
                    self->laser_state = false;
                }
            }
        }
        // else if (((original_button & CPG_BUTTON_Y_BRAKE_MASK) == CPG_BUTTON_Y_BRAKE_MASK) &&
        //          ((original_button & ~(CPG_BUTTON_Y_BRAKE_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        // {
        //     if ((self->button_state.function_state == BUTTON_STATE_Y_BRAKE_ON) ||
        //         (self->button_state.function_state == BUTTON_STATE_Y_BRAKE_OFF))
        //     {
        //         state = self->button_state.function_state;
        //     }
        //     else
        //     {
        //         if (self->y_brake_state == false)
        //         {
        //             state = BUTTON_STATE_Y_BRAKE_ON;
        //             self->y_brake_state = true;
        //         }
        //         else
        //         {
        //             state = BUTTON_STATE_Y_BRAKE_OFF;
        //             self->y_brake_state = false;
        //         }
        //     }
        // }
        else if (((original_button & CPG_BUTTON_OVERRIDE_RIGHT_MASK) == CPG_BUTTON_OVERRIDE_RIGHT_MASK) &&
                 ((original_button & ~(CPG_BUTTON_OVERRIDE_RIGHT_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_OVERRIDE_RIGHT;
        }
        else if (((original_button & CPG_BUTTON_OVERRIDE_LEFT_MASK) == CPG_BUTTON_OVERRIDE_LEFT_MASK) &&
                 ((original_button & ~(CPG_BUTTON_OVERRIDE_LEFT_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_OVERRIDE_LEFT;
        }
        else if (((original_button & CPG_BUTTON_OVERRIDE_FORWARD_MASK) == CPG_BUTTON_OVERRIDE_FORWARD_MASK) &&
                 ((original_button & ~(CPG_BUTTON_OVERRIDE_FORWARD_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_OVERRIDE_FORWARD;
        }
        else if (((original_button & CPG_BUTTON_OVERRIDE_BACK_MASK) == CPG_BUTTON_OVERRIDE_BACK_MASK) &&
                 ((original_button & ~(CPG_BUTTON_OVERRIDE_BACK_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_OVERRIDE_BACK;
        }
        else if (((original_button & CPG_BUTTON_OVERRIDE_UP_MASK) == CPG_BUTTON_OVERRIDE_UP_MASK) &&
                 ((original_button & ~(CPG_BUTTON_OVERRIDE_UP_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_OVERRIDE_UP;
        }
        else if (((original_button & CPG_BUTTON_OVERRIDE_DOWN_MASK) == CPG_BUTTON_OVERRIDE_DOWN_MASK) &&
                 ((original_button & ~(CPG_BUTTON_OVERRIDE_DOWN_MASK | CPG_BUTTON_IGNORE_MASK)) == 0))
        {
            state = BUTTON_STATE_OVERRIDE_DOWN;
        }
        else
        {
            state = BUTTON_STATE_RELEASE;
        }
    }
    else
    {
        state = BUTTON_STATE_RELEASE;
    }
    return state;
}
#define FKP_BUTTON_VALID_MASK (0xFFFFF97F)
#define CPG_BUTTON_VALID_MASK (0xFFFFFFEF)
#define KEYBOARD_FAULT_CHECK_TIMEOUT (30000)
static int32_t app_keyboard_fault_check(app_keyboard_t *self,
                                        button_type_t button_type,
                                        uint32_t original_button)
{
    if (self == NULL)
    {
        return -1;
    }
    static uint32_t last_time_fkp = 0;
    static uint32_t last_time_cpg_l = 0;
    static uint32_t last_time_cpg_r = 0;
    switch (button_type)
    {
    case BUTTON_TYPE_FKP:
        if ((original_button & FKP_BUTTON_VALID_MASK) == 0)
        {
            last_time_fkp = osKernelGetTickCount();
            self->fkp_state = 0;
            return 0;
        }
        else
        {
            if (osKernelGetTickCount() - last_time_fkp > KEYBOARD_FAULT_CHECK_TIMEOUT)
            {
                self->fkp_state = 1;
                return -2;
            }
        }
        break;
    case BUTTON_TYPE_CPG_L:
        if ((original_button & CPG_BUTTON_VALID_MASK) == 0)
        {
            last_time_cpg_l = osKernelGetTickCount();
            self->cpg_state_l = 0;
            return 0;
        }
        else
        {
            if (osKernelGetTickCount() - last_time_cpg_l > KEYBOARD_FAULT_CHECK_TIMEOUT)
            {
                self->cpg_state_l = 1;
                return -3;
            }
        }
        break;
    case BUTTON_TYPE_CPG_R:
        if ((original_button & CPG_BUTTON_VALID_MASK) == 0)
        {
            last_time_cpg_r = osKernelGetTickCount();
            self->cpg_state_r = 0;
            return 0;
        }
        else
        {
            if (osKernelGetTickCount() - last_time_cpg_r > KEYBOARD_FAULT_CHECK_TIMEOUT)
            {
                self->cpg_state_r = 1;
                return -4;
            }
        }
        break;
    default:
        break;
    }
    return 0;
}
int32_t app_keyboard_button_state_get(app_keyboard_t *self, button_type_t button_type, uint32_t original_button, button_state_t *button_state)
{
    int32_t ret = 0;
    button_state_t button_state_temp;
    if ((self == NULL) ||
        ((button_type != BUTTON_TYPE_FKP) && (button_type != BUTTON_TYPE_CPG_L) && (button_type != BUTTON_TYPE_CPG_R)) ||
        (button_state == NULL))
    {
        ret = -1;
    }
    osMutexAcquire(self->mutex, osWaitForever);

    switch (button_type)
    {
    case BUTTON_TYPE_FKP:
        ret = app_keyboard_fault_check(self, button_type, original_button);
        if (ret != 0)
        {
            self->fkp_Button = (original_button & ~FKP_BUTTON_VALID_MASK);
        }
        else
        {
            self->fkp_Button = original_button;
        }
        self->button_state.special_button.emergency_state_fkp = (self->fkp_Button & FKP_BUTTON_EMERGENCY_BIT_MASK) ? 1 : 0;
        self->button_state.special_button.power_on_state = (self->fkp_Button & FKP_BUTTON_POWER_ON_BIT_MASK) ? 1 : 0;
        self->button_state.special_button.auto_power_on_switch_state = (self->fkp_Button & FKP_BUTTON_AUTO_POWER_ON_SWITCH_BIT_MASK) ? 1 : 0;
        self->button_state.special_button.hv_key_state = (self->fkp_Button & FKP_BUTTON_HV_KEY_BIT_MASK) ? 1 : 0;
        if (self->fkp_Button & FKP_BUTTON_STOP_BIT_MASK)
        {
            self->button_state.function_state = BUTTON_STATE_FKP_STOP;
            self->button_priority = PRIORITY_FKP;
            self->fkp_stop_state = true;
            *button_state = self->button_state;
            ret = 0;
            goto ret;
        }
        else
        {
            self->fkp_stop_state = false;
            if (self->cpg_stop_state_l || self->cpg_stop_state_r)
            {
                *button_state = self->button_state;
                ret = 0;
                goto ret;
            }
            if (self->button_priority > PRIORITY_FKP)
            {
                *button_state = self->button_state;
                ret = 0;
                goto ret;
            }

            self->button_state.function_state = fkp_button_state_get(self, self->fkp_Button);
            if (self->button_state.function_state != BUTTON_STATE_RELEASE)
            {
                self->button_priority = PRIORITY_FKP;
            }
            else
            {
                self->button_priority = PRIORITY_NONE;
            }
            *button_state = self->button_state;
            ret = 0;
            goto ret;
        }
        break;
    case BUTTON_TYPE_CPG_L:
        ret = app_keyboard_fault_check(self, button_type, original_button);
        if (ret != 0)
        {
            self->cpg_button_l = (original_button & ~CPG_BUTTON_VALID_MASK);
        }
        else
        {
            self->cpg_button_l = original_button;
        }
        self->button_state.special_button.emergency_state_cpg_l = (self->cpg_button_l & CPG_BUTTON_EMERGENCY_BIT_MASK) ? 1 : 0;
        if (self->cpg_button_l & CPG_BUTTON_Y_BRAKE_BIT_MASK)
        {
            if (self->y_brake_state_mutex == false)
            {
                if (self->y_brake_state == false)
                {
                    self->button_state.special_button.y_brake_state = 1;
                    self->y_brake_state = true;
                    self->y_brake_state_mutex = true;
                }
                else
                {
                    self->button_state.special_button.y_brake_state = 0;
                    self->y_brake_state = false;
                    self->y_brake_state_mutex = true;
                }
            }
        }
        else
        {
            self->y_brake_state_mutex = false;
        }

        if (self->cpg_button_l & CPG_BUTTON_STOP_BIT_MASK)
        {
            self->button_state.function_state = BUTTON_STATE_CPG_STOP;
            self->button_priority = PRIORITY_CPG;
            self->cpg_stop_state_l = true;
            *button_state = self->button_state;
            ret = 0;
            goto ret;
        }
        else
        {
            self->cpg_stop_state_l = false;

            if (self->fkp_stop_state || self->cpg_stop_state_r)
            {
                *button_state = self->button_state;
                ret = 0;
                goto ret;
            }

            if (self->button_priority > PRIORITY_CPG)
            {
                *button_state = self->button_state;
                ret = 0;
                goto ret;
            }

            button_state_temp.function_state = cpg_button_state_get(self, self->cpg_button_l | self->cpg_button_r);

            if (button_state_temp.function_state != BUTTON_STATE_RELEASE)
            {
                self->button_priority = PRIORITY_CPG;
                self->button_state.function_state = button_state_temp.function_state;
            }
            else
            {
                if (self->button_priority == PRIORITY_FKP)
                {
                    *button_state = self->button_state;
                    ret = 0;
                    goto ret;
                }
                else
                {
                    self->button_priority = PRIORITY_NONE;
                    self->button_state.function_state = button_state_temp.function_state;
                }
            }

            *button_state = self->button_state;
            ret = 0;
            goto ret;
        }
        break;
    case BUTTON_TYPE_CPG_R:
        ret = app_keyboard_fault_check(self, button_type, original_button);
        if (ret != 0)
        {
            self->cpg_button_r = (original_button & ~CPG_BUTTON_VALID_MASK);
        }
        else
        {
            self->cpg_button_r = original_button;
        }

        self->button_state.special_button.emergency_state_cpg_r = (self->cpg_button_r & CPG_BUTTON_EMERGENCY_BIT_MASK) ? 1 : 0;
        if (self->cpg_button_r & CPG_BUTTON_Y_BRAKE_BIT_MASK)
        {
            if (self->y_brake_state_mutex == false)
            {
                if (self->y_brake_state == false)
                {
                    self->button_state.special_button.y_brake_state = 1;
                    self->y_brake_state = true;
                    self->y_brake_state_mutex = true;
                }
                else
                {
                    self->button_state.special_button.y_brake_state = 0;
                    self->y_brake_state = false;
                    self->y_brake_state_mutex = true;
                }
            }
        }
        else
        {
            self->y_brake_state_mutex = false;
        }
        if (self->cpg_button_r & CPG_BUTTON_STOP_BIT_MASK)
        {
            self->button_state.function_state = BUTTON_STATE_CPG_STOP;
            self->button_priority = PRIORITY_CPG;
            self->cpg_stop_state_r = true;
            *button_state = self->button_state;
            ret = 0;
            goto ret;
        }
        else
        {
            self->cpg_stop_state_r = false;
            if (self->fkp_stop_state || self->cpg_stop_state_l)
            {
                *button_state = self->button_state;
                ret = 0;
                goto ret;
            }
            if (self->button_priority > PRIORITY_CPG)
            {
                *button_state = self->button_state;
                ret = 0;
                goto ret;
            }

            button_state_temp.function_state = cpg_button_state_get(self, self->cpg_button_l | self->cpg_button_r);

            if (button_state_temp.function_state != BUTTON_STATE_RELEASE)
            {
                self->button_priority = PRIORITY_CPG;
                self->button_state.function_state = button_state_temp.function_state;
            }
            else
            {
                if (self->button_priority == PRIORITY_FKP)
                {
                    *button_state = self->button_state;
                    ret = 0;
                    goto ret;
                }
                else
                {
                    self->button_priority = PRIORITY_NONE;
                    self->button_state.function_state = button_state_temp.function_state;
                }
            }

            *button_state = self->button_state;
            ret = 0;
            goto ret;
        }
        break;
    default:
        break;
    }
ret:
    osMutexRelease(self->mutex);
    return ret;
}
#define FKP_LED_BLINK_ENABLE_MASK (0x1)
#define FKP_LED_BLINK_PREPARE_MASK (0x2)
#define FKP_LED_BLINK_KV_MASK (0x4)
#define FKP_LED_BLINK_MV_MASK (0x8)
#define FKP_LED_BLINK_UNLOAD_MASK (0x10)
#define FKP_VIBRATION_MASK (0x1)
#define FKP_BEEP_MASK (0x2)

#define CPG_LED_BLINK_ENABLE_MASK (0x1)
#define CPG_LED_BLINK_SETUP_MASK (0x2)
#define CPG_LED_BLINK_LOAD_MASK (0x4)
#define CPG_LED_BLINK_UNLOAD_MASK (0x8)
#define CPG_LED_BLINK_LIGHT_MASK (0x10)
#define CPG_LED_BLINK_LASER_MASK (0x20)
#define CPG_LED_BLINK_Y_BRAKE_MASK (0x40)
#define CPG_LED_BLINK_STOP_MASK (0x80)
#define CPG_VIBRATION_MASK (0x1)
#define CPG_BEEP_MASK (0x2)

int32_t app_keyboard_prompt_state_get(app_keyboard_t *self, button_type_t button_type, prompt_state_t prompt_state, original_prompt_t *original_prompt)
{
    if ((self == NULL) ||
        ((button_type != BUTTON_TYPE_FKP) && (button_type != BUTTON_TYPE_CPG_L) && (button_type != BUTTON_TYPE_CPG_R)) ||
        (original_prompt == NULL))
    {
        return -1;
    }

    self->prompt_state = prompt_state;

    switch (button_type)
    {
    case BUTTON_TYPE_FKP:
        if ((self->prompt_state.led_blink_prepare == 1) || (self->prompt_state.led_blink_unload == 1))
        {
            self->fkp_original_prompt.Led_blink |= FKP_LED_BLINK_ENABLE_MASK;
            if (self->prompt_state.led_blink_prepare == 1)
            {
                self->fkp_original_prompt.Led_blink |= FKP_LED_BLINK_PREPARE_MASK;
            }
            else
            {
                self->fkp_original_prompt.Led_blink &= ~FKP_LED_BLINK_PREPARE_MASK;
            }
            if (self->prompt_state.led_blink_unload == 1)
            {
                self->fkp_original_prompt.Led_blink |= FKP_LED_BLINK_UNLOAD_MASK;
            }
            else
            {
                self->fkp_original_prompt.Led_blink &= ~FKP_LED_BLINK_UNLOAD_MASK;
            }
        }
        else
        {
            self->fkp_original_prompt.Led_blink &= ~FKP_LED_BLINK_ENABLE_MASK;
            self->fkp_original_prompt.Led_blink &= ~FKP_LED_BLINK_PREPARE_MASK;
            self->fkp_original_prompt.Led_blink &= ~FKP_LED_BLINK_UNLOAD_MASK;
        }
        if (self->prompt_state.led_blink_kv == 1)
        {
            self->fkp_original_prompt.Led_blink |= FKP_LED_BLINK_KV_MASK;
        }
        else
        {
            self->fkp_original_prompt.Led_blink &= ~FKP_LED_BLINK_KV_MASK;
        }
        if (self->prompt_state.led_blink_mv == 1)
        {
            self->fkp_original_prompt.Led_blink |= FKP_LED_BLINK_MV_MASK;
        }
        else
        {
            self->fkp_original_prompt.Led_blink &= ~FKP_LED_BLINK_MV_MASK;
        }

        if (self->button_priority == PRIORITY_FKP)
        {
            if (self->button_state.function_state == BUTTON_STATE_PREPARE)
            {
                if (self->prompt_state.prepare_vibration == 1)
                {
                    self->fkp_original_prompt.userPrompt.vibration = 1;
                }
                else
                {
                    self->fkp_original_prompt.userPrompt.vibration = 0;
                }
                if (self->prompt_state.prepare_beep == 1)
                {
                    self->fkp_original_prompt.userPrompt.beep = 1;
                }
                else
                {
                    self->fkp_original_prompt.userPrompt.beep = 0;
                }
            }
            else if (self->button_state.function_state == BUTTON_STATE_UNLOAD)
            {
                if (self->prompt_state.unload_vibration == 1)
                {
                    self->fkp_original_prompt.userPrompt.vibration = 1;
                }
                else
                {
                    self->fkp_original_prompt.userPrompt.vibration = 0;
                }
                if (self->prompt_state.unload_beep == 1)
                {
                    self->fkp_original_prompt.userPrompt.beep = 1;
                }
                else
                {
                    self->fkp_original_prompt.userPrompt.beep = 0;
                }
            }
            else
            {
                self->fkp_original_prompt.userPrompt.vibration = 0;
                self->fkp_original_prompt.userPrompt.beep = 0;
            }
        }
        else
        {
            self->fkp_original_prompt.userPrompt.vibration = 0;
            self->fkp_original_prompt.userPrompt.beep = 0;
        }

        *original_prompt = self->fkp_original_prompt;
        break;
    case BUTTON_TYPE_CPG_L:
    case BUTTON_TYPE_CPG_R:
        if ((self->prompt_state.led_blink_setup == 1) || (self->prompt_state.led_blink_unload == 1) || (self->prompt_state.led_blink_load == 1))
        {
            self->cpg_original_prompt.Led_blink |= CPG_LED_BLINK_ENABLE_MASK;
            if (self->prompt_state.led_blink_setup == 1)
            {
                self->cpg_original_prompt.Led_blink |= CPG_LED_BLINK_SETUP_MASK;
            }
            else
            {
                self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_SETUP_MASK;
            }
            if (self->prompt_state.led_blink_unload == 1)
            {
                self->cpg_original_prompt.Led_blink |= CPG_LED_BLINK_UNLOAD_MASK;
            }
            else
            {
                self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_UNLOAD_MASK;
            }
            if (self->prompt_state.led_blink_load == 1)
            {
                self->cpg_original_prompt.Led_blink |= CPG_LED_BLINK_LOAD_MASK;
            }
            else
            {
                self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_LOAD_MASK;
            }
        }
        else
        {
            self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_ENABLE_MASK;
            self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_SETUP_MASK;
            self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_UNLOAD_MASK;
            self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_LOAD_MASK;
        }

        if (self->button_priority == PRIORITY_CPG)
        {
            switch (self->button_state.function_state)
            {
            case BUTTON_STATE_SETUP:
                if (self->prompt_state.setup_vibration == 1)
                {
                    self->cpg_original_prompt.userPrompt.vibration = 1;
                }
                else
                {
                    self->cpg_original_prompt.userPrompt.vibration = 0;
                }
                if (self->prompt_state.setup_beep == 1)
                {
                    self->cpg_original_prompt.userPrompt.beep = 1;
                }
                else
                {
                    self->cpg_original_prompt.userPrompt.beep = 0;
                }
                break;
            case BUTTON_STATE_LOAD:
                if (self->prompt_state.load_vibration == 1)
                {
                    self->cpg_original_prompt.userPrompt.vibration = 1;
                }
                else
                {
                    self->cpg_original_prompt.userPrompt.vibration = 0;
                }
                if (self->prompt_state.load_beep == 1)
                {
                    self->cpg_original_prompt.userPrompt.beep = 1;
                }
                else
                {
                    self->cpg_original_prompt.userPrompt.beep = 0;
                }
                break;
            case BUTTON_STATE_UNLOAD:
                if (self->prompt_state.unload_vibration == 1)
                {
                    self->cpg_original_prompt.userPrompt.vibration = 1;
                }
                else
                {
                    self->cpg_original_prompt.userPrompt.vibration = 0;
                }
                if (self->prompt_state.unload_beep == 1)
                {
                    self->cpg_original_prompt.userPrompt.beep = 1;
                }
                else
                {
                    self->cpg_original_prompt.userPrompt.beep = 0;
                }
                break;
            default:
                self->cpg_original_prompt.userPrompt.vibration = 0;
                self->cpg_original_prompt.userPrompt.beep = 0;
                break;
            }
        }
        else
        {
            self->cpg_original_prompt.userPrompt.vibration = 0;
            self->cpg_original_prompt.userPrompt.beep = 0;
        }

        *original_prompt = self->cpg_original_prompt;
        break;
    default:
        break;
    }
    return 0;
}
int32_t app_keyboard_button_state_filter(app_keyboard_t *self,
                                         filter_type_t filter_type,
                                         button_state_t original_button_state,
                                         button_state_t *filtered_button_state)
{
    if ((self == NULL) || (filtered_button_state == NULL))
    {
        return -1;
    }
    switch (filter_type)
    {
    case FILTER_TYPE_NONE:
        memcpy(filtered_button_state, &original_button_state, sizeof(button_state_t));
        break;
    case FILTER_TYPE_RTM_ON_PLC:
        switch (original_button_state.function_state)
        {
        case BUTTON_STATE_RIGHT:
        case BUTTON_STATE_LEFT:
        case BUTTON_STATE_FORWARD:
        case BUTTON_STATE_BACK:
        case BUTTON_STATE_UP:
        case BUTTON_STATE_DOWN:
        case BUTTON_STATE_LIGHT_ON:
        case BUTTON_STATE_LIGHT_OFF:
        case BUTTON_STATE_LASER_ON:
        case BUTTON_STATE_LASER_OFF:
        case BUTTON_STATE_REVERSE1:
        case BUTTON_STATE_REVERSE2:
        case BUTTON_STATE_OVERRIDE_RIGHT:
        case BUTTON_STATE_OVERRIDE_LEFT:
        case BUTTON_STATE_OVERRIDE_FORWARD:
        case BUTTON_STATE_OVERRIDE_BACK:
        case BUTTON_STATE_OVERRIDE_UP:
        case BUTTON_STATE_OVERRIDE_DOWN:
        case BUTTON_STATE_TALK_BACK:
            filtered_button_state->function_state = BUTTON_STATE_RELEASE;
            *(uint8_t *)&filtered_button_state->special_button = *(uint8_t *)&original_button_state.special_button;
            break;
        default:
            filtered_button_state->function_state = original_button_state.function_state;
            *(uint8_t *)&filtered_button_state->special_button = *(uint8_t *)&original_button_state.special_button;
            break;
        }
        break;
    case FILTER_TYPE_PSM:
        switch (original_button_state.function_state)
        {
        case BUTTON_STATE_KV:
        case BUTTON_STATE_MV:
        case BUTTON_STATE_RESET:
        case BUTTON_STATE_LIGHT_ON:
        case BUTTON_STATE_LIGHT_OFF:
        case BUTTON_STATE_LASER_ON:
        case BUTTON_STATE_LASER_OFF:
        case BUTTON_STATE_TALK_BACK:
            filtered_button_state->function_state = BUTTON_STATE_RELEASE;
            filtered_button_state->special_button.y_brake_state = original_button_state.special_button.y_brake_state;
            break;
        default:
            filtered_button_state->function_state = original_button_state.function_state;
            filtered_button_state->special_button.y_brake_state = original_button_state.special_button.y_brake_state;
            break;
        }
        break;
    case FILTER_TYPE_GMM:
        switch (original_button_state.function_state)
        {
        case BUTTON_STATE_SETUP:
        case BUTTON_STATE_LOAD:
        case BUTTON_STATE_PREPARE:
        case BUTTON_STATE_KV:
        case BUTTON_STATE_MV:
        case BUTTON_STATE_UNLOAD:
        case BUTTON_STATE_RESET:
        case BUTTON_STATE_RIGHT:
        case BUTTON_STATE_LEFT:
        case BUTTON_STATE_FORWARD:
        case BUTTON_STATE_BACK:
        case BUTTON_STATE_UP:
        case BUTTON_STATE_DOWN:
        case BUTTON_STATE_LIGHT_ON:
        case BUTTON_STATE_LIGHT_OFF:
        case BUTTON_STATE_LASER_ON:
        case BUTTON_STATE_LASER_OFF:
        case BUTTON_STATE_REVERSE1:
        case BUTTON_STATE_REVERSE2:
        case BUTTON_STATE_OVERRIDE_RIGHT:
        case BUTTON_STATE_OVERRIDE_LEFT:
        case BUTTON_STATE_OVERRIDE_FORWARD:
        case BUTTON_STATE_OVERRIDE_BACK:
        case BUTTON_STATE_OVERRIDE_UP:
        case BUTTON_STATE_OVERRIDE_DOWN:
        case BUTTON_STATE_TALK_BACK:
            filtered_button_state->function_state = BUTTON_STATE_RELEASE;
            *(uint8_t *)&filtered_button_state->special_button = 0;
            break;
        default:
            filtered_button_state->function_state = original_button_state.function_state;
            *(uint8_t *)&filtered_button_state->special_button = 0;
            break;
        }
        break;
    case FILTER_TYPE_RTM_OFF_ARM:
        switch (original_button_state.function_state)
        {
        case BUTTON_STATE_KV:
        case BUTTON_STATE_MV:
        case BUTTON_STATE_FKP_STOP:
        case BUTTON_STATE_RESET:
        case BUTTON_STATE_REVERSE1:
        case BUTTON_STATE_REVERSE2:
        case BUTTON_STATE_CPG_STOP:
        case BUTTON_STATE_TALK_BACK:
            filtered_button_state->function_state = BUTTON_STATE_RELEASE;
            *(uint8_t *)&filtered_button_state->special_button = *(uint8_t *)&original_button_state.special_button;
            break;
        default:
            filtered_button_state->function_state = original_button_state.function_state;
            *(uint8_t *)&filtered_button_state->special_button = *(uint8_t *)&original_button_state.special_button;
            break;
        }
        break;
    default:
        return -2;
        break;
    }
    return 0;
}
int32_t app_keyboard_cpg_original_prompt_set(app_keyboard_t *self,
                                             cpg_original_prompt_set_type_t cpg_original_prompt_set_type,
                                             uint8_t led_state)
{
    if (self == NULL)
    {
        return -1;
    }

    switch (cpg_original_prompt_set_type)
    {
    case CPG_ORIGINAL_PROMPT_SET_TYPE_LIGHT:
        if (led_state == 1)
        {
            self->cpg_original_prompt.Led_blink |= CPG_LED_BLINK_LIGHT_MASK;
        }
        else
        {
            self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_LIGHT_MASK;
        }
        break;
    case CPG_ORIGINAL_PROMPT_SET_TYPE_LASER:
        if (led_state == 1)
        {
            self->cpg_original_prompt.Led_blink |= CPG_LED_BLINK_LASER_MASK;
        }
        else
        {
            self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_LASER_MASK;
        }
        break;
    case CPG_ORIGINAL_PROMPT_SET_TYPE_Y_BRAKE:
        if (led_state == 1)
        {
            self->cpg_original_prompt.Led_blink |= CPG_LED_BLINK_Y_BRAKE_MASK;
        }
        else
        {
            self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_Y_BRAKE_MASK;
        }
        break;
    case CPG_ORIGINAL_PROMPT_SET_TYPE_STOP:
        if (led_state == 1)
        {
            self->cpg_original_prompt.Led_blink |= CPG_LED_BLINK_STOP_MASK;
        }
        else
        {
            self->cpg_original_prompt.Led_blink &= ~CPG_LED_BLINK_STOP_MASK;
        }
        break;
    default:
        break;
    }
    return 0;
}
int32_t app_keyboard_cpg_original_prompt_check(app_keyboard_t *self,
                                               cpg_original_prompt_set_type_t cpg_original_prompt_set_type,
                                               uint8_t led_state)
{
    uint32_t current_time = 0;
    static uint32_t last_time = 0;
    if (self == NULL)
    {
        return -1;
    }
    current_time = osKernelGetTickCount();
    switch (cpg_original_prompt_set_type)
    {
    case CPG_ORIGINAL_PROMPT_SET_TYPE_LIGHT:
        break;
    case CPG_ORIGINAL_PROMPT_SET_TYPE_LASER:
        break;
    case CPG_ORIGINAL_PROMPT_SET_TYPE_Y_BRAKE:
        if ((self->y_brake_state == true) && (led_state == 1))
        {
            last_time = current_time;
        }
        else if ((self->y_brake_state == false) && (led_state == 0))
        {
            last_time = current_time;
        }
        if ((current_time - last_time) > 1000)
        {
            last_time = current_time;
            return -1;
        }
        break;
    case CPG_ORIGINAL_PROMPT_SET_TYPE_STOP:
        break;
    default:
        break;
    }
    return 0;
}
uint8_t app_keyboard_fault_get(app_keyboard_t *self, button_type_t button_type)
{
    if (self == NULL)
    {
        return -1;
    }
    switch (button_type)
    {
    case BUTTON_TYPE_FKP:
        return self->fkp_state;
    case BUTTON_TYPE_CPG_L:
        return self->cpg_state_l;
    case BUTTON_TYPE_CPG_R:
        return self->cpg_state_r;
    default:
        break;
    }
    return -1;
}