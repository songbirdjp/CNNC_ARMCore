#ifndef W5500H7_NONREALTIMEDATAPROCESS_H
#define W5500H7_NONREALTIMEDATAPROCESS_H

#include "cmsis_os.h"

#define MAX_RECV_PACK_SIZE 173
#define RECV_BUF_LEN    MAX_RECV_PACK_SIZE*8

typedef struct {
    uint16_t frmTag;
    uint16_t frmType;
    uint16_t frmLength;
    uint16_t totalPackInOneBeam;
    uint16_t bankNo;
    uint16_t packIndexInOneBeam;
}FRAME_HEAD;

typedef struct {
    uint16_t CPLimitPos[4];
    uint16_t crcHigh;
    uint16_t crcLow;
}FRAME_END;

extern uint8_t recvBuf[RECV_BUF_LEN],recvBufCpy[RECV_BUF_LEN];
extern osMessageQueueId_t networkRecvQueueHandle;

void nrtDataMainLoop(void);
bool InitCrc32Table(void);

#endif //W5500H7_NONREALTIMEDATAPROCESS_H
