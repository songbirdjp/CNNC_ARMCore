#include "bgm_def.h"
#include "sys_cfg.h"
#include <stddef.h>
#include <stdlib.h>
#include "plan_data.h"
#include "ulog.h"

#define BGM_ARM_HW_VERSION  0x19

/* 1. bgm with dose board communication interface */
int8_t dose_handshake(enum uart_id id)
{
    int8_t ret = 0;
    uint8_t *fw_ver = system_info_get()->fw_version;
    uint8_t versionToHandshake[5] = {0};

    versionToHandshake[0] = BGM_ARM_HW_VERSION;
    versionToHandshake[1] = strtoul(&fw_ver[0], NULL, 10);;//sw version XX
    versionToHandshake[2] = strtoul(&fw_ver[3], NULL, 10);;//sw version YY
    versionToHandshake[3] = strtoul(&fw_ver[6], NULL, 10);;//sw version ZZ
    versionToHandshake[4] = (uint8_t)id;

    ret = dose_data_info_set(id, DOSE_INFO_VERSION, versionToHandshake, sizeof(versionToHandshake));
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_adc_value_set(enum uart_id id, uint32_t value)
{
    int8_t ret = 0;
    ret = dose_data_info_set(id, DOSE_INFO_ADC_CALI, &value, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_dac_value_set(enum uart_id id, uint32_t value)
{
    int8_t ret = 0;
    ret = dose_data_info_set(id, DOSE_INFO_DAC_CALI, &value, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_meter_value_set(enum uart_id id, float dose_meter)
{
    int8_t ret = 0;

    uint16_t value = (uint16_t)(dose_meter * 10.0f);

    ret = dose_data_info_set(id, DOSE_INFO_METER_SET, &value, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_prf_value_set(enum uart_id id, uint8_t prf)
{
    int8_t ret = 0;

    ret = dose_data_info_set(id, DOSE_INFO_PRF_SET, &prf, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_generate_mode_set(enum uart_id id, uint8_t mode)
{
    int8_t ret = 0;

    ret = dose_data_info_set(id, DOSE_INFO_GENERATE_MODE_SET, &mode, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_pulse_mode_set(enum uart_id id, uint8_t pulse_mode)
{
    int8_t ret = 0;

    ret = dose_data_info_set(id, DOSE_INFO_PULSE_MODE_SET, &pulse_mode, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_fsm_state_set(enum uart_id id, enum dose_fsm_state state)
{
    int8_t ret = 0;

    uint8_t stat = (uint8_t)state;

    ret = dose_data_info_set(id, DOSE_INFO_FSM_STATE_SET, &stat, sizeof(stat));
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_fsm_state_get(enum uart_id id, enum dose_fsm_state *state)
{
    *state = dose_data_info_get(id, DOSE_INFO_FSM_STATE_GET, state);
    return 0;
}

int8_t dose_fsm_state_polling(enum uart_id id)
{
    int8_t ret = 0;

    ret = dose_data_info_set(id, DOSE_INFO_FSM_STATE_GET, NULL, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_beam_cumulated_clear(enum uart_id id)
{
    int8_t ret = 0;

    ret = dose_data_info_set(id, DOSE_INFO_CUMULATED_CLEAR, NULL, 0);
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_beam_parameter_set(enum uart_id id, uint16_t beam_id)
{
    int8_t ret = 0;
    struct one_beam_order beam_obj = {0};

    ret = getPlanBeamData(beam_id, &beam_obj);
    if (ret != 1)
    {
        LOG_E("get beam data err: %d\r\n", ret);
        return -1;
    }

    ret = dose_data_info_set(id, DOSE_INFO_BEAM_SET, &beam_obj, 0);

    /* 1. beam unlock */

    /* 2. beam meter */

    /* 3. beam cp & ri num */

    /* 4. beam cp & ri map */

    /* 5. beam ri value */

    /* 6. beam lock and validate */

    return ret;
}

int8_t dose_radiation_data_get(enum uart_id id)
{
    int8_t ret = 0;
    uint8_t data = 0x01;

    ret = dose_data_info_set(id, DOSE_INFO_RADIATION_GET, &data, sizeof(data));
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

int8_t dose_radiation_index_set(enum uart_id id, uint16_t index, uint8_t emergency)
{
    int8_t ret = 0;
    uint8_t data[3] = {0};

    data[0] = index;
    data[1] = index >> 8;
    data[2] = emergency;

    ret = dose_data_info_set(id, DOSE_INFO_RADIATION_SET, data, sizeof(data) / sizeof(uint8_t));
    if(ret != 0)
    {
        LOG_E("dose info set err: %d\r\n", ret);
    }

    return ret;
}

#ifndef DOSE_CMD_TEST
#include "shell.h"
static int8_t dose_cmd_test(int8_t argc, uint8_t **argv)
{
    int8_t ret = 0;

    if(argc != 4)
    {
        printf("dose_cmd_test: invalid arguments\r\n");
        return -1;
    }

    switch (atoi(argv[1]))
    {
    case 0:
        dose_handshake(atoi(argv[2]));
        break;
    case 1:
        dose_adc_value_set(atoi(argv[2]), atoi(argv[3]));
        break;
    case 2:
        dose_dac_value_set(atoi(argv[2]), atoi(argv[3]));
        break;
    case 3:
        dose_fsm_state_set(atoi(argv[2]), DOSE_FSM_STATE_IDLE);
        break;
    case 4:
        uint8_t state = 0;
        dose_fsm_state_get(atoi(argv[2]), &state);
        printf("state = %d\r\n", state);
        break;
    case 5:
        dose_fsm_state_polling(atoi(argv[2]));
        break;
    case 6:
        dose_beam_cumulated_clear(atoi(argv[2]));
        break;
    case 7:
        dose_beam_parameter_set(atoi(argv[2]), atoi(argv[3]));
        break;
    case 8:
        dose_radiation_data_get(atoi(argv[2]));
        break;
    case 9:
        dose_radiation_index_set(atoi(argv[2]), atoi(argv[3]), 0);
        break;
    case 10:
        dose_meter_value_set(atoi(argv[2]), atof(argv[3]));
        break;
    case 11:
        dose_prf_value_set(atoi(argv[2]), atoi(argv[3]));
        break;
    case 12:
        dose_generate_mode_set(atoi(argv[2]), atoi(argv[3]));
        break;
    case 13:
        dose_pulse_mode_set(atoi(argv[2]), atoi(argv[3]));
        break;
    default:
        break;
    }

    return ret;
}
MSH_CMD_EXPORT_ALIAS(dose_cmd_test, dose_cmd_test, test dose cmd);
#endif



/* 2. bgm with afc board communication interface */
void BGM_SendCmd(enum uart_id uartID, UARTCmdType_t cmdType, uint8_t *cmdData,uint8_t len)
{
    struct cmd_object BGMCmdToSend;
    BGMCmdToSend.id.bits.cmd_id = 0;
    BGMCmdToSend.id.bits.cmd_ack = 1;
    BGMCmdToSend.type = cmdType;
    BGMCmdToSend.len = len;
    BGMCmdToSend.data = cmdData;
    uart_cmd_write(uartID,&BGMCmdToSend);
}

#if 0
void AFC_GetADCValueByFrame(void)
{
    uint8_t _afcCmd[2] = {0x60,0x05};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2); 
}

extern BGMStateMachine_t ARMcurrentState;
extern BGMStateMachine_t PLCcurrentState;

void Shell_ModifyARMFSM(int8_t argc, uint8_t **argv)
{
    uint8_t temp;
    temp = (uint8_t)strtol((char *)argv[1], NULL, 16); 
    ARMcurrentState = temp;
}
MSH_CMD_EXPORT_ALIAS(Shell_ModifyARMFSM,MAFSM,"Modify All FSM");

void Shell_CheckALLFSM(void)
{
    uint8_t cmdToCheck[2] = {0xc0,0x01};
    BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown, cmdToCheck,2); 
    //PLCcurrentState
    LOG_E("PLCcurrentState = %d\r\n",PLCcurrentState);
    LOG_E("ARM currentState = %d\r\n",ARMcurrentState);
}
MSH_CMD_EXPORT_ALIAS(Shell_CheckALLFSM,ReadAllFSM,"Read All FSM");
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
    dose_handshake(uartID); 
}
MSH_CMD_EXPORT_ALIAS(Shell_BGM2Dose_Handshake,B2DHS,"Dose Board Handshake Set");

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

void BGMShell_CtrlDoseBoardFSM(int8_t argc, uint8_t **argv)
{
        for (int i = 0; i < argc; i++)
        {
            LOG_I("arg[%d]: %s\n", i, argv[i]);
        } 
        uint8_t shellDoseFSM = 0;
        shellDoseFSM = (uint8_t)strtol((char *)argv[1], NULL, 16); 
        LOG_I("shellDoseFSM = %d\r\n",shellDoseFSM);
        dose_fsm_state_set((DoseFsmState_t) shellDoseFSM);
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

void BGM_SetDoseMode(enum uart_id uartID,uint8_t doseMode)//BGM_SetDoseBoardDose
{
    uint8_t doseCmd[3] = {0x40,0x00,0x000};
    doseCmd[2] = doseMode;
    BGM_SendCmd(BGM_UART_DOSE1,UARTCmdType_CommandDown,doseCmd,3); 
    BGM_SendCmd(BGM_UART_DOSE2,UARTCmdType_CommandDown,doseCmd,3); 
}
void BGMShell_SetDoseMode(int8_t argc, uint8_t **argv)
{
    enum uart_id id;
    uint16_t shellPara = 0;
    id = (enum uart_id)strtol((char *)argv[1], NULL, 16);
    shellPara =(uint8_t)strtol((char *)argv[2], NULL, 16);
    BGM_SetDoseMode(id,shellPara);
}
MSH_CMD_EXPORT_ALIAS(BGMShell_SetDoseMode,DOSEModeSet,"DOSE Mode Set dummy or Normal");

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
#endif