#ifndef __TCP_CONFIG_PORT_H__
#define __TCP_CONFIG_PORT_H__

#include "stdint.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DATA_BUF_SIZE  2048

typedef struct {
    uint8_t gDATABUF[DATA_BUF_SIZE];
    uint16_t Len;
}TCP_DATA_t;


uint8_t tcp_link_status(void);
int8_t tcp_init(osMessageQueueId_t queue);
int8_t do_tcp_client(uint8_t sn);
uint8_t socket_num_get(void);
uint8_t tcp_link_detect(void);
int8_t tcp_link_state_recover(void);
int8_t tcp_data_recv_with_block(void);

#ifdef __cplusplus
}
#endif

#endif