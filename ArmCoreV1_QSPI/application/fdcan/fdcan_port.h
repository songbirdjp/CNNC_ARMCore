#ifndef __FDCAN_PORT_H__
#define __FDCAN_PORT_H__

#include "drv_fdcan.h"

#ifdef __cplusplus
extern "C" {
#endif


int8_t fdcan1_enable_switch(uint32_t enable);
int8_t fdcan1_data_write(uint32_t id, uint8_t *buf, uint8_t len);
int8_t fdcan1_data_read(struct fdcan_rx_msg *msg, uint32_t timeout);

#ifdef __cplusplus
}
#endif


#endif /* __FDCAN_PORT_H__ */