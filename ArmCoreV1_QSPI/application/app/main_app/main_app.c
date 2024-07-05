#include "main_app.h"
#include "fmc_sdram_port.h"
#include "fpga_rw.h"
#include "tcp_client.h"
#include "main.h"
#include "init_call.h"

#define PARAM_SETTING_TAG 1
#define PLAN_DATA_SETTING_TAG 2

#define	 CRC_TABLE_SIZE		256

#ifdef BANKA
#define BANK_NO 1 //A=1 B=2
#else
#define BANK_NO 2 //A=1 B=2
#endif

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

typedef struct {
    uint16_t fsmState;
    uint16_t planCmd;
    uint8_t totalBeam; // < 30
    uint16_t totalRIInBeam[MAX_BEAM_NUM];   // total RI in one beam, < 4096
    uint32_t oneBeamSize[MAX_BEAM_NUM];
    uint16_t beamIndex;
    uint16_t radiationIndex;
    uint16_t faultInfo1;
    uint16_t faultInfo2;
    uint16_t rtPosUpload[RT_ARM_UPLOAD_POS_LEN/2];
}RT_BEAM_DATA;

typedef struct {
    uint16_t versionARM;
    uint16_t versionFPGA;
    uint16_t leafNcarInterlock[83];
   // uint16_t carrierInterlock;
    uint16_t jawInterlock[2];
    uint16_t powerInterlock;
    uint16_t fanInterlock;
    uint16_t boardLoss;
    uint16_t armStatus;
    uint16_t FPGAStatus;
}INTERLOCK_FEEDBACK;

typedef struct {
    uint16_t bankNo;
    uint16_t packIndexInOneBeam;
    uint16_t errorCode;
    uint16_t leafSecondPos[82];
    uint16_t carrierSecondPos;   // total RI in one beam, < 2048
    uint16_t jawSecondPos[2];
}SECOND_POS_FEEDBACK;

typedef struct {
    uint8_t carrierPosMaxL;//[MAX_CP_IN_BEAM];
    uint8_t carrierPosMaxH;
    uint8_t carrierPosMinL;//[MAX_CP_IN_BEAM];
    uint8_t carrierPosMinH;
    uint16_t carrierPos;//[MAX_CP_IN_BEAM];
    uint16_t pausePos;
}CARRIER_POS;

typedef struct {
    uint16_t ri;
    float speed;
    uint8_t retCode;
}CARRIER_PERIOD_INFO;

enum serverFsmStates {FSM_NOSTATE,FSM_INIT,FSM_IDLE,FSM_PREPARE,FSM_READY,FSM_SERVO,FSM_MANUAL,FSM_FAULT,FSM_SHUTDOWN};
enum planCommand {NO_USE,SEND_PLAN,CLOSE_PLAN};
enum carrierPosType {MAX,MIN};

//#define ringb_is_empty(q) (q->head == q->tail)
//#define ringb_is_full(q) (((q->tail+1)%q->size) == q->head )

static uint32_t CrcTable[CRC_TABLE_SIZE];
static uint32_t crcCal = 0xffffffff;
static uint16_t lastPackIndex = 0;
static __IO uint8_t* pSDRAM;
static __IO uint8_t* pSDRAMCAL;
RT_BEAM_DATA rtBeamData;
INTERLOCK_FEEDBACK interlockFeedback;
SECOND_POS_FEEDBACK secondPosFeedback;
//static bool calCarrierFlag = 0;
static FRAME_HEAD frameHead;
static FRAME_END frameEnd;

CARRIER_POS carrierPosCal;

bool InitCrc32Table(void);
void sendParamtoFPGA(void);
void calCarrierTrajectory(void);

uint16_t *feedback = NULL, feedback16Len;

void TCPFeedbackInit(void)
{
    feedback16Len = (3+sizeof(interlockFeedback)/2) + (3+sizeof(secondPosFeedback)/2);
    feedback = (uint16_t *)pvPortMalloc(feedback16Len*2);
    if (feedback != NULL)
    {
        memset(feedback, 0, feedback16Len*2);
    }
    else
    {
        printf("%s file malloc err(line:%d)\r\n", __FILE__, __LINE__);
    }    
}

uint16_t beam_cmd_get(void)
{
    return rtBeamData.planCmd;
}


void nrtInit(void)
{
    InitCrc32Table();
    pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    pSDRAMCAL = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
#if 1
    for(uint8_t i = 0; i < TOTAL_FPGA_CMD_NUM; i++)
    {
        sndCtrl.singleSize[i] = sendCmd[i].TxLen+6;
        if(sendCmd[i].useAsParam)  sndCtrl.totalSize += sndCtrl.singleSize[i];//calculate parameter buf size
    }
    sndCtrl.paramSendBuf = (uint8_t*)pvPortMalloc(sndCtrl.totalSize);
    memset(sndCtrl.paramSendBuf, 0, sndCtrl.totalSize);
   // for(uint16_t i=0; i < sndCtrl.totalSize; i++) printf("0x%x ", sndCtrl.paramSendBuf[i]);
#endif
    secondPosFeedback.bankNo = BANK_NO;
}

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

void sendFeedback(void)
{
    uint16_t typeLen = 0, *pFDAry = feedback;

   // if(rtBeamData.planCmd == SEND_PLAN) printf("%d ",secondPosFeedback.packIndexInOneBeam );
    *pFDAry++ = 102;
    *pFDAry++ = 2;
    typeLen = sizeof(secondPosFeedback)/2;
    *pFDAry++ = typeLen;        //payload length
  //  *pFDAry++ = secondPosFeedback.bankNo;
   // *pFDAry++ = secondPosFeedback.packIndexInOneBeam;
 //   *pFDAry++ = secondPosFeedback.errorCode;
  //  memset(pFDAry, 1, (typeLen-3)*2);
    memcpy(pFDAry, &secondPosFeedback, typeLen*2);
    pFDAry += typeLen;

    *pFDAry++ = 101;
    *pFDAry++ = 2;  //upload:2
    typeLen = sizeof(interlockFeedback)/2;
    *pFDAry++ = typeLen;        //payload length
    memcpy(pFDAry, &interlockFeedback, typeLen*2);
   // memset(pFDAry, 1, typeLen*2);
  //  printf("pack index %d\r\n", secondPosFeedback.packIndexInOneBeam);

    tcp_client_data_send((uint8_t *)feedback, feedback16Len*2);
}

void ntrRecvParamAndPlan(TCP_DATA_t* info)
{
    uint16_t u8LenTotal = 0, i;
    uint32_t crcInData;
    uint16_t last, payloadLength;

    if (info->Len > 0) u8LenTotal = info->Len;
    else return;

    //head
    frameHead.frmTag = (info->gDATABUF[1] << 8) + info->gDATABUF[0];
    frameHead.frmType = (info->gDATABUF[3] << 8) + info->gDATABUF[2];
    frameHead.frmLength = (info->gDATABUF[5] << 8) + info->gDATABUF[4];
    frameHead.bankNo = (info->gDATABUF[7] << 8) + info->gDATABUF[6];
//    printf("head1: %d %d %d %d\r\n", frameHead.frmTag, frameHead.frmType,frameHead.frmLength,frameHead.bankNo);

    if(frameHead.frmTag == PLAN_DATA_SETTING_TAG)
    {
        frameHead.totalPackInOneBeam = (info->gDATABUF[9] << 8) + info->gDATABUF[8];
        frameHead.packIndexInOneBeam = (info->gDATABUF[11] << 8) + info->gDATABUF[10];
      //  printf("head2: %d %d\r\n",frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);

        if(frameHead.packIndexInOneBeam == 1)   lastPackIndex = 0;
        payloadLength = u8LenTotal - 20;
        if (frameHead.frmLength != payloadLength)
        {
            secondPosFeedback.errorCode = 0xf1;
            printf("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
            return;
        }

        if(frameHead.packIndexInOneBeam > lastPackIndex)    lastPackIndex = frameHead.packIndexInOneBeam;
        else{
            secondPosFeedback.errorCode = 0xf5;
            printf("recv error #5: last index= %d index = %d!!! \r\n", frameHead.packIndexInOneBeam, lastPackIndex);
            frameHead.packIndexInOneBeam = lastPackIndex;
            return;
        }

        if ((frameHead.packIndexInOneBeam > frameHead.totalPackInOneBeam) || (frameHead.packIndexInOneBeam < 1)) {
            secondPosFeedback.errorCode = 0xf2;
            printf("recv error #2: total pack = %d, pack index = %d!!!\r\n",
                   frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);
            return;
        }

        if (frameHead.bankNo != BANK_NO) {
            secondPosFeedback.errorCode = 0xf3;
            printf("recv error #3: bank no = %d!!! \r\n", frameHead.bankNo);
            return;
        }
        uint16_t saveLength,sdramLength;
        uint8_t *pBeamData = pSDRAM;
        if (frameHead.packIndexInOneBeam == 1)
        {
          //  uint8_t temp[frameHead.frmLength];
            printf("recv plan %dB-> first pack of beam!\r\n", u8LenTotal);
            saveLength = frameHead.frmLength+4;
            sdramLength = saveLength;
            for(i=0; i<saveLength; i++)
            {
                *pBeamData++ = info->gDATABUF[12+i];
                if((i -5)%RT_SAVE_PAYLOAD_LEN == 0){//0 + leaf pos*80 + 0, for FPGA need
                    *pBeamData++ = 0;
                    *pBeamData++ = 0;
                    sdramLength += 2;
                }
                else if((i -165)%RT_SAVE_PAYLOAD_LEN == 0){//0 + leaf pos*80 + 0, for FPGA need
                    *pBeamData++ = 0;
                    *pBeamData++ = 0;
                    sdramLength += 2;
                }
              //  printf("%x ", info->gDATABUF[12+i]);
            }
         //   for(i = 12; i < frameHead.frmLength; i++)  printf("0x%x ", info->gDATABUF[i]);
            crcCal = 0xffffffff;
            crcCal = Crc32Buffer(crcCal, &info->gDATABUF[12], saveLength);//ok
        }
        else {
            //  uint8_t temp[frameHead.frmLength - 4];
            saveLength = frameHead.frmLength;
            sdramLength = saveLength;
            for (i = 0; i < saveLength; i++) {
                *pBeamData++ = info->gDATABUF[16 + i];
                if ((i - 1) % RT_SAVE_PAYLOAD_LEN == 0) {//0 + leaf pos*80 + 0, for FPGA need
                    *pBeamData++ = 0;
                    *pBeamData++ = 0;
                    sdramLength += 2;
                } else if ((i - 161) % RT_SAVE_PAYLOAD_LEN == 0) {//0 + leaf pos*80 + 0, for FPGA need
                    *pBeamData++ = 0;
                    *pBeamData++ = 0;
                    sdramLength += 2;
                }
            }

            crcCal = Crc32Buffer(crcCal, &info->gDATABUF[16], saveLength);//ok
        }
        pSDRAM += sdramLength;

        if (frameHead.packIndexInOneBeam == frameHead.totalPackInOneBeam) //the last pack in one beam
        {
           // printf("%d ", info->gDATABUF[12+i]);
            lastPackIndex = 0;
            last = u8LenTotal - 1;
            frameEnd.crcHigh = (info->gDATABUF[last - 2] << 8) + info->gDATABUF[last - 3];
            frameEnd.crcLow = (info->gDATABUF[last] << 8) + info->gDATABUF[last - 1];
            crcCal ^= 0xffffffff;
            crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;

           // printf("recv crc: %u calculate crc: %u\r\n", crcInData, crcCal);
            if (crcInData != crcCal) {
                secondPosFeedback.errorCode = 0xf4;
                printf("recv error #4: crcInData: %u crcCal: %u!!! \r\n", crcInData, crcCal);
                return;
            }

            //record each beam info
            // 1 beam in SDRAM: totalRI + BeamIndex + (RI1 + RI2 + ...+RI(totalRI)), sizeof(RI) = RT_PAYLOAD_LEN
            rtBeamData.totalBeam = (info->gDATABUF[15] << 8) + info->gDATABUF[14];//current beam index
            rtBeamData.totalRIInBeam[rtBeamData.totalBeam] = (info->gDATABUF[13] << 8) + info->gDATABUF[12];
            rtBeamData.oneBeamSize[rtBeamData.totalBeam] =
                    4 + RT_SDRAM_PAYLOAD_LEN
                    * rtBeamData.totalRIInBeam[rtBeamData.totalBeam];
            if (rtBeamData.totalBeam >= MAX_BEAM_NUM) printf("Warn: beam > 30, will not be sent to FPGA!\r\n");
            printf("Beam %d transfer finish, size %ld, total RI %d\r\n",
                   rtBeamData.totalBeam, rtBeamData.oneBeamSize[rtBeamData.totalBeam], rtBeamData.totalRIInBeam[rtBeamData.totalBeam]);
            calCarrierTrajectory();
#if 0
            pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
            pBeamData = pSDRAM + 6;
            sndCtrl.pCrt = sndCtrl.paramSendBuf;
            makeSingleSendAry(24, pBeamData, RT_DOWNLOAD_PAYLOAD_LEN, 1);

            // for(uint8_t i = 0; i <sndCtrl.singleSize[24]; i++)  printf("%x ",sndCtrl.paramSendBuf[i]);
          //   printf("\r\n");

            FPGA_WriteByteArray(sndCtrl.paramSendBuf, sndCtrl.singleSize[24]);
#endif
        }
        secondPosFeedback.packIndexInOneBeam = frameHead.packIndexInOneBeam;
        secondPosFeedback.errorCode = 0; //ok
    //    printf("recv success #0!!! \r\n");
    }
    else if(frameHead.frmTag == PARAM_SETTING_TAG)
    {
        printf("recv parameter %d bytes ", u8LenTotal);
        if (frameHead.frmLength != (u8LenTotal - 10)) {
            secondPosFeedback.errorCode = 0xf1;
            printf("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
            return;
        }
        if (frameHead.bankNo != BANK_NO) {
            secondPosFeedback.errorCode = 0xf3;
            printf("recv error #3: bank no = %d!!! \r\n", frameHead.bankNo);
            return;
        }
        crcCal = 0xffffffff;
        crcCal = Crc32Buffer(crcCal, &info->gDATABUF[6], frameHead.frmLength);
        crcCal ^= 0xffffffff;
        last = u8LenTotal - 1;
        frameEnd.crcHigh = (info->gDATABUF[last-2] << 8) + info->gDATABUF[last - 3];
        frameEnd.crcLow = (info->gDATABUF[last] << 8) + info->gDATABUF[last - 1];
        crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;
        if (crcInData != crcCal)
        {
            secondPosFeedback.errorCode = 0xf4;
            printf("recv error #4: crc error: %u %u!!! \r\n", crcInData, crcCal);
            return;
        }
        // makeParamSendAry(info->gDATABUF);
      //  for(uint16_t i=0; i < sndCtrl.totalSize; i++) printf("0x%x ", sndCtrl.paramSendBuf[i]);
        // sendParamtoFPGA();
       // printf("size %d\r\n", sndCtrl.totalSize);
        printf("crc:%u\r\n", crcInData);
        
        make_para_for_fpga(info->gDATABUF);

    }

    memset(&info, 0, sizeof(info));
    memset(&frameHead, 0, sizeof(frameHead));
    memset(&frameEnd, 0, sizeof(frameEnd));
}

void sendParamtoFPGA(void)
{
  //  printf("size: %d\r\n", sndCtrl.totalSize);
  //  for(uint16_t i = 0; i<sndCtrl.totalSize; i++ )  printf("0x%x ", sndCtrl.paramSendBuf[i]);
    FPGA_WriteByteArray(sndCtrl.paramSendBuf, sndCtrl.totalSize);
}

void sendCPtoFPGA(uint16_t beamIndex, uint16_t RIIndex)
{
    uint8_t *pBeamData;
    uint8_t send_buf[174] = {0};

    pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    pBeamData = pSDRAM;

   // rtBeamData.totalBeam = 2;
  //  rtBeamData.totalRIInBeam[1] = 0x704;
    if((beamIndex <= 0) || (beamIndex > rtBeamData.totalBeam) || (RIIndex <= 0) || (RIIndex > rtBeamData.totalRIInBeam[beamIndex]))
    {
        printf("Error: Invalid beam/RI index %d,%d,%d,%d\r\n",
               rtBeamData.totalBeam,rtBeamData.totalRIInBeam[beamIndex],beamIndex,RIIndex);
        return;
    }
   // printf("BEAM%d.RI%d\r\n", beamIndex, RIIndex);

    for(uint8_t i = 1; i < beamIndex; i++)
    {
      //  if(rtBeamData.oneBeamSize[i] == 0)  return; //beamIndex >= 2
        pBeamData += rtBeamData.oneBeamSize[i];
      //  printf("beam %u size %u\r\n", i, rtBeamData.oneBeamSize[i]);
    }   //skip front beams
  //  printf("%d %d %d %d %d %d\r\n", *pBeamData,*(pBeamData+1),*(pBeamData+2),*(pBeamData+3),*(pBeamData+4),*(pBeamData+5));
    pBeamData += 4; //skip current beam head (total RI + beam index)

    pBeamData += RT_SDRAM_PAYLOAD_LEN*(RIIndex - 1);//skip front RIs
    pBeamData += 2; //skip current RI head (ControlPoint index)
#ifdef TEST
    rtBeamData.faultInfo2 = 0;
    for(UINT8 i = 0; i < RT_ARM_UPLOAD_POS_LEN; i+=2){
        rtBeamData.rtPosUpload[i/2] = (pBeamData[i+1] <<8) + pBeamData[i];
        //printf("**** 0x%x 0x%x 0x%x====", recvBuf[i], recvBuf[i+1], rtDataUpload[i/2]);
        //  rtDataUpload[i/2] = 1;
    }
    memcpy(secondPosFeedback.leafSecondPos,  rtBeamData.rtPosUpload, 82*2);
    secondPosFeedback.carrierSecondPos = rtBeamData.rtPosUpload[82];
    memcpy(secondPosFeedback.jawSecondPos,  &rtBeamData.rtPosUpload[83], 2*2);

  /*  makeSingleSendAry(24, pBeamData, 166, 1,1);
    pBeamData += 166;
    pBeamData += 4;//skip X/Y Jaw pos
    makeSingleSendAry(24, pBeamData, 8, 0,1);//CP limit pos
    for(uint8_t i = 0; i < sndCtrl.singleSize[24]; i++)  printf("%x ",sndCtrl.cmdSendBuf[i]);
    printf("\r\n");*/
#else
  //  makeSingleSendAry(24, pBeamData, RT_DOWNLOAD_PAYLOAD_LEN, 1);
    // makeSingleSendAry(24, pBeamData, 166, 1,1);
    memcpy(send_buf, pBeamData, 166);
    pBeamData += 166;
    pBeamData += 4;//skip X/Y Jaw pos
    // makeSingleSendAry(24, pBeamData, 8, 0,1);//CP limit pos
   // for(uint8_t i = 0; i < sndCtrl.singleSize[24]; i++)  printf("%x ",sndCtrl.cmdSendBuf[i]);
   // printf("\r\n");

    // FPGA_WriteByteArray(sndCtrl.cmdSendBuf, MAX_CMD_DATA_SIZE);

    memcpy(&send_buf[166], pBeamData, 8);

    make_cmd_to_fpga(24, send_buf, 174);

#endif
}

#define ENCODE_CNT_PER_MM   325
#define ENCODE_LEAF_RANGE   (75*ENCODE_CNT_PER_MM)//24375,leaf start from 10mm, range is 75mm
#define MM_CARRIER_40P_ORIG 13.5
#define ENCODE_CARRIER_40P_ORIG (uint16_t)(MM_CARRIER_40P_ORIG*ENCODE_CNT_PER_MM) //4387
#define MM_LEAF_40P_ORIG (MM_CARRIER_40P_ORIG+10)//23.5
#define ENCODE_LEAF_40P_ORIG  (uint16_t)(MM_LEAF_40P_ORIG*ENCODE_CNT_PER_MM)//7637
#define ENCODE_LEAF_CARRIER_MIN_DIST    10*ENCODE_CNT_PER_MM//3250
#define ENCODE_LEAF_MAX_POS (85*ENCODE_CNT_PER_MM)    //27625

void calMaxMinPos(uint16_t *pos)
{
    uint8_t i;
    uint16_t max,min,carMin,carMax;

    max = min = pos[0];
    for (i = 1; i < 80; i++) {
        if (max < pos[i]) max = pos[i];
        if (min > pos[i]) min = pos[i];
    }

    int32_t possibleMin = max - ENCODE_LEAF_MAX_POS;
    if(possibleMin >= ENCODE_CARRIER_40P_ORIG)   carMin = possibleMin;//unit:mm
    else  carMin = ENCODE_CARRIER_40P_ORIG;
    int32_t possibleMax = min - ENCODE_LEAF_CARRIER_MIN_DIST;
    if(possibleMax >= ENCODE_CARRIER_40P_ORIG)   carMax = possibleMax;//unit:mm
    else  carMax = ENCODE_CARRIER_40P_ORIG;
    carrierPosCal.carrierPosMinL = carMin&0x00ff;
    carrierPosCal.carrierPosMinH = (carMin&0xff00) >> 8;
    carrierPosCal.carrierPosMaxL = carMax&0x00ff;
    carrierPosCal.carrierPosMaxH = (carMax&0xff00) >> 8;

    //  printf("%u %d ", max, possibleMin);
    //  printf("c(Min:%u Max:%u)\r\n", carMin, carMax);
}

uint16_t rdCarrierPosFromSDRAM(uint16_t ri, bool type) //1: min pos 0: max pos
{
    uint16_t pos;
    pSDRAMCAL = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    uint8_t *pBeamData = pSDRAMCAL;
    for(uint8_t i = 1; i < rtBeamData.totalBeam; i++)
    {
        pBeamData += rtBeamData.oneBeamSize[i];
    }   //skip front beams

    pBeamData += 4;//totalRI + Beam index
    pBeamData += RT_SDRAM_PAYLOAD_LEN*(ri - 1);//nRI

    if(type)  pBeamData += 168; //skip RI + 82 leaf pos + carrier target pos
    else pBeamData += 170;//skip RI + 81 leaf pos = 82*2B
    pos = (*pBeamData++ << 8) + *pBeamData;

    return pos;
}

void wrCarrierPos2SDRAM(uint16_t ri, uint16_t pos)
{
    pSDRAMCAL = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    uint8_t *pBeamData = pSDRAMCAL;
    for(uint8_t i = 1; i < rtBeamData.totalBeam; i++)
    {
        pBeamData += rtBeamData.oneBeamSize[i];
    }   //skip front beams

   // printf("sd %d %u\r\n",ri, pos);
    pBeamData += 4;//totalRI + Beam index
    pBeamData += RT_SDRAM_PAYLOAD_LEN*(ri - 1);//nRI
    pBeamData += 166;//skip RI + 82 leaf pos = 83*2B
    *pBeamData++ = pos&0x00ff;
    *pBeamData = (pos&0xff00) >> 8;
}
#if 1
void calEachCarrierPosBtw(uint16_t changeSpeedRI, uint16_t farmostRI,int8_t dir)
{
    //  printf("==%d\r\n", i);
    float_t startPos,middlePos;
    float_t speed = 0;
    uint16_t middlePosMin, middlePosMax, middlePos0,i;

    if (dir > 0) {
        startPos = (float_t)rdCarrierPosFromSDRAM(changeSpeedRI, MAX);
        middlePos = (float_t) rdCarrierPosFromSDRAM(farmostRI, MIN);//calculate each pos between start and farmost loop
        speed = (middlePos - startPos) / (float_t) (farmostRI - changeSpeedRI);
    } else if (dir < 0) {
        startPos = (float_t)rdCarrierPosFromSDRAM(changeSpeedRI, MIN);
        middlePos = (float_t) rdCarrierPosFromSDRAM(farmostRI, MAX);
        speed = (startPos - middlePos) / (float_t) (farmostRI - changeSpeedRI);
    }

    //  printf("%d - %d: %f %f %f\r\n", changeSpeedRI,farmostRI,speed,startPos,middlePos);
    for (i = (changeSpeedRI + 1); i <= farmostRI; i++) {
        startPos += dir * speed;
        middlePos0 = (uint16_t) (startPos + 0.5 * dir);//rounding off
        middlePosMin = rdCarrierPosFromSDRAM(i, MIN);
        middlePosMax = rdCarrierPosFromSDRAM(i, MAX);
        if ((middlePos0 < middlePosMin) || (middlePos0 > middlePosMax)) {
            printf("skip %d %u\r\n", i, middlePos0);//skip the speed not pass all previous periods
            break;
        }
        wrCarrierPos2SDRAM(i,middlePos0);
        // printf("middle pos %d:%u\r\n", i, middlePos0);
        //  printf("fast %f carrierPosCal.carrierPos[%d]:%u\r\n", speed, i, carrierPosCal.carrierPos[i]);
    }
}

CARRIER_PERIOD_INFO findConstraintPos(uint16_t changeSpeedRI, float_t farmostSpeed, uint16_t farmostRI,int8_t dir)
{
    uint16_t /*calRI = MAX_CP_IN_BEAM - changeSpeedRI,*/ minSpeedRI = 0,i,carrierPos;
  //  uint16_t totalRI = rtBeamData.totalRIInBeam[rtBeamData.totalBeam];
    float_t speedArry[farmostRI];//speedArry[calRI + 1];
    float_t startPos, middlePos;
    CARRIER_PERIOD_INFO slow;

  //  printf("start ri %d puase ri %d\r\n", changeSpeedRI,farmostRI);
 //   for(i = changeSpeedRI; i <= totalRI; i++)
    for(i = changeSpeedRI; i <= farmostRI; i++)
    {
        if(i == changeSpeedRI)
        {
            if(dir > 0) startPos = (float_t)rdCarrierPosFromSDRAM(i, MAX);//constraint
            else if(dir < 0) startPos = (float_t)rdCarrierPosFromSDRAM(i, MIN);//constraint

          //  minSpeedRI = 1 + changeSpeedRI;
            speedArry[0] = 0;
            continue;
        }
        if(dir > 0){
            middlePos = (float_t)rdCarrierPosFromSDRAM(i, MAX);
            speedArry[i] = (middlePos - startPos)/(float_t)(i - changeSpeedRI);//constraint
        }
        else if(dir < 0){
            middlePos = (float_t)rdCarrierPosFromSDRAM(i, MIN);
            speedArry[i] = (startPos - middlePos)/(float_t)(i - changeSpeedRI);//constraint
        }
     //   printf("speed %d: %f\r\n", i, speedArry[i]);
        if(speedArry[i] < -1e-6){
            slow.ri = i;
            slow.retCode = 1;

            return slow;
        }
        if(( farmostSpeed - speedArry[i] ) > 1e-6)
        {
            minSpeedRI = i;
            for( i = (changeSpeedRI+1); i <= minSpeedRI; i++)//calculate each pos between start and slowest loop
            {
                startPos += dir*speedArry[minSpeedRI];
                carrierPos = (uint16_t)(startPos+0.5*dir);
                wrCarrierPos2SDRAM(i,carrierPos);
                printf("slow %f carrierPos[%d]:%u\r\n", speedArry[minSpeedRI],i, carrierPos);
            }

            slow.ri = minSpeedRI;
            slow.retCode = 2;
            return slow;
        }
    }

    slow.retCode = 3;
    return slow;
}

CARRIER_PERIOD_INFO findFarmostPos(uint16_t startRI, uint16_t pauseRI, int8_t dir)
{
    uint16_t i;
   // uint16_t totalRI = rtBeamData.totalRIInBeam[rtBeamData.totalBeam],i;
    float_t startPosF, middlePosF;
    float_t speedArry[pauseRI]/*, speedMax*/;
    CARRIER_PERIOD_INFO fastest;

    for(i = startRI; i <= pauseRI; i++)  //find out the fastest period, and its speed should satisfy all previous period
    {
        if(i == startRI)
        {
            if(dir > 0){
                startPosF = (float_t)rdCarrierPosFromSDRAM(i, MAX);
                printf("startPosF: %f\r\n", startPosF);
            }
            else if(dir < 0){
                startPosF = (float_t)rdCarrierPosFromSDRAM(i, MIN);
                printf("startPosF: %f\r\n", startPosF);
            }

          //  speedMax = 0;
          //  maxSpeedRI = 1 + startRI;
            fastest.speed = 0;
            fastest.ri = 1 + startRI;
            speedArry[0] = 0;
            continue;
        }

        if(dir > 0){
            middlePosF = (float_t)rdCarrierPosFromSDRAM(i, MIN);
            speedArry[i - startRI] = (middlePosF - startPosF)/(float_t)(i - startRI);//should reach
        }
        else if(dir < 0){
            middlePosF = (float_t)rdCarrierPosFromSDRAM(i, MAX);
            speedArry[i - startRI] = (startPosF - middlePosF)/(float_t)(i - startRI);//should reach
        }

        if(( speedArry[i - startRI] - fastest.speed ) > 1e-6)//find the farmost period in this direction
        {
            fastest.speed = speedArry[i - startRI];
            fastest.ri = i;
            printf("max speed %d: %f\r\n", fastest.ri, fastest.speed);
          //  if(dir > 0) startPosF = (float_t)rdCarrierPosFromSDRAM(startRI, MAX);
           // else if(dir < 0) startPosF = (float_t)rdCarrierPosFromSDRAM(startRI, MIN);
        }
    }

    return fastest;
}

uint16_t calculateOneMovement(uint16_t startRI, int8_t dir)//calculate carrier pos from start to pause
{
    uint8_t state = 1;
    CARRIER_PERIOD_INFO maxSpeedPeriod, constraintPeriod;
    uint16_t totalRI = rtBeamData.totalRIInBeam[rtBeamData.totalBeam];
	//printf("startRI %d\r\n", startRI);
    constraintPeriod.ri = startRI;
    while(1)
    {
        switch(state)
        {
            case 1:
				printf("step 1 ");
                maxSpeedPeriod = findFarmostPos(startRI, totalRI, dir);
                state = 2;
                break;
            case 2:
				printf("step 2 ");
                constraintPeriod =
                        findConstraintPos(constraintPeriod.ri, maxSpeedPeriod.speed,maxSpeedPeriod.ri, dir);
                state = constraintPeriod.retCode;
                if(state == 1)  totalRI = constraintPeriod.ri;
                break;
            case 3:
				printf("step 3 ");
                calEachCarrierPosBtw(constraintPeriod.ri, maxSpeedPeriod.ri,dir);
                printf("maxSpeed %d: %f\r\n", maxSpeedPeriod.ri, maxSpeedPeriod.speed);
                return maxSpeedPeriod.ri;
            default:break;
        }
    }
}

uint16_t findDirection(uint16_t startRI)
{
    uint16_t intersectMin, minInterRI, intersectMax, maxInterRI,pauseRI,i;
    uint16_t posMin, posMax, carrierPos;
    uint16_t totalRI = rtBeamData.totalRIInBeam[rtBeamData.totalBeam];

    intersectMin =  rdCarrierPosFromSDRAM(startRI, MIN);
    intersectMax =  rdCarrierPosFromSDRAM(startRI, MAX);
    printf("total ri %d start ri %d %u %u\r\n", totalRI, startRI, intersectMin, intersectMax);
    for(uint16_t ri = startRI; ri <= totalRI; ri++) {
        //the first stage - find out the carrier prepare pos and fix move direction
        // we need to calculate public intersection, choose intersection edge as carrier prepare pos
        //printf("%d ", ri);
        posMin =  rdCarrierPosFromSDRAM(ri, MIN);
        posMax =  rdCarrierPosFromSDRAM(ri, MAX);
        if (intersectMax < posMin)
        {
            //determine the init pos for the first stage, choose right edge which is closest
            //  carrierInitPos = intersectMax;
            for (i = startRI; i <= maxInterRI; i++){
                wrCarrierPos2SDRAM(i, intersectMax);
            }
            printf("Forward out-of-range ri %d min pos %u, start ri %d init pos %u\r\n", ri, posMin, maxInterRI, intersectMax);
            pauseRI = calculateOneMovement(maxInterRI, 1);//move forward, calculate next movement
            carrierPosCal.pausePos = rdCarrierPosFromSDRAM(pauseRI, MIN);
            printf("pause ri %d\r\n", pauseRI);
            break;
//                carrierNextStartPos = carrierPosCal.carrierPosMin[ri];
//                    nextStartRI = ri;
//                    printf("Forward find the first no-inter point: %d(%u,%u) init pos %u next pos %u\r\n",
//                           ri,carrierPosCal.carrierPosMin[ri],carrierPosCal.carrierPosMax[ri],carrierInitPos,carrierNextStartPos);
        } else if (intersectMin > posMax) {
            //choose left edge
            //  carrierInitPos = intersectMin;
            for (i = startRI; i <= minInterRI; i++){
                wrCarrierPos2SDRAM(i,intersectMin);
            }
            printf("Backward out-of-range ri %d max pos %u, start ri %d init pos %u\r\n", ri, posMax, minInterRI, intersectMin);
            pauseRI = calculateOneMovement(minInterRI, -1);//move backward
            carrierPosCal.pausePos = rdCarrierPosFromSDRAM(pauseRI, MAX);
            printf("pause ri %d\r\n", pauseRI);
            break;
        }else //calculate carrier pos intersection for each RI
        {
            if (intersectMin < posMin) {
                intersectMin = posMin;
                minInterRI = ri;
            }

            if (intersectMax > posMax) {
                intersectMax = posMax;
                maxInterRI = ri;
            }
          //  printf("%d (%u, %u)\r\n", ri, intersectMin, intersectMax);

            if (ri == totalRI) { //can't find no-intersection period
                pauseRI = totalRI;
                for (i = startRI; i <= totalRI; i++){
                    if(carrierPosCal.pausePos == 0) carrierPosCal.pausePos = intersectMin;
                    wrCarrierPos2SDRAM(i,carrierPosCal.pausePos);
                    // printf("carrierPos[%d]:%u\r\n",i,carrierPosCal.pausePos);
                }
                printf("can't find no-intersection period - prepare pos:%u\r\n", carrierPosCal.pausePos);
            }
        }
    }

    return pauseRI;
}
#endif

void calCarrierTrajectory(void)
{
    pSDRAMCAL = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    uint8_t *pBeamData = pSDRAMCAL;
    uint16_t leafPos[80];
    uint16_t startRI, pauseRI;
    uint16_t totalRI = rtBeamData.totalRIInBeam[rtBeamData.totalBeam];
  //  uint16_t point[MAX_CP_IN_BEAM]={7761, 14378,21133,28037,35100,42333,49748,57362};
    memset(&carrierPosCal, 0, sizeof(carrierPosCal));
    for(uint8_t i = 1; i < rtBeamData.totalBeam; i++)
    {
        pBeamData += rtBeamData.oneBeamSize[i];
    }   //skip front beams
  //  printf("####%x %x#####\r\n", pBeamData[0],pBeamData[1]);
    pBeamData += 4; //totalRI + Beam index
  //  for(uint16_t ri = 1; ri <= MAX_CP_IN_BEAM; ri++)
    for(uint16_t ri = 1; ri <= totalRI; ri++)
    {
        pBeamData += 4;//RI + 1st insert leaf
        memcpy(leafPos, pBeamData, 160); //copy out 80 leaf pos in RI
      //  for(i = 0; i < 80; i++) printf("%d ", leafPos[i]);
      //  printf("####%d###\r\n",ri);
//        for (i = 0; i < 80; i++) {
//            leafPos[i] = point[ri];
//            if (leafPos[i] < ENCODE_LEAF_40P_ORIG) {
//                printf("Invalid leaf pos %d %d %u\r\n", ri, i, leafPos[i]);
//                return;
//            }
  //      }
     //   printf("#%d ",ri);
        calMaxMinPos(leafPos);//calculate carrier pos range
#if 1
        pBeamData += 164;//80 leaf pos + second insert leaf + carrier target pos
        *pBeamData++ = carrierPosCal.carrierPosMinH;//write carrier min to JAWX
        *pBeamData++ = carrierPosCal.carrierPosMinL;
        *pBeamData++ = carrierPosCal.carrierPosMaxH;//write carrier max to JAWY
        *pBeamData++ = carrierPosCal.carrierPosMaxL;
        pBeamData += 8;//CP lim pos(8B)
         //  printf("[min:%u max:%u]\r\n",rdCarrierPosFromSDRAM(ri, MIN),rdCarrierPosFromSDRAM(ri, MAX));
#else
        pBeamData += RT_SAVE_PAYLOAD_LEN;
#endif
    }
#if 1
    pauseRI = 1;
    do{
        pauseRI = findDirection(pauseRI);
    }
    while(pauseRI != totalRI);

   // for(i = 1; i <=totalRI; i++) wrCarrierPos2SDRAM(i);
    printf("carrier calculate finish!\r\n");
#endif
}
//uint8_t aaaa[RT_DOWNLOAD_PAYLOAD_LEN];
//uint8_t a;
static bool calCarrierFlag0 = 1;
void nrtDataMainLoop(void)
{
  //  for(uint8_t i = 0; i < RT_DOWNLOAD_PAYLOAD_LEN; i++)    aaaa[i] = i;
   // sndCtrl.pCrt = sndCtrl.paramSendBuf;
 //   makeSingleSendAry(24, aaaa, RT_DOWNLOAD_PAYLOAD_LEN, 1);
 //   FPGA_WriteByteArray(sndCtrl.cmdSendBuf, sndCtrl.singleSize[24]);
   // FPGA_WriteByteArray(sndCtrl.paramSendBuf, sndCtrl.singleSize[24]);
#if 0
       if(calCarrierFlag0){
           rtBeamData.totalBeam = 1;
           rtBeamData.totalRIInBeam[rtBeamData.totalBeam] = 1501;
           calCarrierTrajectory();
           calCarrierFlag0 = 0;
       }

       else{
           rtBeamData.totalBeam = 1;
           rtBeamData.totalRIInBeam[1] = 0x704;
           sendCPtoFPGA(1, 1320);
       }
#endif
    if(rtBeamData.fsmState == FSM_IDLE)
    {
        if(rtBeamData.planCmd == CLOSE_PLAN)
        {
            uint8_t *pBeamData;
         //   uint32_t crc;

            printf("Clear plan data!!!\r\n");
            pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
            pBeamData = pSDRAM;
            for(uint8_t i = 1; i <= rtBeamData.totalBeam; i++){
                memset(pBeamData, 0, rtBeamData.oneBeamSize[i]);
              //  crc = 0xffffffff;
              //  crc = Crc32Buffer(crc, pBeamData, rtBeamData.oneBeamSize[i]);
             //   crc ^= 0xffffffff;
               // printf("%d sdram crc %u \r\n", rtBeamData.oneBeamSize[i], crc);
                pBeamData += rtBeamData.oneBeamSize[i];
            }

            rtBeamData.totalBeam = 0;
            pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
            memset(rtBeamData.totalRIInBeam, 0, MAX_BEAM_NUM);
            memset(rtBeamData.oneBeamSize, 0, MAX_BEAM_NUM);
        }
    /*    if(sndCtrl.paramSendBuf != NULL)
        {
            vPortFree(sndCtrl.paramSendBuf);
            sndCtrl.paramSendBuf = 0;
        }*/
    }
}



/********************************************************************************************/
#include "ethercat.h"

#define DATA_PROCESS_FPGA_EVENT   (1<<0)
#define DATA_PROCESS_LAN_EVENT    (1<<1)
#define DATA_PROCESS_TCP_EVENT    (1<<2)
static osEventFlagsId_t data_process_eventHandle = NULL;

static int8_t non_realtime_fpga_data_process(uint8_t *recvBuf)
{
    uint32_t checkSum = 0;

    for (uint16_t i = 2; i < (RECV_BUF_LEN - 1); i++) 
    {
        checkSum += recvBuf[i];
    }

    if (((uint8_t)checkSum != recvBuf[RECV_BUF_LEN - 1]) || (checkSum == 0))
    {
        printf("Checksum err 0x%x 0x%x 0x%x 0x%x 0x%x\r\n", recvBuf[0],recvBuf[1],recvBuf[2],recvBuf[3],recvBuf[4]);
        return -1;
    }
    else
    {
        uint8_t i;
        if (recvBuf[4] == PACKF0_CMD) 
        {

            for (i = 0; i < (RT_FPGA_UPLOAD_PAYLOAD_LEN - 2); i += 2) 
            {   //RT 0 - 165 ：82 leaf and carrier pos
                rtBeamData.rtPosUpload[i/2] = (recvBuf[i + FPGA_RT_UPLOAD_START] << 8) + recvBuf[i + FPGA_RT_UPLOAD_START + 1];
            }
            rtBeamData.faultInfo1 = recvBuf[FPGA_RT_UPLOAD_START+166];//RT 166
            rtBeamData.faultInfo2 = recvBuf[FPGA_RT_UPLOAD_START+167];//RT 167

            for (i = 0; i < 164; i += 2) 
            {  //NRT 0 - 163 ：82 leaf second pos
                secondPosFeedback.leafSecondPos[i/2] = (recvBuf[i + FPGA_NRT_UPLOAD_START] << 8) + recvBuf[i + FPGA_NRT_UPLOAD_START + 1];                            
            }
            interlockFeedback.boardLoss = (recvBuf[FPGA_NRT_UPLOAD_START + 166] & 0xc0) >> 6;//NRT 166 bit6-7
            interlockFeedback.FPGAStatus = (recvBuf[FPGA_NRT_UPLOAD_START + 166] << 8) + recvBuf[FPGA_NRT_UPLOAD_START + 167]; //NRT 166-167
                    
        }
        else if (recvBuf[4] == PACKF1_CMD) 
        {

            for (i = 0; i < (RT_FPGA_UPLOAD_PAYLOAD_LEN - 2); i += 2) 
            {   //RT 0 - 165 ：82 leaf and carrier pos
                rtBeamData.rtPosUpload[i/2] = (recvBuf[i + FPGA_RT_UPLOAD_START] << 8) + recvBuf[i + FPGA_RT_UPLOAD_START + 1];
            }
            rtBeamData.faultInfo1 = recvBuf[FPGA_RT_UPLOAD_START+166];//RT 166
            rtBeamData.faultInfo2 = recvBuf[FPGA_RT_UPLOAD_START+167];//RT 167

            for (i = 0; i < (NRT_FPGA_UPLOAD_PAYLOAD_LEN - 2); i += 2) 
            {//NRT 0 - 165 ：82 leaf and carrier interlock
               interlockFeedback.leafNcarInterlock[i/2] = (recvBuf[i + FPGA_NRT_UPLOAD_START] << 8) + recvBuf[i + FPGA_NRT_UPLOAD_START + 1];
            }
            interlockFeedback.versionFPGA = (recvBuf[FPGA_NRT_UPLOAD_START + 166] << 8) + recvBuf[FPGA_NRT_UPLOAD_START + 167];//NRT 166-167
                    
        }
    }

    return 0;
}

#include "lan9252_app.h"
static int8_t realtime_ethercat_data_process(void)
{
    static uint16_t oldState = 0, oldPlanCmd = 0, oldRadiationIndex = 0, oldBeamIndex = 0;

    TOBJ7010 recv_data = {0};
    TOBJ6000 send_data = {0};

    TOBJ7010 *recv = (TOBJ7010 *)ethercat_recv_data_get((uint16_t *)&recv_data, sizeof(recv_data));
    TOBJ6000 *send = (TOBJ6000 *)ethercat_send_data_get((uint16_t *)&send_data, sizeof(send_data));
    if (recv == NULL || send == NULL)
    {
        printf("ethercat data get failed\r\n");
        return -1;
    }

    #define INFO_OUT_OFFSET     1
    #define DATA_OUT1_OFFSET    9

    #define INFO_IN_OFFSET     1
    #define DATA_IN1_OFFSET    9
    #define DATA_IN2_OFFSET    17

    uint16_t *InfoOutPTR = ((uint16_t *)&recv)[INFO_OUT_OFFSET];
    uint16_t *DataOut1PTR = ((uint16_t *)&recv)[DATA_OUT1_OFFSET];
    uint16_t *InfoIn1PTR = ((uint16_t *)&send)[INFO_IN_OFFSET];    //echo
    uint16_t *DataIn1PTR = ((uint16_t *)&send)[DATA_IN1_OFFSET];   //fault info, use 2
    uint16_t *DataIn2PTR = ((uint16_t *)&send)[DATA_IN2_OFFSET];   //leaf pos 1-8

    // uint32_t checkSum = 0;

    // for (uint16_t i = 2; i < (RECV_BUF_LEN - 1); i++) 
    // {
    //     checkSum += recvBuf[i];
    // }

    // if (((uint8_t)checkSum != recvBuf[RECV_BUF_LEN - 1]) || (checkSum == 0))
    // {
    //     printf("Checksum err 0x%x 0x%x 0x%x 0x%x 0x%x\r\n", recvBuf[0],recvBuf[1],recvBuf[2],recvBuf[3],recvBuf[4]);
    // }
    // else
    // {
    //     uint8_t i;
    //     if (recvBuf[4] == PACKF0_CMD) 
    //     {

    //         for (i = 0; i < (RT_FPGA_UPLOAD_PAYLOAD_LEN - 2); i += 2) 
    //         {   //RT 0 - 165 ：82 leaf and carrier pos
    //             rtBeamData.rtPosUpload[i/2] = (recvBuf[i + FPGA_RT_UPLOAD_START] << 8) + recvBuf[i + FPGA_RT_UPLOAD_START + 1];
    //         }
    //         rtBeamData.faultInfo1 = recvBuf[FPGA_RT_UPLOAD_START+166];//RT 166
    //         rtBeamData.faultInfo2 = recvBuf[FPGA_RT_UPLOAD_START+167];//RT 167

    //         for (i = 0; i < 164; i += 2) 
    //         {  //NRT 0 - 163 ：82 leaf second pos
    //             secondPosFeedback.leafSecondPos[i/2] = (recvBuf[i + FPGA_NRT_UPLOAD_START] << 8) + recvBuf[i + FPGA_NRT_UPLOAD_START + 1];                            
    //         }
    //         interlockFeedback.boardLoss = (recvBuf[FPGA_NRT_UPLOAD_START + 166] & 0xc0) >> 6;//NRT 166 bit6-7
    //         interlockFeedback.FPGAStatus = (recvBuf[FPGA_NRT_UPLOAD_START + 166] << 8) + recvBuf[FPGA_NRT_UPLOAD_START + 167]; //NRT 166-167
                    
    //     }
    //     else if (recvBuf[4] == PACKF1_CMD) 
    //     {

    //         for (i = 0; i < (RT_FPGA_UPLOAD_PAYLOAD_LEN - 2); i += 2) 
    //         {   //RT 0 - 165 ：82 leaf and carrier pos
    //             rtBeamData.rtPosUpload[i/2] = (recvBuf[i + FPGA_RT_UPLOAD_START] << 8) + recvBuf[i + FPGA_RT_UPLOAD_START + 1];
    //         }
    //         rtBeamData.faultInfo1 = recvBuf[FPGA_RT_UPLOAD_START+166];//RT 166
    //         rtBeamData.faultInfo2 = recvBuf[FPGA_RT_UPLOAD_START+167];//RT 167

    //         for (i = 0; i < (NRT_FPGA_UPLOAD_PAYLOAD_LEN - 2); i += 2) 
    //         {//NRT 0 - 165 ：82 leaf and carrier interlock
    //            interlockFeedback.leafNcarInterlock[i/2] = (recvBuf[i + FPGA_NRT_UPLOAD_START] << 8) + recvBuf[i + FPGA_NRT_UPLOAD_START + 1];
    //         }
    //         interlockFeedback.versionFPGA = (recvBuf[FPGA_NRT_UPLOAD_START + 166] << 8) + recvBuf[FPGA_NRT_UPLOAD_START + 167];//NRT 166-167
                    
    //     }
    // }

    InfoIn1PTR[0] = rtBeamData.faultInfo1&0x000f;
    memcpy(&InfoIn1PTR[1], &InfoOutPTR[1], sizeof(uint16_t) * 7);         //rt upload， echo

    DataIn1PTR[0] = DataOut1PTR[0];
    DataIn1PTR[1] = rtBeamData.faultInfo1&0x00f0;
    if(interlockFeedback.boardLoss&0x0007)  DataIn1PTR[1] |= 0x0002;
    DataIn1PTR[2] = rtBeamData.faultInfo2&0x00ff;

    memcpy(DataIn2PTR, rtBeamData.rtPosUpload, sizeof(uint16_t) * (8 * 10 + 3));

    rtBeamData.fsmState = InfoOutPTR[0];   //save rt cmd
    rtBeamData.beamIndex = InfoOutPTR[1];
    rtBeamData.radiationIndex = InfoOutPTR[3];
    rtBeamData.planCmd = DataOut1PTR[0];

    if(oldState != rtBeamData.fsmState){
        printf("fsm state: %d -> %d\r\n",oldState,rtBeamData.fsmState);
        uint8_t newState = rtBeamData.fsmState;
        // makeSingleSendAry(25, &newState, 1, 1,1);//0x50
        make_cmd_to_fpga(25, &newState, 1);
        // FPGA_WriteByteArray(sndCtrl.cmdSendBuf, sndCtrl.singleSize[25]);
        oldState = rtBeamData.fsmState;
    }
    if(oldPlanCmd != rtBeamData.planCmd){
        printf("plan cmd: %d -> %d\r\n",oldPlanCmd,rtBeamData.planCmd);
        if(rtBeamData.planCmd == SEND_PLAN){//Plan send start
            oldBeamIndex = oldRadiationIndex = 0;
        }
        if((rtBeamData.planCmd == NO_USE)&&(oldPlanCmd == SEND_PLAN)){ //Plan send finish
            secondPosFeedback.packIndexInOneBeam = 0;
            secondPosFeedback.errorCode = 0;
        }
        oldPlanCmd = rtBeamData.planCmd;
    }
    if(((rtBeamData.fsmState == FSM_IDLE)||(rtBeamData.fsmState == FSM_SERVO))&&(rtBeamData.beamIndex > 0))
    {
        if(oldBeamIndex != rtBeamData.beamIndex){
          //  printf("RI: %d.%d -> %d.%d\r\n",oldBeamIndex,oldRadiationIndex,rtBeamData.beamIndex,rtBeamData.radiationIndex);
            sendCPtoFPGA(rtBeamData.beamIndex, rtBeamData.radiationIndex);
            oldRadiationIndex = rtBeamData.radiationIndex;
            oldBeamIndex = rtBeamData.beamIndex;
        }
        else if(oldRadiationIndex != rtBeamData.radiationIndex){
          //  printf("RI: %d.%d -> %d.%d\r\n",oldBeamIndex,oldRadiationIndex,rtBeamData.beamIndex,rtBeamData.radiationIndex);
            sendCPtoFPGA(rtBeamData.beamIndex, rtBeamData.radiationIndex);
            oldRadiationIndex = rtBeamData.radiationIndex;
            oldBeamIndex = rtBeamData.beamIndex;
        }
    }


    return ethercat_send_data_update(send, sizeof(send_data));
}

static int8_t non_realtime_tcp_callback(void)
{
    if(beam_cmd_get() == NO_USE)
    {
        sendFeedback();
    }

    return 0;
}

static int8_t non_realtime_tcp_recv_data_callback(void)
{
    osEventFlagsSet(data_process_eventHandle, DATA_PROCESS_TCP_EVENT);
    return 0;
}

static int8_t non_realtime_fpga_recv_data_callback(void)
{
    osEventFlagsSet(data_process_eventHandle, DATA_PROCESS_FPGA_EVENT);
    return 0;
}

static int8_t data_process_init(void)
{
    int8_t ret = 0;
    ret = tcp_establish_cb_register(non_realtime_tcp_callback);
    if (ret != 0)
    {
        printf("tcp callback register err:%d\r\n", ret);
        return ret;
    }

    ret = tcp_recv_data_callback_register(non_realtime_tcp_recv_data_callback);
    if (ret != 0)
    {
        printf("tcp recv data callback register err:%d\r\n", ret);
        return ret;
    }

    ret = ethercat_slave_appl_cb_register(data_process_eventHandle, DATA_PROCESS_LAN_EVENT, realtime_ethercat_data_process);
    if (ret != 0)
    {
        printf("ethercat callback register err:%d\r\n", ret);
        return ret;
    }

    ret = recv_from_fpga_callback_register(non_realtime_fpga_recv_data_callback);
    if (ret != 0)
    {
        printf("recv fpga callback register err:%d\r\n", ret);
        return ret;
    }

    return 0;
}

static void DataProccessTask(void *argument)
{
  /* USER CODE BEGIN DataProccessTask */
    nrtInit();
    /* Infinite loop */
   // osDelay(500);
    for(;;)
    {
        nrtDataMainLoop();
        osDelay(100);
    }
  /* USER CODE END DataProccessTask */
}
static void data_process_entry(void *argument)
{
  /* USER CODE BEGIN data_process_entry */
  /* Infinite loop */
  osStatus_t stat = 0;
  uint32_t event_flag = 0;  
  uint8_t recv_from_fpga_buf[RECV_BUF_LEN];
  TCP_DATA_t tcp_info = {0};

  osDelay(1000);

  TCPFeedbackInit();
  data_process_init();  /* register callback functions for tcp 、ethercat、fpga */

  /* here need add start functions for data receive */
  recv_from_fpga_data_start();

  for(;;)
  {

    event_flag = osEventFlagsWait(data_process_eventHandle, DATA_PROCESS_FPGA_EVENT | DATA_PROCESS_LAN_EVENT | DATA_PROCESS_TCP_EVENT, osFlagsWaitAny, osWaitForever);
    if (event_flag & DATA_PROCESS_LAN_EVENT)
    {
        ethercat_recv_data_update_with_block(0);
    }

    if (event_flag & DATA_PROCESS_FPGA_EVENT)
    {
        stat = recv_from_fpga_data_get(recv_from_fpga_buf);
        if (stat == osOK)
        {
            non_realtime_fpga_data_process(recv_from_fpga_buf);

            printf("buf: %x %x %x %x\r\n", recv_from_fpga_buf[0], recv_from_fpga_buf[1], recv_from_fpga_buf[2], recv_from_fpga_buf[3]);
        }
        else
        {
            printf("no msg in spi2 rx queue:%d\r\n", stat);
        }
    }

    if (event_flag & DATA_PROCESS_TCP_EVENT)
    {
        stat = tcp_client_data_recv_get_with_block(&tcp_info, 0);
        if (stat == osOK)
        {
            ntrRecvParamAndPlan(&tcp_info);

            sendFeedback();

            // printf("tcp_info len:%d\r\n", tcp_info.Len);
            // printf("tcp_info %x %x %x %x\r\n", tcp_info.gDATABUF[0], tcp_info.gDATABUF[1], tcp_info.gDATABUF[2], tcp_info.gDATABUF[3]);
        }
        else
        {
            printf("no msg in tcp rx queue:%d\r\n", stat);
        }
    }
    
  }
  /* USER CODE END data_process_entry */
}

static int8_t main_app_thread_init(void)
{
    osThreadAttr_t recv_data_process_thread_attributes = {
    .name = "recv_data_process_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    osThreadAttr_t DataProcess_attributes = {
    .name = "DataProcess",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityNormal,
    };

    data_process_eventHandle = osEventFlagsNew(NULL);
    if (data_process_eventHandle == NULL)
    {
        printf("event data process create failed\r\n");
        return -1;
    }

    osThreadId_t DataProcessHandle = osThreadNew(DataProccessTask, NULL, &DataProcess_attributes);
    if (DataProcessHandle == NULL)
    {
        printf("thread data process create failed\r\n");
        return -1;
    }

    osThreadId_t recv_data_process_threadHandle = osThreadNew(data_process_entry, NULL, &recv_data_process_thread_attributes);
    if (recv_data_process_threadHandle == NULL)
    {
        printf("thread recv data process create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(main_app_thread_init);