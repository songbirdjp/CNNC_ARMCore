#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "stdint.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DATA_BUF_SIZE  2048

typedef struct {
    uint8_t sn;
    uint8_t gDATABUF[DATA_BUF_SIZE];
    uint16_t Len;
}TCP_DATA_t;


osStatus_t tcp_client_data_recv_get_with_block(TCP_DATA_t *buf, uint32_t timeout);
int32_t tcp_client_data_send(uint8_t s, uint8_t *buf, uint16_t len);
int8_t tcp_establish_cb_register(void (*fun_cb)(void));
int8_t tcp_recv_data_callback_register(void (*fun_cb)(void *arg));

#ifdef __cplusplus
}
#endif

#endif