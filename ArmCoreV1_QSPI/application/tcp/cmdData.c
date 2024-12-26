

#include <stdio.h>
#include "main_app.h"
#include "hw_crc.h"
#include "stdint.h"
#include "cmdData.h"
#include "ulog.h"

#include "websocket.h"
#include "drv_flash.h"
#include "flash_port.h"

// #include "string.h"
#define PLAN_DATA_LENGTH 1918
#define PLAN_PAYLOAD_LENGTH (PLAN_DATA_LENGTH - 6)
#define	 CRC_TABLE_SIZE		256

static uint32_t CrcTable[CRC_TABLE_SIZE];
static uint32_t crcCal = 0xffffffff;
static uint16_t lastPackIndex = 0;

static uint16_t *feedback, feedback16Len;
static CMD2UART_DATA nrtCommand;
// static FRAME_HEAD frameHead;
// static FRAME_END frameEnd;

static APP_DATA_SEND activeSendData[] = {
   // {"tcpFeedback", 0, TCP_SEND_PERIOD, WDT_BINDATA, ALL_CLIENTS, NULL, NULL},
};
// void nrtCommand_AFCParameterParse(CMD2UART_DATA* nrtCommand)
// {
//     switch(nrtCommand->data[1])
//     {
//         case 0x00:
//             AFC_SetAFCControlMode(nrtCommand->data[2]);
//             break;
//         case 0x01:  
//             // AFC_SetTim23Delay((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
//             break;      
//         case 0x02:
//             //AFC_SetADCSampleDelay((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
//             // AFC_SetTim23Delay((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
//             // LOG_E("AFC_SetTim23Delay: %d\r\n", (nrtCommand->data[3]<<8)|nrtCommand->data[2]);
//             break;
//         case 0x03:
//             AFC_DeleteADCData();
//             break;      
//         case 0x04:
//             //AFC_GetADCValueByFrame();
//             break;
//         default:
//             break;
//     }
// }   
// void nrtCommand_MagMotorParse(CMD2UART_DATA* nrtCommand)
// {
//     switch(nrtCommand->data[1])
//     {
//         case 0x00:
//             AFC_IS_MagMotorFindZeroOK();    
//             break;
//         case 0x01:
//             AFC_MagMotorSetPos((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
//             break;
//         case 0x02:
//             AFC_MagMotorRunByStep(nrtCommand->data[2],(nrtCommand->data[4]<<8)|nrtCommand->data[3]);
//             break;
//         case 0x03:
//             AFC_MagMotorGetEncValue();
//             break;
//         case 0x04:
//             AFC_MagMotorSetPresetPos((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
//             break;
//         case 0x05:
//             // AFC_MagMotorGetPresetPos();
//             break;
//         default:
//             break;  
//     }
// }
// void nrtCommand_AFTMotorParse(CMD2UART_DATA* nrtCommand)
// {
//     switch(nrtCommand->data[1])
//     {
//         case 0x00:
//             AFC_IS_AFTMotorFindZeroOK();
//             break;
//         case 0x01:
//             AFC_AFTMotorSetPos((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
//             break;
//         case 0x02:
//             AFC_AFTMotorRunByStep(nrtCommand->data[2],(nrtCommand->data[4]<<8)|nrtCommand->data[3]);
//             break;
//         case 0x03:
//             AFC_AFTMotorGetEncValue();
//             break;
//         case 0x04:
//             AFC_AFTMotorSetPresetPos((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
//             break;
//         case 0x05:
//             AFC_SetAFTBrakeStatus(nrtCommand->data[2]);
//             break;
//         case 0x06:
//             AFC_SetAFTMotorStatus(nrtCommand->data[2]);
//             break;
//         case 0x07:
//             //AFC_AFTMotorSetDeadZone((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
//             break;
//         default:
//             break;  
//     }
// }
void nrtCommand_PowerStatusParse(CMD2UART_DATA* nrtCommand)
{
    switch(nrtCommand->data[1])
    {
        case 0x00:
            break;
        case 0x01:
            break;
        case 0x02:
            break;
        default:
            break;
    }
}
// void nrtRecvCommandParse(APP_DATA_RECV* info)
// {       
//     nrtCommand.id = info->tcpData[6];
//     nrtCommand.type = info->tcpData[7];
//     nrtCommand.len = (info->tcpData[9] << 8) + info->tcpData[8];
//     memcpy(nrtCommand.data, &info->tcpData[10], nrtCommand.len);
//     LOG_E("nrtCommand.id = %x\r\n", nrtCommand.id);
//     LOG_E("nrtCommand.type = %x\r\n", nrtCommand.type);
//     LOG_E("nrtCommand.len = %x\r\n", nrtCommand.len);
//     LOG_E("nrtCommand.data[0] = %x\r\n", nrtCommand.data[0]);
//     LOG_E("nrtCommand.data[1] = %x\r\n", nrtCommand.data[1]);
//     // for(int i = 0; i < nrtCommand.len; i++)
//     // {
//     //     LOG_E("nrtCommand.data[%d] = %x\r\n", i, nrtCommand.data[i]);
//     // }
//     // LOG_E("nrtCommand.id = %x\r\n", nrtCommand.id);
//     // LOG_E("nrtCommand.type = %x\r\n", nrtCommand.type);
//     // LOG_E("nrtCommand.len = %x\r\n", nrtCommand.len);
//     // LOG_E("nrtCommand.data[0] = %x\r\n", nrtCommand.data[0]);
//     // LOG_E("nrtCommand.data[1] = %x\r\n", nrtCommand.data[1]);
//     switch(nrtCommand.data[0])
//     {
//         case 0x01:
//             nrtCommand_AFCParameterParse(&nrtCommand);
//             break;
//         case 0x40:
//             nrtCommand_MagMotorParse(&nrtCommand);
//             break;
//         case 0x41:
//             nrtCommand_AFTMotorParse(&nrtCommand);
//             break;
//         case 0x60:
//             break;
//         default:
//             break;
//     }
// }
/*void nrtCommand_AFCParameterParse(CMD2UART_DATA* nrtCommand)
{
    switch(nrtCommand->data[1])
    {
        case 0x00:
            AFC_SetAFCControlMode(nrtCommand->data[2]);
            break;
        case 0x01:  
           // AFC_SetADCSampleMode(nrtCommand->data[2]);
            AFC_SetTim23Delay((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
            break;      
        case 0x02:
            AFC_SetADCSampleDelay((nrtCommand->data[3]<<8)|nrtCommand->data[2]);
            break;
        case 0x03:
            AFC_DeleteADCData();
            break;      
        case 0x04:
            //AFC_GetADCValueByFrame();
            break;
        default:
            break;
    }
} */
void AFC_AFCConfigFeedback()
{
    switch(nrtCommand.data[1])
    {
        case 0x00:
            nrtCommand.data[2] = 0x00;
            break;
        case 0x01:
            break;
        case 0x02:
            //AFC_SetTim23Delay((nrtCommand.data[3]<<8)|nrtCommand.data[2]);
            break;
        case 0x03:
            break;
        default:
            break;
    }
}
// extern MotorCtrlParam_TypeDef magMotorCtrlParam;
// void AFC_MagMotorFeedback(APP_DATA_RECV* info)
// {
//     uint8_t read_data[5] = {0};
//     switch(nrtCommand.data[1])
//     {
//         case 0x00:
          
//             break;
//         case 0x01:
//             nrtCommand.data[2] = magMotorCtrlParam.encoderValCurrent;
//             nrtCommand.data[3] = magMotorCtrlParam.encoderValCurrent >> 8;
//             LOG_E("Mag Motor Current Encoder Value: %d\r\n", magMotorCtrlParam.encoderValCurrent);  
//             break;
//         case 0x02:
//             nrtCommand.data[2] = magMotorCtrlParam.encoderValCurrent;
//             nrtCommand.data[3] = magMotorCtrlParam.encoderValCurrent >> 8;
//             LOG_E("Mag Motor Current Encoder Value: %d\r\n", magMotorCtrlParam.encoderValCurrent);  
//             break;  
//         case 0x03:
//             nrtCommand.data[2] = magMotorCtrlParam.encoderValCurrent;
//             nrtCommand.data[3] = magMotorCtrlParam.encoderValCurrent >> 8;
//             LOG_E("Mag Motor Current Encoder Value: %d\r\n", magMotorCtrlParam.encoderValCurrent);  
//             break;
//         default:
//             break;
//     }
//       uint16_t  *pFDAry = feedback;
//     *pFDAry++ = 0x31;//tag
//     *pFDAry++ = 0x00;//control
//     *pFDAry++ = sizeof(read_data )+ 6; //  length 
//     memcpy(pFDAry, read_data, sizeof(read_data));
//     pFDAry += 10*16; // 16个uint16_t移动16步
//     ws_send(info->sn, feedback,sizeof(read_data )+ 6, true, false, WDT_BINDATA);
// }
// void AFC_AFTMotorFeedback()
// {

// }
// void nrtRecvCommandFeedback(APP_DATA_RECV* info)
// {
//     switch(nrtCommand.data[0])
//     {
//         case 0x01:
//             AFC_AFCConfigFeedback();
//             break;
//         case 0x40:
//             AFC_MagMotorFeedback(info);
//             break;
//         case 0x41:
//             AFC_AFTMotorFeedback();
//             break;  
//         default:
//             break;
//     }
// }
// extern DEVICE_FLASH *flash;
// uint16_t WS_AFC_ADC_Flash_addr = 0;
// extern uint8_t adc_data[320];
// void AFC_ADCSampleDataFeedback(uint8_t socket)
// {
//     uint8_t read_data[320] = {0};
//     //flash->read(flash, WS_AFC_ADC_Flash_addr, read_data, 10*16 * sizeof(uint16_t), 1000);
//     // for (int i = 0; i < 8* 16; i++)     
//     // {
//     //     //LOG_E("wswswsread_data[%d] = %x\r\n", i, read_data[i]);
//     // }
//     // LOG_E("socket = %d\r\n", socket);
//     uint16_t  *pFDAry = feedback;
//     *pFDAry++ = 0xFF;//tag
//     *pFDAry++ = 0x00;//control
//     *pFDAry++ = sizeof(adc_data )+ 6; //  length 
//     memcpy(pFDAry, adc_data, sizeof(adc_data));
//     pFDAry += 10*16; // 16个uint16_t移动16步
//     ws_send(socket, feedback,sizeof(adc_data )+ 6, true, false, WDT_BINDATA);
// }