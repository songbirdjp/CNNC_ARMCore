#ifndef __W5500_CONFIG_PORT_H__
#define __W5500_CONFIG_PORT_H__

#include "wizchip_conf.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

int8_t device_w5500_init(wiz_NetInfo *net_info);
int8_t device_w5500_rx_buffer_init(uint8_t *buf, uint16_t len);
int8_t device_w5500_rx_queue_init(osMessageQueueId_t queue);
int8_t device_w5500_interrupt_init(uint8_t sn);
uint8_t device_w5500_phy_link_status_get(void);
int8_t device_w5500_link_state_recover(uint8_t sn);
int8_t device_w5500_data_recv_with_block(void);

#ifdef __cplusplus
}
#endif

#endif