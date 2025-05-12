#ifndef __DOSE_APP_H__
#define __DOSE_APP_H__

#include "bgm_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

enum dose_fsm_state
{
    DOSE_FSM_STATE_INIT = 1,
    DOSE_FSM_STATE_IDLE,
    DOSE_FSM_STATE_PRELIMINARY,
    DOSE_FSM_STATE_PREPARE,
    DOSE_FSM_STATE_READY,
    DOSE_FSM_STATE_WORK,
    DOSE_FSM_STATE_PARK = 10,
    DOSE_FSM_STATE_MANUAL,
    DOSE_FSM_STATE_COMPLETE,
    DOSE_FSM_STATE_SHUTDOWN,
    DOSE_FSM_STATE_POWERSAVER,
    DOSE_FSM_STATE_TERMINATE,
    DOSE_FSM_STATE_INTERRUPT,
    DOSE_FSM_STATE_PRELIMINARY_BEGIN = 30,
    DOSE_FSM_STATE_MAX
};

enum dose_info_index
{
    DOSE_INFO_VERSION = 0,
    DOSE_INFO_FSM_STATE_SET,
    DOSE_INFO_FSM_STATE_GET,
    DOSE_INFO_ADC_CALI,
    DOSE_INFO_DAC_CALI,
    DOSE_INFO_BEAM_SET,
    DOSE_INFO_BEAM_GET,
    DOSE_INFO_METER_SET,
    DOSE_INFO_METER_GET,
    DOSE_INFO_DOSE_RATE_SET,
    DOSE_INFO_DOSE_RATE_GET,
    DOSE_INFO_PRF_SET,
    DOSE_INFO_PRF_GET,
    DOSE_INFO_GENERATE_MODE_SET,
    DOSE_INFO_GENERATE_MODE_GET,
    DOSE_INFO_PULSE_MODE_SET,
    DOSE_INFO_PULSE_MODE_GET,
    DOSE_INFO_RADIATION_SET,
    DOSE_INFO_RADIATION_GET,
    DOSE_INFO_INTERLOCK_SET,
    DOSE_INFO_INTERLOCK_GET,
    DOSE_INFO_CUMULATED_CLEAR,
    DOSE_INFO_RADIATION_INDEX_GET,
    DOSE_INFO_MAX
};

float dose_data_info_get(enum uart_id id, enum dose_info_index index, void *data);  /* get dose data info from local sram */
int8_t dose_data_info_set(enum uart_id id, enum dose_info_index index, void *data, uint16_t len);   /* set or get dose data info vai uart cmd */

#ifdef __cplusplus
}
#endif

#endif /* __DOSE_APP_H__ */