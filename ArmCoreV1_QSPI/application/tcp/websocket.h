#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#include <stdint.h>
#include <stdbool.h>

#define TCP_WEBSOCKET

#define CONTROLLER_USER_AGENT   "TcWebSocket"//TwinCAT webSocket client

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
    int8_t* name;
    uint16_t length;    //unit: Byte
    int32_t controlSignal;   //-1 - send once     0 - stop send     1~2147483647 - send period
    uint8_t sendMode;//send by text:WDT_TXTDATA   send by binary:WDT_BINDATA
    uint8_t assignedClientType;//0 - program  1 - service. In active send, to assign the data receiver
    void* tcpData;
}APP_DATA_SEND;

typedef struct
{
    uint16_t length;    //unit: Byte
    uint8_t sn; //socket number data come from
    uint8_t clientType; //0 - program  1 - service. 
    uint8_t* tcpData;
}APP_DATA_RECV;


int8_t ws_send_data_process(uint8_t s);
int8_t ws_data_process_callback_register(void (*cb)(APP_DATA_RECV* info));
int32_t ws_send(uint8_t s, void *buff, int32_t buffLen, bool fin, bool mask, Ws_DataType type);

#endif //__WEBSOCKET_H__
