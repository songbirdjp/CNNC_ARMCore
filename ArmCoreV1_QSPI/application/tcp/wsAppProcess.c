//#include "planData.h"
#include <stdio.h>
#include "wsAppProcess.h"
#include "main_app.h"
#include "hw_crc.h"
#include "init_call.h"
#include <float.h>

static uint32_t crcCal = 0xffffffff;
APP_DATA_SEND activeSendData[ACTIVE_SEND_ITEM_NUM];
static ACTIVE_SEND_ATTACHED attachedInfo[ACTIVE_SEND_ITEM_NUM];

void cmdFeedback(APP_DATA_RECV *info, uint16_t errCode)
{
    FRAME_HEAD head;
    FEEDBACK_DATA ack= {0};//payload
    uint8_t buf[10] = {0};

    head.frmTag = (info->tcpData[1] << 8) + info->tcpData[0];
    head.frmType = (info->tcpData[3] << 8) + info->tcpData[2];
    head.frmLength = 4;
    ack.messagetype = head.frmTag;
    ack.messageFeedback = errCode;
    packPayload_without_crc(head, &ack, buf);

    ws_send(info->sn, buf, 10, true, false, WDT_BINDATA);
}

void recv_process_example(APP_DATA_RECV *info)
{
    uint16_t tag = (info->tcpData[1] << 8) + info->tcpData[0];
    uint16_t type = (info->tcpData[3] << 8) + info->tcpData[2];

    /* add payload process here */
#if 0
    int8_t ret = 0;

    ret = websocket_shell_cmd_parse(info->sn, info->tcpData, info->length);
    if (ret != 0)
    {
        printf("websocket shell cmd parse err: %d\r\n", ret);
    }
    
    switch(tag)
    {
        case PLAN_DATA_SETTING_TAG: //recv plan
           LOG_I("recv plan data!\r\n");
          // clearPlan();
           nrtRecvPlan(info);
           planFeedback(info->sn);       
        break;
        case PLAN_DATA_CLEAR_TAG:
            LOG_I("clear plan data!\r\n");
            clearPlan();
            clearPlanFeedback(info->sn);
        break;
        case SINGLE_AXIS_MOVE_TAG:
            LOG_I("single move!\r\n");
            singleMoveProcess(info);
        break;
        case PSM_SERVICE_CMD_TAG:
            LOG_I("psm service cmd!!!!!!!!!!\r\n");
            break;
        case PARAM_SETTING_TAG:
            LOG_I("parameter setting!\r\n");
            paramRecvProcess(info);
            break;
        default: 
            uint32_t test_u32 = 0;
            float test_f32 = 3.14;

            test_u32 = (info->tcpData[3] << 24) + (info->tcpData[2] << 16) + (info->tcpData[1] << 8) + info->tcpData[0];
            float test = *(float *)&test_u32;
            uint8_t pFDAry[10]= {0x05,0x06};
            ws_send(0x02, pFDAry, 10, true, false, WDT_BINDATA);

            LOG_E("recv unknow data! test= %f\r\n", test);




        
        break;
    } 
#endif
    for(uint8_t i = 0; i < ACTIVE_SEND_ITEM_NUM; i++)
    {
        if((attachedInfo[i].tag == tag) &&(attachedInfo[i].type ==type))
        {
            attachedInfo[i].tag = 0; 
            break;
        }
    }
}

int16_t getPayload(APP_DATA_RECV* info, uint8_t** pdata)
{
    FRAME_HEAD frameHead = {0};
    FRAME_END frameEnd = {0};
    uint8_t headLength = sizeof(FRAME_HEAD), endLength = sizeof(FRAME_END);
    uint8_t *data = (uint8_t *)info->tcpData;
    uint16_t u8LenTotal = info->length;
    int16_t payloadLen;

    // for(uint8_t i = 0; i < u8LenTotal; i++) printf(" %x", data[i]);
    // printf("\r\n");
    memcpy(&frameHead, data, headLength);
   //  printf("head1: %d %d %d\r\n", frameHead.frmTag, frameHead.frmType,frameHead.frmLength);
 
     if((frameHead.frmLength + headLength + endLength) != u8LenTotal)
     {
         LOG_E("recv error #1: tcp buf len = %d frame len =%d!!! \r\n", u8LenTotal, frameHead.frmLength);
         return LENGTH_ERR;
     }
 
     crcCal = hardware_crc_calculate(CRC32, &data[headLength], frameHead.frmLength);
     crcCal^= 0xFFFFFFFF;
    // printf("hw crcCal: %#.8x\r\n", crcCal^0xFFFFFFFF);
 
     memcpy(&frameEnd, &data[u8LenTotal - endLength], endLength);
   //  printf("%x %x %x %x %x\r\n",data[u8LenTotal - 4],data[u8LenTotal - 3],data[u8LenTotal - 2],data[u8LenTotal - 1],frameEnd.crc);
    // printf("recv crc: %x calculate crc: %x\r\n", crcInData, crcCal);
     if (frameEnd.crc != crcCal) {
         LOG_E("recv error #4: crcInData: %x crcCal: %x!!! \r\n", frameEnd.crc, crcCal);
         return CRC_ERR;
     } 

     payloadLen = (int16_t)frameHead.frmLength;
     *pdata = &data[headLength];
   //  memcpy(pdata, &data[headLength], payloadLen);
    // printf("pdata: %d %d\r\n", *pdata, payloadLen);

     return payloadLen;
}

uint16_t getPayload_without_Check(APP_DATA_RECV* info, uint8_t** pdata)
{
    FRAME_HEAD frameHead = {0};
    uint8_t headLength = sizeof(FRAME_HEAD);
    uint8_t *data = (uint8_t *)info->tcpData;
    uint16_t u8LenTotal = info->length;

   // for(uint8_t i = 0; i < u8LenTotal; i++) printf(" %x", data[i]);
  //  printf("\r\n");
    memcpy(&frameHead, data, headLength);
    *pdata = &data[headLength];
    // printf("head1: %d %d %d\r\n", frameHead.frmTag, frameHead.frmType,frameHead.frmLength);

     return frameHead.frmLength;
}

int8_t packPayload(FRAME_HEAD head, uint8_t *payload, uint8_t *arr)
{
    FRAME_END end;
    uint8_t headLength = sizeof(FRAME_HEAD), endLength = sizeof(FRAME_END);

    memcpy(arr, &head, headLength);
    memcpy(&arr[headLength], payload, head.frmLength); 

    end.crc = hardware_crc_calculate(CRC32, payload, head.frmLength);
    end.crc^= 0xFFFFFFFF;  
    memcpy(arr + headLength + head.frmLength, &end, endLength);
    
    return 1;
}

int8_t packPayload_without_crc(FRAME_HEAD head, uint8_t *payload, uint8_t *arr)
{
    uint8_t headLength = sizeof(FRAME_HEAD);

    memcpy(arr, &head, headLength);
    memcpy(&arr[headLength], payload, head.frmLength); 
    
    return 1;
}

int8_t activeSend(uint8_t index, uint8_t clientType, FRAME_HEAD head, uint8_t *payload, uint32_t timeout)
{
    packPayload(head, payload, attachedInfo[index].packedFrame);
    activeSendData[index].assignedClientType = clientType;
    activeSendData[index].name = NULL;
    activeSendData[index].sendMode = WDT_BINDATA; 
    activeSendData[index].tcpData = attachedInfo[index].packedFrame;
    activeSendData[index].length = head.frmLength + 6;
    activeSendData[index].controlSignal = TO_SEND;

    attachedInfo[index].tag = head.frmTag;
    attachedInfo[index].type = head.frmType;
    attachedInfo[index].sendCnt = 1;
    osStatus_t status = osTimerStart(attachedInfo[index].sendTimer, timeout);
    if (status != osOK)
    {
        return -1;
    }

    return 0;
}

#define MAX_RESEND_TIMES    3
static void resend_timer_callback(void *arg)
{
    uint8_t index = (uint8_t *)arg;

   // printf("timer %d callback\r\n", index);
    if((attachedInfo[index].sendCnt >= MAX_RESEND_TIMES)||(attachedInfo[index].sendCnt <= 0)){
        osTimerStop(attachedInfo[index].sendTimer);
        memset(attachedInfo[index].packedFrame, 0, MAX_SEND_BUF_LEN);
        attachedInfo[index].sendCnt = 0;
        attachedInfo[index].type = 0;
        attachedInfo[index].tag = 0;
        return;
    }   
    if(attachedInfo[index].tag){
        activeSendData[index].controlSignal = TO_SEND;
        attachedInfo[index].sendCnt++;
    } 
    else{
        osTimerStop(attachedInfo[index].sendTimer);
        memset(attachedInfo[index].packedFrame, 0, MAX_SEND_BUF_LEN);
        attachedInfo[index].sendCnt = 0;
        attachedInfo[index].type = 0;
    }   
}

static int8_t websocket_app_data_init(void)
{
    sendStructInfo.pActiveSend = activeSendData;
    sendStructInfo.sendItemNum = ACTIVE_SEND_ITEM_NUM;

    for(uint8_t i = 0; i < ACTIVE_SEND_ITEM_NUM; i++)
    {
        attachedInfo[i].timerIndex = i;
        attachedInfo[i].sendTimer = osTimerNew(resend_timer_callback,osTimerOnce,(void *)&attachedInfo[i].timerIndex,NULL);
    }
    
    return 0;
}
INIT_APP_EXPORT(websocket_app_data_init);