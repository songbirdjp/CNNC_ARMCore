#include "planData.h"
#include "fmc_sdram_port.h"
#include "fpga_rw.h"
#include <stdio.h>
#include "main_app.h"
#include "hw_crc.h"

#define PARAM_SETTING_TAG 1
#define PLAN_DATA_SETTING_TAG 2

#define	 CRC_TABLE_SIZE		256

#define RT_DOWNLOAD_PAYLOAD_LEN  184 //plan data from plc
#define RT_SAVE_PAYLOAD_LEN  (RT_DOWNLOAD_PAYLOAD_LEN+2)    //+ RI
#define RT_SDRAM_PAYLOAD_LEN  (RT_SAVE_PAYLOAD_LEN+4)       //+ 2 leaf pos, because get 80 leafs pos from PLC, but fpga need 82 leafs pos

static uint32_t CrcTable[CRC_TABLE_SIZE];
static uint32_t crcCal = 0xffffffff;
static uint16_t lastPackIndex = 0;
static __IO uint8_t* pSDRAM;
static __IO uint8_t* pSDRAMCAL;
static uint16_t *feedback, feedback16Len;
BEAM_DATA rtBeamData;
REALTIME_FEEDBACK rtFeedback;
INTERLOCK_FEEDBACK interlockFeedback;
SECOND_POS_FEEDBACK secondPosFeedback;
static FRAME_HEAD frameHead;
static FRAME_END frameEnd;

static APP_DATA_SEND activeSendData[] = {
    {"tcpFeedback", 0, TCP_SEND_PERIOD, WDT_BINDATA, CONTROLLER, NULL, NULL},
};

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

uint32_t Crc32Buffer(uint32_t crc, uint8_t *buf, uint32_t size)
{
    uint32_t i;
 
    for (i = 0; i < size; i++)
    {
        crc = CrcTable[(crc^buf[i]) & 0xff] ^ (crc >> 8);
    }
    return crc;
}

 void planDataCheck(uint8_t *pSDBeamEnd)
 {
    uint8_t *pBeamData = pSDBeamEnd - rtBeamData.oneBeamSize[rtBeamData.beamIndex];
    uint16_t leafPos[80], max, xJawTarget, beamID,radiationID;
    uint16_t totalRI = rtBeamData.totalRIInBeam[rtBeamData.beamIndex];

    pBeamData += 2; //skip total RI 
    memcpy(beamID, pBeamData, 2);
    pBeamData += 2;//skip beam index
    for(uint16_t ri = 1; ri <= totalRI; ri++)
    {
        memcpy(radiationID, pBeamData, 2);
        pBeamData += 4; //skip (RI index + 1st big leaf)
        memcpy(leafPos, pBeamData, 160); //copy out 80 leaf pos in RI
        max = leafPos[0];//get max leaf pos
        for (uint8_t i = 1; i < 80; i++) {
            if (max < leafPos[i])   max = leafPos[i];
        }
        pBeamData += 160;//skip leaf pos * 80
        pBeamData += 4; //skip (2th big leaf + carrier pos)
        memcpy(xJawTarget, pBeamData, 2);
        if((max/325/0.44*0.213 - xJawTarget/2.5/ENCODER_CNT_PER_MM) > 59)
        {
            printf("Xjaw can't mask leaf end %d %d!\r\n",beamID,radiationID);
            interlockFeedback.jawInterlock[X] |= 0x10;
        }
        pBeamData += 20;
    }
 }

void updateNRTFeedback(void)
{
    uint16_t typeLen = 0, *pFDAry = feedback;

    *pFDAry++ = 102;
    *pFDAry++ = 2;
    typeLen = sizeof(secondPosFeedback)/2;
    *pFDAry++ = typeLen;        //payload length 
    memcpy(pFDAry, &secondPosFeedback, typeLen*2);
    pFDAry += typeLen;

    *pFDAry++ = 101;
    *pFDAry++ = 2;  //upload:2
    typeLen = sizeof(interlockFeedback)/2;
    *pFDAry++ = typeLen;        //payload length
    memcpy(pFDAry, &interlockFeedback, typeLen*2);
}

int8_t nrtRecvParamAndPlan(APP_DATA_RECV* info)//return( <0:error =0:parameter >0:plan )
{
    uint16_t u8LenTotal = 0, i;
    uint32_t crcInData;
    uint16_t last, payloadLength;
    uint8_t *data = (uint8_t *)info->tcpData;

    if (info->length > 0) u8LenTotal = info->length;
    else return -1;

    //head
    frameHead.frmTag = (data[1] << 8) + data[0];
    frameHead.frmType = (data[3] << 8) + data[2];
    frameHead.frmLength = (data[5] << 8) + data[4];
    frameHead.bankNo = (data[7] << 8) + data[6];
   // printf("head1: %d %d %d %d\r\n", frameHead.frmTag, frameHead.frmType,frameHead.frmLength,frameHead.bankNo);

    if(frameHead.frmTag == PLAN_DATA_SETTING_TAG)
    {
        frameHead.totalPackInOneBeam = (data[9] << 8) + data[8];
        frameHead.packIndexInOneBeam = (data[11] << 8) + data[10];
      //  printf("head2: %d %d\r\n",frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);

        if(frameHead.packIndexInOneBeam == 1)   lastPackIndex = 0;
        payloadLength = u8LenTotal - 20;
        if(frameHead.frmLength != payloadLength)
        {
            secondPosFeedback.errorCode = 0xf1;
            printf("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
            return -1;
        }

        if ((frameHead.packIndexInOneBeam > frameHead.totalPackInOneBeam) || (frameHead.packIndexInOneBeam < 1)) {
            secondPosFeedback.errorCode = 0xf2;
            printf("recv error #2: total pack = %d, pack index = %d!!!\r\n",
            frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);
            return -1;
        }

        if (frameHead.bankNo != BANK_NO) {
            secondPosFeedback.errorCode = 0xf3;
            printf("recv error #3: bank no = %d!!! \r\n", frameHead.bankNo);
            return -1;
        }
        
        if((SDRAM_BANK1_ADDR + SDRAM_BANK1_SIZE - (uint32_t)pSDRAM) < frameHead.frmLength) 
        {
            secondPosFeedback.errorCode = 0xf6;
            printf("recv error #6: sdram is full, clear it!!! \r\n");
            return -1; 
        }

        if(frameHead.packIndexInOneBeam > lastPackIndex)    lastPackIndex = frameHead.packIndexInOneBeam;
        else{
            secondPosFeedback.errorCode = 0xf5;
            printf("recv error #5: last index= %d index = %d!!! \r\n", frameHead.packIndexInOneBeam, lastPackIndex);
            frameHead.packIndexInOneBeam = lastPackIndex;
            return -1;
        }

        uint16_t saveLength,sdramLength;
        uint8_t *pBeamData = pSDRAM;

        if (frameHead.packIndexInOneBeam == 1)
        {
            ++rtBeamData.totalBeam;
          //  uint8_t temp[frameHead.frmLength];
         //   printf("recv plan %dB-> first pack of beam!\r\n", u8LenTotal);
            saveLength = frameHead.frmLength+4;
            sdramLength = saveLength;
            for(i=0; i<saveLength; i++)
            {
                *pBeamData++ = data[12+i];
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

               // printf("%d ", info->gDATABUF[12+i]);
            }
          //  for(i = 12; i < frameHead.frmLength; i++)  printf("%x ", data[i]);
         //   crcCal = 0xffffffff;
          //  crcCal = Crc32Buffer(crcCal, &data[12], saveLength);//ok
            crcCal = hardware_crc_calculate(CRC32, &data[12], saveLength);
        }
        else {
            saveLength = frameHead.frmLength;
            sdramLength = saveLength;
            
            for (i = 0; i < saveLength; i++) {
                *pBeamData++ = data[16 + i];
               //  printf("%x\r\n",pBeamData);
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

          //  crcCal = Crc32Buffer(crcCal, &data[16], saveLength);//ok
            crcCal = hardware_crc_calculate_continue(&data[16], saveLength);
        }
        pSDRAM += sdramLength;

        if (frameHead.packIndexInOneBeam == frameHead.totalPackInOneBeam) //the last pack in one beam
        {
          //  for(int16_t i = 0; i <  frameHead.frmLength; i++)   printf("%d ", data[12+i]);
            lastPackIndex = 0;
            last = u8LenTotal - 1;
            frameEnd.crcHigh = (data[last - 2] << 8) + data[last - 3];
            frameEnd.crcLow = (data[last] << 8) + data[last - 1];
            crcCal ^= 0xffffffff;
            crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;

           // printf("recv crc: %u calculate crc: %u\r\n", crcInData, crcCal);
            if (crcInData != crcCal) {
                secondPosFeedback.errorCode = 0xf4;
                printf("recv error #4: crcInData: %u crcCal: %u!!! \r\n", crcInData, crcCal);
                return -1;
            }

            //record each beam info
            // 1 beam in SDRAM: totalRI + BeamIndex + (RI1 + RI2 + ...+RI(totalRI)), sizeof(RI) = RT_PAYLOAD_LEN
            rtBeamData.beamIndex = (data[15] << 8) + data[14];//current beam index
            // rtBeamData.totalRIInBeam[rtBeamData.beamIndex] = (data[13] << 8) + data[12];
            // rtBeamData.oneBeamSize[rtBeamData.beamIndex] =
            //         4 + RT_SDRAM_PAYLOAD_LEN
            //         * rtBeamData.totalRIInBeam[rtBeamData.beamIndex];
            uint8_t beamBufIndex = 0;
            if(rtBeamData.totalBeam > 0)    beamBufIndex = rtBeamData.totalBeam - 1;
            rtBeamData.totalRIInBeam[beamBufIndex] = (data[13] << 8) + data[12];
            rtBeamData.oneBeamSize[beamBufIndex] =
                     4 + RT_SDRAM_PAYLOAD_LEN
                     * rtBeamData.totalRIInBeam[beamBufIndex];
            if (rtBeamData.totalBeam >= MAX_BEAM_NUM) printf("Warn: beam > 30, will not be sent to FPGA!\r\n");
            printf("Beam %d transfer finish, size is %d, crc is %u\r\n",
                   rtBeamData.beamIndex, rtBeamData.oneBeamSize[beamBufIndex], crcInData);
            
            planDataCheck(pSDRAM);
        }
        secondPosFeedback.packIndexInOneBeam = frameHead.packIndexInOneBeam;
        secondPosFeedback.errorCode = 0xF0; //ok
      //  printf("recv success #0!!! \r\n");
    }
    else if(frameHead.frmTag == PARAM_SETTING_TAG)
    {
        printf("recv parameter %d bytes ", u8LenTotal);
        if (frameHead.frmLength != (u8LenTotal - 10)) {
            secondPosFeedback.errorCode = 0xf1;
            printf("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
            return -1;
        }
        if (frameHead.bankNo != BANK_NO) {
            secondPosFeedback.errorCode = 0xf3;
            printf("recv error #3: bank no = %d!!! \r\n", frameHead.bankNo);
            return -1;
        }
        // printf("table:");
        // for(uint16_t m = 0; m < 256; m++)   printf("%d ", CrcTable[m]);
        // printf("\r\n");
        // printf("data:");
        // for(uint16_t n = 0; n < frameHead.frmLength; n++)   printf("%d ", data[6+n]);
        // printf("\r\n");
      //  crcCal = 0xffffffff;
      //  crcCal = Crc32Buffer(crcCal, &data[6], frameHead.frmLength);
        crcCal = hardware_crc_calculate(CRC32, &data[6], frameHead.frmLength);
        crcCal ^= 0xffffffff;
    
        last = u8LenTotal - 1;
        frameEnd.crcHigh = (data[last-2] << 8) + data[last - 3];
        frameEnd.crcLow = (data[last] << 8) + data[last - 1];
        crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;
        if (crcInData != crcCal)
        {
            secondPosFeedback.errorCode = 0xf4;
            printf("recv error #4: crc error: %u %u!!! \r\n", crcInData, crcCal);
            return -1;
        }
        printf("crc:%u\r\n", crcInData);
        secondPosFeedback.errorCode = 0xF0; //ok
#ifndef TEST
        make_para_for_fpga(data);
        plcSetJawParam(data);
#endif
    }
    else{
        printf("error data pack:no valid tag!\r\n");
    }    

    memset(&info, 0, sizeof(info));
    memset(&frameHead, 0, sizeof(frameHead));
    memset(&frameEnd, 0, sizeof(frameEnd));

    return 1;
}

uint8_t sendCPtoDevice(uint16_t beamIndex, uint16_t RIIndex, struct JawFlagType JawPos)
{
    __IO uint8_t *pBeamData;
    uint8_t send_buf[184] = {0};
  //  memset(&JawPos, 0 , sizeof(struct JawFlagType));

    pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    pBeamData = pSDRAM;

   // rtBeamData.totalBeam = 2;
  //  rtBeamData.totalRIInBeam[1] = 0x704;
    if((beamIndex <= 0) || (RIIndex <= 0)/* || (RIIndex > rtBeamData.totalRIInBeam[beamIndex])*/)
    {
        printf("Error: Invalid beam/RI index %d,%d,%d\r\n",rtBeamData.totalBeam,beamIndex,RIIndex);
        return 0;
    }
   // printf("BEAM%d.RI%d\r\n", beamIndex, RIIndex);
#if 0
    for(uint8_t i = 1; i < beamIndex; i++)
    {
      //  if(rtBeamData.oneBeamSize[i] == 0)  return; //beamIndex >= 2
        pBeamData += rtBeamData.oneBeamSize[i];
      //  printf("beam %u size %u\r\n", i, rtBeamData.oneBeamSize[i]);
    }   //skip front beams
#endif
    uint16_t localBeamIndex;
    uint8_t skipBeamCnt = 0;
    while(1)
    {
        localBeamIndex = (pBeamData[3] << 8) + pBeamData[2];
        if(localBeamIndex != beamIndex)
        {
            pBeamData += rtBeamData.oneBeamSize[skipBeamCnt];
            if(++skipBeamCnt >=  rtBeamData.totalBeam){
                printf("Can't find beam%d\r\n", beamIndex);
                return 0;
            }
        } 
        else    break;
    }	  
  //  printf("%d %d %d %d %d %d\r\n", *pBeamData,*(pBeamData+1),*(pBeamData+2),*(pBeamData+3),*(pBeamData+4),*(pBeamData+5));
    pBeamData += 4; //skip current beam head (total RI + beam index)

    pBeamData += RT_SDRAM_PAYLOAD_LEN*(RIIndex - 1);//skip front RIs
    pBeamData += 2; //skip current RI head (ControlPoint index)
#ifdef TEST
    rtFeedback.faultInfo2 = 0;
    for(uint8_t i = 0; i < RT_FPGA_UPLOAD_POS_LEN; i+=2){
        rtFeedback.rtPosUpload[i/2] = (pBeamData[i+1] <<8) + pBeamData[i];
        //printf("**** 0x%x 0x%x 0x%x====", recvBuf[i], recvBuf[i+1], rtDataUpload[i/2]);
        //  rtDataUpload[i/2] = 1;
    }
    memcpy(secondPosFeedback.leafSecondPos,  rtFeedback.rtPosUpload, 82*2);
    secondPosFeedback.carrierSecondPos = rtFeedback.rtPosUpload[82];
    memcpy(secondPosFeedback.jawSecondPos,  &rtFeedback.jawRTPos, 2*2);

  /*  makeSingleSendAry(24, pBeamData, 166, 1,1);
    pBeamData += 166;
    pBeamData += 4;//skip X/Y Jaw pos
    makeSingleSendAry(24, pBeamData, 8, 0,1);//CP limit pos
    for(uint8_t i = 0; i < sndCtrl.singleSize[24]; i++)  printf("%x ",sndCtrl.cmdSendBuf[i]);
    printf("\r\n");*/
#else
  //  makeSingleSendAry(24, pBeamData, RT_DOWNLOAD_PAYLOAD_LEN, 1);
    // makeSingleSendAry(24, pBeamData, 166, 1,1);
    memmove(send_buf, pBeamData, 166);
    pBeamData += 166;

    uint16_t pos[2];
    pos[X] = (pBeamData[1] << 8) + pBeamData[0];
    pos[Y] = (pBeamData[3] << 8) + pBeamData[2];
    messageToJawTask(JawPos, PLAN_DATA, XY, pos);
    
    pBeamData += 4;//skip X/Y Jaw pos
    // makeSingleSendAry(24, pBeamData, 8, 0,1);//CP limit pos
   // for(uint8_t i = 0; i < sndCtrl.singleSize[24]; i++)  printf("%x ",sndCtrl.cmdSendBuf[i]);
   // printf("\r\n");

    memmove(&send_buf[166], pBeamData, 18);
    make_cmd_to_fpga(CMD_TAR_SET, send_buf);
    make_cmd_to_fpga(CMD_SPD_ANA, &send_buf[174]);
    make_cmd_to_fpga(CMD_MOV_TIM, &send_buf[182]);
   // printf("beamid %d ri %d:",beamIndex,RIIndex);
   // printf("Jaw x %d y %d\r\n",pos[X],pos[Y]);
   // for(uint8_t j = 0; j < 184; j++)    printf("%x ",send_buf[j]);
   // printf("\r\n");
#endif

    return 1;
}

void nrtRecvDataProcess(APP_DATA_RECV* info)
{
    nrtRecvParamAndPlan(info);
    updateNRTFeedback();
    ws_send(info->sn,  feedback, feedback16Len*2, true, false, WDT_BINDATA);
	secondPosFeedback.errorCode = 0;								
}

void setTCPSendControlSignal(uint8_t itemIndex, int32_t setVal)
{
    activeSendData[itemIndex].controlSignal = setVal;
}

void clearPlan(void)
{
    uint8_t *pBeamData;
	uint32_t sdTotalSize = 0;
  
    printf("Clear plan!\r\n");
    pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    pBeamData = pSDRAM;
    for(uint8_t i = 0; i < MAX_BEAM_NUM; i++)   sdTotalSize += rtBeamData.oneBeamSize[i];
    memset(pBeamData, 0, sdTotalSize);

    rtBeamData.totalBeam = 0;															
    rtBeamData.beamIndex = 0;
    memset(rtBeamData.totalRIInBeam, 0, sizeof(rtBeamData.totalRIInBeam));
    memset(rtBeamData.oneBeamSize, 0, sizeof(rtBeamData.oneBeamSize));
}

void planDataInit(void)
{
    InitCrc32Table();
    pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    pSDRAMCAL = (__IO uint8_t *) (SDRAM_BANK1_ADDR);

    feedback16Len = (3+sizeof(interlockFeedback)/2) + (3+sizeof(secondPosFeedback)/2);
    feedback = (uint16_t*)pvPortMalloc(feedback16Len*2);
    if(feedback != NULL)   memset(feedback, 0, feedback16Len*2);
    else    printf("%s file malloc err(line:%d)\r\n", __FILE__, __LINE__);  
#if 0
    for(uint8_t i = 0; i < TOTAL_FPGA_CMD_NUM; i++)
    {
        sndCtrl.singleSize[i] = sendCmd[i].TxLen+6;
        if(sendCmd[i].useAsParam)  sndCtrl.totalSize += sndCtrl.singleSize[i];//calculate parameter buf size
    }
 #endif
    interlockFeedback.versionARM = 2;
    secondPosFeedback.bankNo = BANK_NO;
	memset(rtBeamData.totalRIInBeam, 0, sizeof(rtBeamData.totalRIInBeam));
    memset(rtBeamData.oneBeamSize, 0, sizeof(rtBeamData.oneBeamSize));																	  

    osMutexAttr_t tcp_send_mutex_attributes = {
    .name = "tcp_send_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    activeSendData[0].sendUpdateMutexHandle = osMutexNew(&tcp_send_mutex_attributes);
    activeSendData[0].length = feedback16Len*2;
    activeSendData[0].tcpData = feedback;
    sendStructInfo.pActiveSend = activeSendData;
    sendStructInfo.sendItemNum = sizeof(activeSendData)/sizeof(APP_DATA_SEND);
}
