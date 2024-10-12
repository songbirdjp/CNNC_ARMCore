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
#include "ulog.h"
#include "console.h"

const uint8_t BGM_ARM_IO_Version[4] = {0x19,0,0,1};//Hardware version0x19 ,firmware version xx,yy,zz 


int BGM2AFC_Handshake(void)
{
    struct cmd_object BGM2AFCHandshake;
    BGM2AFCHandshake.id.byte = 0x80;
    BGM2AFCHandshake.type = UARTCmdType_HandshakeDown;
    BGM2AFCHandshake.len = sizeof(BGM_ARM_IO_Version);
    BGM2AFCHandshake.data = BGM_ARM_IO_Version;
    LOG_I("BGM2AFC_Handshake\r\n");
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

void BGM_SendCmd(enum uart_id uartID,UARTCmdType_t cmdType, uint8_t *cmdData,uint8_t len)
{
    struct cmd_object BGMCmdToSend;
    BGMCmdToSend.id.byte = 0x80;
    BGMCmdToSend.type = cmdType;
    BGMCmdToSend.len = len;
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
        BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown, AFCCmdData,argc - 1); 
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
        BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown, Dose1CmdData,argc - 1); 
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
        BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown, Dose2CmdData,argc - 1 ); 
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

    LOG_I("//////////////////////BGM Interlocks Status//////////////////////// \r\n");
    LOG_I("CoolingLv1Detect        =   %d\r\n",AllInterlocks.exGPIODetect.bits.CoolingLv1Detect);
    LOG_I("CoolingLv2Detect        =   %d\r\n",AllInterlocks.exGPIODetect.bits.CoolingLv2Detect);
    LOG_I("WaterSW1Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW1Detect);
    LOG_I("WaterSW2Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW2Detect);
    LOG_I("WaterSW3Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW3Detect);
    LOG_I("WaterSW4Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW4Detect);
    LOG_I("WaterSW5Detect          =   %d\r\n",AllInterlocks.exGPIODetect.bits.WaterSW5Detect);
    LOG_I("SF6HighDetect           =   %d\r\n",AllInterlocks.exGPIODetect.bits.SF6HighDetect);
    LOG_I("SF6LowDetect            =   %d\r\n",AllInterlocks.exGPIODetect.bits.SF6LowDetect);
    LOG_I("EPSStateOPDetect        =   %d\r\n",AllInterlocks.exGPIODetect.bits.EPSStateOPDetect);
    LOG_I("nEPSStateFaultDetect    =   %d\r\n",AllInterlocks.exGPIODetect.bits.nEPSStateFaultDetect);
    LOG_I("VPSStateFaultDetect     =   %d\r\n",AllInterlocks.exGPIODetect.bits.VPSStateFaultDetect);
    LOG_I("VPSStateOPDetect        =   %d\r\n",AllInterlocks.exGPIODetect.bits.VPSStateOPDetect);
    LOG_I("GatingDetect            =   %d\r\n",AllInterlocks.exGPIODetect.bits.GatingDetect);
    LOG_I("HVConFBDetect           =   %d\r\n",AllInterlocks.exGPIODetect.bits.HVConFBDetect);
    LOG_I("MVTreatmentENDetect     =   %d\r\n",AllInterlocks.exGPIODetect.bits.MVTreatmentENDetect);
    LOG_I("Dose1Detect             =   %d\r\n",AllInterlocks.Dose1Detect);
    LOG_I("Dose2Detect             =   %d\r\n",AllInterlocks.Dose2Detect);
    LOG_I("EmergencyDetect         =   %d\r\n",AllInterlocks.EmergencyDetect);
    LOG_I("HvEnDetect              =   %d\r\n",AllInterlocks.HvEnDetect);
    LOG_I("LvOKDetect              =   %d\r\n",AllInterlocks.LvOKDetect);
    LOG_I("ModArcDetect            =   %d\r\n",AllInterlocks.ModArcDetect);
    LOG_I("ModHvONDetect           =   %d\r\n",AllInterlocks.ModHvONDetect);
    LOG_I("ModSumDetect            =   %d\r\n",AllInterlocks.ModSumDetect);
    LOG_I("ModTrigONDetect         =   %d\r\n",AllInterlocks.ModTrigONDetect);
    LOG_I("ModTriggerInhibitDetect =   %d\r\n",AllInterlocks.ModTriggerInhibitDetect);
    LOG_I("PulseInhibitDetect      =   %d\r\n",AllInterlocks.PulseInhibitDetect);
    LOG_I("///////////////////////////////////////////////////////////////////// \r\n");
    
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
void BGM_CtrlDoseBoardFSM(DoseFsmState_t doseFSM)
{
    static uint8_t fsmCmd[3] = {0xc0,0x00,0x00};
    if(doseFSM == 0)
    {
        fsmCmd[2] = 0x00;
    }
    else if(doseFSM == 1)
    {
        fsmCmd[2] = 0x01;
    }
    else if(doseFSM == 2)
    {
        fsmCmd[2] = 0x02;
    }
    else if(doseFSM == 3)
    {
        fsmCmd[2] = 0x03;
    }
    else if(doseFSM == 4)
    {
        fsmCmd[2] = 0x04;
    }
    else if(doseFSM == 5)
    {
        fsmCmd[2] = 0x05;
    }
    else if(doseFSM == 6)
    {
        fsmCmd[2] = 0x06;
    }
    else 
    {
        fsmCmd[2] = 0x07;//fault
    }
    BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown,fsmCmd,3); 
    BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown,fsmCmd,3);
}
void BGMShell_CtrlDoseBoardFSM(int8_t argc, uint8_t **argv)
{
        for (int i = 0; i < argc; i++)
        {
            LOG_I("arg[%d]: %s\n", i, argv[i]);
        } 
        uint8_t shellDoseFSM = 0;
        shellDoseFSM = (uint8_t)strtol((char *)argv[1], NULL, 16); 
        LOG_I("shellDoseFSM = %d\r\n",shellDoseFSM);
        BGM_CtrlDoseBoardFSM((DoseFsmState_t) shellDoseFSM);
}
MSH_CMD_EXPORT_ALIAS(BGMShell_CtrlDoseBoardFSM,DOSE1FSM,"CtrlDoseBoard1 FSM");

void BGM_SetDoseBoardPRF(enum uart_id uartID,uint8_t prfVal)
{
    uint8_t prfCmd[3] = {0x41,0x01,0x00};
    prfCmd[2] = prfVal;
    BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown,prfCmd,3); 
    BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown,prfCmd,3); 
}

void BGMShell_SetDoseBoardPRF(int8_t argc, uint8_t **argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    enum uart_id id;
    uint8_t shellDosePRF = 0;
    id = (enum uart_id)strtol((char *)argv[1], NULL, 16);
    shellDosePRF = (uint8_t)strtol((char *)argv[2], NULL, 16); 
    BGM_SetDoseBoardPRF(id,shellDosePRF);
}
MSH_CMD_EXPORT_ALIAS(BGMShell_SetDoseBoardPRF,DOSEPRF,"Set DoseBoard PRF");

void BGM_SetDoseBoardDose(enum uart_id uartID,uint16_t doseVal)
{
    uint8_t doseCmd[4] = {0x42,0x01,0x00,0x00};
    doseCmd[2] = doseVal;
    doseCmd[3] = doseVal >> 8;
    BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown,doseCmd,4); 
    BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown,doseCmd,4); 
}

void BGMShell_SetDoseBoardDose(int8_t argc, uint8_t **argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    enum uart_id id;
    uint16_t shellSetDose = 0;
    id = (enum uart_id)strtol((char *)argv[1], NULL, 16);
    shellSetDose =((uint8_t)strtol((char *)argv[2], NULL, 16))|(((uint8_t)strtol((char *)argv[3], NULL, 16)) << 8 ); 
    BGM_SetDoseBoardDose(id,shellSetDose);
}
MSH_CMD_EXPORT_ALIAS(BGMShell_SetDoseBoardDose,DOSEdoseset,"Set DoseBoard aim dose");


void BGM_LockDoseCaliPara(enum uart_id uartID,uint8_t _lockStatus)
{
    uint8_t LockCmd[3] = {0x01,0x00,0x00};
    LockCmd[2] = _lockStatus;
    BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown,LockCmd,3); 
    BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown,LockCmd,3); 
}

void BGMShell_LockDoseCaliPara(int8_t argc, uint8_t **argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    enum uart_id id;
    uint16_t shellLockPara = 0;
    id = (enum uart_id)strtol((char *)argv[1], NULL, 16);
    shellLockPara =(uint8_t)strtol((char *)argv[2], NULL, 16);
    BGM_LockDoseCaliPara(id,shellLockPara);
}
MSH_CMD_EXPORT_ALIAS(BGMShell_LockDoseCaliPara,DOSELockPara,"Set DOSELockPara");

void BGM_LockBeamData(enum uart_id uartID,uint8_t _lockStatus)
{
    uint8_t LockCmd[3] = {0x43,0x00,0x00};
    LockCmd[2] = _lockStatus;
    BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown,LockCmd,3); 
    BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown,LockCmd,3); 
}

void BGMShell_LockBeamData(int8_t argc, uint8_t **argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    enum uart_id id;
    uint16_t shellLockPara = 0;
    id = (enum uart_id)strtol((char *)argv[1], NULL, 16);
    shellLockPara =(uint8_t)strtol((char *)argv[2], NULL, 16);
    BGM_LockBeamData(id,shellLockPara);
}
MSH_CMD_EXPORT_ALIAS(BGMShell_LockBeamData,DOSELockBeamData,"Set LockBeamData");

void BGM_SetDoseBoardKadc(enum uart_id uartID,uint32_t kadcVal)
{
    uint8_t KadcCmd[5] = {0x02,0x00,0x00,0x00,0x00};
    KadcCmd[2] = kadcVal;
    KadcCmd[3] = kadcVal >> 8;
    KadcCmd[4] = kadcVal >> 16;
    BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown,KadcCmd,5);  
     BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown,KadcCmd,5); 
}

void BGMShell_SetDoseBoardKadc(int8_t argc, uint8_t **argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    enum uart_id id;
    uint8_t shelltemp[3] = {0};
    uint32_t shellDosekadc = 0;
    id = (enum uart_id)strtol((char *)argv[1], NULL, 16);
    shelltemp[0] =  (uint8_t)strtol((char *)argv[2], NULL, 16); 
    shelltemp[1] =  (uint8_t)strtol((char *)argv[3], NULL, 16);
    shelltemp[2] =  (uint8_t)strtol((char *)argv[4], NULL, 16);
    shellDosekadc = ((shelltemp[2] << 16)|(shelltemp[1] << 8)|(shelltemp[0]));
    BGM_SetDoseBoardKadc(id,shellDosekadc);
}
MSH_CMD_EXPORT_ALIAS(BGMShell_SetDoseBoardKadc,DOSEkadc,"Set DoseBoard kadc");

void BGM_SetDoseBoardDAC(enum uart_id uartID,uint32_t dacVal)
{
    uint8_t dacCmd[4] = {0x03,0x00,0x00,0x00};
    dacCmd[2] = dacVal;
    dacCmd[3] = dacVal >> 8;
    BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown,dacCmd,4);  
    BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown,dacCmd,4);  
}

void BGMShell_SetDoseBoardDAC(int8_t argc, uint8_t **argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("arg[%d]: %s\n", i, argv[i]);
    } 
    enum uart_id id;
    uint8_t shelltemp[3] = {0};
    uint32_t shellDosedac = 0;
    id = (enum uart_id)strtol((char *)argv[1], NULL, 16);
    shelltemp[0] =  (uint8_t)strtol((char *)argv[2], NULL, 16); 
    shelltemp[1] =  (uint8_t)strtol((char *)argv[3], NULL, 16);
    shellDosedac = ((shelltemp[1] << 8)|(shelltemp[0]));
    BGM_SetDoseBoardDAC(id,shellDosedac);
}
MSH_CMD_EXPORT_ALIAS(BGMShell_SetDoseBoardDAC,DOSEdac,"Set DoseBoard dac");