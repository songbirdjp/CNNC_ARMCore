#ifndef __WS_APP_PROCESS_H__
#define __WS_APP_PROCESS_H__

#include <stdint.h>
#include "websocket.h"
#include "ulog.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LENGTH_ERR  -1
#define CRC_ERR  -2

#define ACTIVE_SEND_ITEM_NUM   2
#define MAX_SEND_BUF_LEN    100

typedef struct {
    uint16_t frmTag;
    uint16_t frmType;
    uint16_t frmLength;
}__attribute__((aligned(1), packed))FRAME_HEAD;

typedef struct {
   uint32_t crc;
}FRAME_END;

typedef struct {
    uint16_t tag;
    uint16_t type;
    osTimerId_t sendTimer;
    uint8_t sendCnt;
    uint8_t packedFrame[MAX_SEND_BUF_LEN];
    uint8_t timerIndex;
 }ACTIVE_SEND_ATTACHED;

 typedef struct
{
  //  uint16_t tag;
  //  uint16_t type;
  //  uint16_t length;
    uint16_t messagetype;   
    uint16_t messageFeedback;
}FEEDBACK_DATA;

void tcp_recv_data_process(APP_DATA_RECV *info);
int16_t getPayload(APP_DATA_RECV* info, uint8_t** pdata);
uint16_t getPayload_without_Check(APP_DATA_RECV* info, uint8_t** pdata);
int8_t packPayload(FRAME_HEAD head, uint8_t *payload, uint8_t *arr);
int8_t packPayload_without_crc(FRAME_HEAD head, uint8_t *payload, uint8_t *arr);
int8_t activeSend(uint8_t index, uint8_t clientType, FRAME_HEAD head, uint8_t *payload, uint32_t timeout);

#ifdef __cplusplus
}
#endif
#endif