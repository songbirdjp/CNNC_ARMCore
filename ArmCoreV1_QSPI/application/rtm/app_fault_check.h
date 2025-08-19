/**
 * @file app_fault_check.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-08-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef _APP_FAULT_CHECK_H_
#define _APP_FAULT_CHECK_H_

#include <stdint.h>
#include "app_state_machine.h"
#include "cmsis_os2.h"
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct manage_info
    {
        uint32_t status_word;
        uint32_t status_mask;  //用作状态管理，与status位对应，置1表示已被读取，置0表示未被读取
        uint32_t control_word; //用作属性管理，与status位对应，置1表示可以通过读清除，置0表示需要写清除
        osMutexId_t mutex_id;
    } manage_info_t;

#define bit_set(value, bit) ((value) |= (1 << (bit)))
#define bit_clean(value, bit) ((value) &= ~(1 << (bit)))
#define bit_get(value, bit) ((value >> bit) & 1)

    int32_t manage_info_init(manage_info_t *self, uint32_t control_word);
    int32_t manage_info_status_word_set(manage_info_t *self, uint32_t status_bit, uint32_t status_value);

    int32_t manage_info_status_word_get(manage_info_t *self, uint32_t *status_word);
    int32_t manage_info_status_bit_get(manage_info_t *self, uint32_t status_bit);

    /******************************************************************************/
    enum app_not_ready_table_bit_pos
    {
        NOT_READY_INIT_BIT_POS = 0,
        NOT_READY_TREATMENT_ROOM_SEARCH_BIT_POS = 1,
        NOT_READY_COVER_DOOR_BIT_POS = 2,
        NOT_READY_DOOR_BIT_POS = 3,
        NOT_READY_MAX_BIT_POS = 31
    };
    enum app_interlock_table_bit_pos
    {
        INTERLOCK_HARDWARE_FAULT_BIT_POS = 0,
        INTERLOCK_SOFTWARE_FAULT_BIT_POS = 1,
        INTERLOCK_SLIP_RING_FAULT_BIT_POS = 2,
        INTERLOCK_GMM_BIT_POS = 3,
        INTERLOCK_PSM_BIT_POS = 4,
        INTERLOCK_FKP_BIT_POS = 5,
        INTERLOCK_CPG_BIT_POS = 6,
        INTERLOCK_EMERGENCY_STOP_BIT_POS = 7,
        INTERLOCK_TREATMENT_ROOM_DOOR_FAULT_BIT_POS = 8,
        INTERLOCK_ROLLER_BIT_POS = 9,
        INTERLOCK_COVER_DOOR_BIT_POS = 10,
        INTERLOCK_HV_EN_BIT_POS = 11,
        INTERLOCK_KV_TREATMENT_EN_BIT_POS = 12,
        INTERLOCK_MV_TREATMENT_EN_BIT_POS = 13,
        INTERLOCK_DOOR_INTERLOCK_BIT_POS = 14,
        INTERLOCK_UPS_POWER_LOW_BIT_POS = 15,
        INTERLOCK_RTC_WD_OK_BIT_POS = 16,
        INTERLOCK_MAX_BIT_POS = 31
    };

    typedef struct app_state_table
    {
        uint32_t rtm_main_state;
        uint32_t psm_state;
        uint32_t gmm_state;
        uint32_t fkp_state;
        uint32_t cpg_state;
        uint32_t rtm_on_state;
        uint32_t plc_state;
        uint32_t dido_state;
        uint32_t data_record;
    } app_state_table_t;

    typedef struct app_interlock_table
    {
        uint32_t not_ready_event;
        uint32_t warning_interlock;
        uint32_t minor_interlock;
        uint32_t serious_interlock;
    } interlock_table_t;

    typedef struct rtm_fault_check
    {
        char fault_table[INTERLOCK_MAX_BIT_POS][STATE_MACHINE_MAX];
        uint32_t cur_time;
        uint32_t last_time;

        interlock_table_t fixed_interlock_table;

        app_state_table_t fixed_app_state_table;
        app_state_table_t dynamic_app_state_table;

        uint32_t interlock_override;
        uint32_t unready_override;
    } rtm_fault_check_t;

#define NO_FAULT (0x00)
#define NOT_READY_EVENT (0x01)
#define WARNING_INTERLOCK (0x02)
#define MINOR_INTERLOCK (0x04)
#define SERIOUS_INTERLOCK (0x08)
    int32_t fault_table_init(rtm_fault_check_t *self);

    int32_t fault_check(rtm_fault_check_t *self, uint8_t state, void *arg);
    void fault_check_init(rtm_fault_check_t *self);
    void fault_clear(rtm_fault_check_t *self);
    int32_t fault_override(rtm_fault_check_t *self);

    int32_t fault_override_set(rtm_fault_check_t *self, uint32_t unready_override, uint32_t interlock_override);
    int32_t fault_interlock_table_get(rtm_fault_check_t *self, interlock_table_t *interlock_table);
#ifdef __cplusplus
}
#endif

#endif
