#include "planData.h"
#include <stdio.h>
#include "main_app.h"
#include "hw_crc.h"

#define PLAN_DATA_LENGTH 1918
#define PLAN_PAYLOAD_LENGTH (PLAN_DATA_LENGTH - 6)

#define	 CRC_TABLE_SIZE		256

#define RT_DOWNLOAD_PAYLOAD_LEN  174 //plan data from plc
#define RT_SAVE_PAYLOAD_LEN  (RT_DOWNLOAD_PAYLOAD_LEN+2)    //+ RI
#define RT_SDRAM_PAYLOAD_LEN  (RT_SAVE_PAYLOAD_LEN+4)       //+ 2 leaf pos, because get 80 leafs pos from PLC, but fpga need 82 leafs pos

static uint32_t CrcTable[CRC_TABLE_SIZE];
static uint32_t crcCal = 0xffffffff;
static uint16_t lastPackIndex = 0;
static __IO uint8_t* pSDRAM;
static __IO uint8_t* pSDRAMCAL;
static uint16_t *feedback, feedback16Len;
static BEAM_DATA nrtBeamData;
static FRAME_HEAD frameHead;
static FRAME_END frameEnd;

static APP_DATA_SEND activeSendData[] = {
   // {"tcpFeedback", 0, TCP_SEND_PERIOD, WDT_BINDATA, ALL_CLIENTS, NULL, NULL},
};

// bool InitCrc32Table(void)
// {
//     uint32_t c;
//     uint32_t i, j;

//     for (i = 0; i < CRC_TABLE_SIZE; i++)
//     {
//         c = (uint32_t)i;
//         for (j = 0; j < 8; j++)
//         {
//             if (c & 1)
//                 c = 0xedb88320 ^ (c >> 1);
//             else
//                 c = c >> 1;
//         }
//         CrcTable[i] = c;
//     }

//     return 1;
// }

// uint32_t Crc32Buffer(/*uint32_t crc,*/ uint8_t *buf, uint32_t size)
// {
//     uint32_t i;
//     uint32_t crc = 0xffffffff;
 
//     for (i = 0; i < size; i++)
//     {
//         crc = CrcTable[(crc^buf[i]) & 0xff] ^ (crc >> 8);
//     }
//     return crc^0xffffffff;
// }

// void planFeedback(uint8_t sn)
// {
//     uint16_t typeLen = 0, *pFDAry = feedback;

//     *pFDAry++ = 102;
//     *pFDAry++ = 2;
//     typeLen = 4;//bytes
//     *pFDAry++ = typeLen;        //payload length 
//     *pFDAry++ = nrtBeamData.packIndexInOneBeam;
//     *pFDAry = nrtBeamData.errorCode;

//     ws_send(sn, feedback, feedback16Len*2, true, false, WDT_BINDATA);
//     nrtBeamData.errorCode = 0;
//     #if 0
//     pFDAry += typeLen; 
//     *pFDAry++ = 101;
//     *pFDAry++ = 2;  //upload:2
//     typeLen = sizeof(interlockFeedback)/2;
//     *pFDAry++ = typeLen;        //payload length
//     memcpy(pFDAry, &interlockFeedback, typeLen*2);
//     #endif
// }

// int8_t nrtRecvPlan(APP_DATA_RECV* info)
// {
//     uint16_t u8LenTotal = 0, i;
//     uint32_t crcInData;
//     uint16_t last, payloadLength;
//     uint16_t headLength = sizeof(FRAME_HEAD), endLength = sizeof(FRAME_END);
//     uint8_t *data = (uint8_t *)info->tcpData;

//     if (info->length > 0) u8LenTotal = info->length;
//     else return -1;

//     //head
//     frameHead.frmTag = (data[1] << 8) + data[0];
//     frameHead.frmType = (data[3] << 8) + data[2];
//     frameHead.frmLength = (data[5] << 8) + data[4];
//   //  printf("head1: %d %d %d\r\n", frameHead.frmTag, frameHead.frmType,frameHead.frmLength);

//     if((u8LenTotal != PLAN_DATA_LENGTH) || (frameHead.frmLength != PLAN_PAYLOAD_LENGTH))
//     {
//         nrtBeamData.errorCode = 0xf1;
//         printf("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
//         return -1;
//     }

//     frameHead.totalPackInOneBeam = (data[7] << 8) + data[6];
//     frameHead.packIndexInOneBeam = (data[9] << 8) + data[8];
//     frameHead.CPQuantityInPack = (data[14] << 8) + data[13];
//     frameHead.RIQuantityInPack = (data[16] << 8) + data[15];
//   //  printf("head2:%d %d %d %d\r\n", frameHead.totalPackInOneBeam,frameHead.packIndexInOneBeam,frameHead.CPQuantityInPack,frameHead.RIQuantityInPack);

//     if(frameHead.packIndexInOneBeam == 1)
//     {
//         lastPackIndex = 0;
//         nrtBeamData.beamIndex = data[headLength];//current beam index
//     }   
//     nrtBeamData.totalCPInBeam[nrtBeamData.beamIndex] += frameHead.CPQuantityInPack;
//     nrtBeamData.totalRIInBeam[nrtBeamData.beamIndex] += frameHead.RIQuantityInPack;
//   //  printf("3: %d %d\r\n",nrtBeamData.totalCPInBeam[nrtBeamData.beamIndex], nrtBeamData.totalRIInBeam[nrtBeamData.beamIndex]);
        
//     if(frameHead.packIndexInOneBeam == (lastPackIndex + 1))    lastPackIndex = frameHead.packIndexInOneBeam;
//     else{
//         nrtBeamData.errorCode = 0xf5;
//         printf("recv error #5: current index= %d last index = %d!!! \r\n", frameHead.packIndexInOneBeam, lastPackIndex);
//         frameHead.packIndexInOneBeam = lastPackIndex;
//         return -1;
//     }

//     if((frameHead.packIndexInOneBeam > frameHead.totalPackInOneBeam) || (frameHead.packIndexInOneBeam < 1)) 
//     {
//         nrtBeamData.errorCode = 0xf2;
//         printf("recv error #2: total pack = %d, pack index = %d!!!\r\n",frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);
//         return -1;
//     }

//     if((SDRAM_BANK1_ADDR + SDRAM_BANK1_SIZE - (uint32_t)pSDRAM) < frameHead.frmLength) 
//     {
//         nrtBeamData.errorCode = 0xf6;
//         printf("recv error #6: sdram is full, clear it!!! \r\n");
//         return -1; 
//     }

//     uint16_t saveLength = u8LenTotal - headLength - endLength;
//     crcCal = Crc32Buffer(/*crcCal,*/ &data[headLength], saveLength);
//     last = u8LenTotal - 1;
//    // printf("%x %x %x %x\r\n",data[last - 3],data[last - 2],data[last - 1],data[last]);
//     frameEnd.crcHigh = (data[last - 2] << 8) + data[last - 3];
//     frameEnd.crcLow = (data[last] << 8) + data[last - 1];
//     crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;

//    // printf("recv crc: %u calculate crc: %x\r\n", crcInData, crcCal);
//     if (crcInData != crcCal) {
//         nrtBeamData.errorCode = 0xf4;
//         printf("recv error #4: crcInData: %u crcCal: %u!!! \r\n", crcInData, crcCal);
//         return -1;
//     }

//     /*save data to sdram*/
//     uint16_t saveDataIndex = headLength;
//     __IO uint8_t *pBeamData;

//     if (frameHead.packIndexInOneBeam == 1)
//     {
//         uint16_t planTotalCP = (data[saveDataIndex + 10] << 8) + data[saveDataIndex + 9];
//         if(nrtBeamData.totalCPInBeam[nrtBeamData.beamIndex] > planTotalCP)
//         {
//             nrtBeamData.errorCode = 0xf3;
//             printf("recv error #3: CP SUM: %d  Plan total CP: %d!!! \r\n", nrtBeamData.totalCPInBeam[nrtBeamData.beamIndex], planTotalCP);
//             return -1;
//         }
//         pBeamData = pSDRAM;
//       //  uint8_t tmp1[11];
//         for(i=0; i<11; i++) //beamID,doseRateSet,beamMeterSet,totalCP, 11B
//         {
//             pBeamData[i] = data[saveDataIndex + i];
//           //  tmp1[i] = pBeamData[i];
//           //  printf("%d ",tmp1[i]);
//         }
//       //  printf("\r\n");
//        // printf("save id %d\r\n", data[saveDataIndex]);
//         // float doseRateSet,beamMeterSet;
//         // memcpy(&doseRateSet, &data[saveDataIndex + 1],4);
//         // memcpy(&beamMeterSet, &data[saveDataIndex + 5],4);
//         // printf("%f,%f\r\n",doseRateSet,beamMeterSet);
//         nrtBeamData.pCPData = pBeamData;
//         nrtBeamData.oneBeamSize[nrtBeamData.beamIndex] += 11;
//         uint8_t tmp[11];
//         __IO uint8_t *p = pSDRAM;
//         printf("3 pSDRAM %lu\r\n", pSDRAM);
//         for(i=0; i<11; i++) //beamID,doseRateSet,beamMeterSet,totalCP, 11B
//         {
//             tmp[i] = p[i];
//             printf("%x %d ", p, tmp[i]);
//         }
//         printf("\r\n");
//     }   
//     saveDataIndex += 11;
//   //  printf("CP ");
//     for(i=0; i<frameHead.CPQuantityInPack*2; i++) //update CP data area
//     {
//         *nrtBeamData.pCPData++ = data[saveDataIndex + i];
//         nrtBeamData.oneBeamSize[nrtBeamData.beamIndex]++;
//   //      printf("%d ", data[saveDataIndex + i]);
//     }
//    // printf("\r\n");
       
//     if (frameHead.packIndexInOneBeam == 1)
//     {
//         saveDataIndex += 32*2;
//         uint16_t planTotalRI = (data[saveDataIndex + 1] << 8) + data[saveDataIndex];
//         if(nrtBeamData.totalRIInBeam[nrtBeamData.beamIndex] > planTotalRI)
//         {
//             nrtBeamData.errorCode = 0xf7;
//             printf("recv error #7: RI SUM: %d, Plan total RI: %d!!! \r\n", nrtBeamData.totalRIInBeam[nrtBeamData.beamIndex], planTotalRI);
//             return -1;
//         }

//         pBeamData += MAX_CP_IN_BEAM*2; //skip CP data area  
//         for(i=0; i<2; i++)//totalRI, 2B
//         {
//             *pBeamData++ = data[saveDataIndex + i];
//                // printf("%d ", info->gDATABUF[12+i]);
//         }
//         nrtBeamData.pRIData = pBeamData;
//         nrtBeamData.oneBeamSize[nrtBeamData.beamIndex] += 2;
//     }
         
//     saveDataIndex += 2;
//    // printf("RI ");
//     for(i=0; i<frameHead.RIQuantityInPack*sizeof(RADIATION_POINT_DATA); i++) //update RI data area 
//     {
//         *nrtBeamData.pRIData++ =data[saveDataIndex + i];
//         nrtBeamData.oneBeamSize[nrtBeamData.beamIndex]++;
//        // if(frameHead.packIndexInOneBeam == 1)  printf("%x ",data[saveDataIndex + i]);
//     }
//     // float tmp1,tmp2,tmp3;
//     // uint16_t ri = (data[saveDataIndex + 1] << 8) + data[saveDataIndex];
//     // memcpy(&tmp1, &data[saveDataIndex + 2],4);
//     // memcpy(&tmp2, &data[saveDataIndex + 6],4);
//     // memcpy(&tmp3, &data[saveDataIndex + 10],4);
//    // printf("%d %f %f %f\r\n",ri,tmp1,tmp2,tmp3);
//     if (frameHead.packIndexInOneBeam == frameHead.totalPackInOneBeam) //the last pack in one beam
//     {
//           //  for(int16_t i = 0; i <  frameHead.frmLength; i++)   printf("%d ", data[12+i]);
//         if(++nrtBeamData.totalBeam >= MAX_BEAM_NUM){
//             printf("warn: beam:%d > 30!\r\n",nrtBeamData.totalBeam);
//             return -1;
//         }    
//         lastPackIndex = 0;
//         pSDRAM += nrtBeamData.oneBeamSize[nrtBeamData.beamIndex];
//             //record each beam info
//         printf("Beam %d transfer finish, size is %dB\r\n", nrtBeamData.beamIndex, nrtBeamData.oneBeamSize[nrtBeamData.beamIndex]);
//     }
//     nrtBeamData.packIndexInOneBeam = frameHead.packIndexInOneBeam;
//     nrtBeamData.errorCode = 0xf0; //ok
//    // printf("recv success #0 %d!!! \r\n",nrtBeamData.errorCode);
//   //  }
//     #if 0
//     else if(frameHead.frmTag == PARAM_SETTING_TAG)
//     {
//         printf("recv parameter %d bytes ", u8LenTotal);
//         if (frameHead.frmLength != (u8LenTotal - 10)) {
//             rtBeamData.errorCode = 0xf1;
//             printf("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
//             return -1;
//         }
       
//         crcCal = 0xffffffff;
//         crcCal = Crc32Buffer(crcCal, &data[6], frameHead.frmLength);
//         crcCal ^= 0xffffffff;
//         last = u8LenTotal - 1;
//         frameEnd.crcHigh = (data[last-2] << 8) + data[last - 3];
//         frameEnd.crcLow = (data[last] << 8) + data[last - 1];
//         crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;
//         if (crcInData != crcCal)
//         {
//             rtBeamData.errorCode = 0xf4;
//             printf("recv error #4: crc error: %u %u!!! \r\n", crcInData, crcCal);
//             return -1;
//         }

//         printf("crc:%u\r\n", crcInData);
//     }
//     #endif

//     memset(&info, 0, sizeof(info));
//     memset(&frameHead, 0, sizeof(frameHead));
//     memset(&frameEnd, 0, sizeof(frameEnd));

//     return 1;
// }

// uint8_t getPlanBeamData(uint16_t beamIndex, BEAM_DATA_ADDR *retBeamData)
// {
//     pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
//     __IO uint8_t *pBeamData = pSDRAM;
//    // BEAM_DATA_ADDR retAddr;
//     uint16_t localBeamIndex;
//     uint8_t skipBeamCnt = 0, ret;

//     if(beamIndex < 0){
//         printf("Error: Invalid beam index %d\r\n",beamIndex);
//         return 0;
//     }

//     while(1)
//     {
//         localBeamIndex = pBeamData[0];
//         printf("id%d %d\r\n",skipBeamCnt, localBeamIndex);
//         if(localBeamIndex != beamIndex){
//             pBeamData += nrtBeamData.oneBeamSize[localBeamIndex];
//             if(++skipBeamCnt >=  nrtBeamData.totalBeam){
//                 printf("Can't find beam%d\r\n", beamIndex);
//                 return 0;
//             }
//         } 
//         else{
//             uint8_t tmp[11];
//            for(uint8_t i=0; i<11; i++) //beamID,doseRateSet,beamMeterSet,totalCP, 11B
//         {
//             tmp[i] = pBeamData[i];
//             printf("%x %d ", pBeamData, pBeamData[i]);
//         }
//         printf("\r\n");
//             retBeamData->pBeamData = pBeamData;
//             retBeamData->length = nrtBeamData.oneBeamSize[localBeamIndex];
//             return 1;
//         }    
//     }  
// }

// void setTCPSendControlSignal(uint8_t itemIndex, int32_t setVal)
// {
//     activeSendData[itemIndex].controlSignal = setVal;
// }

// uint8_t clearPlan(void)
// {
//     printf("Clear plan!\r\n");
//     pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
//     uint8_t *pBeamData = pSDRAM;
//     uint16_t localBeamIndex;
//     uint8_t skipBeamCnt = 0;
//     bool clearFlag = 0;

//     if(nrtBeamData.totalBeam <= 0)  return 1;
//     while(!clearFlag)
//     {
//         localBeamIndex = pBeamData[0];
//         memset(pBeamData, 0, nrtBeamData.oneBeamSize[localBeamIndex]);
//         if(++skipBeamCnt >=  nrtBeamData.totalBeam) clearFlag = 1;
//         else    pBeamData += nrtBeamData.oneBeamSize[localBeamIndex];
//     }

//     nrtBeamData.pCPData = NULL;
//     nrtBeamData.pRIData = NULL;
//     nrtBeamData.totalBeam = 0;
//     nrtBeamData.beamIndex = 0;
//     memset(nrtBeamData.totalCPInBeam, 0, MAX_BEAM_NUM);
//     memset(nrtBeamData.totalRIInBeam, 0, MAX_BEAM_NUM);
//     memset(nrtBeamData.oneBeamSize, 0, MAX_BEAM_NUM);

//     return clearFlag;
// }

// // void planDataInit(void)
// // {
// //     InitCrc32Table();
// //  //   HAL_StatusTypeDef ret = hardware_crc_config(CRC32);
// //  //   printf("crc config********:%d\r\n", ret);
// //     pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
// //     pSDRAMCAL = (__IO uint8_t *) (SDRAM_BANK1_ADDR);

// //     feedback16Len = 5;
// //     feedback = (uint16_t*)pvPortMalloc(feedback16Len*2);

// //     memset(nrtBeamData.totalCPInBeam, 0, MAX_BEAM_NUM);
// //     memset(nrtBeamData.totalRIInBeam, 0, MAX_BEAM_NUM);
// //     memset(nrtBeamData.oneBeamSize, 0, MAX_BEAM_NUM);

// //     #if 0
// //     osMutexAttr_t tcp_send_mutex_attributes = {
// //     .name = "tcp_send_mutex",
// //     .attr_bits = osMutexRecursive | osMutexPrioInherit
// //     };
   
// //     activeSendData[0].sendUpdateMutexHandle = osMutexNew(&tcp_send_mutex_attributes);
// //     activeSendData[0].length = feedback16Len*2;
// //     activeSendData[0].tcpData = feedback;
// //     sendStructInfo.pActiveSend = activeSendData;
// //     sendStructInfo.sendItemNum = sizeof(activeSendData)/sizeof(APP_DATA_SEND);
// //     #endif
// // }

// int8_t ExecuteConsoleCmd(uint16_t _consoleCmd, uint8_t _consoleInt)
// {
//     switch (_consoleCmd)
//     {
//     case 0://get
//        // printf("index %d\r\n",_consoleInt);
//       // __IO uint8_t *pBeamData;
//       // pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
//      //  pBeamData = pSDRAM;
//         BEAM_DATA_ADDR riData;
//         memset(&riData, 0, sizeof(BEAM_DATA_ADDR));
        
//       //  riData.pBeamData = pSDRAM;
//         getPlanBeamData( _consoleInt, &riData);
//         float fDosRateSet = 0, fBeamMeterSet = 0;
//       //  riData.pBeamData++;
//        // memcpy(&fDosRateSet, &tmp[1], 4);
//        // memmove(&fBeamMeterSet, &riData.pBeamData[18], 4);
//       //  printf(" %f %f\r\n", fDosRateSet,fBeamMeterSet);
//         break;
//     case 1: clearPlan();
//         break;
//     default:    break;
//     }
// }

// #include "shell.h"
// static int8_t cmd_plan_debug(uint8_t argc, uint8_t **argv) 
// {                                                           
//     uint16_t type, axes;                                    
//     double value;
//     char *validCmd[] = {"get", "clr"};
//     uint8_t cmdNum = sizeof(validCmd) / sizeof(char *);

//     if (argc < 2)
//     {
//         printf("argv too few\r\n");
//         return -1;
//     }
//     // printf("%s %s %s\r\n", argv[1],argv[2],argv[3]);

//     for (uint8_t i = 0; i < cmdNum; i++)
//     {
//         if (strstr(argv[1], validCmd[i]) != NULL)
//         {
//             type = i;
//             break;
//         }
//     }
//     if (type >= cmdNum)
//     {
//         printf("Invalid cmd type %s!\r\n", argv[1]);
//         return -1;
//     }

//     return ExecuteConsoleCmd(type, *argv[2]-'0');
// }
// MSH_CMD_EXPORT_ALIAS(cmd_plan_debug, plan_debug, plan debug);
