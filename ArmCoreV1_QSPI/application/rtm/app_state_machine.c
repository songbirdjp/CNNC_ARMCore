/**
 * @file app_state_machine.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-05-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "app_state_machine.h"
#include "app_fault_check.h"
#include "ulog.h"
#include "rtm_main.h"
#include "app_manage.h"
#define RTM_INIT_WAIT_TIME (100000)
static int32_t stateMachine_ctor(StateMachine_t *self,
                                 StateHandler_t initial)
{
    if (self == NULL || initial == NULL)
    {
        return -1;
    }
    memset(self, 0, sizeof(StateMachine_t));
    self->StateHandler = initial;
    return 0;
}
static int32_t stateMachine_init(StateMachine_t *self, Event_t const *e)
{
    if (self == NULL)
    {
        return -1;
    }
    self->StateHandler(self, e);
    return 0;
}
static int32_t stateMachine_dispatch(StateMachine_t *self, Event_t const *e)
{
    if (self == NULL || e == NULL)
    {
        return -1;
    }
    if (e->sig >= MAX_SIG)
    {
        return -2;
    }
    if (self->StateHandler == NULL)
    {
        return -3;
    }
    StateHandler_t handler = self->StateHandler;
    Event_t event = {0};
    State_t status = handler(self, e);

    while (status == RET_TRAN)
    {
        event.sig = EXIT_SIG;
        handler(self, &event);

        handler = self->StateHandler;

        event.sig = ENTER_SIG;
        status = self->StateHandler(self, &event);
    }
    return status;
}

/*************************************state machine start**********************/

static State_t system_initialization(void *self, Event_t const *const e);
static State_t system_systemOn(void *self, Event_t const *const e);
static State_t system_shutdown(void *self, Event_t const *const e);
static State_t system_powerSaver(void *self, Event_t const *const e);
static State_t system_mv_preliminary(void *self, Event_t const *const e);
static State_t system_mv_prepare(void *self, Event_t const *const e);
static State_t system_mv_ready(void *self, Event_t const *const e);
static State_t system_mv_radiation(void *self, Event_t const *const e);
static State_t system_mv_complete(void *self, Event_t const *const e);
static State_t system_mv_interrupt(void *self, Event_t const *const e);
static State_t system_mv_terminate(void *self, Event_t const *const e);

static State_t system_kv_preliminary(void *self, Event_t const *const e);
static State_t system_kv_prepare(void *self, Event_t const *const e);
static State_t system_surview_ready(void *self, Event_t const *const e);
static State_t system_surview_radiation(void *self, Event_t const *const e);
static State_t system_ct_ready(void *self, Event_t const *const e);
static State_t system_ct_radiation(void *self, Event_t const *const e);
static State_t system_kv_complete(void *self, Event_t const *const e);
static State_t system_kv_terminate(void *self, Event_t const *const e);

static State_t module_init(void *self, Event_t const *const e);
static State_t module_manual(void *self, Event_t const *const e);
static State_t module_idle(void *self, Event_t const *const e);
static State_t module_shutdown(void *self, Event_t const *const e);
static State_t module_powerSaver(void *self, Event_t const *const e);
static State_t module_mv_preliminary(void *self, Event_t const *const e);
static State_t module_mv_prepare(void *self, Event_t const *const e);
static State_t module_mv_ready(void *self, Event_t const *const e);
static State_t module_mv_work(void *self, Event_t const *const e);
static State_t module_mv_complete(void *self, Event_t const *const e);
static State_t module_mv_interrupt(void *self, Event_t const *const e);
static State_t module_mv_terminate(void *self, Event_t const *const e);
static State_t module_kv_preliminary(void *self, Event_t const *const e);
static State_t module_kv_prepare(void *self, Event_t const *const e);
static State_t module_surview_ready(void *self, Event_t const *const e);
static State_t module_surview_work(void *self, Event_t const *const e);
static State_t module_ct_ready(void *self, Event_t const *const e);
static State_t module_ct_work(void *self, Event_t const *const e);
static State_t module_kv_complete(void *self, Event_t const *const e);
static State_t module_kv_terminate(void *self, Event_t const *const e);

static State_t system_initialization(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_initialization exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        rtm_sm->current_state = INITIALIZATION_SIG;
        status = TRAN(&module_init);
        // LOG_I("system_initialization enter\r\n");
        break;
    }
    }
    return status;
}

static State_t module_init(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    uint32_t cur_time = 0;
    static uint32_t last_time = 0;
    uint32_t ret = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_STAND_RESERVE = 1;
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RTM_SystemShutDown = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        last_time = osKernelGetTickCount();
        // LOG_I("module_init enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_init exit\r\n");
        // ret = osThreadFlagsGet();
        // if (ret & APP_RTM_THREAD_FLAG_ETHERCAT_READY)
        // {
        rtm->rtm_module_info[RTM_MODULE_RTM_ON].tx_disable = MODULE_TX_ENABLE;
        // }
        // if (ret & APP_RTM_THREAD_FLAG_RTM_OFF_READY)
        // {
        rtm->rtm_module_info[RTM_MODULE_PSM].tx_disable = MODULE_TX_ENABLE;
        // }
        // if (ret & APP_RTM_THREAD_FLAG_ICM_READY)
        // {
        rtm->rtm_module_info[RTM_MODULE_FKP].tx_disable = MODULE_TX_ENABLE;
        // }
        // if (ret & APP_RTM_THREAD_FLAG_BGM_READY)
        // {
        rtm->rtm_module_info[RTM_MODULE_CPG].tx_disable = MODULE_TX_ENABLE;
        // }
        // if (ret & APP_RTM_THREAD_FLAG_QAM_READY)
        // {
        rtm->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].tx_disable = MODULE_TX_ENABLE;
        // }
        if (manage_info_status_bit_get(&rtm->manage_info, RTM_MAIN_MEMORY_STATE_BIT) ||
            manage_info_status_bit_get(&rtm->manage_info, RTM_MAIN_WAIT_GMM_SYSTEM_ON_STATE_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_PSM].manage_info, MODULE_PERIPHERAL_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_PSM].manage_info, MODULE_TX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_PSM].manage_info, MODULE_RX_INIT_BIT) ||
            // manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_GMM].manage_info, MODULE_PERIPHERAL_INIT_BIT) ||
            // manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_GMM].manage_info, MODULE_TX_INIT_BIT) ||
            // manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_GMM].manage_info, MODULE_RX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_FKP].manage_info, MODULE_PERIPHERAL_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_FKP].manage_info, MODULE_TX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_FKP].manage_info, MODULE_RX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_CPG].manage_info, MODULE_PERIPHERAL_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_CPG].manage_info, MODULE_TX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_CPG].manage_info, MODULE_RX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_RTM_ON].manage_info, MODULE_PERIPHERAL_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_RTM_ON].manage_info, MODULE_TX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_RTM_ON].manage_info, MODULE_RX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].manage_info, MODULE_PERIPHERAL_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].manage_info, MODULE_TX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->rtm_module_info[RTM_MODULE_RTM_OFF_PLC].manage_info, MODULE_RX_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->app_dido.manage_info, DIDO_PERIPHERAL_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->app_dido.manage_info, DIDO_DI_INIT_BIT) ||
            manage_info_status_bit_get(&rtm->app_dido.manage_info, DIDO_DO_INIT_BIT))
        {
            manage_info_status_word_set(&rtm->manage_info, RTM_MAIN_INIT_STATE_BIT, 1);
        }
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case TIME_SIG:
    {
        if (app_rtm_thread_flag_get(1) > 0)
        {
            check_finish = fault_check(&rtm->fault_check,
                                       STATE_MACHINE_INIT,
                                       rtm);
            if (check_finish == 0)
            {
                if (rtm->gmm_current_state == SYSTEM_ON_SIG)
                {
                    status = TRAN(&system_systemOn);
                }
                else
                {
                    if ((cur_time - last_time) > RTM_INIT_WAIT_TIME)
                    {
                        if (rtm->gmm_current_state != SYSTEM_ON_SIG)
                        {
                            manage_info_status_word_set(&rtm->manage_info, RTM_MAIN_WAIT_GMM_SYSTEM_ON_STATE_BIT, 1);
                        }
                        status = TRAN(&system_systemOn);
                    }
                }
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            cur_time = osKernelGetTickCount();
            if ((cur_time - last_time) > RTM_INIT_WAIT_TIME)
            {
                if (rtm->gmm_current_state != SYSTEM_ON_SIG)
                {
                    manage_info_status_word_set(&rtm->manage_info, RTM_MAIN_WAIT_GMM_SYSTEM_ON_STATE_BIT, 1);
                }
                status = TRAN(&system_systemOn);
            }
            else
            {
                ret = osThreadFlagsGet();
                if ((ret & APP_RTM_THREAD_FLAG_DI_READY) &&
                    (ret & APP_RTM_THREAD_FLAG_DO_READY))
                {
                    app_di_get(&(rtm->app_dido), &dido_structure);
                    // TODO:罩壳状态获取
                    app_do_get(&(rtm->app_dido), &dido_structure);
                    dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
                    dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
                    app_do_set(&(rtm->app_dido), &dido_structure);

                    fault_check(&rtm->fault_check,
                                STATE_MACHINE_INIT,
                                rtm);
                }
                status = HANDLED();
            }
        }
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_manual(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        // LOG_I("module_manual enter\r\n");
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        status = HANDLED();
        break;
    }
    case MANUAL_EXIT_SIG:
    {
        // LOG_I("module_manual exit\r\n");
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        status = TRAN(&system_systemOn);
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_systemOn(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;

    switch (e->sig)
    {
    case ENTER_SIG:
    {
        // LOG_I("system_systemOn enter\r\n");
        rtm_sm->current_state = SYSTEM_ON_SIG;
        status = TRAN(&module_idle);
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_systemOn exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_idle(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    static uint16_t PLC_info = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_idle enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        check_finish = -1;
        // LOG_I("module_idle exit\r\n");
        status = HANDLED();
        break;
    }
    case INITIALIZATION_SIG:
    {
        HAL_NVIC_SystemReset(); // reset system
        // status = TRAN(&system_initialization);
        break;
    }
    case MANUAL_ENTER_SIG:
    {
        status = TRAN(&module_manual);
        break;
    }
    case SHUTDOWN_SIG:
    {
        status = TRAN(&system_shutdown);
        break;
    }
    case POWER_SAVER_SIG:
    {
        status = TRAN(&system_powerSaver);
        break;
    }
    case MV_PREPARE_SIG:
    {
        if ((check_finish == 0) && (fault_override(&(rtm->fault_check)) == NO_FAULT))
        {
            status = TRAN(&system_mv_preliminary);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case KV_PRELIMINARY_SIG:
    {
        if ((check_finish == 0) && (fault_override(&(rtm->fault_check)) == NO_FAULT))
        {
            status = TRAN(&system_kv_preliminary);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_IDLE,
                                   rtm);
        if ((rtm->PLC_info & 0x02) != (PLC_info & 0x02))
        {
            app_do_get(&(rtm->app_dido), &dido_structure);
            PLC_info = rtm->PLC_info;
            if (PLC_info & 0x02)
            {
                dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RTM_AutoPowerUp = 1;
                // TODO:通知FKP当前是自动开机状态
            }
            else
            {
                dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RTM_AutoPowerUp = 0;
                // TODO:通知FKP当前是手动开机状态
            }
            app_do_set(&(rtm->app_dido), &dido_structure);
        }
        status = HANDLED();
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_shutdown(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = SHUTDOWN_SIG;
        status = TRAN(&module_shutdown);
        // LOG_I("system_shutdown enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        status = HANDLED();
        // LOG_I("system_shutdown exit\r\n");
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_shutdown(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static uint16_t PLC_info = 0;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        rtm->rtm_module_info[RTM_MODULE_PSM].tx_disable = MODULE_TX_DISABLE;
        // LOG_I("module_shutdown enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        uint8_t fkp_power_state = 0x00;
        rtm_set_data_distribute(rtm->rtm_module_info->queue_group[RTM_MODULE_FKP], FKP_ID, SEND_FKP_POWER_OFF_CMD, &fkp_power_state, sizeof(fkp_power_state));

        PLC_info = 0;
        // LOG_I("module_shutdown exit\r\n");
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_initialization);
        break;
    }
    case TIME_SIG:
    {
        if ((rtm->PLC_info & 0x01) && (PLC_info == 0))
        {
            PLC_info = 1;
            app_do_get(&(rtm->app_dido), &dido_structure);
            dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RTM_SystemShutDown = 1;
            dido_structure.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn = 0;
            dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
            dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
            app_do_set(&(rtm->app_dido), &dido_structure);

            uint8_t fkp_power_state = 0x01;
            rtm_set_data_distribute(rtm->rtm_module_info->queue_group[RTM_MODULE_FKP], FKP_ID, SEND_FKP_POWER_OFF_CMD, &fkp_power_state, sizeof(fkp_power_state));
        }
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_powerSaver(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = POWER_SAVER_SIG;
        status = TRAN(&module_powerSaver);
        // LOG_I("system_powerSaver enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_powerSaver exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_powerSaver(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);

        // LOG_I("module_powerSaver enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_powerSaver exit\r\n");
        status = HANDLED();
        break;
    }
    case SHUTDOWN_SIG:
    {
        status = TRAN(&system_shutdown);
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_systemOn);
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_preliminary(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_PREPARE_SIG;
        status = TRAN(&module_mv_preliminary);
        // LOG_I("system_mv_preliminary enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_preliminary exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_preliminary(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_mv_preliminary enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_preliminary exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate); // TODO change to system_terminate
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_PRELIMINARY,
                                   rtm);
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                status = TRAN(&system_mv_prepare);
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case MANUAL_ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        status = HANDLED();
        break;
    }
    case MANUAL_EXIT_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        status = HANDLED();
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_PREPARE_SIG;
        status = TRAN(&module_mv_prepare);
        // LOG_I("system_mv_prepare enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_prepare exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_mv_prepare enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_prepare exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate); // TODO change to system_terminate
        break;
    }
    case MV_READY_SIG:
    {
        if ((check_finish == 0) && (fault_override(&(rtm->fault_check)) == NO_FAULT))
        {
            status = TRAN(&system_mv_ready);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_PREPARE,
                                   rtm);
        status = HANDLED();
        break;
    }
    case MANUAL_ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        status = HANDLED();
        break;
    }
    case MANUAL_EXIT_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        status = HANDLED();
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_READY_SIG;
        status = TRAN(&module_mv_ready);
        // LOG_I("system_mv_ready enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_ready exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    static int32_t check_finish = -1;
    dido_structure_t dido_structure = {0};
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_mv_ready enter\r\n");
        fault_check_init(&(rtm->fault_check));
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_ready exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case MV_RADIATION_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                status = TRAN(&system_mv_radiation);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case MV_INTERRUPT_SIG:
    {
        status = TRAN(&system_mv_interrupt);
        break;
    }
    case MV_TERMINATE_SIG:
    {
        status = TRAN(&system_mv_terminate);
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_READY,
                                   rtm);
        if (check_finish == 0)
        {
            int32_t fault_flag = fault_override(&(rtm->fault_check));

            if ((fault_flag & SERIOUS_INTERLOCK) != 0)
            {
                status = TRAN(&system_mv_interrupt);
            }
            else if ((fault_flag & MINOR_INTERLOCK) != 0)
            {
                status = TRAN(&system_mv_interrupt);
            }
            else if ((fault_flag & WARNING_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & NOT_READY_EVENT) != 0)
            {
                status = HANDLED();
            }
            else if (fault_flag == NO_FAULT)
            {
                status = HANDLED();
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_radiation(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_RADIATION_SIG;
        status = TRAN(&module_mv_work);
        // LOG_I("system_mv_work enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_work exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_work(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RadiationIndicator = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_mv_work enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_work exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case MV_COMPLETE_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                status = TRAN(&system_mv_complete);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case MV_INTERRUPT_SIG:
    {
        status = TRAN(&system_mv_interrupt);
        break;
    }
    case MV_TERMINATE_SIG:
    {
        status = TRAN(&system_mv_terminate);
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_WORK,
                                   rtm);
        if (check_finish == 0)
        {
            int32_t fault_flag = fault_override(&(rtm->fault_check));

            if ((fault_flag & SERIOUS_INTERLOCK) != 0)
            {
                status = TRAN(&system_mv_terminate);
            }
            else if ((fault_flag & MINOR_INTERLOCK) != 0)
            {
                status = TRAN(&system_mv_interrupt);
            }
            else if ((fault_flag & WARNING_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & NOT_READY_EVENT) != 0)
            {
                status = HANDLED();
            }
            else if (fault_flag == NO_FAULT)
            {
                status = HANDLED();
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    // case ERROR_SIG:
    // {
    //     fault_clear(&(rtm->fault_check));
    //     check_finish = -1;
    //     status = HANDLED();
    //     break;
    // }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_complete(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_COMPLETE_SIG;
        status = TRAN(&module_mv_complete);
        // LOG_I("system_mv_complete enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_complete exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_complete(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RadiationIndicator = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_mv_complete enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_complete exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_systemOn);
        break;
    }
    case MV_PREPARE_SIG:
    {
        status = TRAN(&system_mv_prepare);
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_COMPLETE,
                                   rtm);
        status = HANDLED();
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_interrupt(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_INTERRUPT_SIG;
        status = TRAN(&module_mv_interrupt);
        // LOG_I("system_mv_interrupt enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_interrupt exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_interrupt(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RadiationIndicator = 0;

        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_mv_interrupt enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_interrupt exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case MV_READY_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                app_do_get(&(rtm->app_dido), &dido_structure);
                dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
                dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 1;
                dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
                app_do_set(&(rtm->app_dido), &dido_structure);
                status = TRAN(&system_mv_ready);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case MV_TERMINATE_SIG:
    {
        status = TRAN(&system_mv_terminate);
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_INTERRUPT,
                                   rtm);
        if (check_finish == 0)
        {
            int32_t fault_flag = fault_override(&(rtm->fault_check));

            if ((fault_flag & SERIOUS_INTERLOCK) != 0)
            {
                status = TRAN(&system_mv_terminate);
            }
            else if ((fault_flag & MINOR_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & WARNING_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & NOT_READY_EVENT) != 0)
            {
                status = HANDLED();
            }
            else if (fault_flag == NO_FAULT)
            {
                status = HANDLED();
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_mv_terminate(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = MV_TERMINATE_SIG;
        status = TRAN(&module_mv_terminate);
        // LOG_I("system_mv_terminate enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_mv_terminate exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_mv_terminate(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RadiationIndicator = 0;

        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_mv_terminate enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_mv_terminate exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_systemOn);
        // TODO:高级故障清除
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_TERMINATE,
                                   rtm);
        status = HANDLED();
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_kv_terminate(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = SYSTEM_ON_SIG;
        status = TRAN(&module_kv_terminate);
        // LOG_I("system_kv_terminate enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_kv_terminate exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_kv_terminate(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RadiationIndicator = 0;

        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_kv_terminate enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_kv_terminate exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_TERMINATE,
                                   rtm);
        if (check_finish == 0)
        {
            status = TRAN(&system_systemOn);
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_kv_preliminary(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = KV_PRELIMINARY_SIG;
        status = TRAN(&module_kv_preliminary);
        // LOG_I("system_kv_preliminary enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_kv_preliminary exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_kv_preliminary(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_kv_preliminary enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_kv_preliminary exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case KV_PREPARE_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                status = TRAN(&system_kv_prepare);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case MANUAL_ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        status = HANDLED();
        break;
    }
    case MANUAL_EXIT_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        status = HANDLED();
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_KV_PRELIMINARY,
                                   rtm);
        if (check_finish == 0)
        {
            int32_t fault_flag = fault_override(&(rtm->fault_check));

            if ((fault_flag & SERIOUS_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & MINOR_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & WARNING_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & NOT_READY_EVENT) != 0)
            {
                status = HANDLED();
            }
            else if (fault_flag == NO_FAULT)
            {
                status = HANDLED();
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_kv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = KV_PREPARE_SIG;
        status = TRAN(&module_kv_prepare);
        // LOG_I("system_kv_prepare enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_kv_prepare exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_kv_prepare(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_kv_prepare enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_kv_prepare exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case SURVIEW_READY_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                status = TRAN(&system_surview_ready);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case CT_READY_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                status = TRAN(&system_ct_ready);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_KV_PREPARE,
                                   rtm);
        if (check_finish == 0)
        {
            int32_t fault_flag = fault_override(&(rtm->fault_check));

            if ((fault_flag & SERIOUS_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & MINOR_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & WARNING_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & NOT_READY_EVENT) != 0)
            {
                status = HANDLED();
            }
            else if (fault_flag == NO_FAULT)
            {
                status = HANDLED();
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_surview_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = SURVIEW_READY_SIG;
        status = TRAN(&module_surview_ready);
        // LOG_I("system_surview_ready enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_surview_ready exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_surview_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    static int32_t check_finish = -1;
    dido_structure_t dido_structure = {0};
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_surview_ready enter\r\n");
        fault_check_init(&(rtm->fault_check));
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_surview_ready exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case SURVIEW_RADIATION_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == 0)
            {
                status = TRAN(&system_surview_radiation);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_SURVIEW_READY,
                                   rtm);
        if (check_finish == 0)
        {
            int32_t fault_flag = fault_override(&(rtm->fault_check));

            if ((fault_flag & SERIOUS_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & MINOR_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & WARNING_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & NOT_READY_EVENT) != 0)
            {
                status = HANDLED();
            }
            else if (fault_flag == NO_FAULT)
            {
                status = HANDLED();
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_surview_radiation(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = SURVIEW_RADIATION_SIG;
        status = TRAN(&module_surview_work);
        // LOG_I("system_surview_radiation enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_surview_radiation exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_surview_work(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RadiationIndicator = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_surview_work enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_surview_work exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case KV_COMPLETE_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                status = TRAN(&system_kv_complete);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_SURVIEW_WORK,
                                   rtm);
        if (check_finish == 0)
        {
            int32_t fault_flag = fault_override(&(rtm->fault_check));

            if ((fault_flag & SERIOUS_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & MINOR_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & WARNING_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & NOT_READY_EVENT) != 0)
            {
                status = HANDLED();
            }
            else if (fault_flag == NO_FAULT)
            {
                status = HANDLED();
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    // case ERROR_SIG:
    // {
    //     fault_clear(&(rtm->fault_check));
    //     check_finish = -1;
    //     status = HANDLED();
    //     break;
    // }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_ct_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = CT_READY_SIG;
        status = TRAN(&module_ct_ready);
        // LOG_I("system_ct_ready enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_ct_ready exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_ct_ready(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    static int32_t check_finish = -1;
    dido_structure_t dido_structure = {0};
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN = 1;
        dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        // LOG_I("module_ct_ready enter\r\n");
        fault_check_init(&(rtm->fault_check));
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_ct_ready exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case CT_RADIATION_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                status = TRAN(&system_ct_radiation);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_CT_READY,
                                   rtm);
        if (check_finish == 0)
        {
            int32_t fault_flag = fault_override(&(rtm->fault_check));

            if ((fault_flag & SERIOUS_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & MINOR_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & WARNING_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & NOT_READY_EVENT) != 0)
            {
                status = HANDLED();
            }
            else if (fault_flag == NO_FAULT)
            {
                status = HANDLED();
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_ct_radiation(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = CT_RADIATION_SIG;
        status = TRAN(&module_ct_work);
        // LOG_I("system_ct_radiation enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_ct_radiation exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_ct_work(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    dido_structure_t dido_structure = {0};
    static int32_t check_finish = -1;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RadiationIndicator = 1;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_ct_work enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_ct_work exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_kv_terminate);
        break;
    }
    case KV_COMPLETE_SIG:
    {
        if (check_finish == 0)
        {
            if (fault_override(&(rtm->fault_check)) == NO_FAULT)
            {
                status = TRAN(&system_kv_complete);
            }
            else
            {
                status = HANDLED();
            }
        }
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_CT_WORK,
                                   rtm);
        if (check_finish == 0)
        {
            int32_t fault_flag = fault_override(&(rtm->fault_check));

            if ((fault_flag & SERIOUS_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & MINOR_INTERLOCK) != 0)
            {
                status = TRAN(&system_kv_terminate);
            }
            else if ((fault_flag & WARNING_INTERLOCK) != 0)
            {
                status = HANDLED();
            }
            else if ((fault_flag & NOT_READY_EVENT) != 0)
            {
                status = HANDLED();
            }
            else if (fault_flag == NO_FAULT)
            {
                status = HANDLED();
            }
            else
            {
                status = HANDLED();
            }
        }
        else
        {
            status = HANDLED();
        }
        break;
    }
    // case ERROR_SIG:
    // {
    //     fault_clear(&(rtm->fault_check));
    //     check_finish = -1;
    //     status = HANDLED();
    //     break;
    // }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t system_kv_complete(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        rtm_sm->current_state = KV_COMPLETE_SIG;
        status = TRAN(&module_kv_complete);
        // LOG_I("system_kv_complete enter\r\n");
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("system_kv_complete exit\r\n");
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
static State_t module_kv_complete(void *self, Event_t const *const e)
{
    State_t status;
    rtm_StateMachine_t *rtm_sm = (rtm_StateMachine_t *)self;
    app_rtm_main_t *rtm = (app_rtm_main_t *)(rtm_sm->parameters);
    static int32_t check_finish = -1;
    dido_structure_t dido_structure = {0};
    switch (e->sig)
    {
    case ENTER_SIG:
    {
        app_do_get(&(rtm->app_dido), &dido_structure);
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_RadiationIndicator = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn = 0;
        dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn = 0;
        app_do_set(&(rtm->app_dido), &dido_structure);
        fault_check_init(&(rtm->fault_check));
        // LOG_I("module_kv_complete enter\r\n");
        status = HANDLED();
        break;
    }
    case EXIT_SIG:
    {
        // LOG_I("module_kv_complete exit\r\n");
        check_finish = -1;
        status = HANDLED();
        break;
    }
    case SYSTEM_ON_SIG:
    {
        status = TRAN(&system_systemOn);
        break;
    }
    case TIME_SIG:
    {
        check_finish = fault_check(&rtm->fault_check,
                                   STATE_MACHINE_KV_COMPLETE,
                                   rtm);
        status = HANDLED();
        break;
    }
    case ERROR_SIG:
    {
        fault_clear(&(rtm->fault_check));
        check_finish = -1;
        status = HANDLED();
        break;
    }
    default:
    {
        status = IGNORED();
        break;
    }
    }
    return status;
}
void rtm_state_machine_ctor(rtm_StateMachine_t *self, void *parameters)
{
    if (self == NULL)
    {
        return;
    }
    memset(self, 0, sizeof(rtm_StateMachine_t));
    self->parameters = parameters;
    self->current_state = NULL_SIG;
    stateMachine_ctor(self, system_initialization);
    Event_t event = {NULL_SIG, NULL};
    stateMachine_dispatch(self, &event);
}

int32_t rtm_state_dispatch(rtm_StateMachine_t *self, Event_t const *e)
{
    if (self == NULL || e == NULL)
    {
        return -1;
    }

    int32_t ret = stateMachine_dispatch(self, e);
    if (ret != 0)
    {
        return -2;
    }

    return 0;
}
rtm_state_t rtm_get_state(rtm_StateMachine_t *self)
{
    if (self == NULL)
    {
        return 0;
    }
    return self->current_state;
}

/*************************************state machine end************************/
