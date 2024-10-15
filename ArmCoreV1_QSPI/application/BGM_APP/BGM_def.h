#ifndef __BGM_DEF_H__
#define __BGM_DEF_H__
/***************FSM State Definations************** */
// #include "ethercat.h"
// #include "lan9252_app.h"
// #include "lan9252_appObjects.h"
#include "IOE.h"
#include "AFCCmd.h"
#include "bgm_uart.h"

typedef enum {
    AFCTriggerPin,  //  GPIOG Pin 14
    QAMTriggerPin,  //  GPIOC Pin 7
    BGMTriggerPin   //  GPIOD Pin 13
} TriggerIO_Name;

typedef struct {
    GPIO_TypeDef *GPIOx;  
    uint16_t GPIO_Pin;   
} GPIOConfig;

typedef enum {
    BGM_STATE_BOOT,//0
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
    BGM_STATE_INTERRUPT,//16
} BGMStateMachine_t;

typedef struct {
     ExpandGPIOStatus_t exGPIODetect;
     uint8_t LvOKDetect;
     uint8_t HvEnDetect;
     uint8_t ModArcDetect;
     uint8_t ModHvONDetect;
     uint8_t ModSumDetect;
     uint8_t ModTrigONDetect;
     uint8_t EmergencyDetect;
     uint8_t PulseInhibitDetect;
     uint8_t ModTriggerInhibitDetect;
     uint8_t Dose1Detect;
     uint8_t Dose2Detect;
} BGMInterlocksDetect_t;

// typedef struct {  
//     uint16_t WaterSwData;   
//     uint16_t WaterCoolingData;  
//     uint16_t SF6Data;    
//     uint16_t EPSInterlockData;
//     uint16_t VPSInterlockData;
//     uint16_t MODInterlockData;
//     uint16_t RTMInterlockData;
//     uint16_t PRFCtrlData;
//     uint16_t AFCCtrlData;
// } BGMInterlockData_t;//Ethercat to ARM

// typedef struct {  
   
// } DoseDataOut_t;////Ethercat to ARM

// typedef struct {  
//     BGMInterlockData_t E2ABGMInterlock;
//     BGMStateMachine_t PLC2ARM_FSM;
    
// } ECATDataOut_t;//Ethercat to ARM

// typedef struct {  
//     BGMInterlockData_t A2EBGMInterlock;  
//     BGMStateMachine_t ARM2PLC_FSM;
// } ECATDataIn_t;//ARM to Ethercat


typedef enum{
    Dose_FSM_STATE_INIT = 0,
    Dose_FSM_STATE_IDLE,
    Dose_FSM_STATE_DUMMY,
    Dose_FSM_STATE_PREPARE,
    Dose_FSM_STATE_READY,
    Dose_FSM_STATE_RADIATION,
    Dose_FSM_STATE_COMPLETE,
    Dose_FSM_STATE_FAULT,
    Dose_FSM_STATE_MAX
}DoseFsmState_t;


static void BGMIOEfunc(void *argument);
static void BGMFSMfunc(void *argument);
void TriggerOutCtrl(TriggerIO_Name TriggerPin, uint32_t _triggerHighTime_us, uint32_t _triggerLowTime_us);
void ARMSendToECATQueueSend(uint16_t* value_to_send);
uint16_t* ARMSendToECATQueueRecv(void);
void BGMEthercatDataParse(uint16_t * EcatDataOut);
void ECATSendToARMQueueSend(uint16_t *valueOut);
void BGM_SendCmd(enum uart_id uartID,UARTCmdType_t cmdType, uint8_t *cmdData,uint8_t len);
uint16_t* ECATSendToARMQueueRecv(void);
int BGM2Dose_Handshake(enum uart_id uartID);
int BGM2AFC_Handshake(void);
BGMInterlocksDetect_t BGM_ReadAllInterlocks(void);
uint16_t BGM_ReadModInterlocks(void);

void BGM_CtrlDoseBoardFSM(DoseFsmState_t doseFSM);
void BGM_SetDoseBoardPRF(enum uart_id uartID,uint8_t prfVal);
void BGM_SetDoseBoardDose(enum uart_id uartID,uint16_t doseVal);
void BGM_LockBeamData(enum uart_id uartID,uint8_t _lockStatus);
void BGM_LockDoseCaliPara(enum uart_id uartID,uint8_t _lockStatus);

void BGM_SetDoseMode(enum uart_id uartID,uint8_t doseMode);
void BGM_SetDoseBoardKadc(enum uart_id uartID,uint32_t kadcVal);
void BGM_SetDoseBoardDAC(enum uart_id uartID,uint32_t dacVal);
// void ECATSendToARMQueueSend(TOBJ7010 *valueOut);
#endif /* __BGM_DEF_H__ */