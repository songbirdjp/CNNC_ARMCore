#ifndef __ADCS7476_PORT_H__
#define __ADCS7476_PORT_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT   "spi4"
#define DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT    "spi2"


int8_t device_adcs7476_init(uint8_t *device_name);
int8_t device_adcs7476_open(uint8_t *device_name);
int8_t device_adcs7476_buffer_init(uint8_t *device_name, uint8_t *buf, uint16_t len);
int8_t device_adcs7476_queue_init(uint8_t *device_name, osMessageQueueId_t queue);
int8_t device_adcs7476_callback_register(uint8_t *device_name, int8_t (*cb)(void *arg));
int8_t device_adcs7476_sample_interval_set(uint16_t sample_interval_10ns);
int8_t device_adcs7476_sample_start(void);

#ifdef __cplusplus
}
#endif


#endif /* __ADCS7476_PORT_H__ */