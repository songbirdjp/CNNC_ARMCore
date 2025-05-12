#include "plan_data.h"
#include "fmc_sdram_port.h"
#include "hw_crc.h"
#include "init_call.h"

#define PLAN_DATA_LENGTH    2022
#define PLAN_PAYLOAD_LENGTH (PLAN_DATA_LENGTH - 6)
#define	CRC_TABLE_SIZE      256

static uint32_t CrcTable[CRC_TABLE_SIZE] = {0};
static uint32_t crcCal = 0xffffffff;
static uint16_t lastPackIndex = 0;
static __IO uint8_t* pSDRAM = NULL;
static uint16_t *feedback = NULL, feedback16Len = 0;
static BEAM_DATA nrtBeamData = {0};
static FRAME_HEAD frameHead = {0};
static FRAME_END frameEnd = {0};

static APP_DATA_SEND activeSendData[] = {
   // {"tcpFeedback", 0, TCP_SEND_PERIOD, WDT_BINDATA, ALL_CLIENTS, NULL, NULL},
};

static bool InitCrc32Table(void)
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

static uint32_t Crc32Buffer(/*uint32_t crc,*/ uint8_t *buf, uint32_t size)
{
    uint32_t i;
    uint32_t crc = 0xffffffff;

    for (i = 0; i < size; i++)
    {
        crc = CrcTable[(crc^buf[i]) & 0xff] ^ (crc >> 8);
    }
    return crc^0xffffffff;
}

int32_t planFeedback(uint8_t sn, uint8_t tag_fb)
{
    int32_t ret = 0;
    uint16_t typeLen = 0, *pFDAry = feedback;

    *pFDAry++ = tag_fb;
    *pFDAry++ = 2;
    typeLen = 4;//bytes
    *pFDAry++ = typeLen;        //payload length 
    *pFDAry++ = nrtBeamData.packIndexInOneBeam;
    *pFDAry = nrtBeamData.errorCode;

    ret = ws_send(sn, feedback, feedback16Len*2, true, false, WDT_BINDATA);
    nrtBeamData.errorCode = 0;
    #if 0
    pFDAry += typeLen; 
    *pFDAry++ = 101;
    *pFDAry++ = 2;  //upload:2
    typeLen = sizeof(interlockFeedback)/2;
    *pFDAry++ = typeLen;        //payload length
    memcpy(pFDAry, &interlockFeedback, typeLen*2);
    #endif

    return (ret <= 0 ? -1 : 0);
}

int8_t nrtRecvPlan(APP_DATA_RECV *info)
{
    uint16_t u8LenTotal = 0, i;
    uint32_t crcInData;
    uint16_t payloadLength;
    uint16_t headLength = sizeof(FRAME_HEAD), endLength = sizeof(FRAME_END);
    uint8_t *data = (uint8_t *)info->tcpData;

    if (info->length > 0) u8LenTotal = info->length;
    else return -1;

    //head
    memcpy(&frameHead, data, headLength);
 //   printf("head1: %d %d %d\r\n", frameHead.frmTag, frameHead.frmType,frameHead.frmLength);

    if((u8LenTotal != PLAN_DATA_LENGTH) || (frameHead.frmLength != PLAN_PAYLOAD_LENGTH))
    {
        nrtBeamData.errorCode = 0xf1;
        printf("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
        return -1;
    }
  //  printf("head2:%d %d %d %d\r\n", frameHead.totalPackInOneBeam,frameHead.packIndexInOneBeam,frameHead.CPQuantityInPack,frameHead.RIQuantityInPack);

    if(frameHead.packIndexInOneBeam == 1)
    {
        lastPackIndex = 0;
        nrtBeamData.beamIndex = data[headLength];//current beam index
    }   

    nrtBeamData.totalCPInBeam[nrtBeamData.beamIndex] += frameHead.CPQuantityInPack;
    nrtBeamData.totalRIInBeam[nrtBeamData.beamIndex] += frameHead.RIQuantityInPack;
   // printf("3: %d %d\r\n",frameHead.CPQuantityInPack, frameHead.RIQuantityInPack);

    if(frameHead.packIndexInOneBeam == (lastPackIndex + 1))    lastPackIndex = frameHead.packIndexInOneBeam;
    else{
        nrtBeamData.errorCode = 0xf5;
        printf("recv error #5: current index= %d last index = %d!!! \r\n", frameHead.packIndexInOneBeam, lastPackIndex);
        frameHead.packIndexInOneBeam = lastPackIndex;
        return -1;
    }

    if((frameHead.packIndexInOneBeam > frameHead.totalPackInOneBeam) || (frameHead.packIndexInOneBeam < 1)) 
    {
        nrtBeamData.errorCode = 0xf2;
        printf("recv error #2: total pack = %d, pack index = %d!!!\r\n",frameHead.totalPackInOneBeam, frameHead.packIndexInOneBeam);
        return -1;
    }

    uint16_t saveLength = u8LenTotal - headLength - endLength;
    if((SDRAM_BANK1_ADDR + SDRAM_BANK1_SIZE - (uint32_t)pSDRAM) < saveLength) 
    {
        nrtBeamData.errorCode = 0xf6;
        printf("recv error #6: sdram isn't enough, clear it!!! \r\n");
        return -1; 
    }
    crcCal = hardware_crc_calculate(CRC32, &data[headLength], saveLength);
    crcCal^= 0xFFFFFFFF;
   // printf("hw crcCal: %#.8x\r\n", crcCal^0xFFFFFFFF);
   // crcCal = Crc32Buffer(&data[headLength], saveLength);

   // printf("%x %x %x %x\r\n",data[u8LenTotal - 4],data[u8LenTotal - 3],data[u8LenTotal - 2],data[u8LenTotal - 1]);
    memcpy(&frameEnd, &data[u8LenTotal - 4], endLength);
    crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;

   // printf("recv crc: %x calculate crc: %x\r\n", crcInData, crcCal);
    if (crcInData != crcCal) {
        nrtBeamData.errorCode = 0xf4;
        printf("recv error #4: crcInData: %u crcCal: %u!!! \r\n", crcInData, crcCal);
        return -1;
    }

    /*save data to sdram*/
    uint16_t saveDataIndex = headLength;
    __IO uint8_t *pBeamData;
    SDRAM_DATA sdFixData = {0};
    uint16_t sdFixDataLen = sizeof(SDRAM_DATA);
    uint16_t riDataLen = sizeof(RADIATION_POINT_DATA);

    if (frameHead.packIndexInOneBeam == 1)
    {
        memcpy(&sdFixData, &data[saveDataIndex], sdFixDataLen);
        if(nrtBeamData.totalCPInBeam[nrtBeamData.beamIndex] > sdFixData.CPQuantityInBeam)
        {
            nrtBeamData.errorCode = 0xf3;
            printf("recv error #3: CP SUM: %d  Plan total CP: %d!!! \r\n", nrtBeamData.totalCPInBeam[nrtBeamData.beamIndex], sdFixData.CPQuantityInBeam);
            return -1;
        }
        if(nrtBeamData.totalRIInBeam[nrtBeamData.beamIndex] > sdFixData.RIQuantityInBeam)
        {
            nrtBeamData.errorCode = 0xf7;
            printf("recv error #7: RI SUM: %d, Plan total RI: %d!!! \r\n", nrtBeamData.totalRIInBeam[nrtBeamData.beamIndex],sdFixData.RIQuantityInBeam);
            return -1;
        }
        pBeamData = pSDRAM;
        sdFixData.beamType = frameHead.beamType;
        sdFixData.deliveryType = frameHead.deliveryType;
        memcpy(pBeamData, &sdFixData, sdFixDataLen);
        pBeamData += sdFixDataLen;
        nrtBeamData.pCPData = pBeamData;
        nrtBeamData.pRIData = nrtBeamData.pCPData +  MAX_CP_IN_BEAM*2;
        nrtBeamData.oneBeamSize[nrtBeamData.beamIndex] += sdFixDataLen; //length fix area, beamID,doseRateSet,beamMeterSet,totalCP,totalRI
        nrtBeamData.oneBeamSize[nrtBeamData.beamIndex] += MAX_CP_IN_BEAM*2;//cp area, always 512B in one beam in sdram
    }   
    saveDataIndex += sdFixDataLen;
    memcpy(nrtBeamData.pCPData, &data[saveDataIndex], frameHead.CPQuantityInPack*2);//update cp area
    // printf("cp %d %x|",frameHead.CPQuantityInPack, nrtBeamData.pCPData);
    // for(i = 0; i < frameHead.CPQuantityInPack*2; i++)
    // { 
    //     printf("(%d %d)", nrtBeamData.pCPData[i], data[saveDataIndex+i]);
    // }
    // printf("\r\n");
    nrtBeamData.pCPData += frameHead.CPQuantityInPack*2;
    saveDataIndex += CP_IN_PACK*2;
    memcpy(nrtBeamData.pRIData, &data[saveDataIndex], frameHead.RIQuantityInPack*riDataLen);
    // printf("ri %d|",frameHead.RIQuantityInPack);
    // for(i = 0; i < riDataLen; i++)
    // { 
    //     printf("%d %d ", nrtBeamData.pRIData[i], data[saveDataIndex + i]);
    // }
    // printf("\r\n");
    nrtBeamData.pRIData += frameHead.RIQuantityInPack*riDataLen;
    nrtBeamData.oneBeamSize[nrtBeamData.beamIndex] += frameHead.RIQuantityInPack*riDataLen;
 #if 0  
//     for(i=0; i<frameHead.CPQuantityInPack*2; i++) //update CP data area
//     {
//         *nrtBeamData.pCPData++ = data[saveDataIndex + i];
//         nrtBeamData.oneBeamSize[nrtBeamData.beamIndex]++;
//   //      printf("%d ", data[saveDataIndex + i]);
//     }
   // printf("\r\n");

    // if (frameHead.packIndexInOneBeam == 1)
    // {
      //  saveDataIndex += 32*2;
      //  uint16_t planTotalRI = (data[saveDataIndex + 1] << 8) + data[saveDataIndex];
        // if(nrtBeamData.totalRIInBeam[nrtBeamData.beamIndex] > sdData.RIQuantityInBeam)
        // {
        //     nrtBeamData.errorCode = 0xf7;
        //     printf("recv error #7: RI SUM: %d, Plan total RI: %d!!! \r\n", nrtBeamData.totalRIInBeam[nrtBeamData.beamIndex],sdData.RIQuantityInBeam);
        //     return -1;
        // }

     //   pBeamData += MAX_CP_IN_BEAM*2; //skip CP data area  
        // for(i=0; i<2; i++)//totalRI, 2B
        // {
        //     *pBeamData++ = data[saveDataIndex + i];
        //        // printf("%d ", info->gDATABUF[12+i]);
        // }
//         memcpy(pBeamData, &data[saveDataIndex],2);
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
    // float tmp1,tmp2,tmp3;
    // uint16_t ri = (data[saveDataIndex + 1] << 8) + data[saveDataIndex];
    // memcpy(&tmp1, &data[saveDataIndex + 2],4);
    // memcpy(&tmp2, &data[saveDataIndex + 6],4);
    // memcpy(&tmp3, &data[saveDataIndex + 10],4);
   // printf("%d %f %f %f\r\n",ri,tmp1,tmp2,tmp3);
#endif
    if (frameHead.packIndexInOneBeam == frameHead.totalPackInOneBeam) //the last pack in one beam
    {
        if(++nrtBeamData.totalBeam >= MAX_BEAM_NUM){
            printf("warn: beam:%d > 30!\r\n",nrtBeamData.totalBeam);
            return -1;
        }    
        lastPackIndex = 0;
        pSDRAM += nrtBeamData.oneBeamSize[nrtBeamData.beamIndex];
            //record each beam info
        printf("Beam %d transfer finish, size is %uB\r\n", nrtBeamData.beamIndex, nrtBeamData.oneBeamSize[nrtBeamData.beamIndex]);
    }
    nrtBeamData.packIndexInOneBeam = frameHead.packIndexInOneBeam;
    nrtBeamData.errorCode = 0xf0; //ok
   // printf("recv success #0 %d!!! \r\n",nrtBeamData.errorCode);
  //  }
    #if 0
    else if(frameHead.frmTag == PARAM_SETTING_TAG)
    {
        printf("recv parameter %d bytes ", u8LenTotal);
        if (frameHead.frmLength != (u8LenTotal - 10)) {
            rtBeamData.errorCode = 0xf1;
            printf("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
            return -1;
        }

        crcCal = 0xffffffff;
        crcCal = Crc32Buffer(crcCal, &data[6], frameHead.frmLength);
        crcCal ^= 0xffffffff;
        last = u8LenTotal - 1;
        frameEnd.crcHigh = (data[last-2] << 8) + data[last - 3];
        frameEnd.crcLow = (data[last] << 8) + data[last - 1];
        crcInData = (frameEnd.crcHigh << 16) + frameEnd.crcLow;
        if (crcInData != crcCal)
        {
            rtBeamData.errorCode = 0xf4;
            printf("recv error #4: crc error: %u %u!!! \r\n", crcInData, crcCal);
            return -1;
        }

        printf("crc:%u\r\n", crcInData);
    }
    #endif

    memset(&info, 0, sizeof(info));
    memset(&frameHead, 0, sizeof(frameHead));
    memset(&frameEnd, 0, sizeof(frameEnd));

    return 0;
}

int8_t getPlanBeamData(uint16_t beamIndex, struct one_beam_order *beam_info)
{
    __IO uint8_t *pBeamData = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    uint16_t localBeamIndex = 0;
    uint8_t skipBeamCnt = 0;

    while(1)
    {
        localBeamIndex = (pBeamData[1] << 8) + pBeamData[0];
        if (localBeamIndex >= MAX_BEAM_NUM)
        {
            printf("invalid beam index: %d\r\n", localBeamIndex);
            return -1;
        }
        // printf("find %d:beam id %d\r\n",skipBeamCnt, localBeamIndex);
        if(localBeamIndex != beamIndex){
            pBeamData += nrtBeamData.oneBeamSize[localBeamIndex];
            if(++skipBeamCnt >=  nrtBeamData.totalBeam){
                printf("Can't find beam%d\r\n", beamIndex);
                return -1;
            }
        }
        else{
            beam_info->info= (struct one_beam_order *)pBeamData;
            beam_info->cp_ri_map = (uint16_t *)(pBeamData + sizeof(SDRAM_DATA));
            beam_info->ri_data = (RADIATION_POINT_DATA *)(pBeamData + sizeof(SDRAM_DATA) + MAX_CP_IN_BEAM * 2);

            return 0;
        }    
    }

    return 0;
}

void setTCPSendControlSignal(uint8_t itemIndex, int32_t setVal)
{
    activeSendData[itemIndex].controlSignal = setVal;
}

int8_t clearPlan(void)
{
    pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    uint8_t *pBeamData = pSDRAM;
    uint32_t totalSize = 0;

    if(nrtBeamData.totalBeam <= 0){
        printf("no beam data to clear!\r\n");
        return 0;
    }  

    for(uint8_t i = 0; i < MAX_BEAM_NUM; i++)   totalSize += nrtBeamData.oneBeamSize[i];
    memset(pBeamData, 0, totalSize);
    nrtBeamData.pCPData = NULL;
    nrtBeamData.pRIData = NULL;
    nrtBeamData.totalBeam = 0;
    nrtBeamData.beamIndex = 0;
    memset(nrtBeamData.totalCPInBeam, 0, sizeof(nrtBeamData.totalCPInBeam));
    memset(nrtBeamData.totalRIInBeam, 0, sizeof(nrtBeamData.totalRIInBeam));
    memset(nrtBeamData.oneBeamSize, 0, sizeof(nrtBeamData.oneBeamSize));

    printf("clear beam data finished!\r\n");
    return 0;
}

static int8_t planDataInit(void)
{
   // InitCrc32Table();
    pSDRAM = (__IO uint8_t *) (SDRAM_BANK1_ADDR);

    feedback16Len = 5;
    feedback = (uint16_t*)pvPortMalloc(feedback16Len*2);

    memset(nrtBeamData.totalCPInBeam, 0, sizeof(nrtBeamData.totalCPInBeam));
    memset(nrtBeamData.totalRIInBeam, 0, sizeof(nrtBeamData.totalRIInBeam));
    memset(nrtBeamData.oneBeamSize, 0, sizeof(nrtBeamData.oneBeamSize));

    #if 0
    osMutexAttr_t tcp_send_mutex_attributes = {
    .name = "tcp_send_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    activeSendData[0].sendUpdateMutexHandle = osMutexNew(&tcp_send_mutex_attributes);
    activeSendData[0].length = feedback16Len*2;
    activeSendData[0].tcpData = feedback;
    sendStructInfo.pActiveSend = activeSendData;
    sendStructInfo.sendItemNum = sizeof(activeSendData)/sizeof(APP_DATA_SEND);
    #endif

    return 0;
}
INIT_ENV_EXPORT(planDataInit);

#ifndef BEAM_DATA_TEST
#include "shell.h"
static int8_t ExecuteConsoleCmd(uint16_t _consoleCmd, uint8_t _consoleInt)
{

    struct one_beam_order beam_info ={0};
    SDRAM_DATA sdFixData = {0};
    uint16_t sdFixDataLen = sizeof(SDRAM_DATA);
    uint16_t cp[MAX_CP_IN_BEAM];
    RADIATION_POINT_DATA RI;

    switch (_consoleCmd)
    {
    case 0://get
        printf("index %d\r\n",_consoleInt);
        if(getPlanBeamData( _consoleInt, &beam_info))
        {
            memcpy(&sdFixData, beam_info.info, sdFixDataLen);

            printf("beam:\r\n");
            printf("%d %d %f %f %d %d\r\n", sdFixData.beamID,sdFixData.beamType,sdFixData.deliveryType,sdFixData.doseRateSet,sdFixData.beamMeterSet,sdFixData.CPQuantityInBeam,sdFixData.RIQuantityInBeam);

            memcpy(cp, beam_info.cp_ri_map, MAX_CP_IN_BEAM*2);

            printf("cp:\r\n");
            for(uint16_t i = 0; i < MAX_CP_IN_BEAM; i++) 
                printf("%.4x ",cp[i]);
            printf("\r\n");

            printf("RI:\r\n");
            memcpy(&RI, beam_info.ri_data++, sizeof(RADIATION_POINT_DATA));
            printf("1---RI%u %f %f %f\r\n", RI.RadiationPointIndex, RI.DeliveryTime, RI.fCumulativeDose, RI.fDoseRate);

            memcpy(&RI, beam_info.ri_data, sizeof(RADIATION_POINT_DATA));
            printf("2---RI%u %f %f %f\r\n", RI.RadiationPointIndex, RI.DeliveryTime, RI.fCumulativeDose, RI.fDoseRate);
        }
        break;
    case 1: //clear
        clearPlan();
        break;
    default:    break;
    }

    return 0;
}

static int8_t cmd_plan_debug(uint8_t argc, uint8_t **argv) 
{                                                           
    uint8_t type = 0;                                    
    char *validCmd[] = {"get", "clr"};
    uint8_t cmdNum = sizeof(validCmd) / sizeof(char *);

    if (argc < 2)
    {
        printf("argv too few\r\n");
        return -1;
    }

    for (uint8_t i = 0; i < cmdNum; i++)
    {
        if (strstr(argv[1], validCmd[i]) != NULL)
        {
            type = i;
            break;
        }
    }
    if (type >= cmdNum)
    {
        printf("Invalid cmd type %s!\r\n", argv[1]);
        return -1;
    }

    return ExecuteConsoleCmd(type, atoi(argv[2]));
}
MSH_CMD_EXPORT_ALIAS(cmd_plan_debug, plan_debug, plan debug);

static int8_t cmd_plan_test(uint8_t argc, uint8_t **argv)
{
    __IO uint16_t *ptr_16 = (__IO uint16_t *) (SDRAM_BANK1_ADDR);

    struct one_beam
    {
        SDRAM_DATA info;
        uint16_t cp_ri_map[MAX_CP_IN_BEAM];
        RADIATION_POINT_DATA ri_data[64];
    };

    struct one_beam beam = {0};

    beam.info.beamID = 0;
    beam.info.doseRateSet = 5.0f;
    beam.info.beamMeterSet = 12.0f;
    beam.info.CPQuantityInBeam = 16;
    beam.info.RIQuantityInBeam = 32;

    for (uint8_t i = 0; i < beam.info.CPQuantityInBeam; i++)
    {
        beam.cp_ri_map[i] = i * 2;
    }

    for (uint8_t i = 0; i < beam.info.RIQuantityInBeam; i++)
    {
        beam.ri_data[i].RadiationPointIndex = i;
        beam.ri_data[i].DeliveryTime = i * 0.001f;
        beam.ri_data[i].fCumulativeDose = i * 0.01f;
        beam.ri_data[i].fDoseRate = i * 0.1f;
    }

    memcpy(ptr_16, &beam, sizeof(struct one_beam));

    // for (uint32_t i = 0; i < 1024; i++) 
    // {
    //     ptr_16[i] = i;
    // }

    // for (uint32_t i = 0; i < 1024; i++) 
    // {
    //     printf("%.4x ", ptr_16[i]);

    //     if ((i + 1) % 16 == 0)
    //     {
    //         printf("\r\n");
    //     }
    // }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_plan_test, plan_test, plan test);
#endif