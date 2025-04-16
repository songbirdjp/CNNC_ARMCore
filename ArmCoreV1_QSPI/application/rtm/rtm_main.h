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
    uint32_t ID;
    osPriority_t module_priority;
    heartbeat_t heartbeat_info_rx; // receive heartbeat info
    heartbeat_t heartbeat_info_tx; // send heartbeat info

    uart_protocol_t uart_protocol;
    osMessageQueueId_t module_queue;

    uint32_t id_group[RTM_MODULE_MAX];
    osMessageQueueId_t queue_group[RTM_MODULE_MAX];
} rtm_module_info_t;

typedef struct app_fault_table
{
    uint32_t serial_fault : 1;
    uint32_t ethercat_fault : 1;
    uint32_t dido_fault : 1;
    uint32_t reserved : 6;
} app_fault_table_t;

typedef struct app_rtm_main
{
    manage_info_t manage_info;

    app_fault_table_t fault_table;
    rtm_state_machine_t state_machine;

    rtm_module_info_t rtm_module_info[RTM_MODULE_MAX];

    osEventFlagsId_t ethercat_Event;
    app_dido_t app_dido;
} app_rtm_main_t;

typedef struct rtm_event
{
    Event_t super;
    dido_structure_t *dido_structure;
} rtm_event_t;

#ifdef __cplusplus
}
#endif

#endif /* __RTM_MAIN_H__ */