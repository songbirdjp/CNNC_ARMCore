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
#include "app_keyboard.h"
#include "app_fkp.h"
#include "app_cpg.h"
#include "app_prompt.h"
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint32_t id_ack;
        uint8_t type;
        uint16_t length;
        uint8_t data[UART_PROTOCOL_DATA_MAX_LENGTH];
    } __attribute__((aligned(1), packed)) payload_t;

    typedef struct
    {
        payload_t payload;
        uint16_t length;
    } __attribute__((aligned(1), packed)) queue_frame_t;

    void app_rtm_event_output_set(void);
    // void app_rtm_ethercat_state_op_set(void);
    // void app_rtm_ethercat_state_op_clean(void);
    enum
    {
        BROADCAST_ID = 0x00,
        RTM_ON_PLC_ID = 0x01,
        RTM_ON_ARM_ID = 0x02,
        ICM_ID = 0x04,
        BGM_ID = 0x08,
        QAM_ID = 0x10,
        BSM_ID = 0x20, // not used in this version
        RTM_OFF_ARM_ID = 0x40,
        GMM_ID = 0x80,
        PSM_ID = 0x100,
        FKP_ID = 0x200,
        CPG_ID = 0x400
    };

    enum
    {
        SEND_BEAM_ID_CMD = 0x00,
        SEND_GMM_RADIATION_INDEX_CMD = 0x01,
        SEND_RTM_OFF_ARM_CURRENT_STATE_CMD = 0x61,
        SEND_FKP_POWER_OFF_CMD = 0x62,
        SEND_RTM_OFF_ARM_DIDO_CMD = 0x63,
        SEND_RTM_OFF_INFO_CMD = 0x64,
        SEND_RTM_OFF_BUTTON_CMD = 0x65,
        SEND_RTM_OFF_BLINK_CMD = 0x66,
        SEND_RTM_OFF_USER_PROMPT_CMD = 0x67,
        SEND_RTM_OFF_TRM_STATE_CMD = 0x68,
        SEND_RTM_OFF_LOAD_POSITION_CMD = 0x69,
        SEND_PSM_CURRENT_STATE_CMD = 0x71,
        SEND_PSM_INFO_CMD = 0x72,
        SEND_PSM_BRAKE_CMD = 0x73,
        SEND_GMM_CURRENT_STATE_CMD = 0x81,
        SEND_GMM_INFO_CMD = 0x82,
        SEND_GMM_PLC_INFO_CMD = 0x83,
        SEND_FKP_BUTTON_CMD = 0x91,
        SEND_CPG_BUTTON_CMD = 0xA1,
        SEND_MAX_CMD,
        SEND_RTM_OFF_ARM_REQUIRE_STATE_CMD = SEND_MAX_CMD,
        SEND_RTM_OFF_PROMPT_CMD = SEND_MAX_CMD + 1,
    };

    enum
    {
        RECEIVE_BEAM_ID_CMD = 0x00,
        RECEIVE_RADIATION_INDEX_CMD = 0x01,
        RECEIVE_FAULT_CLEAR_CMD = 0x02,
        RECEIVE_SYSTEM_STATE_CMD = 0x03,
        RECEIVE_RTM_OFF_ARM_REQUIRE_STATE_CMD = 0x16,
        RECEIVE_TREATMENT_RECORD_STATE_CMD = 0x18,
        RECEIVE_FKP_TIMESTAMP_CMD = 0x1C,
        RECEIVE_GMM_REQUIRE_STATE_CMD = 0x19,
        RECEIVE_PSM_REQUIRE_STATE_CMD = 0x1A,
        RECEIVE_STATE_SYNC_CMD = 0x1E,
        RECEIVE_PSM_CTRL_CMD = 0x22,
        RECEIVE_GMM_CTRL_CMD = 0x23,
        RECEIVE_FKP_DOSE_CMD = 0x33,
        RECEIVE_MAX_CMD,
        RECEIVE_RTM_ON_ARM_CURRENT_STATE_CMD = RECEIVE_MAX_CMD,
    };

    typedef enum
    {

        RTM_MODULE_RTM_ON = 0,
        // RTM_MODULE_GMM,
        RTM_MODULE_PSM,
        RTM_MODULE_FKP,
        RTM_MODULE_CPG,
        RTM_MODULE_RTM_OFF_ARM,
        RTM_MODULE_RTM_OFF_PLC,

        RTM_MODULE_MAX
    } rtm_module_t;

    typedef enum
    {
        MODULE_TX_ENABLE = 0,
        MODULE_TX_DISABLE,
    } module_tx_state_t;
    typedef struct rtm_module_info
    {
#define MODULE_INIT_BIT (0)
#define MODULE_LINK_STATE_BIT (1)
#define MODULE_FAULT_STATE_BIT (2)
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

    typedef struct psm_info
    {
        uint32_t psm_move_status;
        float psm_position_x_cur;
        float psm_position_y_cur;
        float psm_position_z_cur;
        float psm_position_x_r_tar;
        float psm_position_y_r_tar;
        float psm_position_z_r_tar;
        float psm_velocity_x_cur;
        float psm_velocity_y_cur;
        float psm_velocity_z_cur;
        float psm_velocity_x_r_tar;
        float psm_velocity_y_r_tar;
        float psm_velocity_z_r_tar;
    } __attribute__((aligned(1), packed)) psm_info_t;

    typedef struct rtm_ethercat_info
    {
#define ETHERCAT_SLAVE_INIT_BIT (0)
#define ETHERCAT_LINK_STATE_BIT (1)
        manage_info_t manage_info;
    } rtm_ethercat_info_t;

    typedef struct app_rtm_main
    {
#define RTM_MAIN_INIT_BIT (0)
        manage_info_t manage_info;
        app_data_record_t app_data_record;

        rtm_fault_check_t fault_check;
        interlock_table_t interlock_table;

        uint8_t gmm_current_state;
        uint8_t psm_current_state;
        uint32_t psm_not_ready_event;
        psm_info_t psm_info;
        uint16_t PLC_info;
        uint32_t interlock_override;
        uint32_t unready_override;
        uint8_t beamID;
        uint16_t led_belt;

        rtm_StateMachine_t state_machine;

        rtm_module_info_t rtm_module_info[RTM_MODULE_MAX];

        osEventFlagsId_t ethercat_Event;
        rtm_ethercat_info_t rtm_ethercat_info;

        app_keyboard_t app_keyboard;
        app_cpg_t app_cpg;
        app_dido_t app_dido;
        app_prompt_t app_prompt;
        uint8_t cpg_fkp_emergency_stop;
    } app_rtm_main_t;

    int32_t rtm_set_data_distribute(osMessageQueueId_t queue_id, uint32_t ID, uint8_t cmd, uint8_t *data, uint16_t len);
#ifdef __cplusplus
}
#endif

#endif /* __RTM_MAIN_H__ */
