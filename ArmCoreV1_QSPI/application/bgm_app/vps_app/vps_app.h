#ifndef __VPS_APP_H__
#define __VPS_APP_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif


enum vps_read_write_id
{
    VPS_SOFTWARE_VERSION = 0,
    VPS_RUN_STATUS,
    VPS_VOLTAGE_OUTPUT,
    VPS_CURRENT_OUTPUT,
    VPS_POWER_VOLTAGE,
    VPS_BUTTON_LOCK,
    VPS_FAULT_STOP,
    VPS_FAULT_RESET,
    VPS_FAULT_CURRENT,
    VPS_FAULT_CODE_H,
    VPS_FAULT_CODE_L,
    VPS_FAULT_RECORD_1,
    VPS_FAULT_RECORD_2,
    VPS_FAULT_RECORD_3,
    VPS_FAULT_RECORD_4,
    VPS_FIRE_COUNT,
    VPS_FIRE_COUNT_UPLIMIT,
    VPS_FIRE_STOP_TIME,
    VPS_REMOTE_MODE,
    VPS_START_MODE,
    VPS_REMOTE_START_ENABLE,
};

struct vps_status
{
    uint16_t software_version;
    uint8_t button_lock;
    uint8_t run_status;
    float voltage_output;
    float current_output;
    float power_voltage;

    uint8_t fault_stop;
    uint16_t fault_cur;
    uint16_t fault_record[4];
    uint32_t fault_code;

    uint16_t fire_count;
    uint16_t fire_count_uplimit;
    uint16_t fire_stop_time;

    osMutexId_t mutex;
};

struct vps_status *vps_state_get(struct vps_status *buf);

#ifdef __cplusplus
}
#endif

#endif /* __VPS_APP_H__ */