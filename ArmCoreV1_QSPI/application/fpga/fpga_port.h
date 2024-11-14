#ifndef __FPGA_PORT_H__
#define __FPGA_PORT_H__

#include "stdint.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_SEND_TO_FPGA_NAME_DEFAULT    "spi3"
#define DEVICE_RECV_FROM_FPGA_NAME_DEFAULT  "spi2"


int8_t device_send_to_fpga_init(uint8_t *device_name);
int8_t device_send_to_fpga_open(void);
int8_t device_send_to_fpga_write(uint8_t *buf, uint16_t size, uint32_t timeout);
int8_t device_recv_from_fpga_init(uint8_t *device_name);
int8_t device_recv_from_fpga_buffer_init(uint8_t *buf, uint16_t len);
int8_t device_recv_from_fpga_queue_init(osMessageQueueId_t queue);
int8_t device_recv_from_fpga_callback_register(int8_t (*cb)(void *arg));
int8_t device_recv_from_fpga_open(void);

#ifdef __cplusplus
}
#endif

#endif