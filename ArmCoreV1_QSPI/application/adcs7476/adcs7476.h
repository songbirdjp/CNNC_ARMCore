#ifndef __ADCS7476_H__
#define __ADCS7476_H__

#include <stdint.h>
#include "adcs7476_port.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADCS7476_ID1_NAME   DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT
#define ADCS7476_ID2_NAME   DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT

#define BUF_LEN 100   /* sample buffer length, which indicates the number of samples to be read at a time */

int8_t adcs7476_sample_enable(uint8_t enable);
int8_t adcs7476_object_data_read(uint8_t *device_name, uint16_t *data, uint16_t len, uint32_t timeout);
int8_t adcs7476_object_data_limit_set(uint8_t *device_name, uint16_t limit_h, uint16_t limit_l);
int8_t adcs7476_object_data_limit_get(uint8_t *device_name, uint16_t *limit_h, uint16_t *limit_l);
int8_t adcs7476_object_data_limit_fault_get(uint8_t *device_name);
int8_t adcs7476_object_data_callback_register(void (*callback)(void *arg));

#ifdef __cplusplus
}
#endif

#endif /* __ADCS7476_H__ */