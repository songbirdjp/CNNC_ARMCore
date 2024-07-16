#ifndef F407_W5500_HTTPSERVER_H
#define F407_W5500_HTTPSERVER_H

#include <stddef.h>
#include "main.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "string.h"
#include "stdbool.h"

#define MAX_HTTPSEND_NUMBERS 64
#define DATA_BUF_SIZE  2048
#define MAX_CLIENT_NUM   2

typedef enum
{
    WDT_NULL = 0, // 非标准数据包
    WDT_MINDATA,  // 0x0：中间数据包
    WDT_TXTDATA,  // 0x1：txt类型数据包
    WDT_BINDATA,  // 0x2：bin类型数据包
    WDT_DISCONN,  // 0x8：断开连接类型数据包 收到后需手动 close(fd)
    WDT_PING,     // 0x8：ping类型数据包 ws_recv 函数内自动回复pong
    WDT_PONG,     // 0xA：pong类型数据包
} Ws_DataType;

typedef struct
{
    uint32_t Message_Digest[5];
    uint32_t Length_Low;
    uint32_t Length_High;
    uint8_t Message_Block[64];
    int32_t Message_Block_Index;
    int32_t Computed;
    int32_t Corrupted;
} SHA1Context;

typedef struct
{
    int8_t* name;
    uint16_t Length;    //unit: Byte
    uint8_t status;   //send: 1 - send 2 - stop send | recv: 1 - update 2 - keep value, application should set it to 2 after use
    uint16_t sendPeriod;//unit: 100 ms, if this value is 0, it means using status to control when to send, otherwise send periodic
    uint8_t sendMode;//send by text:WDT_TXTDATA   send by binary:WDT_BINDATA
    uint16_t reqFsmState;//Only r/w data in this FSM state
    void *tcpData;
    void (*cbFunc)(void);
}TCP_DATA_ITEMS;

typedef struct
{
    int8_t socketNum;
    uint8_t destIP[4];
    int8_t clientType; //0 - developer, data come from program  1 - service people, data come from browser How to distinguish?
    int32_t connectStatus;// -1 - fail  1 - success
    uint32_t loopCnt;
}CLIENT_INFO;

typedef struct
{
    uint16_t beamIdTest;
    uint32_t pauseIdTest;
    uint32_t gantryTest;
    uint8_t verifyIdTest;
    bool beamEndTest;
    int8_t couchInfoTest[];
}DATA_TEST;

void tcp_app_init(void);
int8_t do_tcp_server_send(uint8_t s);
void tcp_send_process(void);

#endif //F407_W5500_HTTPSERVER_H
