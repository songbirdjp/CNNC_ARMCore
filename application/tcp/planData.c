#include "planData.h"
#include "fmc_sdram_port.h"
#include "fpga_rw.h"
#include <stdio.h>
#include "main_app.h"
#include "hw_crc.h"
#include "carrierCal.h"	
#include "ulog.h"		   

#define PARAM_SETTING_TAG 1
#define PLAN_DATA_SETTING_TAG 2
#define PLAN_SETTING_FINISH_TAG 3

#define	 CRC_TABLE_SIZE		256

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
static bool carrierFollowFlag, beamRecvNotFinish;
uint16_t jawPlanPos[2] = {0};      // 分配内存并初始化
uint16_t jawPlanMotionTime = 0;

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

int8_t planDataCheck(uint8_t *pSDBeamStart)
 {
    uint8_t *pBeamData = pSDBeamStart, i,j;
    uint16_t leafPos[80], max, min,jawTarget[2],totalRI,beamID,radiationID;
    SD_BEAM_DATA beamStruct= {0};
  
    for(i = 0; i < rtBeamData.totalBeam; i++)
    {
        totalRI = (pBeamData[1] << 8) + pBeamData[0];
        pBeamData += sizeof(beamStruct.totalRI); //skip total RI 
        beamID = (pBeamData[1] << 8) + pBeamData[0];
        pBeamData += sizeof(beamStruct.beamID);//skip beam index
        LOG_I("totalRI %d,beamID %d\r\n", totalRI, beamID);
        for(uint16_t ri = 1; ri <= totalRI; ri++)
        {
            radiationID = (pBeamData[1] << 8) + pBeamData[0];
          //  LOG_I("radiationID %d,", radiationID);
            pBeamData += sizeof(beamStruct.riStruct.RI) + sizeof(beamStruct.riStruct.bigLeafTarget1); //skip (RI index + 1st big leaf)
            for (j = 0; j < 80; j++) {
                leafPos[j] = (pBeamData[2*j+1] << 8) + pBeamData[2*j];
               // LOG_I("leaf pos %d,", leafPos[i]);
            }
            max = leafPos[0];//get max leaf pos
            for (j = 1; j < 80; j++) {
                if (max < leafPos[j])   max = leafPos[j];
            //  if (min > leafPos[j])   min = leafPos[j];
            }
            pBeamData += sizeof(beamStruct.riStruct.leafTarget);//skip leaf pos * 80
            pBeamData += sizeof(beamStruct.riStruct.bigLeafTarget2) + sizeof(beamStruct.riStruct.carrierTarget); //skip (2th big leaf + carrier pos)
            jawTarget[X] = (pBeamData[1] << 8) + pBeamData[0];
          //  jawTarget[Y] = (pBeamData[3] << 8) + pBeamData[2];
         //   LOG_I("%d xJawTarget %d\r\n", ri, jawTarget[X]);
            if((max/325/0.44*0.213 - jawTarget[X]/2.5/ENCODER_CNT_PER_MM) > 59)// 325enc  =  1mm leaf，leaf / center = 0.44,
            // xjaw / center = 0.213
            {
                // LOG_I("Xjaw can't mask leaf end %d %d %d %d!\r\n",beamID,radiationdID, jawTarget[X], max);
                interlockFeedback.jawInterlock[X] |= 0x10;
                return -1;
            }
            #if 0
            for(j = 0; j < XY; j++){
                jawTarget[j] /= 2.5;
                if(planJawPosCheck(jawTarget[j], 0, j, (uint16_t)MAX_PREPARE_INPOS) == -1){
                    LOG_E("jaw%d pos invalid %d %d %d!\r\n",j,beamID,radiationID,jawTarget[j]);
                    return -1;
                }  
            } 
            #endif
            pBeamData += (sizeof(SD_RI_DATA) - offsetof(SD_RI_DATA, jawTarget[X]));
        }
    }

    return 0;
}

void updateNRTFeedback(enum feedbackType type)
{
    uint16_t typeLen = 0, *pFDAry = feedback;
    if(type == ACTIVE)  secondPosFeedback.errorCode = 0xff;

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
//    LOG_I("recv plan length = %d\r\n", u8LenTotal);
    //head
    frameHead.frmTag = (data[1] << 8) + data[0];
    frameHead.frmType = (data[3] << 8) + data[2];
    frameHead.frmLength = (data[5] << 8) + data[4];
    frameHead.bankNo = (data[7] << 8) + data[6];
   // LOG_I("head1: %d %d %d %d\r\n", frameHead.frmTag, frameHead.frmType,frameHead.frmLength,frameHead.bankNo);

    if(frameHead.frmTag == PLAN_DATA_SETTING_TAG)
    {
        frameHead.totalPackInOneBeam = (data[9] << 8) + data[8];
        frameHead.packIndexInOneBeam = (data[11] << 8) + data[10];
      //  LOG_I("head2: %d %d\r\n",frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);

        if(frameHead.packIndexInOneBeam == 1)   lastPackIndex = 0;
        payloadLength = u8LenTotal - 16;
        if(frameHead.frmLength != payloadLength)
        {
            secondPosFeedback.errorCode = 0xf1;
            LOG_E("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", payloadLength, frameHead.frmLength);
            return -1;
        }

        if ((frameHead.packIndexInOneBeam > frameHead.totalPackInOneBeam) || (frameHead.packIndexInOneBeam < 1)) {
            secondPosFeedback.errorCode = 0xf2;
            LOG_E("recv error #2: total pack = %d, pack index = %d!!!\r\n",
            frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);
            return -1;
        }

        if (frameHead.bankNo != BANK_NO) {
            secondPosFeedback.errorCode = 0xf3;
            LOG_E("recv error #3: bank no = %d!!! \r\n", frameHead.bankNo);
            return -1;
        }
        
        if((SDRAM_BANK1_ADDR + SDRAM_BANK1_SIZE - (uint32_t)pSDRAM) < frameHead.frmLength) 
        {
            secondPosFeedback.errorCode = 0xf6;
            LOG_E("recv error #6: sdram is full, clear it!!! \r\n");
            return -1; 
        }

        if(frameHead.packIndexInOneBeam > lastPackIndex)    lastPackIndex = frameHead.packIndexInOneBeam;
        else{
            secondPosFeedback.errorCode = 0xf5;
            LOG_E("recv error #5: last index= %d index = %d!!! \r\n", frameHead.packIndexInOneBeam, lastPackIndex);
            frameHead.packIndexInOneBeam = lastPackIndex;
            return -1;
        }

        uint16_t sdramLength;
        uint8_t *pBeamData = pSDRAM;

      //  saveLength = frameHead.frmLength;

        last = u8LenTotal - 1;
        frameEnd.crcHigh = (data[last - 2] << 8) + data[last - 3];
        frameEnd.crcLow = (data[last] << 8) + data[last - 1];
        crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;

        crcCal = 0xffffffff;
        crcCal = Crc32Buffer(crcCal, &data[12], frameHead.frmLength);
       // crcCal = hardware_crc_calculate(CRC32, &data[12], saveLength); 
        crcCal ^= 0xffffffff;
           // LOG_I("recv crc: %u calculate crc: %u\r\n", crcInData, crcCal);
        if (crcInData != crcCal) {
            secondPosFeedback.errorCode = 0xf4;
            LOG_E("recv error #4: crcInData: %x crcCal: %x!!! \r\n", crcInData, crcCal);
            return -1;
        }

        if (frameHead.packIndexInOneBeam == 1)
        {
            if(beamRecvNotFinish) {
                secondPosFeedback.errorCode = 0xf7;
               // LOG_E("recv error #7: last beam not finish!\r\n");
                return -1;
            } 
            else    beamRecvNotFinish = 1;

            if (++rtBeamData.totalBeam > MAX_BEAM_NUM){
                --rtBeamData.totalBeam;
				secondPosFeedback.errorCode = 0xf8;
                LOG_E("recv error #8: beam sum %d > 50, will not save!\r\n",rtBeamData.totalBeam);
                return -1;
            } 
          //  uint8_t temp[frameHead.frmLength];
         //   LOG_I("recv plan %dB-> first pack of beam!\r\n", u8LenTotal);
            sdramLength = frameHead.frmLength;
            for(i=0; i<frameHead.frmLength; i++)
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

               // LOG_I("%d ", info->gDATABUF[12+i]);
            }
          //  for(i = 12; i < saveLength; i++)  LOG_I("%x ", data[i]);
          //  LOG_I("\r\n");
         //   crcCal = 0xffffffff;
        //    crcCal = Crc32Buffer(crcCal, &data[12], saveLength);//ok
          //  LOG_I("%d crc %x\r\n", saveLength, (crcCal^0xffffffff));
        }
        else {
            // saveLength = frameHead.frmLength;
            // sdramLength = saveLength;
            // crcCal = hardware_crc_calculate(CRC32, &data[16], saveLength);
            // if (crcInData != crcCal) {
            //     secondPosFeedback.errorCode = 0xf4;
            //     LOG_E("recv error #4: crcInData: %x crcCal: %x!!! \r\n", crcInData, crcCal);
            //     return -1;
            // }
            sdramLength = frameHead.frmLength - 4;
            for (i = 0; i < (frameHead.frmLength - 4); i++) {
                *pBeamData++ = data[16 + i];
               //  LOG_I("%x\r\n",pBeamData);
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
          //  crcCal = 0xffffffff;
           // crcCal = Crc32Buffer(crcCal, &data[16], saveLength);//ok
        }
           // LOG_I("recv crc: %u calculate crc: %u\r\n", crcInData, crcCal);

        pSDRAM += sdramLength;
        if (frameHead.packIndexInOneBeam == frameHead.totalPackInOneBeam) //the last pack in one beam
        {
          //  for(int16_t i = 0; i <  frameHead.frmLength; i++)   LOG_I("%d ", data[12+i]);
            //record each beam info
            // 1 beam in SDRAM: totalRI + BeamIndex + (RI1 + RI2 + ...+RI(totalRI)), sizeof(RI) = RT_PAYLOAD_LEN
            rtBeamData.beamIndex = (data[15] << 8) + data[14];//current beam index
            uint8_t beamBufIndex = 0;
            if(rtBeamData.totalBeam > 0)    beamBufIndex = rtBeamData.totalBeam - 1;
            rtBeamData.totalRIInBeam[beamBufIndex] = (data[13] << 8) + data[12];
            rtBeamData.oneBeamSize[beamBufIndex] =
                     4 + RT_SDRAM_PAYLOAD_LEN
                     * rtBeamData.totalRIInBeam[beamBufIndex];
            beamRecvNotFinish = 0;
         //   if (rtBeamData.totalBeam >= MAX_BEAM_NUM) LOG_E("Warn: beam > 30, will not be sent to FPGA!\r\n");
            // LOG_I("Beam %d finish, ri is %d, size is %d\r\n",
            //        rtBeamData.beamIndex, rtBeamData.totalRIInBeam[beamBufIndex], rtBeamData.oneBeamSize[beamBufIndex]);
        }

        secondPosFeedback.packIndexInOneBeam = frameHead.packIndexInOneBeam;
        secondPosFeedback.errorCode = 0xF0; //ok
      //  LOG_I("recv success #0!!! \r\n");
    }
    else if(frameHead.frmTag == PARAM_SETTING_TAG)
    {
        // LOG_I("recv parameter %d bytes ", u8LenTotal);
        if (frameHead.frmLength != (u8LenTotal - 10)) {
            secondPosFeedback.errorCode = 0xf1;
            LOG_E("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
            return -1;
        }
        if (frameHead.bankNo != BANK_NO) {
            secondPosFeedback.errorCode = 0xf3;
            LOG_E("recv error #3: bank no = %d!!! \r\n", frameHead.bankNo);
            return -1;
        }
        // LOG_I("table:");
        // for(uint16_t m = 0; m < 256; m++)   LOG_I("%d ", CrcTable[m]);
        // LOG_I("\r\n");
        // LOG_I("data:");
        // for(uint16_t n = 0; n < frameHead.frmLength; n++)   LOG_I("%d ", data[6+n]);
        // LOG_I("\r\n");
        crcCal = 0xffffffff;
        crcCal = Crc32Buffer(crcCal, &data[6], frameHead.frmLength);
    //    crcCal = hardware_crc_calculate(CRC32, &data[6], frameHead.frmLength);
        crcCal ^= 0xffffffff;
    
        last = u8LenTotal - 1;
        frameEnd.crcHigh = (data[last-2] << 8) + data[last - 3];
        frameEnd.crcLow = (data[last] << 8) + data[last - 1];
        crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;
        if (crcInData != crcCal)
        {
            secondPosFeedback.errorCode = 0xf4;
            LOG_E("recv error #4: crc error: %x %x!!! \r\n", crcInData, crcCal);
            return -1;
        }
        LOG_I("crc:%u\r\n", crcInData);
        secondPosFeedback.errorCode = 0xF0; //ok
#ifndef TEST
        make_para_for_fpga(data);
        plcSetJawParam(data);
		if((data[80] & 0x60) == 0x40){
           carrierFollowFlag = 1;//enable carrier following function 
           plcSetCarrierParam(data);
        }  
		else	carrierFollowFlag = 0;
#endif
    }
    else if(frameHead.frmTag == PLAN_SETTING_FINISH_TAG)
    {
      //  for(i = 0; i<6; i++)    LOG_I("%d ", data[i]);
     //   LOG_I("\r\n");
        crcCal = 0xffffffff;
        crcCal = Crc32Buffer(crcCal, data, 6);
        crcCal ^= 0xffffffff;
    
        last = u8LenTotal - 1;
        frameEnd.crcHigh = (data[last-2] << 8) + data[last - 3];
        frameEnd.crcLow = (data[last] << 8) + data[last - 1];
        crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;
        if (crcInData != crcCal)
        {
            secondPosFeedback.errorCode = 0xf4;
            LOG_E("recv error #4: crc error: %x %x!!! \r\n", crcInData, crcCal);
            return -1;
        }

      //  pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
        if(carrierFollowFlag == 1)  osEventFlagsSet(carrier_cal_eventHandle, 1);
           // calCarrierTrajectory(pSDRAM, rtBeamData.totalBeam);
        // if(planDataCheck(pSDRAM) < 0){
        //     secondPosFeedback.errorCode = 0xf7;
        //     LOG_E("recv error #7: plan error!!!\r\n");
        //     return -1;
        // }
        secondPosFeedback.errorCode = 0xF0; //ok
    }
    else{
        LOG_E("error data pack:no valid tag %d %d!\r\n",u8LenTotal,frameHead.frmTag);
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
      //  LOG_E("Error: Invalid beam/RI index %d,%d,%d\r\n",rtBeamData.totalBeam,beamIndex,RIIndex);
        return 0;
    }

    if((carrierFollowFlag) && (!rtBeamData.carrierCalFinish))
    {
       // LOG_E("Error: carrier pos not finish! %d,%d\r\n",carrierFollowFlag, rtBeamData.carrierCalFinish);
        return 0;
    }
  //  LOG_I("%d.%d\r\n", beamIndex, RIIndex);
#if 0
    for(uint8_t i = 1; i < beamIndex; i++)
    {
      //  if(rtBeamData.oneBeamSize[i] == 0)  return; //beamIndex >= 2
        pBeamData += rtBeamData.oneBeamSize[i];
      //  LOG_I("beam %u size %u\r\n", i, rtBeamData.oneBeamSize[i]);
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
              //  LOG_E("Can't find beam%d\r\n", beamIndex);
                return 0;
            }
        } 
        else    break;
    }	  
  //  LOG_I("%d %d %d %d %d %d\r\n", *pBeamData,*(pBeamData+1),*(pBeamData+2),*(pBeamData+3),*(pBeamData+4),*(pBeamData+5));
    pBeamData += 4; //skip current beam head (total RI + beam index)

    pBeamData += RT_SDRAM_PAYLOAD_LEN*(RIIndex - 1);//skip front RIs
    pBeamData += 2; //skip current RI head (ControlPoint index)
#ifdef TEST
    rtFeedback.faultInfo2 = 0;
    for(uint8_t i = 0; i < RT_FPGA_UPLOAD_POS_LEN; i+=2){
        rtFeedback.rtPosUpload[i/2] = (pBeamData[i+1] <<8) + pBeamData[i];
        //LOG_I("**** 0x%x 0x%x 0x%x====", recvBuf[i], recvBuf[i+1], rtDataUpload[i/2]);
        //  rtDataUpload[i/2] = 1;
    }
    memcpy(secondPosFeedback.leafSecondPos,  rtFeedback.rtPosUpload, 82*2);
    secondPosFeedback.carrierSecondPos = rtFeedback.rtPosUpload[82];
    memcpy(secondPosFeedback.jawSecondPos,  &rtFeedback.jawRTPos, 2*2);

  /*  makeSingleSendAry(24, pBeamData, 166, 1,1);
    pBeamData += 166;
    pBeamData += 4;//skip X/Y Jaw pos
    makeSingleSendAry(24, pBeamData, 8, 0,1);//CP limit pos
    for(uint8_t i = 0; i < sndCtrl.singleSize[24]; i++)  LOG_I("%x ",sndCtrl.cmdSendBuf[i]);
    LOG_I("\r\n");*/
#else
    memmove(send_buf, pBeamData, 166);
    pBeamData += 166;

    // 1. 提取位置 (原逻辑)
    uint16_t pos[2];
    pos[X] = (pBeamData[1] << 8) + pBeamData[0];
    pos[Y] = (pBeamData[3] << 8) + pBeamData[2];

    // 2. 提取时间 (提前读取，不要等后面)
    // 根据你原来的代码: jawPlanMotionTime = (pBeamData[4+17] << 8) + pBeamData[4+16];
    // pBeamData 当前指向 JawPos，时间在 JawPos 之后的第 16 字节
    // 跳过 4字节(JawPos) + 16字节(其他) = 偏移 20 字节处是 TimeLow
    uint16_t motionTime = (pBeamData[21] << 8) + pBeamData[20];

    // LOG_I("pos[X]: %d pos[Y]: %d\r\n", pos[X], pos[Y]);
    // LOG_I("motionTime: %d\r\n", motionTime);

    // 3. 打包数据 (X位置, Y位置, 时间)
    uint16_t msg_payload[4];
    msg_payload[0] = pos[X];
    msg_payload[1] = pos[Y];
    msg_payload[2] = motionTime;
    msg_payload[3] = (RIIndex == rtBeamData.totalRIInBeam[beamIndex - 1]) ? 1 : 0; //last RI flag

    // LOG_I("rtBeamData.totalRIInBeam[%d]: %d\r\n", beamIndex, rtBeamData.totalRIInBeam[beamIndex - 1]);

    // 4. 发送给 Jaw 任务 (携带了时间!)
    messageToJawTask(JawPos, PLAN_DATA, XY, msg_payload);

    // 5. 更新全局变量 (保持你原有的逻辑，防止其他地方用到)
    pBeamData += 4;//skip X/Y Jaw pos
    jawPlanPos[X] = pos[X];
    jawPlanPos[Y] = pos[Y];
    jawPlanMotionTime = motionTime;

    memmove(&send_buf[166], pBeamData, 18);
  //  LOG_I("car pos %x %x\r\n", send_buf[164], send_buf[165]);
    make_cmd_to_fpga(CMD_TAR_SET, send_buf);
    make_cmd_to_fpga(CMD_SPD_ANA, &send_buf[174]);
    make_cmd_to_fpga(CMD_MOV_TIM, &send_buf[182]);
   // LOG_I("beamid %d ri %d:",beamIndex,RIIndex);
   // LOG_I("Jaw x %d y %d\r\n",pos[X],pos[Y]);
   // for(uint8_t j = 0; j < 184; j++)    LOG_I("%x ",send_buf[j]);
   // LOG_I("\r\n");
#endif

    return 1;
}

void nrtRecvDataProcess(APP_DATA_RECV* info)
{
    // LOG_I("nrtRecvDataProcess\r\n");
    nrtRecvParamAndPlan(info);
    updateNRTFeedback(PLAN);
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
  
    LOG_I("Clear plan!\r\n");
    pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    pBeamData = pSDRAM;
    for(uint8_t i = 0; i < MAX_BEAM_NUM; i++)   sdTotalSize += rtBeamData.oneBeamSize[i];
    memset(pBeamData, 0, sdTotalSize);

    rtBeamData.totalBeam = 0;															
    rtBeamData.beamIndex = 0;
    rtBeamData.carrierCalFinish = 0;
    interlockFeedback.jawInterlock[X] &= ~0x10;
    interlockFeedback.jawInterlock[Y] &= ~0x10;
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
    else    LOG_I("%s file malloc err(line:%d)\r\n", __FILE__, __LINE__);  
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
    carrierFollowFlag = 1;
    beamRecvNotFinish = 0;
}
