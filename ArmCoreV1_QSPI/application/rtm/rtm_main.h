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
#include "app_data_record.h"
#ifdef __cplusplus
extern "C"
{
#endif

    void app_rtm_event_output_set(void);
    void app_rtm_ethercat_state_op_set(void);
    void app_rtm_ethercat_state_op_clean(void);
    enum
    {
        BROADCAST_ID = 0x00,
        RTM_ON_PLC_ID = 0x01,
        RTM_ON_ARM_ID = 0x02,
        ICM_ID = 0x04,
        BGM_ID = 0x08,
        QAM_ID = 0x10,
        // BSM_ID = 0x20,    // not used in this version
        RTM_OFF_ARM_ID = 0x40,
        GMM_ID = 0x80,
        PSM_ID = 0x100,
        FKP_ID = 0x200,
        CPG_ID = 0x400
    };

    enum
    {
        OUTPUT_BEAM_ID_CMD = 0x00,
        OUTPUT_RADIATION_INDEX_CMD = 0x01,
        OUTPUT_FAULT_CLEAR_CMD = 0x02,
        OUTPUT_SYSTEM_STATE_CMD = 0x03,
        OUTPUT_ICM_REQUIRE_STATE_CMD = 0x12,
        OUTPUT_BGM_REQUIRE_STATE_CMD = 0x13,
        OUTPUT_QAM_REQUIRE_STATE_CMD = 0x14,
        // OUTPUT_BSM_REQUIRE_STATE_CMD = 0x15,
        OUTPUT_RTM_OFF_REQUIRE_STATE_CMD = 0x16,
        OUTPUT_RTM_OFF_ARM_TREATMENT_RECORD_CMD = 0x18,
        OUTPUT_GMM_REQUIRE_STATE_CMD = 0x19,
        OUTPUT_PSM_REQUIRE_STATE_CMD = 0x1A,
        OUTPUT_FKP_VIBRATION_CMD = 0x1B,
        OUTPUT_FKP_TIMESTAMP_CMD = 0x1C,
        OUTPUT_STATE_SYNC_CMD = 0x1E,
        OUTPUT_MAX_CMD,
        OUTPUT_RTM_ON_ARM_REQUIRE_STATE_CMD = OUTPUT_MAX_CMD,
    };

    enum
    {
        INPUT_RADIATION_INDEX_CMD = 0x01,
        INPUT_ICM_CURRENT_STATE_CMD = 0x21,
        INPUT_BGM_CURRENT_STATE_CMD = 0x31,
        INPUT_BGM_INFO_CMD = 0x32,
        INPUT_QAM_CURRENT_STATE_CMD = 0x41,
        INPUT_RTM_OFF_ARM_CURRENT_STATE_CMD = 0x61,
        INPUT_RTM_OFF_ARM_DIDO_CMD = 0x63,
        INPUT_RTM_OFF_ARM_BUTTON_CMD = 0x65,
        INPUT_RTM_OFF_ARM_TRM_REQUIRE_CMD = 0x68,
        INPUT_RTM_OFF_ARM_LOAD_POSITION_CMD = 0x69,
        INPUT_FKP_STATE_CMD = 0x6A,
        INPUT_CPG_L_STATE_CMD = 0x6B,
        INPUT_CPG_R_STATE_CMD = 0x6C,
        INPUT_PSM_CURRENT_STATE_CMD = 0x71,
        INPUT_PSM_INFO_CMD = 0x72,
        INPUT_GMM_CURRENT_STATE_CMD = 0x81,
        INPUT_GMM_INFO_CMD = 0x82,
        INPUT_MAX_CMD,
        INPUT_RTM_ON_ARM_CURRENT_STATE_CMD = INPUT_MAX_CMD,
        INPUT_RTM_ON_ARM_DIDO_CMD = INPUT_MAX_CMD + 1,
    };

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

    typedef enum
    {
        MODULE_TX_ENABLE = 0,
        MODULE_TX_DISABLE,
    } module_tx_state_t;
    typedef struct rtm_module_info
    {
#define MODULE_PERIPHERAL_INIT_BIT (0)
#define MODULE_TX_INIT_BIT (1)
#define MODULE_RX_INIT_BIT (2)
#define MODULE_LINK_STATE_BIT (3)
#define MODULE_RX_STATE_BIT (4)
#define MODULE_TX_STATE_BIT (5)
#define MODULE_TX_QUEUE_STATE_BIT (6)
        manage_info_t manage_info;
        uint32_t module_thread_flags;
        app_data_record_t *app_data_record;
        const char *module_name;
        const char *module_type;
        module_tx_state_t tx_disable;
        uint32_t ID;
        osPriority_t module_priority;
        heartbeat_t heartbeat_info_rx; // receive heartbeat info
        heartbeat_t heartbeat_info_tx; // send heartbeat info

        uart_protocol_t uart_protocol;
        osMessageQueueId_t module_queue;

        uint32_t id_group[RTM_MODULE_MAX];
        osMessageQueueId_t queue_group[RTM_MODULE_MAX];
    } rtm_module_info_t;

    typedef struct rtm_ethercat_info
    {
#define ETHERCAT_OP_STATE_BIT (0)
        manage_info_t manage_info;
    } rtm_ethercat_info_t;

    typedef struct app_rtm_main
    {
#define RTM_MAIN_INIT_STATE_BIT (0) /* 初始化状态,下列任意一个状态置位，该位将被置位 */
#define RTM_MAIN_PLC_STATE_BIT (1)
#define RTM_MAIN_ICM_STATE_BIT (2)
#define RTM_MAIN_BGM_STATE_BIT (3)
#define RTM_MAIN_QAM_STATE_BIT (4)
#define RTM_MAIN_BSM_STATE_BIT (5)
#define RTM_MAIN_OFF_STATE_BIT (6)
#define RTM_MAIN_DIDO_STATE_BIT (7)
#define RTM_MAIN_RESET_STATE_BIT (8)
#define RTM_MAIN_MEMORY_STATE_BIT (9)
#define RTM_MAIN_RECORD_STATE_BIT (10)
        manage_info_t manage_info;
        app_data_record_t app_data_record;

        uint16_t TotalStep;
        uint16_t CurrentStep;
        uint16_t ErrorCode;
        rtm_fault_check_t fault_check;
        interlock_table_t interlock_table;
        app_state_table_t app_state_table;
        uint16_t PLC_info;
        uint32_t interlock_override;
        uint32_t unready_override;
        uint8_t beamID;

        uint8_t icm_current_state;
        uint8_t bgm_current_state;
        uint8_t qam_current_state;

        rtm_StateMachine_t state_machine;

        rtm_module_info_t rtm_module_info[RTM_MODULE_MAX];

        osEventFlagsId_t ethercat_Event;
        rtm_ethercat_info_t rtm_ethercat_info;

        app_dido_t app_dido;
    } app_rtm_main_t;

#ifdef __cplusplus
}
#endif

#endif /* __RTM_MAIN_H__ */