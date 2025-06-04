/**
 * @file rtm_main.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-04-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __RTM_MAIN_H__
#define __RTM_MAIN_H__
#include "app_manage.h"
#include "app_dido.h"
#include "uart_protocol.h"
#include "app_state_machine.h"
#ifdef __cplusplus
extern "C"
{
#endif

    void app_rtm_event_output_set(void);
    // void app_rtm_ethercat_state_op_set(void);
    // void app_rtm_ethercat_state_op_clean(void);

    typedef enum
    {

        RTM_MODULE_RTM_ON_PLC = 0,
        RTM_MODULE_RTM_ON_ARM,
        RTM_MODULE_ICM,
        RTM_MODULE_BGM,
        RTM_MODULE_QAM,
        // RTM_MODULE_BSM,

        RTM_MODULE_RTM_OFF,

        RTM_MODULE_MAX
    } rtm_module_t;

    typedef struct rtm_module_info
    {
        manage_info_t manage_info;
        const char *module_name;
        const char *module_type;
        uint32_t ID;
        osPriority_t module_priority;
        heartbeat_t heartbeat_info_rx; // receive heartbeat info
        heartbeat_t heartbeat_info_tx; // send heartbeat info

        uart_protocol_t uart_protocol;
        osMessageQueueId_t module_queue;

        uint32_t id_group[RTM_MODULE_MAX];
        osMessageQueueId_t queue_group[RTM_MODULE_MAX];
    } rtm_module_info_t;

typedef struct app_not_ready_event_table
{
    uint32_t reserved : 32;
} app_not_ready_event_table_t;

typedef struct app_serious_interlock_table
{
    uint32_t HvEN : 1;
    uint32_t KVTreatmentEn : 1;
    uint32_t MVTreatmentEn : 1;
    uint32_t reserved : 29;
} app_serious_interlock_table_t;

    typedef struct app_rtm_main
    {
        manage_info_t manage_info;

    app_not_ready_event_table_t not_ready_event;
    uint32_t warning_interlock;
    uint32_t minor_interlock;
    app_serious_interlock_table_t serious_interlock;

    uint32_t interlock_override;
    uint32_t unready_override;

    // rtm_state_machine_t state_machine;
    rtm_StateMachine_t state_machine;

        rtm_module_info_t rtm_module_info[RTM_MODULE_MAX];

        osEventFlagsId_t ethercat_Event;
        app_dido_t app_dido;
    } app_rtm_main_t;

#ifdef __cplusplus
}
#endif

#endif /* __RTM_MAIN_H__ */