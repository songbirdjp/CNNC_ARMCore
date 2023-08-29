#include "main.h"
//#include "fpga_rw.h"

#define  U16_SIZE DATA_BUF_SIZE/2
#define		CRC_TABLE_SIZE		256
#define  HEAD_LEN_U8   12
#define  HEAD_LEN_U16  HEAD_LEN_U8/2
#define  END_LEN_U16   6
#define FEEDBACK_LEN    4
#define BANK_NO 1 //A=1 B=2

static uint32_t CrcTable[CRC_TABLE_SIZE];
static uint32_t crcCal = 0xffffffff;
static uint16_t dataU16[U16_SIZE - HEAD_LEN_U16 - END_LEN_U16];
static uint16_t feedback[FEEDBACK_LEN];
uint8_t recvBuf[RECV_BUF_LEN],recvBufCpy[RECV_BUF_LEN];

FRAME_HEAD frameHead;
FRAME_END frameEnd;

uint32_t Crc32Buffer(uint32_t crc, uint8_t *buf, uint32_t size)
{
    uint32_t i;
   // uint32_t crc = 0xffffffff;
  //  printf("crc %u \r\n", crc);
    for (i = 0; i < size; i++)
    {
        crc = CrcTable[(crc^buf[i]) & 0xff] ^ (crc >> 8);
     //   printf(" %d ", buf[i]);
    }
    return crc/*^0xffffffff*/;
}

bool InitCrc32Table(void)
{
    uint32_t c;
    uint32_t i, j;

    for (i = 0; i < CRC_TABLE_SIZE; i++)
    {
        c = (uint32_t)i;
        for (j = 0; j < 8; j++)
        {
            if (c & 1)
                c = 0xedb88320 ^ (c >> 1);
            else
                c = c >> 1;
        }
        CrcTable[i] = c;
    }

    return 1;
}

void sendFeedback(uint8_t errno, uint16_t param)
{
    feedback[0] = 101;      //Tag
    feedback[1] = frameHead.frmType;  //frame index
    feedback[2] = 1;        //payload length

    switch(errno)
    {
        case 0: //ok
            feedback[3] = frameHead.packIndexInOneBeam;
            printf("recv success #0!!! \r\n");
            break;
        case 1: //length error
            feedback[3] = 0xff;
            printf("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", param/2, frameHead.frmLength);
            break;
        case 2: //index error
            feedback[3] = 0xff;
            printf("recv error #2: total pack = %d, pack index = %d!!!\r\n", frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);
            break;
        case 3: //bank error
            feedback[3] = 0xff;
            printf("recv error #3: bank no = %d!!! \r\n", frameHead.bankNo);
            break;
        case 4: //crc error
            feedback[3] = 0xff;
            printf("recv error #4: crc error!!! \r\n");
            break;
        default: break;
    }

    send(0, feedback, FEEDBACK_LEN*2);    //返回给服务器
}
#define INDEX 6
void nrtDataMainLoop(void)
{
  //  uint8_t data[sendControl[INDEX].TxLen];
  //  memset(recvBuf, 0xff, RECV_BUF_LEN);
  //  sendtoFPGA(sendControl[INDEX].cmd, data, sendControl[INDEX].TxLen);
  //  FPGA_ReadByteArray(recvBuf, RECV_BUF_LEN);
    //  printf("====read====: ");
    //memcpy(recvBufCpy, recvBuf, RECV_BUF_LEN);
#if 0//cjh del (dma is not opened)
    if(DMAReceiving) {
        for (uint16_t i = 0; i < 8; i++) printf("0x%x ", recvBufCpy[4 + MAX_RECV_PACK_SIZE * i]);
        // for(uint16_t i = 0; i < RECV_BUF_LEN; i++)  printf("0x%x ",recvBuf[i]);
        printf("\r\n");
        DMAReceiving = 0;
    }
#endif
    //printf("recv time: %d\r\n", DMAReceiving);
    //return;

    uint16_t u8LenTotal=0, i;


#if UART_Control
    osMessageQueueGet(networkRecvQueueHandle, &RecvByUART, 0, osWaitForever);//receive data from UART
#else
    osMessageQueueGet(networkRecvQueueHandle, &recvInfo, 0, osWaitForever);//receive data from ethernet
#endif
    if(recvInfo.Len > 0)    u8LenTotal = recvInfo.Len;
    else return;
    //head
    frameHead.frmTag = (recvInfo.gDATABUF[1] << 8) + recvInfo.gDATABUF[0];
    frameHead.frmType = (recvInfo.gDATABUF[3] << 8) + recvInfo.gDATABUF[2];
    frameHead.frmLength = (recvInfo.gDATABUF[5] << 8) + recvInfo.gDATABUF[4];
    frameHead.totalPackInOneBeam = (recvInfo.gDATABUF[7] << 8) + recvInfo.gDATABUF[6];
    frameHead.bankNo = (recvInfo.gDATABUF[9] << 8) + recvInfo.gDATABUF[8];
    frameHead.packIndexInOneBeam = (recvInfo.gDATABUF[11] << 8) + recvInfo.gDATABUF[10];
    //printf("head: %d %d %d %d %d\r\n", frameHead.frmTag, frameHead.frmType,frameHead.frmLength,
     //   frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);

    if(frameHead.frmLength != (u8LenTotal/2 - 12))
    {
        sendFeedback(1, u8LenTotal);
        return;
    }

    if((frameHead.packIndexInOneBeam > frameHead.totalPackInOneBeam)||(frameHead.packIndexInOneBeam < 1))
    {
        sendFeedback(2, 0);
        return;
    }

    if(frameHead.bankNo != BANK_NO)
    {
        sendFeedback(3, 0);
        return;
    }

    uint16_t rdIndex = 0;
    memset(dataU16, 0, sizeof(dataU16));
    for(i = HEAD_LEN_U8; i < (frameHead.frmLength*2+HEAD_LEN_U8); i+=2)
    {
        dataU16[rdIndex] = (recvInfo.gDATABUF[i+1] << 8) + recvInfo.gDATABUF[i];
     //   if(frameHead.packIndexInOneBeam == 2) printf(" dataU16[%d] = %d\r\n", rdIndex, dataU16[rdIndex]);
        rdIndex++;
    }
    if(frameHead.packIndexInOneBeam == 1)
    {
      //  printf("This is first pack %d %d!!!\r\n", frameHead.frmLength, dataU16[0]);
        crcCal = 0xffffffff;
        crcCal = Crc32Buffer(crcCal, dataU16, frameHead.frmLength*2);
    }
    else
    {
        crcCal = Crc32Buffer(crcCal, &dataU16[2], (frameHead.frmLength-2)*2);
    }
   // printf("calculate crc: %u\r\n", crcCal);
    uint16_t last = u8LenTotal - 1;
    uint16_t end[4];
  //  printf("end:");
    for(i = 0; i < END_LEN_U16; i++)
    {
        end[i] = (recvInfo.gDATABUF[last] << 8) + recvInfo.gDATABUF[last-1];
        last -= 2;
     //   printf(" %d ",end[i]);
    }
   // printf("\r\n");
    for(i = 0; i < 4; i++)    frameEnd.CPLimitPos[i] = end[END_LEN_U16 - i];
    frameEnd.crcHigh = end[1];
    frameEnd.crcLow = end[0];

    if(frameHead.packIndexInOneBeam == frameHead.totalPackInOneBeam) //the last pack in one beam
    {
        crcCal ^= 0xffffffff;
        uint32_t crcInData = (frameEnd.crcHigh<<16) + frameEnd.crcLow;
       // printf("recv crc: %u calculate crc: %u\r\n", crcInData, crcCal);
        if(crcInData != crcCal){
            sendFeedback(4, 0);
            return;
        }
    }
    sendFeedback(0, 0); //ok

    memset(&recvInfo, 0, sizeof(recvInfo));
    memset(&frameHead, 0, sizeof(frameHead));
    memset(&frameEnd, 0, sizeof(frameEnd));
}