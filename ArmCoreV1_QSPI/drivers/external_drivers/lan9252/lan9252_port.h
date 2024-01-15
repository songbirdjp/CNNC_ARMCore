#ifndef __LAN9252_PORT_H__
#define __LAN9252_PORT_H__

#include "stdint.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME_OSPI1_DEFAULT       "octospi1"

#define LAN9252_IRQ_EVENT               (1<<0)
#define LAN9252_SYNC0_IRQ_EVENT         (1<<1)
#define LAN9252_SYNC1_IRQ_EVENT         (1<<2)



int8_t device_lan9252_init(uint8_t *device_name);
int8_t device_lan9252_rx_buffer_init(uint8_t *buf, uint16_t len);
int8_t device_lan9252_rx_queue_init(osMessageQueueId_t queue, int8_t (*cb)(void *arg));

int8_t device_lan9252_data_read(uint16_t address, uint8_t *buf, uint32_t num);
int8_t device_lan9252_data_write(uint16_t address, uint8_t *buf, uint32_t num);
int8_t device_lan9252_sqi_mode_set(uint8_t cmd);
int8_t device_lan9252_sqi_data_read(uint16_t address, uint8_t *buf, uint32_t num);
int8_t device_lan9252_sqi_data_write(uint16_t address, uint8_t *buf, uint32_t num);

int32_t device_lan9252_data_recv_with_block(void);

#ifdef __cplusplus
}
#endif

#endif