/**
 * @file app_fault_check.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-08-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "app_fault_check.h"
#include "stdio.h"
#include "rtm_main.h"
#include "ulog.h"
int32_t manage_info_init(manage_info_t *self, uint32_t control_word)
{
    if (self == NULL)
    {
        return -1;
    }
    memset(self, 0, sizeof(manage_info_t));
    self->mutex_id = osMutexNew(NULL);
    if (self->mutex_id == NULL)
    {
        return -2;
    }
    self->control_word = control_word;
    return 0;
}
int32_t manage_info_status_word_set(manage_info_t *self, uint32_t status_bit, uint32_t status_value)
{
    if (self == NULL)
    {
        return -1;
    }
    if (status_value == 1)
    {
        osMutexAcquire(self->mutex_id, osWaitForever);
        bit_set(self->status_word, status_bit);
        bit_clean(self->status_mask, status_bit);
        osMutexRelease(self->mutex_id);
    }
    else if (status_value == 0)
    {
        osMutexAcquire(self->mutex_id, osWaitForever);
        if (bit_get(self->status_mask, status_bit) == 1)
        {
            bit_clean(self->status_word, status_bit);
            bit_set(self->status_mask, status_bit);
        }
        osMutexRelease(self->mutex_id);
    }
    else
    {
        return -1;
    }
    return 0;
}

int32_t manage_info_status_word_get(manage_info_t *self, uint32_t *status_word)
{
    if (self == NULL)
    {
        return -1;
    }
    osMutexAcquire(self->mutex_id, osWaitForever);
    *status_word = self->status_word;
    self->status_word &= ~(self->control_word);
    self->status_mask = 0xFFFFFFFF;
    osMutexRelease(self->mutex_id);
    return 0;
}

int32_t manage_info_status_bit_get(manage_info_t *self, uint32_t status_bit)
{
    if (self == NULL)
    {
        return -1;
    }
    osMutexAcquire(self->mutex_id, osWaitForever);
    int32_t status_value = bit_get(self->status_word, status_bit);
    if (bit_get(self->control_word, status_bit) == 1)
    {
        bit_clean(self->status_word, status_bit);
    }
    bit_set(self->status_mask, status_bit);
    osMutexRelease(self->mutex_id);
    return status_value;
}

static void fault_level_set(rtm_fault_check_t *self, uint8_t state, uint32_t fault_type)
{
    if (self == NULL || state >= STATE_MACHINE_MAX || fault_type >= INTERLOCK_MAX_BIT_POS)
    {
        return;
    }
    if (self->fault_table[fault_type][state] == 'W')
    {
        bit_set(self->fixed_interlock_table.warning_interlock, fault_type);
        bit_clean(self->fixed_interlock_table.minor_interlock, fault_type);
        bit_clean(self->fixed_interlock_table.serious_interlock, fault_type);
    }
    else if (self->fault_table[fault_type][state] == 'M')
    {
        bit_clean(self->fixed_interlock_table.warning_interlock, fault_type);
        bit_set(self->fixed_interlock_table.minor_interlock, fault_type);
        bit_clean(self->fixed_interlock_table.serious_interlock, fault_type);
    }
    else if (self->fault_table[fault_type][state] == 'S')
    {
        bit_clean(self->fixed_interlock_table.warning_interlock, fault_type);
        bit_clean(self->fixed_interlock_table.minor_interlock, fault_type);
        bit_set(self->fixed_interlock_table.serious_interlock, fault_type);
    }
    else
    {
        bit_clean(self->fixed_interlock_table.warning_interlock, fault_type);
        bit_clean(self->fixed_interlock_table.minor_interlock, fault_type);
        bit_clean(self->fixed_interlock_table.serious_interlock, fault_type);
    }
}
static void fault_level_clear(rtm_fault_check_t *self, uint32_t fault_type)
{
    if (self == NULL || fault_type >= INTERLOCK_MAX_BIT_POS)
    {
        return;
    }
    bit_clean(self->fixed_interlock_table.warning_interlock, fault_type);
    bit_clean(self->fixed_interlock_table.minor_interlock, fault_type);
    bit_clean(self->fixed_interlock_table.serious_interlock, fault_type);
}
static void fault_check_interlock_table_update(rtm_fault_check_t *self, uint8_t state)
{
    if (bit_get(self->fixed_app_state_table.rtm_main_state, RTM_MAIN_EMERGENCY_STOP_STATE_BIT) != 0)
    {
        fault_level_set(self, state, INTERLOCK_EMERGENCY_STOP_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_EMERGENCY_STOP_BIT_POS);
    }
    if (bit_get(self->fixed_app_state_table.rtm_main_state, RTM_MAIN_DOOR_STATE_BIT) != 0)
    {
        fault_level_set(self, state, INTERLOCK_DOOR_INTERLOCK_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_DOOR_INTERLOCK_BIT_POS);
    }
    if (bit_get(self->fixed_app_state_table.rtm_main_state, RTM_MAIN_HV_EN_STATE_BIT) != 0)
    {
        fault_level_set(self, state, INTERLOCK_HV_EN_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_HV_EN_BIT_POS);
    }
    if (bit_get(self->fixed_app_state_table.rtm_main_state, RTM_MAIN_KV_TREATMENT_EN_STATE_BIT) != 0)
    {
        fault_level_set(self, state, INTERLOCK_KV_TREATMENT_EN_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_KV_TREATMENT_EN_BIT_POS);
    }
    if (bit_get(self->fixed_app_state_table.rtm_main_state, RTM_MAIN_MV_TREATMENT_EN_STATE_BIT) != 0)
    {
        fault_level_set(self, state, INTERLOCK_MV_TREATMENT_EN_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_MV_TREATMENT_EN_BIT_POS);
    }
    if (bit_get(self->fixed_app_state_table.rtm_main_state, RTM_MAIN_RTC_OK_STATE_BIT) != 0)
    {
        fault_level_set(self, state, INTERLOCK_RTC_WD_OK_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_RTC_WD_OK_BIT_POS);
    }
    if (bit_get(self->fixed_app_state_table.rtm_main_state, RTM_MAIN_INIT_STATE_BIT) != 0)
    {
        bit_set(self->fixed_interlock_table.not_ready_event, NOT_READY_INIT_BIT_POS);
    }
    else
    {
        bit_clean(self->fixed_interlock_table.not_ready_event, NOT_READY_INIT_BIT_POS);
    }
    if (self->fixed_app_state_table.psm_state != 0)
    {
        fault_level_set(self, state, INTERLOCK_PSM_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_PSM_BIT_POS);
    }
    if (self->fixed_app_state_table.gmm_state != 0)
    {
        fault_level_set(self, state, INTERLOCK_GMM_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_GMM_BIT_POS);
    }
    if (self->fixed_app_state_table.fkp_state != 0)
    {
        fault_level_set(self, state, INTERLOCK_FKP_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_FKP_BIT_POS);
    }
    if (self->fixed_app_state_table.cpg_state != 0)
    {
        fault_level_set(self, state, INTERLOCK_CPG_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_CPG_BIT_POS);
    }
    if (self->fixed_app_state_table.rtm_on_state != 0)
    {
        fault_level_set(self, state, INTERLOCK_SLIP_RING_FAULT_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_SLIP_RING_FAULT_BIT_POS);
    }
    if (self->fixed_app_state_table.plc_state != 0)
    {
        fault_level_set(self, state, INTERLOCK_GMM_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_GMM_BIT_POS);
    }
    if (self->fixed_app_state_table.dido_state != 0)
    {
        fault_level_set(self, state, INTERLOCK_HARDWARE_FAULT_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_HARDWARE_FAULT_BIT_POS);
    }
    if (self->fixed_app_state_table.data_record != 0)
    {
        fault_level_set(self, state, INTERLOCK_SOFTWARE_FAULT_BIT_POS);
    }
    else
    {
        fault_level_clear(self, INTERLOCK_SOFTWARE_FAULT_BIT_POS);
    }
}
#define RTM_ERROR_WAIT_TIME (50)
int32_t fault_check(rtm_fault_check_t *self, uint8_t state, void *arg)
{
    int32_t retval = 0;
    app_rtm_main_t *app_rtm = (app_rtm_main_t *)arg;
    self->cur_time = osKernelGetTickCount();
    dido_structure_t dido_structure = {0};
    app_do_get(&(app_rtm->app_dido), &dido_structure);
    app_di_get(&(app_rtm->app_dido), &dido_structure);
    // HvEn check
    if ((0 == dido_structure.mcp23017_0x02_u.mcp23017_0x02_bit.DI_CITB_EMERGENCY4) ||
        (0 == dido_structure.mcp23017_0x00_u.mcp23017_0x00_bit.DI_CITB_EMERGENCY2) ||
        (0 == dido_structure.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_EMERGENCY3) ||
        (0 == dido_structure.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_EMERGENCY1) ||
        (0 == dido_structure.mcp23017_0x02_u.mcp23017_0x02_bit.DI_CITB_EMERGENCY5))
    {
        manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_EMERGENCY_STOP_STATE_BIT, 1);
    }
    else
    {
        manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_EMERGENCY_STOP_STATE_BIT, 0);
    }
    // door check
    if ((dido_structure.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR1 == 0) ||
        (dido_structure.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR2 == 0))
    {
        if ((state == STATE_MACHINE_READY) ||
            (state == STATE_MACHINE_WORK) ||
            (state == STATE_MACHINE_SURVIEW_READY) ||
            (state == STATE_MACHINE_SURVIEW_WORK) ||
            (state == STATE_MACHINE_CT_READY) ||
            (state == STATE_MACHINE_CT_WORK))
        {
            manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_DOOR_STATE_BIT, 1);
        }
        else
        {
            manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_DOOR_STATE_BIT, 0);
        }
        bit_set(self->fixed_interlock_table.not_ready_event, NOT_READY_DOOR_BIT_POS);
    }
    else
    {
        manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_DOOR_STATE_BIT, 0);
        bit_clean(self->fixed_interlock_table.not_ready_event, NOT_READY_DOOR_BIT_POS);
    }
    // search
    if (dido_structure.mcp23017_0x02_u.mcp23017_0x02_bit.DI_TREATMENT_ROOM_DOOR_READY == 0)
    {
        bit_set(self->fixed_interlock_table.not_ready_event, NOT_READY_TREATMENT_ROOM_SEARCH_BIT_POS);
    }
    else
    {
        bit_clean(self->fixed_interlock_table.not_ready_event, NOT_READY_TREATMENT_ROOM_SEARCH_BIT_POS);
    }
    // HvEn check
    if (dido_structure.mcp23017_0x00_u.mcp23017_0x00_bit.DI_HvEn != 1)
    {
        if ((state == STATE_MACHINE_READY) ||
            (state == STATE_MACHINE_WORK) ||
            (state == STATE_MACHINE_SURVIEW_READY) ||
            (state == STATE_MACHINE_SURVIEW_WORK) ||
            (state == STATE_MACHINE_CT_READY) ||
            (state == STATE_MACHINE_CT_WORK))
        {
            if (self->cur_time - self->last_time > RTM_ERROR_WAIT_TIME)
            {
                manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_HV_EN_STATE_BIT, 1);
            }
            else
            {
                retval = -1;
            }
        }
        else
        {
            manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_HV_EN_STATE_BIT, 0);
        }
    }
    // kv_treatment_en check
    if (dido_structure.mcp23017_0x00_u.mcp23017_0x00_bit.DI_KV_TreatmentEN != 1)
    {
        if ((state == STATE_MACHINE_SURVIEW_WORK) ||
            (state == STATE_MACHINE_CT_WORK))
        {
            if (self->cur_time - self->last_time > RTM_ERROR_WAIT_TIME)
            {
                manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_KV_TREATMENT_EN_STATE_BIT, 1);
            }
            else
            {
                retval = -1;
            }
        }
        else
        {
            manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_KV_TREATMENT_EN_STATE_BIT, 0);
        }
    }
    // mv_treatment_en check
    if (dido_structure.mcp23017_0x00_u.mcp23017_0x00_bit.DI_MV_TreatmentEN != 1)
    {
        if (state == STATE_MACHINE_WORK)
        {
            if (self->cur_time - self->last_time > RTM_ERROR_WAIT_TIME)
            {
                manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_MV_TREATMENT_EN_STATE_BIT, 1);
            }
            else
            {
                retval = -1;
            }
        }
        else
        {
            manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_MV_TREATMENT_EN_STATE_BIT, 0);
        }
    }
    // RTC_OK check
    if (dido_structure.mcp23017_0x02_u.mcp23017_0x02_bit.RTC_WD_OK_IN != 1)
    {
        if (self->cur_time - self->last_time > RTM_ERROR_WAIT_TIME)
        {
            manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_RTC_OK_STATE_BIT, 1);
        }
    }
    else
    {
        manage_info_status_word_set(&app_rtm->manage_info, RTM_MAIN_RTC_OK_STATE_BIT, 0);
    }

    if ((app_rtm->psm_current_state == MV_INTERRUPT_SIG) || (app_rtm->psm_current_state == MV_TERMINATE_SIG))
    {
        manage_info_status_word_set(&app_rtm->rtm_module_info[RTM_MODULE_PSM].manage_info, MODULE_STATE_MACHINE_STATE_BIT, 1);
    }
    else
    {
        manage_info_status_word_set(&app_rtm->rtm_module_info[RTM_MODULE_PSM].manage_info, MODULE_STATE_MACHINE_STATE_BIT, 0);
    }

    if ((app_rtm->gmm_current_state == MV_INTERRUPT_SIG) || (app_rtm->gmm_current_state == MV_TERMINATE_SIG))
    {
        manage_info_status_word_set(&app_rtm->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].manage_info, MODULE_STATE_MACHINE_STATE_BIT, 1);
    }
    else
    {
        manage_info_status_word_set(&app_rtm->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].manage_info, MODULE_STATE_MACHINE_STATE_BIT, 0);
    }

    manage_info_status_word_get(&app_rtm->manage_info, &self->dynamic_app_state_table.rtm_main_state);
    manage_info_status_word_get(&app_rtm->rtm_module_info[RTM_MODULE_PSM].manage_info, &self->dynamic_app_state_table.psm_state);
    // manage_info_status_word_get(&app_rtm->rtm_module_info[RTM_MODULE_GMM].manage_info, &self->dynamic_app_state_table.gmm_state);
    manage_info_status_word_get(&app_rtm->rtm_module_info[RTM_MODULE_FKP].manage_info, &self->dynamic_app_state_table.fkp_state);
    manage_info_status_word_get(&app_rtm->rtm_module_info[RTM_MODULE_CPG].manage_info, &self->dynamic_app_state_table.cpg_state);
    manage_info_status_word_get(&app_rtm->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].manage_info, &self->dynamic_app_state_table.plc_state);
    manage_info_status_word_get(&app_rtm->rtm_module_info[RTM_MODULE_RTM_ON].manage_info, &self->dynamic_app_state_table.rtm_on_state);
    manage_info_status_word_get(&app_rtm->app_dido.manage_info, &self->dynamic_app_state_table.dido_state);
    manage_info_status_word_get(&app_rtm->app_data_record.manage_info, &self->dynamic_app_state_table.data_record);

    self->fixed_app_state_table.rtm_main_state |= self->dynamic_app_state_table.rtm_main_state;
    self->fixed_app_state_table.psm_state |= self->dynamic_app_state_table.psm_state;
    self->fixed_app_state_table.gmm_state |= self->dynamic_app_state_table.gmm_state;
    self->fixed_app_state_table.fkp_state |= self->dynamic_app_state_table.fkp_state;
    self->fixed_app_state_table.cpg_state |= self->dynamic_app_state_table.cpg_state;
    self->fixed_app_state_table.plc_state |= self->dynamic_app_state_table.plc_state;
    self->fixed_app_state_table.rtm_on_state |= self->dynamic_app_state_table.rtm_on_state;
    self->fixed_app_state_table.dido_state |= self->dynamic_app_state_table.dido_state;
    self->fixed_app_state_table.data_record |= self->dynamic_app_state_table.data_record;

    fault_check_interlock_table_update(self, state);

    if (bit_get(self->fixed_app_state_table.rtm_main_state, RTM_MAIN_EMERGENCY_STOP_STATE_BIT))
    {
        dido_structure.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn = 0;
        app_do_set(&(app_rtm->app_dido), &dido_structure);
    }
    else
    {
        if (state != STATE_MACHINE_SHUTDOWN)
        {
            dido_structure.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn = 1;
            app_do_set(&(app_rtm->app_dido), &dido_structure);
        }
    }
    return retval;
}
void fault_check_init(rtm_fault_check_t *self)
{
    self->cur_time = osKernelGetTickCount();
    self->last_time = self->cur_time;
}
void fault_clear(rtm_fault_check_t *self)
{
    self->cur_time = osKernelGetTickCount();
    self->last_time = self->cur_time;

    self->fixed_app_state_table.rtm_main_state = self->dynamic_app_state_table.rtm_main_state;
    self->fixed_app_state_table.psm_state = self->dynamic_app_state_table.psm_state;
    self->fixed_app_state_table.gmm_state = self->dynamic_app_state_table.gmm_state;
    self->fixed_app_state_table.fkp_state = self->dynamic_app_state_table.fkp_state;
    self->fixed_app_state_table.cpg_state = self->dynamic_app_state_table.cpg_state;
    self->fixed_app_state_table.plc_state = self->dynamic_app_state_table.plc_state;
    self->fixed_app_state_table.rtm_on_state = self->dynamic_app_state_table.rtm_on_state;
    self->fixed_app_state_table.dido_state = self->dynamic_app_state_table.dido_state;
    self->fixed_app_state_table.data_record = self->dynamic_app_state_table.data_record;
}

int32_t fault_override(rtm_fault_check_t *self)
{
    int32_t retval = NO_FAULT;
    if (self == NULL)
    {
        return SERIOUS_INTERLOCK;
    }
    if (((*(uint32_t *)&(self->fixed_interlock_table.not_ready_event) & (~(self->unready_override))) != 0))
    {
        retval |= NOT_READY_EVENT;
    }
    if (((*(uint32_t *)&(self->fixed_interlock_table.warning_interlock) & (~(self->interlock_override))) != 0))
    {
        retval |= WARNING_INTERLOCK;
    }
    if (((*(uint32_t *)&(self->fixed_interlock_table.minor_interlock) & (~(self->interlock_override))) != 0))
    {
        retval |= MINOR_INTERLOCK;
    }
    if (((*(uint32_t *)&(self->fixed_interlock_table.serious_interlock) & (~(self->interlock_override))) != 0))
    {
        retval |= SERIOUS_INTERLOCK;
    }
    return retval;
}

int32_t fault_override_set(rtm_fault_check_t *self, uint32_t unready_override, uint32_t interlock_override)
{
    if (self == NULL)
    {
        return -1;
    }
    self->interlock_override = interlock_override;
    self->unready_override = unready_override;
    return 0;
}

int32_t fault_interlock_table_get(rtm_fault_check_t *self, interlock_table_t *interlock_table)
{
    if (self == NULL || interlock_table == NULL)
    {
        return -1;
    }
    memcpy(interlock_table, &self->fixed_interlock_table, sizeof(interlock_table_t));
}
int32_t fault_table_init(rtm_fault_check_t *self)
{
    if (self == NULL)
    {
        return -1;
    }
    memset(self->fault_table, 'I', sizeof(self->fault_table));
    for (uint8_t i = INTERLOCK_HARDWARE_FAULT_BIT_POS; i < INTERLOCK_RTC_WD_OK_BIT_POS; i++)
    {
        for (uint8_t j = STATE_MACHINE_INIT; j < STATE_MACHINE_MAX; j++)
        {
            self->fault_table[i][j] = 'M';
        }
        self->fault_table[i][STATE_MACHINE_WORK] = 'S';
        self->fault_table[i][STATE_MACHINE_SURVIEW_WORK] = 'S';
        self->fault_table[i][STATE_MACHINE_CT_WORK] = 'S';
        self->fault_table[i][STATE_MACHINE_TERMINATE] = 'S';
    }
    return 0;
}
