#ifndef __ETHERCAT_H__
#define __ETHERCAT_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cpuluplus
extern "C" {
#endif
typedef enum ethercat_send_index_type
{
    SEND_INDEX_TYPE_6000,
    SEND_INDEX_TYPE_6010,
}ethercat_send_index_type_t;

int8_t ethercat_slave_appl_cb_register(osEventFlagsId_t output_event, uint32_t event_flag, void (*fun_cb)(void));
int8_t ethercat_recv_data_update_with_block(uint32_t timeout);
int8_t ethercat_send_data_update(uint16_t *buf, uint16_t len, ethercat_send_index_type_t index_type);
uint16_t *ethercat_recv_data_get(uint16_t *buf, uint16_t len);
uint16_t *ethercat_send_data_get(uint16_t *buf, uint16_t len, ethercat_send_index_type_t index_type);
int16_t ethercat_state_get(void);
int8_t ethercat_thread_init(void);
#ifdef __cpuluplus
}
#endif


#endif