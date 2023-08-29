#ifndef F407_W5500_HTTPSERVER_H
#define F407_W5500_HTTPSERVER_H

#include <stddef.h>
#include "main.h"
#include "wizchip_conf.h"
#include "retarget.h"
#include "socket.h"
#include "string.h"
#include "stdbool.h"

#define MAX_HTTPSEND_NUMBERS 64
#define DATA_BUF_SIZE  2048

typedef struct {
    uint8_t gDATABUF[DATA_BUF_SIZE];
    uint16_t Len;
}TCP_DATA_t;

extern TCP_DATA_t recvInfo;
extern TCP_DATA_t RecvByUART;
extern int _numbers[MAX_HTTPSEND_NUMBERS];

void do_tcpc(void);
void do_tcp_server(void);
int parse_http_request(char *request);
void SendHttpResponse_Get(int sn);
void led_on(uint8_t led_num);
void led_off(uint8_t led_num);
void SendHttpResponse_Post(int sn);
void handle_request(char *request);
char *get_form_data(char *request, char *keyword);
int Get_IO_Flag(char *request);
void Switch_Respnse(void);
void handleIOStatusRequest(uint8_t sn);
void http_String2Numbers(char *request);
#endif //F407_W5500_HTTPSERVER_H
