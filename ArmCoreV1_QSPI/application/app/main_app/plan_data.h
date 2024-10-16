#ifndef __PLAN_DATA_H__
#define __PLAN_DATA_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif


struct radiation_point_data
{
    float dose_cumulative;
    float dose_rate;
    uint16_t time_expected; /* Unit: ms */
};

#define BEAMS_MAX           1
#define CP_RI_MAP_MAX       256
#define RADIATION_POINT_MAX 2048

struct beam_data
{
    float dose_meter;   /* beam dose cumulative */
    float dose_rate;    /* beam dose rate */

    uint16_t total_cp;  /* control point total num */
    uint16_t total_ri;  /* radiation point total num */
    struct radiation_point_data radiation_data[RADIATION_POINT_MAX];
    uint16_t cp_ri_map[CP_RI_MAP_MAX];  /* control point to radiation point map */

    osMutexId_t mutex;
};


enum beam_data_state
{
    BEAM_DOSE_METER = 0,
    BEAM_DOSE_RATE,
    BEAM_TOTAL_CP,
    BEAM_TOTAL_RI,
    BEAM_RI_DOSE_RATE,
    BEAM_RI_CUMULATIVE,
    BEAM_RI_TIME_EXPECTED,
    BEAM_RI_IN_CP,
    BEAM_RI_IN_CP_MAX,
    BEAM_CP_RI_MAP,
};

int8_t beam_data_pointer_get(uint8_t beam_id, void **ptr);
float beam_data_value_get(uint8_t beam_id, enum beam_data_state state, uint16_t ri_idx);
int8_t beam_data_value_set(uint8_t beam_id, enum beam_data_state state, uint16_t ri_idx, float value);
int8_t beam_data_cleanup(uint8_t beam_id);

#ifdef __cplusplus
}
#endif

#endif /* __PLAN_DATA_H__ */