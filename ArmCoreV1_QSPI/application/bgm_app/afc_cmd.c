#include "afc_cmd.h"
#include "bgm_def.h"
#include <stddef.h>
#include "shell.h" 
#include "ulog.h"

// extern const uint8_t BGM_ARM_IO_Version[4];

int BGM2AFC_Handshake(void)
{
    struct cmd_object BGM2AFCHandshake;
    // BGM2AFCHandshake.id.byte = 0x80;
    // BGM2AFCHandshake.type = UARTCmdType_HandshakeDown;
    // BGM2AFCHandshake.len = sizeof(BGM_ARM_IO_Version);
    // BGM2AFCHandshake.data = BGM_ARM_IO_Version;
    LOG_I("BGM2AFC_Handshake\r\n");
    return uart_cmd_write(BGM_UART_AFC,&BGM2AFCHandshake);
}
/////////////////////////////////////////////////////////////0x01,0x00
void AFC_SetAFCControlMode(uint8_t mode)
{
    uint8_t _afcCmd[3] = {0x01,0x00,0x00};
    _afcCmd[2] = mode;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,3); 
}
/////////////////////////////////////////////////////////////0x01,0x01
void AFC_SetAFCSampleMode(uint8_t mode)
{
    uint8_t _afcCmd[3] = {0x01,0x01,0x00};
    _afcCmd[2] = mode;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,3); 
}
/////////////////////////////////////////////////////////////0x01,0x02
void AFC_SetADCSampleDelay(uint16_t delay)
{
    uint8_t _afcCmd[4] = {0x01,0x02,0x00,0x00};
    _afcCmd[2] = delay;
    _afcCmd[3] = delay >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,4); 
}
/////////////////////////////////////////////////////////////0x01,0x03
void AFC_DeleteADCData(void)
{
    uint8_t _afcCmd[2] = {0x01,0x03};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2);    
}
/////////////////////////////////////////////////////////////0x01,0x04
void AFC_GetADCValueByFrame(void)
{
    uint8_t _afcCmd[2] = {0x01,0x04};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2); 
}
/////////////////////////////////////////////////////////////0x40,0x00
void AFC_IS_MagMotorFindZeroOK(void)
{
    uint8_t _afcCmd[2] = {0x40,0x00};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2); 
}

/////////////////////////////////////////////////////////////0x40,0x01
void AFC_MagMotorSetPos(uint16_t pos)//0x40,0x01
{
    uint8_t _afcCmd[4] = {0x40,0x01,0x00,0x00};
    _afcCmd[2] = pos;
    _afcCmd[3] = pos >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,4); 
}
/////////////////////////////////////////////////////////////0x40,0x02
void AFC_MagMotorRunByStep(uint8_t dir,uint16_t _stepVal)//0x40,0x02
{
    uint8_t _afcCmd[5] = {0x40,0x02,0x00,0x00,0x00};
    _afcCmd[2] = dir;
    _afcCmd[3] = _stepVal;
    _afcCmd[4] = _stepVal >> 8;
    // LOG_I("dir:%d,stepVal:%d\r\n",dir,_stepVal);
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,5); 
}

/////////////////////////////////////////////////////////////0x40,0x03
void AFC_MagMotorGetEncValue(void)//0x40,0x03
{
    uint8_t _afcCmd[2] = {0x40,0x03};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2); 
}

/////////////////////////////////////////////////////////////0x40,0x04
void AFC_MagMotorSetPresetPos(uint16_t pos)//0x40,0x04
{
    uint8_t _afcCmd[4] = {0x40,0x04,0x00,0x00};
    _afcCmd[2] = pos;
    _afcCmd[3] = pos >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,4); 
}
/////////////////////////////////////////////////////////////0x40,0x05
void AFC_MagMotorSetDeadZone(uint16_t deadZone)//0x40,0x05
{
    uint8_t _afcCmd[4] = {0x40,0x05,0x00,0x00};
    _afcCmd[2] = deadZone;
    _afcCmd[3] = deadZone >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,4); 
}
/////////////////////////////////////////////////////////////0x41,0x00
void AFC_IS_AFTMotorFindZeroOK(void)//0x41,0x00
{
    uint8_t _afcCmd[2] = {0x40,0x00};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2); 
}
/////////////////////////////////////////////////////////////0x41,0x01
void AFC_AFTMotorSetPos(uint16_t pos)//0x41,0x01
{
    uint8_t _afcCmd[4] = {0x41,0x01,0x00,0x00};
    _afcCmd[2] = pos;
    _afcCmd[3] = pos >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,4); 
}
/////////////////////////////////////////////////////////////0x41,0x02
void AFC_AFTMotorRunByStep(uint8_t dir,uint16_t _stepVal)//0x41,0x02
{
    uint8_t _afcCmd[5] = {0x41,0x02,0x00,0x00,0x00};
    _afcCmd[2] = dir;
    _afcCmd[3] = _stepVal;
    _afcCmd[4] = _stepVal >> 8;
    //LOG_I("dir:%d,stepVal:%d\r\n",dir,_stepVal);
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,5); 
}
/////////////////////////////////////////////////////////////0x41,0x03
void AFC_AFTMotorGetEncValue(void)//0x41,0x03
{
    uint8_t _afcCmd[2] = {0x41,0x03};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2); 
}

/////////////////////////////////////////////////////////////0x41,0x04
void AFC_AFTMotorSetPresetPos(uint16_t pos)//0x40,0x04
{
    uint8_t _afcCmd[4] = {0x41,0x04,0x00,0x00};
    _afcCmd[2] = pos;
    _afcCmd[3] = pos >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,4); 
}
/////////////////////////////////////////////////////////////0x41,0x05
void AFC_SetAFTBrakeStatus(uint8_t status)
{
    uint8_t _afcCmd[3] = {0x41,0x05,0x00};
    _afcCmd[2] = status;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,3); 
}
/////////////////////////////////////////////////////////////0x41,0x06
void AFC_SetAFTMotorStatus(uint8_t status)
{
    uint8_t _afcCmd[3] = {0x41,0x06,0x00};
    _afcCmd[2] = status;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,3); 
}
/////////////////////////////////////////////////////////////0x41,0x07
void AFC_SetAFTMotorDeadZone(uint16_t deadZone)
{
    uint8_t _afcCmd[4] = {0x41,0x07,0x00,0x00}; 
    _afcCmd[2] = deadZone;
    _afcCmd[3] = deadZone >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,4); 
}

#define AFC_SHELL_CMD
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(BGM2AFC_Handshake,B2AHS,"Dose Board Handshake Set");
MSH_CMD_EXPORT_ALIAS(AFC_SetAFCControlMode,AFCCTRLMODE,"Set AFC Control Mode");
MSH_CMD_EXPORT_ALIAS(AFC_SetAFCSampleMode,AFCSAMPLEMODE,"Set AFC Sample Mode");
void Shell_AFC_SetADCSampleDelay(int8_t argc, uint8_t **argv)
{
    uint16_t delay = (strtol((char *)argv[2], NULL, 16) << 8) | strtol((char *)argv[1], NULL, 16);
    AFC_SetADCSampleDelay(delay);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_SetADCSampleDelay,AFCSAMDELAY,"Set ADC Sample Delay");
MSH_CMD_EXPORT_ALIAS(AFC_DeleteADCData,AFCDATADLT,"Delete ADC Data");
MSH_CMD_EXPORT_ALIAS(AFC_GetADCValueByFrame,AFCDATAGET,"Get ADC Value By Frame");
MSH_CMD_EXPORT_ALIAS(AFC_IS_MagMotorFindZeroOK,AFCMAGFINDZERO,"IS Mag Motor Find Zero OK");
void Shell_AFC_MagMotorSetPos(int8_t argc, uint8_t **argv)//0x40,0x01
{
    uint16_t pos = (strtol((char *)argv[2], NULL, 16) << 8) | strtol((char *)argv[1], NULL, 16);
    AFC_MagMotorSetPos(pos);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_MagMotorSetPos,AFCMAGPOS,"Mag Motor Set Pos");
void Shell_AFC_MagMotorRunByStep(int8_t argc, uint8_t **argv)//0x40,0x02
{
    uint8_t dir = strtol((char *)argv[1], NULL, 16);
    uint16_t _stepVal = (strtol((char *)argv[3], NULL, 16) << 8) | strtol((char *)argv[2], NULL, 16);   
    AFC_MagMotorRunByStep(dir,_stepVal);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_MagMotorRunByStep,AFCMAGSTEP,"Mag Motor Run By Step");
MSH_CMD_EXPORT_ALIAS(AFC_MagMotorGetEncValue,AFCMAGENC,"Mag Motor Get Enc Value");
void Shell_AFC_MagMotorSetPresetPos(int8_t argc, uint8_t **argv)//0x40,0x04
{
    uint16_t pos = (strtol((char *)argv[2], NULL, 16) << 8) | strtol((char *)argv[1], NULL, 16);
    AFC_MagMotorSetPresetPos(pos);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_MagMotorSetPresetPos,AFCMAGPRESET,"Mag Motor Set Preset Pos");
MSH_CMD_EXPORT_ALIAS(AFC_IS_MagMotorFindZeroOK,AFCAFTFINDZERO,"IS Mag Motor Find Zero OK");
void Shell_AFC_AFTMotorSetPos(int8_t argc, uint8_t **argv)//0x40,0x01
{
    uint16_t pos = (strtol((char *)argv[2], NULL, 16) << 8) | strtol((char *)argv[1], NULL, 16);
    AFC_AFTMotorSetPos(pos);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_AFTMotorSetPos,AFTMAGPOS,"AFT Motor Set Pos");//0x41,0x01
void Shell_AFC_AFTMotorRunByStep(int8_t argc, uint8_t **argv)//0x41,0x02
{
    uint8_t dir = strtol((char *)argv[1], NULL, 16);
    uint16_t _stepVal = (strtol((char *)argv[3], NULL, 16) << 8) | strtol((char *)argv[2], NULL, 16);   
    AFC_AFTMotorRunByStep(dir,_stepVal);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_AFTMotorRunByStep,AFCAFTSTEP,"AFT Motor Run By Step");//0x41,0x02
MSH_CMD_EXPORT_ALIAS(AFC_AFTMotorGetEncValue,AFCAFTENC,"AFT Motor Get Enc Value");//0x41,0x03
void Shell_AFC_AFTMotorSetPresetPos(int8_t argc, uint8_t **argv)//0x41,0x04
{
    uint16_t pos = (strtol((char *)argv[2], NULL, 16) << 8) | strtol((char *)argv[1], NULL, 16);
    AFC_AFTMotorSetPresetPos(pos);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_AFTMotorSetPresetPos,AFCAFTPRESET,"AFT Motor Set Preset Pos");//0x41,0x04
void Shell_AFC_SetAFTBrakeStatus(int8_t argc, uint8_t **argv)
{
    uint8_t status = strtol((char *)argv[1], NULL, 16);
    AFC_SetAFTBrakeStatus(status);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_SetAFTBrakeStatus,AFTBRAKE,"AFT Brake Status Set");//0x41,0x05
void Shell_AFC_SetAFTMotorStatus(int8_t argc, uint8_t **argv)
{
    uint8_t status = strtol((char *)argv[1], NULL, 16);
    AFC_SetAFTMotorStatus(status);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_SetAFTMotorStatus,AFTMOTOR,"AFT Motor Status Set");//0x41,0x06
void Shell_AFC_SetAFTMotorDeadZone(int8_t argc, uint8_t **argv)
{
    uint16_t deadZone = (strtol((char *)argv[2], NULL, 16) << 8) | strtol((char *)argv[1], NULL, 16);
    AFC_SetAFTMotorDeadZone(deadZone);
}
MSH_CMD_EXPORT_ALIAS(Shell_AFC_SetAFTMotorDeadZone,AFTMOTORDZ,"AFT Motor Dead Zone Set");//0x41,0x07
#endif