#ifndef __ETHERCAT_H__
#define __ETHERCAT_H__

#include "stdint.h"
#include "cmsis_os2.h"

#ifdef __cpuluplus
extern "C" {
#endif


int8_t ethercat_slave_appl_cb_register(osEventFlagsId_t output_event, uint32_t event_flag, void (*fun_cb)(void));
int8_t ethercat_recv_data_update_with_block(uint32_t timeout);
int8_t ethercat_send_data_update(uint16_t *buf, uint16_t len);
uint16_t *ethercat_recv_data_get(uint16_t *buf, uint16_t len);
uint16_t *ethercat_send_data_get(uint16_t *buf, uint16_t len);

#ifdef __cpuluplus
}
#endif


#endif