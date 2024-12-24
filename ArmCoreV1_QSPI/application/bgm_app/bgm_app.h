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

struct bgm_data_info
{
    enum bgm_fsm_state fsm_state;
    enum bgm_fsm_state fsm_state_request;
    enum bgm_fsm_state fsm_state_request_pre;
    enum bgm_fsm_state fsm_state_request_already;
    uint16_t beam_id;
    uint16_t radiation_index;
    uint8_t dose_mode;
    uint8_t pulse_mode;
    uint8_t cali_mode;
    uint8_t cali_prf;
    uint16_t cali_dac;
    uint32_t cali_adc;    
    float dose_meter;   /* cali mode meter value */
    float dose_meter_dummy;
    uint8_t dummy_end_flag;
    osMutexId_t mutex;
};

struct bgm_data_info *bgm_data_info_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __BGM_APP_H__ */