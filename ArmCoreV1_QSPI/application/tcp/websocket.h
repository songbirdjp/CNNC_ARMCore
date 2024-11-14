#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os2.h"
#define MAX_CLIENT_NUM   2

#define TCP_WEBSOCKET

#define CONTROLLER_AUTHORIZATION   "CONTROLLER"
#define SERVICE_AUTHORIZATION   "SERVICE"

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

typedef enum
{
    TO_SEND = -1,
    STOP_SEND,
} App_SendStatus;

typedef enum
{
    CONTROLLER = 1,
    SERVICE,
    ALL_CLIENTS
} Client_Type;

typedef struct 
{
    int8_t* name;
    uint16_t length;    //unit: Byte
    int32_t controlSignal;   //-1 - send once     0 - stop send     1~2147483647 - send period
    uint8_t sendMode;//send by text:WDT_TXTDATA   send by binary:WDT_BINDATA
    uint8_t assignedClientType;//0 - program  1 - service. In active send, to assign the data receiver
    void* tcpData;
    osMutexId_t sendUpdateMutexHandle;
}APP_DATA_SEND;

typedef struct
{
    uint16_t length;    //unit: Byte
    uint8_t sn; //socket number data come from
    uint8_t recvDataType;
    uint8_t clientType; //0 - program  1 - service. 
    uint8_t* tcpData;
}APP_DATA_RECV;

typedef struct
{
    APP_DATA_SEND * pActiveSend;
    uint16_t sendItemNum;
}SEND_INFO;

typedef struct
{
    int8_t socketNum;
    uint8_t clientType; //0 - controller, data come from program  1 - service, data come from browser. distinguish by IP and PORT
    int32_t connectStatus;// -1 - fail  1 - success
    uint32_t loopCnt;
}CLIENT_INFO;

extern SEND_INFO sendStructInfo;

int32_t ws_send(uint8_t s, void *buff, int32_t buffLen, bool fin, bool mask, Ws_DataType type);
void tcp_server_init(void);
int32_t tcp_send_process(uint8_t s);
bool operateSendMutex(bool opType, uint8_t itemIndex, uint32_t timeout);
bool isSendPeriod(uint8_t sn, uint8_t itemIndex);// to inquire if current loop is sending loop for  a group of period send data
uint8_t isClientTypeMatch(uint8_t sn, uint8_t itemIndex);
int8_t ws_data_process_callback_register(void (*cb)(APP_DATA_RECV *info));
#endif //F407_W5500_HTTPSERVER_H