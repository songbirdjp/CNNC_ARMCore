#include "main_app.h"
#include "websocket.h"
#include "init_call.h"
// #include "ethercat.h"
#include "tcp_tasks.h"
#include "websocket_console.h"
// #include "lan9252_app.h"
// #include "BGM_def.h"
#include "ulog.h"
#define DATA_PROCESS_LAN_EVENT      (1<<0)
#define DATA_PROCESS_TCP_EVENT      (1<<1)
#define DATA_PROCESS_FPGA_EVENT     (1<<2)
static osEventFlagsId_t data_process_eventHandle = NULL;

// extern BGMStateMachine_t ARMcurrentState;
// extern BGMStateMachine_t PLCcurrentState;
// static TOBJ7010 EcatDataOutPrev = {0};
// TOBJ6000 dataToSend = {0};
// void BGMEthercatDataParsePoint(TOBJ7010 *EcatDataOut)
// {
    
//     uint32_t _ecatADCUART1Val = EcatDataOut->DataOut3[3] | (EcatDataOut->DataOut3[4] << 16);
//     uint32_t _ecatADCUART2Val = EcatDataOut->DataOut3[5] | (EcatDataOut->DataOut3[6] << 16);
//     uint32_t _ecatADCUART1ValPrev;
//     uint32_t _ecatADCUART2ValPrev;
//     if (memcmp(&EcatDataOutPrev, EcatDataOut,sizeof(TOBJ7010)) != 0)
//     {
//         LOG_I("BGMEthercatDataParsePoint");
//         if(EcatDataOut->DataOut1[0] != EcatDataOutPrev.DataOut1[0])  
//         {
//             // ARMcurrentState = (BGMStateMachine_t)EcatDataOut->DataOut1[0];
//             PLCcurrentState = (BGMStateMachine_t)EcatDataOut->DataOut1[0];
//             LOG_I("PCL FSM change to %d\r\n",PLCcurrentState);
//             //BGM_CtrlDoseBoardFSM((DoseFsmState_t)EcatDataOut->DataOut1[0]);
//             EcatDataOutPrev.DataOut1[0] =  EcatDataOut->DataOut1[0];
//         }
//             if(ARMcurrentState  ==  BGM_STATE_IDLE)
//             {
//             //prf set to dose by plc
//                     if(EcatDataOut->DataOut3[0] != EcatDataOutPrev.DataOut3[0])  
//                     {
//                         BGM_SetDoseBoardPRF(BGM_UART_DOSE1,EcatDataOut->DataOut3[0]);
//                         LOG_I("Set PRF to %d\r\n",EcatDataOut->DataOut3[0]);
//                         EcatDataOutPrev.DataOut3[0] =  EcatDataOut->DataOut3[0];
//                     }
//                     //Dose Meter set to Dose Board by plc
//                     if(EcatDataOut->DataOut3[1] != EcatDataOutPrev.DataOut3[1])  
//                     {
//                         BGM_SetDoseBoardDose(BGM_UART_DOSE1,EcatDataOut->DataOut3[1]);
//                         //BGM_SetDoseBoardDose(BGM_UART_DOSE2,(EcatDataOut->DataOut3[1]));
//                         LOG_I("Set Dose Meter to %d\r\n",EcatDataOut->DataOut3[1]);
//                         EcatDataOutPrev.DataOut3[1] =  EcatDataOut->DataOut3[1];
//                     }

//                     //Dose Mode set to Dose Board by plc
//                     if(EcatDataOut->DataOut3[7] != EcatDataOutPrev.DataOut3[7])  
//                     {
//                         BGM_SetDoseMode(BGM_UART_DOSE1,EcatDataOut->DataOut3[7]);
//                         BGM_SetDoseMode(BGM_UART_DOSE2,EcatDataOut->DataOut3[7]);
//                         LOG_I("Set Dose Mode to %d\r\n",EcatDataOut->DataOut3[7]);
//                         EcatDataOutPrev.DataOut3[7] =  EcatDataOut->DataOut3[7];
//                     }
//                     //DAC set to Dose Board by plc
//                     if(EcatDataOut->DataOut3[2] != EcatDataOutPrev.DataOut3[2])  
//                     {
//                         BGM_SetDoseBoardDAC(BGM_UART_DOSE1,EcatDataOut->DataOut3[2]);
//                         LOG_I("Set DAC to %d\r\n",EcatDataOut->DataOut3[2]);
//                         EcatDataOutPrev.DataOut3[2] =  EcatDataOut->DataOut3[2];
//                     }
//                     //ADC1 set to Dose Board 1 by plc
//                     if(_ecatADCUART1Val != _ecatADCUART1ValPrev)  
//                     {
//                         BGM_SetDoseBoardKadc(BGM_UART_DOSE1,_ecatADCUART1Val);
//                         LOG_I("Set ADC1 to %ld\r\n",_ecatADCUART1Val);
//                         _ecatADCUART1ValPrev = _ecatADCUART1Val;
//                         //BGM_LockDoseCaliPara(BGM_UART_DOSE1,1);
//                         //BGM_LockDoseCaliPara(BGM_UART_DOSE2,1);
//                         //BGM_CtrlDoseBoardFSM((DoseFsmState_t)3);//change Dose FSM to Prepare
//                     }
//                     //ADC2 set to Dose Board 2 by plc
//                     if(_ecatADCUART2Val != _ecatADCUART2ValPrev)  
//                     {
//                         BGM_SetDoseBoardKadc(BGM_UART_DOSE2,_ecatADCUART2Val);
//                         LOG_I("Set ADC2 to %ld\r\n",_ecatADCUART2Val);
//                         _ecatADCUART2ValPrev = _ecatADCUART2Val;
//                     }
                
//             }
//               //AFC POS set to AFC by plc
//             if(EcatDataOut->DataOut4[2] != EcatDataOutPrev.DataOut4[2])  
//             {
//                 uint8_t cmd[4] = {0x41,0x03,0x00,0x00};
//                 //BGM_SetDoseBoardDAC(BGM_UART_DOSE1,EcatDataOut->DataOut3[2]);
//                 cmd[2] = EcatDataOut->DataOut4[2];
//                 cmd[3] = (EcatDataOut->DataOut4[2]) >> 8;
//                 BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,cmd,4); 
//                 LOG_I("Set AFC pos to %d\r\n",EcatDataOut->DataOut4[2]);
//                 EcatDataOutPrev.DataOut4[2] =  EcatDataOut->DataOut4[2];
//             }
        
//         memcpy(&EcatDataOutPrev, EcatDataOut,sizeof(TOBJ7010));
//     }
// }
// // TOBJ6000 dataToSend = {0};
// static int8_t realtime_ethercat_data_process(void)
// {
//     TOBJ7010 recv_data = {0};
//     TOBJ6000 send_data = {0};

//     TOBJ7010 *recv = (TOBJ7010 *)ethercat_recv_data_get((uint16_t *)&recv_data, sizeof(recv_data));
//     TOBJ6000 *send = (TOBJ6000 *)ethercat_send_data_get((uint16_t *)&send_data, sizeof(send_data));
//     if (recv == NULL || send == NULL)
//     {
//         printf("ethercat data get failed\r\n");
//         return -1;
//     }
//     BGMEthercatDataParsePoint(recv);
//     memcpy(send, &dataToSend, sizeof(TOBJ6000));
//     return ethercat_send_data_update(send, sizeof(send_data));
// }

static int8_t non_realtime_tcp_callback(uint8_t sn)
{
#ifdef TCP_WEBSOCKET
    return ws_send_data_process(sn);
#endif

    return 0;
}

static int8_t non_realtime_tcp_recv_data_callback(void)
{
    osEventFlagsSet(data_process_eventHandle, DATA_PROCESS_TCP_EVENT);
    return 0;
}

static void tcp_recv_data_process(APP_DATA_RECV *info)
{
    int8_t ret = 0;

    ret = websocket_cmd_parse(info->sn, info->tcpData, info->length);
    if (ret != 0)
    {
        printf("websocket cmd parse err: %d\r\n", ret);
    }

    /* add other process here */
}

static int8_t data_process_init(void)
{
    int8_t ret = 0;
    ret = ws_data_process_callback_register(tcp_recv_data_process);
    if (ret != 0)
    {
        printf("websocket data process callback register err: %d\r\n", ret);
        return ret;
    }
    ret = tcp_establish_cb_register(non_realtime_tcp_callback);
    if (ret != 0)
    {
        printf("tcp callback register err: %d\r\n", ret);
        return ret;
    }
    ret = tcp_recv_data_callback_register(non_realtime_tcp_recv_data_callback);
    if (ret != 0)
    {
        printf("tcp recv data callback register err: %d\r\n", ret);
        return ret;
    }
    // ret = ethercat_slave_appl_cb_register(data_process_eventHandle, DATA_PROCESS_LAN_EVENT, realtime_ethercat_data_process);
    // if (ret != 0)
    // {
    //     printf("ethercat callback register err: %d\r\n", ret);
    //     return ret;
    // }
    return 0;
}

static void data_process_entry(void *argument)
{
    osStatus_t stat = 0;
    uint32_t event_flag = 0;
    TCP_DATA_t tcp_info = {0};

    osDelay(1000);

    data_process_init();  /* register callback functions for tcp 、ethercat、fpga */

    for (;;)
    {   
        event_flag = osEventFlagsWait(data_process_eventHandle, DATA_PROCESS_FPGA_EVENT | DATA_PROCESS_LAN_EVENT | DATA_PROCESS_TCP_EVENT, osFlagsWaitAny, osWaitForever);
        // if (event_flag & DATA_PROCESS_LAN_EVENT)
        // {
        //     ethercat_recv_data_update_with_block(0);
        // }
        if (event_flag & DATA_PROCESS_TCP_EVENT)
        {
            stat = tcp_data_recv_get_with_block(&tcp_info, 0);
            if (stat == osOK)
            {
#ifdef TCP_WEBSOCKET
                ws_recv_data_process(&tcp_info);
#endif
            }
            else
            {
                printf("no msg in tcp rx queue: %d\r\n", stat);
            }
        }
    }
}

static int8_t main_app_thread_init(void)
{
    osThreadAttr_t recv_data_process_thread_attributes = {
    .name = "recv_data_process_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    data_process_eventHandle = osEventFlagsNew(NULL);
    if (data_process_eventHandle == NULL)
    {
        printf("event data process create failed\r\n");
        return -1;
    }

    osThreadId_t recv_data_process_threadHandle = osThreadNew(data_process_entry, NULL, &recv_data_process_thread_attributes);
    if (recv_data_process_threadHandle == NULL)
    {
        printf("thread recv data process create failed\r\n");
        return -2;
    }

    return 0;
}
INIT_APP_EXPORT(main_app_thread_init);