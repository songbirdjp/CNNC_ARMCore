#include "main.h"
#include "BGM_def.h"
#include "SPIDriver.h"
#include "cmsis_os2.h"
#include "init_call.h"
#include "drv_tim.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "bgm_uart.h"
#include "AFCCmd.h"
#include "shell.h" 
#include "drv_spi.h"
#include "spi.h"
#include "IOE.h"
#include "ulog.h"
#include "ethercat.h"
#include "lan9252_app.h"

#define ethercatA2EQueue_LENGTH 16
#define ethercatA2EQueue_SIZE 64*sizeof(uint16_t)

#define ethercatE2AQueue_LENGTH 16
#define ethercatE2AQueue_SIZE 64*sizeof(uint16_t)

#define ExpandGPIOQueue_LENGTH 16
#define ExpandGPIOQueue_SIZE    sizeof(uint16_t)

#define isDoseTriggerQueue_LENGTH 16
#define isDoseTriggerQueue_SIZE    sizeof(uint16_t)

volatile BGMStateMachine_t ARMcurrentState = BGM_STATE_BOOT;
volatile BGMStateMachine_t PLCcurrentState = BGM_STATE_BOOT;
extern TOBJ6000 dataToSend;
osMessageQueueId_t ethercatE2AQueueHandle;
const osMessageQueueAttr_t ethercatE2AQueue_attributes = {
    .name = "ethercatE2AQueue"};

osMessageQueueId_t ethercatA2EQueueHandle;
const osMessageQueueAttr_t ethercatA2EQueue_attributes = {
    .name = "ethercatA2EQueue"};

osMessageQueueId_t ExpandGPIOQueueHandle;
const osMessageQueueAttr_t ExpandGPIOQueue_attributes = {
    .name = "ExpandGPIOQueueHandle"};

osMessageQueueId_t isDoseTriggerQueueHandle;
const osMessageQueueAttr_t isDoseTriggerQueue_attributes = {
    .name = "isDoseTriggerQueueHandle"};

osThreadAttr_t BGMFSM_attributes = {
    .name = "BGM_FSM",
    .stack_size = 4096 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

osThreadAttr_t BGMIOE_attributes = {
    .name = "BGM_IOE",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

static int8_t BGMIOE_thread_init(void)
{
    osThreadId_t BGMIOEHandle = osThreadNew(BGMIOEfunc, NULL, &BGMIOE_attributes);
    if (BGMIOEHandle == NULL)
    {
        printf("thread BGMIOE slave create failed\r\n");
        return -1;
    }

    ExpandGPIOQueueHandle = osMessageQueueNew(ExpandGPIOQueue_LENGTH, ExpandGPIOQueue_SIZE, &ExpandGPIOQueue_attributes);
    if (ExpandGPIOQueueHandle == NULL)
    {
        printf("thread ExpandGPIOQueueHandle failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(BGMIOE_thread_init);

static int8_t BGMFSM_thread_init(void)
{
    osThreadId_t BGMFSMHandle = osThreadNew(BGMFSMfunc, NULL, &BGMFSM_attributes);
    if (BGMFSMHandle == NULL)
    {
        printf("thread BGMFSM slave create failed\r\n");
        return -1;
    }

    ethercatE2AQueueHandle = osMessageQueueNew(ethercatE2AQueue_LENGTH, ethercatE2AQueue_SIZE, &ethercatE2AQueue_attributes);
    if (ethercatE2AQueueHandle == NULL)
    {
        printf("thread ethercatE2AQueue failed\r\n");
        return -1;
    }

    ethercatA2EQueueHandle = osMessageQueueNew(ethercatA2EQueue_LENGTH, ethercatA2EQueue_SIZE, &ethercatA2EQueue_attributes);
    if (ethercatA2EQueueHandle == NULL)
    {
        printf("thread ethercatRecvQueue failed\r\n");
        return -1;
    }

    ethercatA2EQueueHandle = osMessageQueueNew(ethercatA2EQueue_LENGTH, ethercatA2EQueue_SIZE, &ethercatA2EQueue_attributes);
    if (ethercatA2EQueueHandle == NULL)
    {
        printf("thread ethercatRecvQueue failed\r\n");
        return -1;
    }

    isDoseTriggerQueueHandle = osMessageQueueNew(isDoseTriggerQueue_LENGTH, isDoseTriggerQueue_SIZE, &isDoseTriggerQueue_attributes);
    if (isDoseTriggerQueueHandle == NULL)
    {
        printf("thread ethercatRecvQueue failed\r\n");
        return -1;
    }
    return 0;
}
INIT_APP_EXPORT(BGMFSM_thread_init);

static int8_t ioe_irq_callback(void)
{
    uint16_t ExpandGPIOData = IOE_GPIORead();

    osMessageQueuePut(ExpandGPIOQueueHandle,&ExpandGPIOData,0,0);    
}
uint16_t isDoseTrigger = 0;
static int8_t DoseTrigger_irq_callback(void)
{
    //osStatus_t tmp;
    // isDoseTrigger = 1;
    // tmp = osMessageQueuePut(isDoseTriggerQueueHandle,&isDoseTrigger,0,0);  
    // if(tmp != osOK)
    // {
    //     LOG_E("isDoseTriggerQueueHandle error = %d\r\n",tmp);
    // }
    // isDoseTrigger = 0;  
    TriggerOutCtrl(BGMTriggerPin, 10, 4000);
}
uint16_t RtDataUP[64] = {0};
static void BGMIOEfunc(void *argument)
{   
     //osMutexId_t dataMutex;
    int IOE_Status = IOE_Init();
    IOE_GPIORead();//for stable
    uint16_t ExpandGPIOValue;
    uint16_t ExpandGPIOValuePrev;
    EPSEnable(1);
    VPSEnable(1);
    ExpandGPIOStatus_t exGPIOState;
    //uint16_t RtDataUP[64] = {0};
    printf("IOE_Status = %d\r\n",IOE_Status);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET);//PRF Enable
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);//Lv Interlock Enable
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);//Hv Interlock Enable
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);//Trig Inhibit Disable

    gpio_pin_irq_callback_register("GPIOG_6", ioe_irq_callback);
    gpio_pin_irq_callback_register("GPIOA_8", DoseTrigger_irq_callback);
    for (;;)
    {
        if(osMessageQueueGetCount(ExpandGPIOQueueHandle) != 0)
        {
            osMessageQueueGet(ExpandGPIOQueueHandle,&ExpandGPIOValue,0,0);
            exGPIOState = IOE_ExpandGPIODataParse(ExpandGPIOValue);
        }
        //RtDataUP[10] = ExpandGPIOValue;
        //(dataMutex, osWaitForever);
        dataToSend.DataIn1[1] = ExpandGPIOValue;
        //osMutexRelease(dataMutex);
       // RtDataUP[11] = BGM_ReadModInterlocks();
        //ARMSendToECATQueueSend(RtDataUP);
        if(BGM_STATE_WORK == ARMcurrentState)
        {
            // if(ExpandGPIOValue != 0xff) 
            // {
            //     BGM_ReadAllInterlocks();
            //     ARMcurrentState = BGM_STATE_TERMINATE;
            //     LOG_I("TERMINATE with Interlock");
            // }
            ;
        }
        osDelay(1);
    }
}

struct cmd_object AFCCmdtest;
uint8_t DoseHandshakeOK = 0;
static void BGMFSMfunc(void *argument)
{
    for (;;)
    {
        BGMFiniteStateMachine();
        // if(osMessageQueueGetCount(ethercatE2AQueueHandle) != 0)
        //BGMEthercatDataParse(ECATSendToARMQueueRecv());// parse data from bus and send to 422
    }
}


void ARMSendToECATQueueSend(uint16_t* value_to_send)
{
    osStatus_t status = osMessageQueuePut(ethercatA2EQueueHandle, value_to_send, 0, osWaitForever);
    if (status != osOK)
    {
        printf("Failed to send data ethercat.\n");
    }
}

uint16_t* ARMSendToECATQueueRecv(void)
{
    static uint16_t received_value[64] = {0};
    osStatus_t status = osMessageQueueGet(ethercatA2EQueueHandle, &received_value, NULL, osWaitForever); 
    if (status == osOK)
    {
        printf("hhhhhhhhhhh: %x\n", received_value[14]);
    }
    return received_value;
}


void ECATSendToARMQueueSend(uint16_t *valueOut)
{
    uint16_t value_to_send[64];
    memcpy(value_to_send,valueOut,64 * sizeof(uint16_t));
    printf("xxxxxxxxxxxxvalue_to_send = %d\r\n",value_to_send[33]);
    osStatus_t status = osMessageQueuePut(ethercatE2AQueueHandle,value_to_send, 0, 0);
    if (status != osOK)
    {
        printf("111status = %d \n",status);
    }
}

uint16_t AFCMotorEn,AFCMotorEnPrev =0;
uint16_t AFCMotorInit,AFCMotorInitOK =0;
uint16_t AFCMotorPos,AFCMotorPosPrev =0;
// uint16_t PLC_FSM_From_Ethercat,PLC_FSM_From_Ethercat_Prev = 0;
void BGMEthercatDataParse(uint16_t * EcatDataOut)
{
    static uint16_t dataToParse[64];
    uint8_t RtDataParseToCmd[16];
    memcpy(dataToParse,EcatDataOut,64 * sizeof(uint16_t));
    AFCMotorPos = dataToParse[35];
    if(AFCMotorPos != AFCMotorPosPrev)
    {
        RtDataParseToCmd[0] = 0x41;
        RtDataParseToCmd[1] = 0x03;
        RtDataParseToCmd[2] = (AFCMotorPos >> 8) & 0xFF; 
        RtDataParseToCmd[3] = AFCMotorPos & 0xFF;        
        BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,RtDataParseToCmd,4);
        AFCMotorPosPrev = AFCMotorPos;
    }
}

uint16_t* ECATSendToARMQueueRecv(void)
{
    static uint16_t received_value[64];
    
        osStatus_t status = osMessageQueueGet(ethercatE2AQueueHandle, received_value,0, osWaitForever);
        // for(uint8_t i=0;i<63;i++)
        // {
        //     LOG_I("received_value = %d\r\n",received_value[i]); 
        // }
        if (status != osOK)
        {
            printf("1Queue status = %d \r\n",status);
        }

    return received_value;
}


uint8_t isDoseSetOK = 0;
uint8_t isPRFOK = 0;
uint8_t isDoseModeOK = 0;
uint8_t isdataCaliLock = 0;
uint8_t isdataCaliUnlock = 0;
uint8_t isBeamDataSetLock = 0;
uint8_t isBeamDataSetUnlock = 0;
uint8_t isDoseReady = 0;
void BGMFiniteStateMachine(void)
{
    uint16_t ethercatALStatus;
    uint8_t cmdToCheckFSM[2] = {0xc0,0x01};
    const uint32_t AFCTriggerTime = 5;
    uint16_t isDoseTrig = 0;
    uint8_t cmdToCheck[2] = {0xc0,0x01};
    dataToSend.DataIn1[0] = ARMcurrentState;
    if(BGM_STATE_BOOT == PLCcurrentState)
    {
        ARMcurrentState = BGM_STATE_BOOT;
    }
    switch (ARMcurrentState)
    {
    case BGM_STATE_BOOT:
        // dataToSend.DataIn3[0] = 1;//maintain cali para lock
        // dataToSend.DataIn3[1] = 1;//maintain beam para lock
        // BGM_LockDoseCaliPara(BGM_UART_DOSE1,1);
        // BGM_LockDoseCaliPara(BGM_UART_DOSE2,1);
        // BGM_LockBeamData(BGM_UART_DOSE1,1);
        // BGM_LockBeamData(BGM_UART_DOSE2,1);//lock all data
        isDoseSetOK = 0;
        isPRFOK = 0;
        isDoseModeOK = 0;
        isdataCaliLock = 0;
        isdataCaliUnlock = 0;
        isBeamDataSetLock = 0;
        isBeamDataSetUnlock = 0;
        isDoseReady = 0;
        dataToSend.DataIn5[0] = 0;
        dataToSend.DataIn5[1] = 0;
        if(BGM_STATE_INIT == PLCcurrentState)//PLC change BOOT to INIT need 5 seconds
        {
            dataToSend.DataIn3[0] = 0;//maintain cali para lock
            dataToSend.DataIn3[1] = 0;//maintain beam para lock
            BGM_LockDoseCaliPara(BGM_UART_DOSE1,1);
            osDelay(1);
            BGM_LockDoseCaliPara(BGM_UART_DOSE2,1);
            osDelay(1);
            BGM_LockBeamData(BGM_UART_DOSE1,1);
            osDelay(1);
            BGM_LockBeamData(BGM_UART_DOSE2,1);//lock all data
            ARMcurrentState = BGM_STATE_INIT;
        }
        osDelay(100);
        break;
    case BGM_STATE_INIT:
           //Handshake with dose board
        if(0 == BGM2Dose_Handshake(BGM_UART_DOSE1))
        {
            osDelay(1);
            if(0 == BGM2Dose_Handshake(BGM_UART_DOSE2))
            {
                // LOG_I("Dose 1&2 Handshake cmd sent OK!");
                ARMcurrentState = BGM_STATE_IDLE;
                BGM_CtrlDoseBoardFSM(1);//handshake ok ,change Dose board FSM to idle
            }
        }
        else
        {
            // LOG_I("Dose 1&2 Handshake cmd sent error!");
        }
        //Handshake with dose board complete
        osDelay(100);
        break;
    case BGM_STATE_IDLE://plc change idle needs cooling & water flow
        if(0 == isdataCaliUnlock)
        {
            uint8_t zeroCmd[2] = {0xc1,0x02};
            BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown, zeroCmd,2);
            BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown, zeroCmd,2);
            BGM_LockDoseCaliPara(BGM_UART_DOSE1,0);// unlock dose board cali parameter 
            BGM_LockDoseCaliPara(BGM_UART_DOSE2,0);// unlock dose board cali parameter 
            isdataCaliUnlock = 1 ;//tell PLC ready to  set cali parameter 
            LOG_I("BGM_STATE_IDLE\r\n");
        }
        if(0 == isBeamDataSetUnlock)
        {
            BGM_LockBeamData(BGM_UART_DOSE1,0);
            BGM_LockBeamData(BGM_UART_DOSE2,0);//unlock all data;
            isBeamDataSetUnlock = 1;
            // LOG_I("BGM_STATE_PREPARE\r\n");
        }
        //waiting for adc dac parameter
        // when plc have sent the parameter,ARM will send them to dose automatically in {BGMEthercatDataParsePoint}
        //then check dose board 's FSM
        // BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown, cmdToCheckFSM,2);
        // BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown, cmdToCheckFSM,2);
        if((isDoseSetOK)&&(isPRFOK)&&(isDoseModeOK))
        {
            if(BGM_STATE_PREPARE == PLCcurrentState)//PLC FSM change to prepare when HMI have sent beam parameters,then arm will lock the data // press button 
            {
                osDelay(100);//waiting for ethercat write data
                //todo :add some read back logic
                if(0 == isDoseReady)
                {
                    BGM_LockDoseCaliPara(BGM_UART_DOSE1,1);
                    BGM_LockDoseCaliPara(BGM_UART_DOSE2,1);
                    BGM_LockBeamData(BGM_UART_DOSE1,1);
                    BGM_LockBeamData(BGM_UART_DOSE2,1);//lock all data;
                    BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_PREPARE);//change dose board fsm to ready
                    BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown, cmdToCheckFSM,2);
                    isDoseReady = 1;
                    // LOG_I("BGM_STATE_READY\r\n");
                   // ARMcurrentState = BGM_STATE_PREPARE;
                }
            }
        }
        osDelay(1000);
        break;
    case BGM_STATE_PREPARE:
        if(BGM_STATE_READY == PLCcurrentState)
        {
            ARMcurrentState = BGM_STATE_READY;
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_READY);
        }
        if(BGM_STATE_TERMINATE == PLCcurrentState)
        {
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_FAULT);//make dose board to fault
            ARMcurrentState = BGM_STATE_TERMINATE;
        }
        osDelay(1000);
        break;
    case BGM_STATE_READY:
        if(BGM_STATE_WORK == PLCcurrentState) //Mod FSM change to TRIG，PLC FSM change to WORK,then change arm to work
        {
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_RADIATION);//change dose board fsm to radiation
            ARMcurrentState = BGM_STATE_WORK;
        }
        if(BGM_STATE_INTERRUPT == PLCcurrentState)
        {
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_FAULT);//make dose board to fault
            ARMcurrentState = BGM_STATE_INTERRUPT;
        }
        if(BGM_STATE_TERMINATE == PLCcurrentState)
        {
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_FAULT);//make dose board to fault
            ARMcurrentState = BGM_STATE_TERMINATE;
        }
        osDelay(500);
        break;
    case BGM_STATE_WORK:
        // if(osMessageQueueGetCount(isDoseTriggerQueueHandle) != 0)
        // {   
        //     osStatus_t tmp;
        //     tmp = osMessageQueueGet(isDoseTriggerQueueHandle,&isDoseTrig,0,0);
        //     if( tmp != osOK)
        //     {
        //         LOG_E("QueueGetisDoseTrig = %d\r\n",tmp);
        //     }
            // TriggerOutCtrl(BGMTriggerPin, AFCTriggerTime, 4000);
            // LOG_I("TriggerOutCtrl\r\n");
            //BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown, cmdToCheck,2); //Polling to check if dose board complete
            //BGM_RtBeamCtrl();
            //when dose board change fsm to complete,whole trig process complete
            // ARM will be changed to BGM_STATE_COMPLETE in function {dose_state_control_parse}
        // }
        // else
        // {
            BGM_RtBeamCtrl();
            osDelay(100);
        // }
        if(BGM_STATE_TERMINATE == PLCcurrentState)
        {
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_FAULT);
            ARMcurrentState = BGM_STATE_TERMINATE;
        }//two ways to dump out of work to terminate : 1.exIO interlock happened  2.PLC change to terminate
        //only dose board will make FSM to complete
        if(BGM_STATE_INTERRUPT == PLCcurrentState)
        {
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_FAULT);
            ARMcurrentState = BGM_STATE_INTERRUPT;
        }
        break;
    case BGM_STATE_TERMINATE:
        if(BGM_STATE_IDLE == PLCcurrentState)//PLC change top IDLE
        {
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_IDLE);//make Dose change to IDLE
            ARMcurrentState = BGM_STATE_IDLE;
            isDoseSetOK = 0;
            isPRFOK = 0;
            isDoseModeOK = 0;
            isdataCaliLock = 0;
            isdataCaliUnlock = 0;
            isBeamDataSetLock = 0;
            isBeamDataSetUnlock = 0;//enable to write parameter again
            isDoseReady = 0;
        }
        // LOG_I("BGM_STATE_TERMINATE\r\n");
        osDelay(1000);
        break;
    case BGM_STATE_COMPLETE:
        if(BGM_STATE_IDLE == PLCcurrentState)
        {
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_IDLE);
            ARMcurrentState = BGM_STATE_IDLE;
        }
        if(BGM_STATE_COMPLETE == PLCcurrentState)
        {   
            BGM_RtBeamCtrl();
            isDoseReady = 0;
            isDoseSetOK = 0;
            isPRFOK = 0;
            isDoseModeOK = 0;
            isdataCaliLock = 0;
            isdataCaliUnlock = 0;
            isBeamDataSetLock = 0;
            isBeamDataSetUnlock = 0;//enable to write parameter again
            // BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_PREPARE);
            // ARMcurrentState = BGM_STATE_PREPARE;
            BGM_LockBeamData(BGM_UART_DOSE1,0);
            BGM_LockBeamData(BGM_UART_DOSE2,0);
            
        }
        if(BGM_STATE_PREPARE == PLCcurrentState)
        {
            ARMcurrentState = BGM_STATE_PREPARE;
            isDoseSetOK = 1;
            isPRFOK = 1;
            isDoseModeOK = 1;
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_PREPARE);
        }
        // LOG_I("BGM_STATE_COMPLETE\r\n");
        osDelay(1000);
        break;
    case BGM_STATE_INTERRUPT:
        if(BGM_STATE_READY == PLCcurrentState)
        {
            ARMcurrentState = BGM_STATE_READY;
            BGM_CtrlDoseBoardFSM(Dose_FSM_STATE_READY);
        }
        if(BGM_STATE_TERMINATE == PLCcurrentState)
        {
            ARMcurrentState = BGM_STATE_TERMINATE;
        }
        osDelay(1000);
        break;
    default:
        ARMcurrentState = BGM_STATE_BOOT;
        break;
    }
}

GPIOConfig TriggerPinTable[] = {
    {GPIOG, GPIO_PIN_14}, // AFCTriggerPin
    {GPIOC, GPIO_PIN_7},  // QAMTriggerPin
    {GPIOD, GPIO_PIN_13}  // BGMTriggerPin
};
void TriggerOutCtrl(TriggerIO_Name TriggerPin, uint32_t _triggerHighTime_us, uint32_t _triggerLowTime_us)
{
    GPIOConfig *gpioConfig = &TriggerPinTable[TriggerPin];
    HAL_GPIO_WritePin(gpioConfig->GPIOx, gpioConfig->GPIO_Pin, GPIO_PIN_SET);
    delay_us(10);
   // osDelay(2);
    HAL_GPIO_WritePin(gpioConfig->GPIOx, gpioConfig->GPIO_Pin, GPIO_PIN_RESET);
    // BGM_RtBeamCtrl();
    // delay_us(_triggerLowTime_us);
}
