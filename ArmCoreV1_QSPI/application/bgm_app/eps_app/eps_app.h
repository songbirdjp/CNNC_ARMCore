#ifndef __EPS_APP_H__
#define __EPS_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum eps_read_write_type
{
    EPS_SOFTWARE_VERSION = 0,
    EPS_EUN_STATUS,
    EPS_VOLTAGE_OUTPUT,
    EPS_CURRENT_OUTPUT,
    EPS_POWER_OUTPUT,
    EPS_STOP_FAULT,
    EPS_RESET_FAULT,
    EPS_FAULT_CURRENT,
    EPS_FAULT_CODE_H,
    EPS_FAULT_CODE_L,
    EPS_FAULT_RECORD_1,
    EPS_FAULT_RECORD_2,
    EPS_FAULT_RECORD_3,
    EPS_FAULT_RECORD_4,
    EPS_FAN_FAULT_ENABLE,
    EPS_STORAGE,
};

struct eps_data
{
    uint8_t software_version;
};


#ifdef __cplusplus
}
#endif

#endif /* __EPS_APP_H__ */