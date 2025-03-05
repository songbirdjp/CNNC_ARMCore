#ifndef __TCP_TASKS_H__
#define __TCP_TASKS_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IS_TCP_SERVER
#define MAX_CLIENT_NUM  8
#define DATA_BUF_SIZE   2048

typedef struct {
    uint8_t sn;
    uint8_t gDATABUF[DATA_BUF_SIZE];
    uint16_t Len;
}TCP_DATA_t;

#ifdef IS_TCP_SERVER
typedef struct
{
    int8_t socketNum;
    uint8_t clientType; //0 - controller, data come from program  1 - service, data come from browser. distinguish by IP and PORT
    int32_t connectStatus;// -1 - fail  1 - success
    uint32_t loopCnt;
}CLIENT_INFO;

extern CLIENT_INFO client[MAX_CLIENT_NUM];
#endif

osStatus_t tcp_data_recv_get_with_block(TCP_DATA_t *buf, uint32_t timeout);
int32_t tcp_data_send(uint8_t s, uint8_t *buf, uint16_t len);
int8_t tcp_establish_cb_register(void (*fun_cb)(uint8_t sn));
int8_t tcp_recv_data_callback_register(void (*fun_cb)(void *arg));
uint8_t tcp_socket_state_get(uint8_t sn); //the state defined in w5500.h
void clearClientInfo(uint8_t s);

#ifdef __cplusplus
}
#endif

#endif