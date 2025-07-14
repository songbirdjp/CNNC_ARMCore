#ifndef __BGM_APP_H__
#define __BGM_APP_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

enum bgm_fsm_state
{
    BGM_STATE_INIT = 1,
    BGM_STATE_IDLE,
    BGM_STATE_PRELIMINARY,
    BGM_STATE_PREPARE,
    BGM_STATE_READY,
    BGM_STATE_WORK,
    BGM_STATE_PARK = 10,
    BGM_STATE_MANUAL,
    BGM_STATE_COMPLETE,
    BGM_STATE_SHUTDOWN,
    BGM_STATE_POWERSAVER,
    BGM_STATE_TERMINATE,
    BGM_STATE_INTERRUPT,
    BGM_STATE_MAX
};

struct dose_error_info
{
    union
    {
        uint32_t bytes;
        struct
        {
            uint32_t cali_lock : 1;
            uint32_t adc_para : 1;
            uint32_t dac_para : 1;
            uint32_t trigger_interval_min : 1;
            uint32_t dose_mode : 1;
            uint32_t pulse_mode : 1;
            uint32_t prf_para : 1;
            uint32_t dose_meter : 1;
            uint32_t cp_num : 1;
            uint32_t ri_num : 1;
            uint32_t cp_tolerate : 1;
            uint32_t cp_ri_map : 1;
            uint32_t ri_info : 1;
            uint32_t beam_info : 1;
            uint32_t treat_lock : 1;
            uint32_t treat_check : 1;
            uint32_t beam_valid : 1;
            uint32_t interlock_override : 1;
            uint32_t work_run : 1;
            uint32_t timestamp : 1;
            uint32_t reserved : 12;
        };
    }error_code;
};

struct bgm_data_info
{
    enum bgm_fsm_state fsm_state;
    enum bgm_fsm_state fsm_state_request;
    enum bgm_fsm_state fsm_state_request_pre;
    enum bgm_fsm_state fsm_state_request_already;
    uint32_t interlock_override;
    uint32_t unready_override;
    uint16_t beam_id;
    uint16_t radiation_index;
    uint8_t deliver_type;
    uint8_t dose_mode;
    uint8_t pulse_mode;
    uint8_t cali_mode;
    uint8_t cali_prf;
    uint16_t cali_dose1_dac;
    uint16_t cali_dose2_dac;
    uint32_t cali_dose1_adc;
    uint32_t cali_dose2_adc;
    float dose_meter;   /* cali mode meter value */
    float dose_meter_dummy;
    uint8_t dose_fsm_state_flag;
    uint32_t error_code;
    struct dose_error_info error_code_dose1;
    struct dose_error_info error_code_dose2;
    osMutexId_t mutex;
};

struct bgm_data_info *bgm_data_info_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __BGM_APP_H__ */