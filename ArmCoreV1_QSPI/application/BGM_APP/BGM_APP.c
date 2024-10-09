#include "main.h"
#include "BGM_def.h"
#include "SPIDriver.h"
#include "ethercat.h"
#include "applInterface.h"
#include "lan9252_port.h"
#include "lan9252_app.h"
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

#define ethercatA2EQueue_LENGTH 16
#define ethercatA2EQueue_SIZE 64*sizeof(uint16_t)

#define ethercatE2AQueue_LENGTH 16
#define ethercatE2AQueue_SIZE 64*sizeof(uint16_t)

#define ExpandGPIOQueue_LENGTH 16
#define ExpandGPIOQueue_SIZE    sizeof(uint16_t)

static BGMStateMachine_t currentState = BGM_STATE_BOOT;

osMessageQueueId_t ethercatE2AQueueHandle;
const osMessageQueueAttr_t ethercatE2AQueue_attributes = {
    .name = "ethercatE2AQueue"};

osMessageQueueId_t ethercatA2EQueueHandle;
const osMessageQueueAttr_t ethercatA2EQueue_attributes = {
    .name = "ethercatA2EQueue"};

osMessageQueueId_t ExpandGPIOQueueHandle;
const osMessageQueueAttr_t ExpandGPIOQueue_attributes = {
    .name = "ExpandGPIOQueueHandle"};

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

    return 0;
}
INIT_APP_EXPORT(BGMFSM_thread_init);

static int8_t ioe_irq_callback(void)
{
    uint16_t ExpandGPIOData = IOE_GPIORead();

    osMessageQueuePut(ExpandGPIOQueueHandle,&ExpandGPIOData,0,0);    
}

static void BGMIOEfunc(void *argument)
{   
    int IOE_Status = IOE_Init();
    IOE_GPIORead();//for stable
    uint16_t ExpandGPIOValue;
    uint16_t ExpandGPIOValuePrev;
    EPSEnable(1);
    VPSEnable(1);
    ExpandGPIOStatus_t exGPIOState;
    uint16_t RtDataUP[64] = {0};
    printf("IOE_Status = %d\r\n",IOE_Status);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET);//PRF Enable
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);//Lv Interlock Enable
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);//Hv Interlock Enable
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);//Trig Ibhibit Disable

    gpio_pin_irq_callback_register("GPIOG_6", ioe_irq_callback);

    for (;;)
    {
        if(osMessageQueueGetCount(ExpandGPIOQueueHandle) != 0)
        {
            osMessageQueueGet(ExpandGPIOQueueHandle,&ExpandGPIOValue,0,0);
            exGPIOState = IOE_ExpandGPIODataParse(ExpandGPIOValue);
        }
        RtDataUP[10] = ExpandGPIOValue;
        RtDataUP[11] = BGM_ReadModInterlocks();
        if(ExpandGPIOValue!=ExpandGPIOValuePrev)
        {
            ARMSendToECATQueueSend(RtDataUP);
            ExpandGPIOValuePrev = ExpandGPIOValue;
            //printf("ExpandGPIOValuePrev = %x\t%x\r\n",ExpandGPIOValuePrev,ExpandGPIOValue);
        }
        osDelay(1);
    }
}

struct cmd_object AFCCmdtest;
static void BGMFSMfunc(void *argument)
{
    static uint8_t doseTest[5]= {0x42,0x01,0xEE,0xAA};
    uint32_t AFCTriggerTime = 4;
    uint16_t ExpandGPIOValue;
    AFCCmdtest.id.byte = 0x80;
    AFCCmdtest.type = 0x02;
    AFCCmdtest.len = 3;
    AFCCmdtest.data = doseTest;
    // BGM2Dose_Handshake(BGM_UART_DOSE1);
    for (;;)
    { 
        //BGM2Dose_Handshake(BGM_UART_DOSE1);
        //uart_cmd_write(BGM_UART_DOSE1,&AFCCmdtest);
        // BGM_ReadAllInterlocks();
        // TriggerOutCtrl(BGMTriggerPin, AFCTriggerTime, 4000);
        // TriggerOutCtrl(AFCTriggerPin, AFCTriggerTime, 4000);
        //uart_cmd_write(BGM_UART_DOSE1,&AFCCmdtest);
        // ECATSendToARMQueueRecv();
        // BGMFiniteStateMachine();
        if(osMessageQueueGetCount(ethercatE2AQueueHandle) != 0)
        {
            BGMEthercatDataParse(ECATSendToARMQueueRecv());
        }
        // printf("test\r\n");
        osDelay(1);
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
    currentState = (BGMStateMachine_t)dataToParse[9];//PLC FSM
    AFCMotorPos = dataToParse[35];
    // printf("AFCMotorInitPrev = %d\r\n",AFCMotorInitPrev);
    if(AFCMotorPos != AFCMotorPosPrev)
    {
        RtDataParseToCmd[0] = 0x41;
        RtDataParseToCmd[1] = 0x03;
        RtDataParseToCmd[2] = (AFCMotorPos >> 8) & 0xFF; 
        RtDataParseToCmd[3] = AFCMotorPos & 0xFF;        
        BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,RtDataParseToCmd);
        printf("dataToParse35 = %d\r\n",dataToParse[35]);
        AFCMotorPosPrev = AFCMotorPos;
    }
    
}

uint16_t* ECATSendToARMQueueRecv(void)
{
    static uint16_t received_value[64];
    if(osMessageQueueGetCount(ethercatE2AQueueHandle) != 0)
    {
        osStatus_t status = osMessageQueueGet(ethercatE2AQueueHandle, received_value,0, 0);
        // printf("received_value[33] = %d \r\n",received_value[33]); 
        if (status != osOK)
        {
            printf("1Queue status = %d \r\n",status);
        }
    }
    return received_value;
}



void BGMFiniteStateMachine(void)
{
    uint16_t ethercatALStatus;
    switch (currentState)
    {
        /*    BGM_STATE_BOOT,//0
    BGM_STATE_INIT,//1
    BGM_STATE_IDLE,//2
    BGM_STATE_PRELIMINARY,//3
    BGM_STATE_PREPARE,//4
    BGM_STATE_READY,//5
    BGM_STATE_WORK,//6

    BGM_STATE_PARK = 10,
    BGM_STATE_MANUAL,//11
    BGM_STATE_COMPLETE,//12
    BGM_STATE_SHUTDOWN,//13
    BGM_STATE_POWERSAVER,//14
    BGM_STATE_TERMINATE,//15
    BGM_STATE_INTERRUPT,//16 */
    case BGM_STATE_BOOT:
        printf("BGM_STATE_BOOT\r\n");
        osDelay(1000);
        break;
    case BGM_STATE_INIT:
        printf("BGM_STATE_INIT\r\n");
        // initStateHandler();
        osDelay(1000);
        // currentState = BGM_STATE_IDLE;
        break;
    case BGM_STATE_IDLE:
        printf("BGM_STATE_IDLE\r\n");
        // idleStateHandler();
        //osDelay(1000);
        // currentState = BGM_STATE_INIT;
        break;

    case BGM_STATE_PRELIMINARY:
        // preliminaryStateHandler();
        printf("BGM_STATE_PRELIMINARY\r\n");
        osDelay(1000);
        break;

    case BGM_STATE_PREPARE:
        // prepareStateHandler();
        printf("BGM_STATE_PREPARE\r\n");
        osDelay(1000);
        break;

    case BGM_STATE_READY:
        // readyStateHandler();
        printf("BGM_STATE_READY\r\n");
        osDelay(1000);
        break;

    case BGM_STATE_WORK:
        // workStateHandler();
        printf("BGM_STATE_WORK\r\n");
        osDelay(1000);
        break;

    case BGM_STATE_POWERSAVER:
        // powerSaverStateHandler();
        printf("BGM_STATE_POWERSAVER\r\n");
        osDelay(1000);
        break;

    case BGM_STATE_TERMINATE:
        // terminateStateHandler();
        printf("BGM_STATE_TERMINATE\r\n");
        osDelay(1000);
        break;

    case BGM_STATE_INTERRUPT:
        // interruptStateHandler();
        printf("BGM_STATE_INTERRUPT\r\n");
        osDelay(1000);
        break;

    case BGM_STATE_PARK:
        // parkStateHandler();
        printf("BGM_STATE_PARK\r\n");
        osDelay(1000);
        break;

    case BGM_STATE_COMPLETE:
        // completeStateHandler();
        printf("BGM_STATE_COMPLETE\r\n");
        osDelay(1000);
        break;

    default:
        currentState = BGM_STATE_BOOT;
        break;
    }
}

void initStateHandler(void)
{
    ;
}
void idleStateHandler(void)
{
    ;
}
void preliminaryStateHandler(void)
{
    ;
}
void prepareStateHandler(void)
{
    ;
}
void readyStateHandler(void)
{
    ;
}
void workStateHandler(void)
{
    ;
}
void powerSaverStateHandler(void)
{
    ;
}
void terminateStateHandler(void)
{
    ;
}
void interruptStateHandler(void)
{
    ;
}
void parkStateHandler(void)
{
    ;
}
void completeStateHandler(void)
{
    ;
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
    delay_us(_triggerHighTime_us);
    HAL_GPIO_WritePin(gpioConfig->GPIOx, gpioConfig->GPIO_Pin, GPIO_PIN_RESET);
    delay_us(_triggerLowTime_us);
}
