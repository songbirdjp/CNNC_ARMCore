#ifndef __TCP_TASKS_H__
#define __TCP_TASKS_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TCP_CLIENT_NAME     "CLIENT"
#define TCP_SHELL_NAME      "SHELL"
#define TCP_SERVICE_NAME    "SERVICE"
#define TCP_CONTROLLER_NAME "CONTROLLER"

#define MAX_SOCKET_NUM  8
#define DATA_BUF_SIZE   2048

struct tcp_data
{
    uint8_t sn;
    uint8_t buf[DATA_BUF_SIZE];
    uint16_t len;
};

osStatus_t tcp_data_recv_get_with_block(struct tcp_data *buf, uint32_t timeout);
int32_t tcp_data_send(uint8_t s, uint8_t *buf, uint16_t len);
int8_t tcp_establish_cb_register(uint8_t *name, int8_t (*fun_cb)(uint8_t sn));
int8_t tcp_recv_data_callback_register(void (*fun_cb)(void *arg));
uint8_t tcp_socket_state_get(uint8_t sn); //the state defined in w5500.h

int8_t socket_server_info_set_by_ws(uint8_t s, uint8_t *name, uint8_t connection, uint8_t multi_enable);
uint8_t *socket_name_get_by_sn(uint8_t sn);
int8_t socket_connect_event_cb(uint8_t sn, uint8_t connect_status);

#ifdef __cplusplus
}
#endif

#endif
