#ifndef __LTC2632_PORT_H__
#define __LTC2632_PORT_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_LTC2632_NAME_DEFAULT    "spi1"


int8_t device_ltc2632_init(uint8_t *device_name);
int8_t device_ltc2632_open(void);
int8_t device_ltc2632_write(uint8_t *buf, uint16_t size, uint32_t timeout);
int8_t device_ltc2632_buffer_init(uint8_t *buf, uint16_t len);
int8_t device_ltc2632_queue_init(osMessageQueueId_t queue);
int8_t device_ltc2632_callback_register(int8_t (*cb)(void *arg));




#ifdef __cplusplus
}
#endif

#endif /* __LTC2632_PORT_H__ */