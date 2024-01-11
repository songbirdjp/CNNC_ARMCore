#ifndef __ETHERCAT_H__
#define __ETHERCAT_H__

#include "stdint.h"

#ifdef __cpuluplus
extern "C" {
#endif



#define DATA_PROCESS_LAN_EVENT    (1<<1)


int8_t ethercat_slave_appl_cb_register(void (*fun_cb)(void));
int8_t ethercat_recv_data_update(void);
int8_t ethercat_send_data_update(uint16_t *buf);
uint16_t *ethercat_recv_data_get(uint16_t *buf);
uint16_t *ethercat_send_data_get(uint16_t *buf);

int8_t ethercat_thread_init(void);

#ifdef __cpuluplus
}
#endif


#endif