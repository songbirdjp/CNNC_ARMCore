#include "fpga_rw.h"
#include "stm32h7xx_hal.h"
#include "fpga_port.h"

bool DMATransmitting = 0;
SEND_CONTROL sndCtrl;

SEND_PARAM sendCmd[TOTAL_FPGA_CMD_NUM] = {
        CMD_BANK_SET, 1,1,//0
        CMD_TAR_MOD,1,1,//1
        CMD_PWM_MOD,1,0,//2
        CMD_PACK_MOD,2,0,//3
        CMD_SINGLE_BANK,1,0,//4
        CMD_CHECK_PASS,1,0,//5
        CMD_CTRL_VALID,11,0,//0x11
        CMD_SCREEN_2ND,11,0,//0x12
        CMD_POS_MNI,18,1,//0x14
        CMD_POS_ADJ,166,0,//0x15
        CMD_OOT_TH,4,0,//0x16
        CMD_WAIT_POS,2,1,//0x18
        CMD_REF_OFFSET,166,1,//0x19
        CMD_SEEK_STEP,2,0,//0x1a
        CMD_PID_LEAF,12,1,//0x20
        CMD_PID_BOX,6,0,//0x21
        CMD_PWM_LIMIT,4,0,//0x22
        CMD_2ND_K,164,1,//0x24
        CMD_2ND_B,164,1,//0x25
        CMD_2ND_TH,2,1,//0x26
        CMD_ADC_MNI,4,1,//0x27
        CMD_DEBUG_ENB,11,0,//0x30
        CMD_DEBUG_REF,11,0,//0x31
        CMD_DEBUG_PWM,2,0,//0x32
        CMD_TAR_SET,174,1,//0x40
        CMD_STA_REQ,10,0//0x50
};

void makeSingleSendAry(uint8_t index, uint8_t * pData, uint8_t size, bool isFirstSegment, bool isCmd)
{
    if(size < 1)    return;

    if(isFirstSegment)
    {
        memset(sndCtrl.cmdSendBuf,0,MAX_CMD_DATA_SIZE);
        sndCtrl.cmdSendBuf[0] = TX_SYNC_BYTE_H;
        sndCtrl.cmdSendBuf[1] = TX_SYNC_BYTE_L;
        sndCtrl.cmdSendBuf[2] = 0;
        sndCtrl.cmdSendBuf[3] = sendCmd[index].TxLen;
        sndCtrl.cmdSendBuf[4] = sendCmd[index].cmd;
        sndCtrl.cmdSendBuf[sndCtrl.singleSize[index] - 1] = sndCtrl.cmdSendBuf[2]
                                                                   + sndCtrl.cmdSendBuf[3] + sndCtrl.cmdSendBuf[4];
        sndCtrl.writeIndex[index] = 5;
    }
    else
    {
        if(sndCtrl.writeIndex[index] >= sndCtrl.singleSize[index])
        {
            printf("Error data fill in: send buf %d is full!\r\n", index);
            return;
        }
    }

    for(uint8_t i = 0; i < size; i+=2)
    {
        if(pData == NULL){
            sndCtrl.cmdSendBuf[sndCtrl.writeIndex[index] + i] = 0;
            sndCtrl.cmdSendBuf[sndCtrl.writeIndex[index] + i + 1] = 0;
          //  printf("fill 0!\r\n");
        }
        else if(size == 1){
            sndCtrl.cmdSendBuf[sndCtrl.writeIndex[index] + i] = pData[i];
            sndCtrl.cmdSendBuf[sndCtrl.singleSize[index] - 1] += pData[i];//checksum
           // printf("size = 1!\r\n");
        }
        else{
            sndCtrl.cmdSendBuf[sndCtrl.writeIndex[index] + i] = pData[i+1];
            sndCtrl.cmdSendBuf[sndCtrl.writeIndex[index] + i + 1] = pData[i];
            sndCtrl.cmdSendBuf[sndCtrl.singleSize[index] - 1] += (pData[i] + pData[i+1]);//checksum
         //   printf("size > 1!\r\n");
        }
    }
    sndCtrl.writeIndex[index] += size;//size;

   // for(uint16_t j=0; j < sndCtrl.singleSize[index]; j++) printf("0x%x ", sndCtrl.cmdSendBuf[j]);
   // printf("\r\n");
    if(isCmd) return;
    if(sndCtrl.writeIndex[index] >= (sndCtrl.singleSize[index] - 1))
    {
        memcpy(sndCtrl.pCrt, sndCtrl.cmdSendBuf, sndCtrl.singleSize[index]);
        sndCtrl.pCrt += sndCtrl.singleSize[index];
      /*  if(index == 24){
            sndCtrl.writeIndex[index] = 5;  //prepare for rt position send to FPGA
            sndCtrl.cmdSendBuf[sndCtrl.singleSize[index] - 1] = sndCtrl.cmdSendBuf[2]
                                                                + sndCtrl.cmdSendBuf[3] + sndCtrl.cmdSendBuf[4];
        }*/
       // else memset(sndCtrl.cmdSendBuf,0,sndCtrl.singleSize[index]);
    }
}

void makeParamSendAry(uint8_t * pData)
{
    sndCtrl.pCrt = sndCtrl.paramSendBuf;

    makeSingleSendAry(0, &pData[6], sendCmd[0].TxLen,1,0);//0
    makeSingleSendAry(1, &pData[58], sendCmd[1].TxLen,1,0);//1
    makeSingleSendAry(8, &pData[34], sendCmd[8].TxLen,1,0);//0x14
    makeSingleSendAry(11, &pData[54], sendCmd[11].TxLen,1,0);//0x18
    makeSingleSendAry(12, &pData[418], 164,1,0);//0x19
    makeSingleSendAry(12, &pData[52], 2,0,0);//0x19
    makeSingleSendAry(14, &pData[8], sendCmd[14].TxLen,1,0);//0x20
    makeSingleSendAry(17, &pData[90],sendCmd[17].TxLen,1,0);//0x24
    makeSingleSendAry(18, &pData[254], sendCmd[18].TxLen,1,0);//0x25
    makeSingleSendAry(19, &pData[30], sendCmd[19].TxLen,1,0);//0x26
    makeSingleSendAry(20, &pData[60], sendCmd[20].TxLen,1,0);//0x27
    uint16_t initPos[82];
    for(uint8_t i =0; i < 82; i++)  initPos[i] = 3250;
    makeSingleSendAry(24, (uint8_t*)initPos, 164,1,0);//0x40
    makeSingleSendAry(24, &pData[56], 2,0,0);//0x40
  //  for(uint8_t i = 0; i < 2; i++)  printf("0x%x ", pData[56+i]);
    initPos[0] = initPos[2] = 35100;
    initPos[1] = initPos[3] = 5687;
    makeSingleSendAry(24, (uint8_t*)initPos, 8,0,0);//0x40
}

void dataOut_CS_Select(void)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
}

void dataOut_CS_Deselect(void)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
}

void FPGA_WriteByteArray(uint8_t *pTxData, uint16_t size)
{
   // printf("write param!!!");
    if(DMATransmitting)  return;

    dataOut_CS_Select();// cs high in interrupt callback
    DMATransmitting = 1;
    // HAL_SPI_Transmit_DMA(&hspi3,pTxData,size);

    //HAL_SPI_Transmit(&hspi3,pTxData,size,100);
  //  dataOut_CS_Deselect();
}

void FPGA_ReadByteArray(uint8_t *pRxData, uint16_t size)
{
   // uint8_t pTxData[size];
   // memset(pTxData, 0, size);

    // HAL_SPI_Receive_DMA(&hspi2,  pRxData , size);
  //  printf("read: ");
  //  for(uint16_t i = 0; i < size; i++)  printf("0x%x ",pRxData[i]);
  //  printf("\r\n");
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    // if(hspi == &hspi3)
    {
       // printf("send irq!!!\r\n");
        dataOut_CS_Deselect();// set cs here
        DMATransmitting = 0;
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    // if(hspi == &hspi2)
    {
        // ringb_push(&ringbufCtrl, dmaBuf);
        // FPGA_ReadByteArray(&ringbufCtrl.array[ringbufCtrl.tail*RECV_BUF_LEN], RECV_BUF_LEN);
       // ringbufCtrl.tail = (ringbufCtrl.tail + 1) % ringbufCtrl.size ;
      //  DMACnt++;
       // FPGA_ReadByteArray(recvBuf, RECV_BUF_LEN);
      //  DMAReceived = 1;
         //  printf("0x%x 0x%x", dmaBuf[0], dmaBuf[1]);
       // for(uint16_t i = 0; i < 8; i++)   printf("0x%x ",recvBufCpy[4+MAX_RECV_PACK_SIZE*i]);
     //   printf("\r\n");
    }
}


/************************************************************************************************************/
static uint8_t recv_buf[RECV_BUF_LEN];

static int8_t fun_cb(void *arg)
{
    extern osEventFlagsId_t data_process_eventHandle;
    osEventFlagsSet(data_process_eventHandle, DATA_PROCESS_FPGA_EVENT);
}

static int8_t recv_from_fpga_init(osMessageQueueId_t queue)
{
    int8_t ret = 0;

    if (queue == NULL)
    {
        printf("queue is null\r\n");
        return -1;
    }

    ret = device_recv_from_fpga_init(DEVICE_RECV_FROM_FPGA_NAME_DEFAULT);
    if (ret != 0)
    {
        printf("device %s init err:%d\r\n", DEVICE_RECV_FROM_FPGA_NAME_DEFAULT, ret);
        return -2;

    }

    ret = device_recv_from_fpga_buffer_init(recv_buf, sizeof(recv_buf));
    if (ret != 0)
    {
        printf("device %s buffer init err:%d\r\n", DEVICE_RECV_FROM_FPGA_NAME_DEFAULT, ret);
        return -3;
    }

    ret = device_recv_from_fpga_queue_init(queue, fun_cb);
    if (ret != 0)
    {
        printf("device %s queue init err:%d\r\n", DEVICE_RECV_FROM_FPGA_NAME_DEFAULT, ret);
        return -4;
    }

    ret = device_recv_from_fpga_open();
    if (ret != 0)
    {
        printf("device %s open err:%d\r\n", DEVICE_RECV_FROM_FPGA_NAME_DEFAULT, ret);
        return -5;
    }

    return ret;

}

static int8_t send_to_fpga_init(void)
{
    int8_t ret = 0;

    ret = device_send_to_fpga_init(DEVICE_SEND_TO_FPGA_NAME_DEFAULT);
    if (ret != 0)
    {
        printf("device %s init err:%d\r\n", DEVICE_SEND_TO_FPGA_NAME_DEFAULT, ret);
        return -1;
    }

    device_send_to_fpga_open();

    return 0;
}

static int8_t send_to_fpga_write(uint8_t *buf, uint16_t size, uint32_t timeout)
{
    return device_send_to_fpga_write(buf, size, timeout);
}

/*
 * thread init
*/
static osMessageQueueId_t recv_from_fpga_queueHandle = NULL;
static osMessageQueueId_t send_to_fpga_queueHandle = NULL;

static void fpga_communication_entry(void *argument)
{
  /* USER CODE BEGIN fpga_communication_entry */
  /* Infinite loop */
  int8_t ret = 0;
  osStatus_t stat = osOK;
  struct send_to_fpga_msg recv_buf = {0};

  send_to_fpga_init();
  recv_from_fpga_init(recv_from_fpga_queueHandle);
  
  for(;;)
  {
    stat = osMessageQueueGet(send_to_fpga_queueHandle, &recv_buf, 0, osWaitForever);
    if (stat != osOK)
    {
        printf("get queue err:%d\r\n", stat);
    }
  
    ret = send_to_fpga_write(recv_buf.buf, recv_buf.len, 1000);
    if (ret != 0)
    {
        printf("send data to fpga err:%d\r\n", ret);
    }

    // printf("recv_buf.len:%d\r\n", recv_buf.len);
    // printf("%x %x %x %x %x %x\r\n", recv_buf.buf[0], recv_buf.buf[1], recv_buf.buf[2], recv_buf.buf[3], recv_buf.buf[4], recv_buf.buf[5]);
  }
  /* USER CODE END fpga_communication_entry */
}

int8_t fpga_thread_init(void)
{
    osThreadAttr_t fpga_communication_thread_attributes = {
    .name = "fpga_communication_thread",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityNormal7,
    };

    send_to_fpga_queueHandle = osMessageQueueNew (10, sizeof(struct send_to_fpga_msg), NULL);
    if (send_to_fpga_queueHandle == NULL)
    {
        printf("queue send to fpga create failed\r\n");
        return -1;
    }

    recv_from_fpga_queueHandle = osMessageQueueNew (3, RECV_BUF_LEN, NULL);
    if (recv_from_fpga_queueHandle == NULL)
    {
        printf("queue recv from fpga create failed\r\n");
        return -1;
    }

    osThreadId_t fpga_communication_threadHandle = osThreadNew(fpga_communication_entry, NULL, &fpga_communication_thread_attributes);
    if (fpga_communication_threadHandle == NULL)
    {
        printf("thread fpga communication create failed\r\n");
        return -1;
    }

    return 0;
}

osStatus_t recv_from_fpga_data_get(uint8_t *buf)
{
    return osMessageQueueGet(recv_from_fpga_queueHandle, buf, 0, 0);
}

void make_cmd_to_fpga(uint8_t index, uint8_t *pData, uint8_t size)
{
    osStatus_t stat;
    struct send_to_fpga_msg send_buf = {0};

    send_buf.len = 5;
    send_buf.buf[0] = TX_SYNC_BYTE_H;
    send_buf.buf[1] = TX_SYNC_BYTE_L;
    send_buf.buf[2] = 0;
    send_buf.buf[3] = sendCmd[index].TxLen;
    send_buf.buf[4] = sendCmd[index].cmd;

    for(uint8_t i = 0; i < size; i+=2)
    {
        if(pData == NULL)
        {
            send_buf.buf[send_buf.len + i] = 0;
            send_buf.buf[send_buf.len + i + 1] = 0;
        }
        else if(size == 1)
        {
            send_buf.buf[send_buf.len + i] = pData[i];
        }
        else
        {
            send_buf.buf[send_buf.len + i] = pData[i+1];
            send_buf.buf[send_buf.len + i + 1] = pData[i];
        }
    }
    send_buf.len += size;//size;
    

    for (uint8_t i = 2; i < send_buf.len; i++)
    {
         send_buf.buf[sndCtrl.singleSize[index] - 1] += send_buf.buf[i];
    }

    send_buf.len = sndCtrl.singleSize[index];

    stat = osMessageQueuePut(send_to_fpga_queueHandle, &send_buf, 0, 1000);
    if (stat != osOK)
    {
        printf("spi3 queue put err:%d\r\n", stat);
    }
}

void make_para_for_fpga(uint8_t *pData)
{
    make_cmd_to_fpga(0, &pData[6], sendCmd[0].TxLen);//0
    make_cmd_to_fpga(1, &pData[58], sendCmd[1].TxLen);//1
    make_cmd_to_fpga(8, &pData[34], sendCmd[8].TxLen);//0x14
    make_cmd_to_fpga(11, &pData[54], sendCmd[11].TxLen);//0x18


    // make_cmd_to_fpga(12, &pData[418], 164,1);//0x19
    // make_cmd_to_fpga(12, &pData[52], 2,0);//0x19

    memcpy(&pData[418 + 164], &pData[52], 2);
    make_cmd_to_fpga(12, &pData[418], 166);//0x19


    make_cmd_to_fpga(14, &pData[8], sendCmd[14].TxLen);//0x20
    make_cmd_to_fpga(17, &pData[90],sendCmd[17].TxLen);//0x24
    make_cmd_to_fpga(18, &pData[254], sendCmd[18].TxLen);//0x25
    make_cmd_to_fpga(19, &pData[30], sendCmd[19].TxLen);//0x26
    make_cmd_to_fpga(20, &pData[60], sendCmd[20].TxLen);//0x27


    uint16_t initPos[87];
    for(uint8_t i = 0; i < 82; i++)
    {
        initPos[i] = 3250;
    }
    
    initPos[82] = pData[57] << 8 | pData[56];
    initPos[83] = initPos[85] = 35100;
    initPos[84] = initPos[86] = 5687;

    // make_cmd_to_fpga(24, (uint8_t*)initPos, 164,1);//0x40
    // make_cmd_to_fpga(24, &pData[56], 2,0);//0x40

    // initPos[0] = initPos[2] = 35100;
    // initPos[1] = initPos[3] = 5687;

    make_cmd_to_fpga(24, (uint8_t*)initPos, 174);//0x40
}
