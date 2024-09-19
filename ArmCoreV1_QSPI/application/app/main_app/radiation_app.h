#ifndef __RADIATION_APP_H__
#define __RADIATION_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum pulse_state
{
    ONE_PULSE_START = 0,
    ONE_PULSE_RUNNING,
    ONE_PULSE_COMPLETE,
    ONE_PULSE_TIMEOUT,
    ONE_PULSE_CLEAR,
    ONE_BEAM_COMPLETE,
    ONE_PULSE_COUNT,
    ONE_PULSE_DOSE,
    DOSE_ACCUMULATED,
};

int8_t dose_value_status_set(enum pulse_state state, uint64_t value);
uint64_t dose_value_status_get(enum pulse_state state);

int8_t adcs7476_value_process(void);

#ifdef __cplusplus
}
#endif

#endif /* __RADIATION_APP_H__ */