#include "plan_data.h"

static struct beam_data beam_data_array[BEAMS_MAX] = {0};

static struct beam_data *beam_data_get(uint8_t beam_id)
{
    if (beam_id >= BEAMS_MAX) 
    {
        return NULL;
    }

    return &beam_data_array[beam_id];
}

uint64_t beam_data_value_get(uint8_t beam_id, enum beam_data_state state, uint16_t ri_idx)
{
    struct beam_data *beam_data = beam_data_get(beam_id);

    if (beam_data == NULL) 
    {
        printf("invalid beam_id: %d\n", beam_id);
        return 0;
    }

    uint64_t value = 0;

    osMutexAcquire(beam_data->mutex, osWaitForever);

    switch (state) 
    {
    case BEAM_DOSE_METER:
        value = beam_data->dose_meter;
        break;
    case BEAM_DOSE_RATE:
        value = beam_data->dose_rate;
        break;
    case BEAM_TOTAL_CP:
        value = beam_data->total_cp;
        break;
    case BEAM_TOTAL_RI:
        value = beam_data->total_ri;
        break;
    case BEAM_RI_DOSE_RATE:
        if (ri_idx >= beam_data->total_ri)
        {
            printf("invalid ri_idx: %d\n", ri_idx);
            break;
        }
        value = beam_data->radiation_data[ri_idx].dose_rate;
        break;
    case BEAM_RI_CUMULATIVE:
        if (ri_idx >= beam_data->total_ri)
        {
            printf("invalid ri_idx: %d\n", ri_idx);
            break;
        }
        value = beam_data->radiation_data[ri_idx].dose_cumulative;
        break;
    case BEAM_RI_TIME_EXPECTED:
        if (ri_idx >= beam_data->total_ri)
        {
            printf("invalid ri_idx: %d\n", ri_idx);
            break;
        }
        value = beam_data->radiation_data[ri_idx].time_expected;
        break;
    case BEAM_RI_IN_CP:
        if (ri_idx >= beam_data->total_ri)
        {
            printf("invalid ri_idx: %d\n", ri_idx);
            break;
        }
        for (uint8_t i = 0; i < beam_data->total_cp; i++)
        {
            if (ri_idx <= beam_data->cp_ri_map[i] && ri_idx > beam_data->cp_ri_map[i-1])
            {
                value = i;
                break;
            }
        }
        break;
    case BEAM_RI_IN_CP_MAX:
        if (ri_idx >= beam_data->total_ri)
        {
            printf("invalid ri_idx: %d\n", ri_idx);
            break;
        }
        for (uint8_t i = 0; i < beam_data->total_cp; i++)
        {
            if (ri_idx <= beam_data->cp_ri_map[i] && ri_idx > beam_data->cp_ri_map[i-1])
            {
                value = beam_data->cp_ri_map[i];
                break;
            }
        }
        break;
    default:
        printf("invalid state: %d\n", state);
        break;
    }

    osMutexRelease(beam_data->mutex);

    return value;
}

int8_t beam_data_value_set(uint8_t beam_id, enum beam_data_state state, uint16_t ri_idx, uint64_t value)
{
    struct beam_data *beam_data = beam_data_get(beam_id);

    if (beam_data == NULL) 
    {
        printf("invalid beam_id: %d\n", beam_id);
        return -1;
    }

    int8_t ret = 0;

    osMutexAcquire(beam_data->mutex, osWaitForever);

    switch (state)
    {
    case BEAM_DOSE_METER:
        beam_data->dose_meter = value;
        break;
    case BEAM_DOSE_RATE:
        beam_data->dose_rate = value;
        break;
    case BEAM_TOTAL_CP:
        if (value > CP_RI_MAP_MAX)
        {
            printf("invalid total_cp: %d\n", (uint16_t)value);
            ret = -1;
            break;
        }
        beam_data->total_cp = value;
        break;
    case BEAM_TOTAL_RI:
        if (value > RADIATION_POINT_MAX)
        {
            printf("invalid total_ri: %d\n", (uint16_t)value);
            ret = -1;
            break;
        }
        beam_data->total_ri = value;
        break;
    case BEAM_RI_DOSE_RATE:
        if (ri_idx >= beam_data->total_ri)
        {
            printf("invalid ri_idx: %d\n", ri_idx);
            ret = -1;
            break;
        }
        beam_data->radiation_data[ri_idx].dose_rate = value;
        break;
    case BEAM_RI_CUMULATIVE:
        if (ri_idx >= beam_data->total_ri)
        {
            printf("invalid ri_idx: %d\n", ri_idx);
            ret = -1;
            break;
        }
        beam_data->radiation_data[ri_idx].dose_cumulative = value;
        break;
    case BEAM_RI_TIME_EXPECTED:
        if (ri_idx >= beam_data->total_ri)
        {
            printf("invalid ri_idx: %d\n", ri_idx);
            ret = -1;
            break;
        }
        beam_data->radiation_data[ri_idx].time_expected = value;
        break;
    case BEAM_CP_RI_MAP:
        if (ri_idx >= beam_data->total_cp)
        {
            printf("invalid cp_idx: %d\n", ri_idx);
            ret = -1;
            break;
        }
        beam_data->cp_ri_map[ri_idx] = value;
        break;
    default:
        printf("invalid state: %d\n", state);
        ret = -1;
        break;
    }

    osMutexRelease(beam_data->mutex);

    return ret;
}

int8_t beam_data_pointer_get(uint8_t beam_id, void **ptr)
{
    if (ptr == NULL) 
    {
        return -1;
    }

    *ptr = (void *)beam_data_get(beam_id);

    return 0;
}

int8_t beam_data_cleanup(uint8_t beam_id)
{
    struct beam_data *beam_data = beam_data_get(beam_id);

    if (beam_data == NULL) 
    {
        printf("invalid beam_id: %d\n", beam_id);
        return -1;
    }

    osMutexAcquire(beam_data->mutex, osWaitForever);
    memset(beam_data, 0, sizeof(struct beam_data) - sizeof(osMutexId_t));
    osMutexRelease(beam_data->mutex);

    return 0;
}
