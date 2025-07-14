#ifndef __EPS_APP_H__
#define __EPS_APP_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

enum eps_read_write_id
{
    EPS_SOFTWARE_VERSION = 0,
    EPS_RUN_STATUS,
    EPS_VOLTAGE_OUTPUT,
    EPS_CURRENT_OUTPUT,
    EPS_POWER_OUTPUT,
    EPS_FAULT_STOP,
    EPS_FAULT_RESET,
    EPS_FAULT_CURRENT,
    EPS_FAULT_CODE_H,
    EPS_FAULT_CODE_L,
    EPS_FAULT_RECORD_1,
    EPS_FAULT_RECORD_2,
    EPS_FAULT_RECORD_3,
    EPS_FAULT_RECORD_4,
    EPS_FAN_FAULT_ENABLE,
    EPS_COMMUNICATION_STORAGE,
};

struct eps_status
{
    uint16_t software_version;
    uint8_t run_status;
    float voltage_output;
    float current_output;
    float power_output;

    uint8_t fault_stop;
    uint16_t fault_cur;
    uint16_t fault_record[4];
    uint32_t fault_code;

    uint8_t fan_fault_enable;
    uint8_t storage;

    osMutexId_t mutex;
};


#ifdef __cplusplus
}
#endif

#endif /* __EPS_APP_H__ */