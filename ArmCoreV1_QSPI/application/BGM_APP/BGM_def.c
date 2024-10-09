#include "main.h"
#include "BGM_def.h"
#include "SPIDriver.h"
#include "ethercat.h"
#include "applInterface.h"
#include "lan9252_port.h"
#include "lan9252_app.h"
#include "cmsis_os2.h"
#include "drv_tim.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "bgm_uart.h"
#include "AFCCmd.h"
#include "shell.h" 
#include "drv_spi.h"
#include "spi.h"
#include "IOE.h"

const uint8_t BGM_ARM_IO_Version[4] = {0x19,0,0,1};//Hardware version0x19 ,firmware version xx,yy,zz 


int BGM2AFC_Handshake(void)
{
    struct cmd_object BGM2AFCHandshake;
    BGM2AFCHandshake.id.byte = 0x80;
    BGM2AFCHandshake.type = UARTCmdType_HandshakeDown;
    BGM2AFCHandshake.len = sizeof(BGM_ARM_IO_Version);
    BGM2AFCHandshake.data = BGM_ARM_IO_Version;
    return uart_cmd_write(BGM_UART_AFC,&BGM2AFCHandshake);
}
MSH_CMD_EXPORT_ALIAS(BGM2AFC_Handshake,B2AHS,"Dose Board Handshake Set");

int BGM2Dose_Handshake(enum uart_id uartID)
{
    uint8_t versionToHandshake[5];
    versionToHandshake[0] = BGM_ARM_IO_Version[0];
    versionToHandshake[1] = BGM_ARM_IO_Version[1];//sw version XX
    versionToHandshake[2] = BGM_ARM_IO_Version[2];//sw version YY
    versionToHandshake[3] = BGM_ARM_IO_Version[3];//sw version ZZ
    versionToHandshake[4] = (uint8_t)uartID;
    struct cmd_object BGM2DoseHandshake;
    BGM2DoseHandshake.id.byte = 0x80;
    BGM2DoseHandshake.type = UARTCmdType_HandshakeDown;
    BGM2DoseHandshake.len = sizeof(versionToHandshake);
    BGM2DoseHandshake.data = versionToHandshake;

    return uart_cmd_write(uartID,&BGM2DoseHandshake);
}

void Shell_BGM2Dose_Handshake(int8_t argc, uint8_t **argv)
{
    printf("BGM to AFC Command set with %d arguments:\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    enum uart_id uartID;
     if(strcmp((char *)argv[1], "01") == 0)
    {
        uartID = BGM_UART_DOSE1;
    }
    else if (strcmp((char *)argv[1], "02") == 0)
    {
        uartID = BGM_UART_DOSE2;
    }
    BGM2Dose_Handshake(uartID); 
}
MSH_CMD_EXPORT_ALIAS(Shell_BGM2Dose_Handshake,B2DHS,"Dose Board Handshake Set");


void BGMShell_BGM2AFTHandshake(int8_t argc, uint8_t **argv)
{
    if(argc >1)
    {
        printf("hand shake command wrong\r\n");
    }
    if(BGM2AFC_Handshake() == 0)
    {
        printf("BGM to AFC handshake command Send success\r\n");
    }
    else
    {
        printf("BGM to AFC handshake command Fail\r\n");
    }
}
MSH_CMD_EXPORT_ALIAS(BGMShell_BGM2AFTHandshake,AFCHS,"AFTHandshake Set");

void BGM_SendCmd(enum uart_id uartID,UARTCmdType_t cmdType, uint8_t *cmdData)
{
    struct cmd_object BGMCmdToSend;
    BGMCmdToSend.id.byte = 0x80;
    BGMCmdToSend.type = cmdType;
    BGMCmdToSend.len = sizeof(cmdData);
    BGMCmdToSend.data = cmdData;
    uart_cmd_write(uartID,&BGMCmdToSend);
}


void BGMShell_BGM2AFTCmd(int8_t argc, uint8_t **argv)
{
    printf("BGM to AFC Command set with %d arguments:\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    if (argc >= 2)  
    {
        uint8_t dataLen = argc - 1; 
        if (dataLen > 16)
         {
            printf("Data length exceeds 16 bytes, which is the maximum allowed.\n");
            return;
        }
        uint8_t AFCCmdData[16] = {0};  
        for (int i = 0; i < dataLen; i++) 
        {
            AFCCmdData[i] = (uint8_t)strtol((char *)argv[i + 1], NULL, 16); 
            //AFCCmdData[i] = (uint8_t)atoi((char *)argv[i + 1]);
        }
        BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown, AFCCmdData); 
    }
    else
    {
        printf("Insufficient arguments.\n");
    }
}
MSH_CMD_EXPORT_ALIAS(BGMShell_BGM2AFTCmd,AFTCMD,"AFTCMD Set");

void BGMShell_BGMtoDose1Cmd(int8_t argc, uint8_t **argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    if (argc >= 2)  
    {
        uint8_t dataLen = argc - 1; 
        if (dataLen > 16)
         {
            printf("Data length exceeds 16 bytes, which is the maximum allowed.\n");
            return;
        }
        uint8_t Dose1CmdData[16] = {0};  
        for (int i = 0; i < dataLen; i++) 
        {
            Dose1CmdData[i] = (uint8_t)strtol((char *)argv[i + 1], NULL, 16); 
        }
        BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown, Dose1CmdData); 
    }
    else
    {
        printf("Insufficient arguments.\n");
    }
}
MSH_CMD_EXPORT_ALIAS(BGMShell_BGMtoDose1Cmd,DOSE1CMD,"Dose 1 CMD Set");

void BGMShell_BGMtoDose2Cmd(int8_t argc, uint8_t **argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    if (argc >= 2)  
    {
        uint8_t dataLen = argc - 1; 
        if (dataLen > 16)
         {
            printf("Data length exceeds 16 bytes, which is the maximum allowed.\n");
            return;
        }
        uint8_t Dose2CmdData[16] = {0};  
        for (int i = 0; i < dataLen; i++) 
        {
            Dose2CmdData[i] = (uint8_t)strtol((char *)argv[i + 1], NULL, 16); 
        }
        BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown, Dose2CmdData); 
    }
    else
    {
        printf("Insufficient arguments.\n");
    }
}
MSH_CMD_EXPORT_ALIAS(BGMShell_BGMtoDose2Cmd,DOSE2CMD,"DOSE 2 CMD Set");

BGMInterlocksDetect_t BGM_ReadAllInterlocks(void)
{   
    BGMInterlocksDetect_t AllInterlocks= {0};
    ExpandGPIOStatus_t exdata = IOE_ExpandGPIODataParse(IOE_GPIORead());
    memcpy(&AllInterlocks.exGPIODetect,&exdata,sizeof(exdata));
    AllInterlocks.ModTrigONDetect  = ReadIO_ModTrigFB();
    AllInterlocks.LvOKDetect  = ReadIO_LvOKDetect();
    AllInterlocks.HvEnDetect  = ReadIO_HvENFB();
    AllInterlocks.ModArcDetect  = ReadIO_ModArcDetect();
    AllInterlocks.ModTrigONDetect  = ReadIO_ModTrigONDetect();

    AllInterlocks.ModHvONDetect = ReadIO_ModHvONDetect();
    AllInterlocks.ModSumDetect  = ReadIO_ModSumDetect();
    AllInterlocks.Dose1Detect  = ReadIO_Dose1Detect();
    AllInterlocks.Dose2Detect  = ReadIO_Dose2Detect();
    AllInterlocks.EmergencyDetect  = ReadIO_EmergencyDetect();

    AllInterlocks.ModTrigONDetect  = ReadIO_PulseInhibitDetect();

    printf("//////////////////////BGM Interlocks Status//////////////////////// \r\n");
    printf("CoolingLv1Detect        =   %d\r\n",AllInterlocks.exGPIODetect.bits.CoolingLv1Detect);
    printf("CoolingLv2Detect        =   %d\r\n",AllInterlocks.exGPIODetect.bits.CoolingLv2Detect);
    printf("WaterSW1Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW1Detect);
    printf("WaterSW2Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW2Detect);
    printf("WaterSW3Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW3Detect);
    printf("WaterSW4Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW4Detect);
    printf("WaterSW5Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW5Detect);
    printf("SF6HighDetect           =   %d\r\n",AllInterlocks.exGPIODetect.bits.SF6HighDetect);
    printf("SF6LowDetect            =   %d\r\n",AllInterlocks.exGPIODetect.bits.SF6LowDetect);
    printf("EPSStateOPDetect        =   %d\r\n",AllInterlocks.exGPIODetect.bits.EPSStateOPDetect);
    printf("nEPSStateFaultDetect    =   %d\r\n",AllInterlocks.exGPIODetect.bits.nEPSStateFaultDetect);
    printf("VPSStateFaultDetect     =   %d\r\n",AllInterlocks.exGPIODetect.bits.VPSStateFaultDetect);
    printf("VPSStateOPDetect        =   %d\r\n",AllInterlocks.exGPIODetect.bits.VPSStateOPDetect);
    printf("GatingDetect            =   %d\r\n",AllInterlocks.exGPIODetect.bits.GatingDetect);
    printf("HVConFBDetect           =   %d\r\n",AllInterlocks.exGPIODetect.bits.HVConFBDetect);
    printf("MVTreatmentENDetect     =   %d\r\n",AllInterlocks.exGPIODetect.bits.MVTreatmentENDetect);
    printf("Dose1Detect             =   %d\r\n",AllInterlocks.Dose1Detect);
    printf("Dose2Detect             =   %d\r\n",AllInterlocks.Dose2Detect);
    printf("EmergencyDetect         =   %d\r\n",AllInterlocks.EmergencyDetect);
    printf("HvEnDetect              =   %d\r\n",AllInterlocks.HvEnDetect);
    printf("LvOKDetect              =   %d\r\n",AllInterlocks.LvOKDetect);
    printf("ModArcDetect            =   %d\r\n",AllInterlocks.ModArcDetect);
    printf("ModHvONDetect           =   %d\r\n",AllInterlocks.ModHvONDetect);
    printf("ModSumDetect            =   %d\r\n",AllInterlocks.ModSumDetect);
    printf("ModTrigONDetect         =   %d\r\n",AllInterlocks.ModTrigONDetect);
    printf("ModTriggerInhibitDetect =   %d\r\n",AllInterlocks.ModTriggerInhibitDetect);
    printf("PulseInhibitDetect      =   %d\r\n",AllInterlocks.PulseInhibitDetect);
    printf("///////////////////////////////////////////////////////////////////// \r\n");
}
MSH_CMD_EXPORT_ALIAS(BGM_ReadAllInterlocks,ReadIO,"Read IO");

uint16_t BGM_ReadModInterlocks(void)
{
    static uint16_t ModInterlockStatus;
    uint8_t ModTrigONStatus;
    uint8_t ModHvONDetectStatus;
    uint8_t ModArcDetectStatus;
    uint8_t ModSumDetectStatus;

    ModTrigONStatus = ReadIO_ModTrigONDetect();
    ModHvONDetectStatus = ReadIO_ModArcDetect();
    ModArcDetectStatus = ReadIO_ModHvONDetect();
    ModSumDetectStatus = ReadIO_ModSumDetect();

    ModInterlockStatus = (uint16_t)((ModTrigONStatus << 7)|(ModTrigONStatus << 6)|(ModArcDetectStatus << 5)|(ModSumDetectStatus << 4));
    return ModInterlockStatus;
}