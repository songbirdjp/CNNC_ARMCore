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

extern const uint8_t BGM_ARM_IO_Version[4];
#define AFC_SHELL_CMD
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
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(BGM2AFC_Handshake,B2AHS,"Dose Board Handshake Set");
#endif
void AFC_SetADCSampleMode(uint8_t mode)
{
    uint8_t _afcCmd[3] = {0x01,0x01,0x00};
    _afcCmd[2] = mode;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,3); 
}
void Shell_AFC_SetADCSampleMode(int8_t argc, uint8_t **argv)
{
    uint8_t mode = strtol((char *)argv[1], NULL, 16);
    AFC_SetADCSampleMode(mode);
}
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(Shell_AFC_SetADCSampleMode,AFCSAMMODE,"Set ADC Sample Mode");
#endif
void AFC_SetADCSampleDelay(uint16_t delay)
{
    uint8_t _afcCmd[3] = {0x01,0x02,0x00};
    _afcCmd[2] = delay;
    _afcCmd[3] = delay >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,3); 
}
void Shell_AFC_SetADCSampleDelay(int8_t argc, uint8_t **argv)
{
    uint16_t delay = (strtol((char *)argv[2], NULL, 16) << 8) | strtol((char *)argv[1], NULL, 16);
    AFC_SetADCSampleDelay(delay);
}
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(Shell_AFC_SetADCSampleDelay,AFCSAMDELAY,"Set ADC Sample Delay");
#endif
void AFC_DeleteADCData(void)
{
    uint8_t _afcCmd[2] = {0x01,0x03};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2);    
}
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(AFC_DeleteADCData,AFCDATADLT,"Delete ADC Data");
#endif
void AFC_GetADCValueByFrame(void)
{
    uint8_t _afcCmd[2] = {0x01,0x04};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2); 
}
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(AFC_GetADCValueByFrame,AFCDATAGET,"Get ADC Value By Frame");
#endif
void AFC_IS_MagMotorFindZeroOK(void)//0x40,0x00
{
    uint8_t _afcCmd[2] = {0x40,0x00};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2); 
}
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(AFC_IS_MagMotorFindZeroOK,AFCMAGFINDZERO,"IS Mag Motor Find Zero OK");
#endif
void AFC_MagMotorSetPos(uint16_t pos)//0x40,0x01
{
    uint8_t _afcCmd[4] = {0x40,0x01,0x00,0x00};
    _afcCmd[2] = pos;
    _afcCmd[3] = pos >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,4); 
}
void Shell_AFC_MagMotorSetPos(int8_t argc, uint8_t **argv)//0x40,0x01
{
    uint16_t pos = (strtol((char *)argv[2], NULL, 16) << 8) | strtol((char *)argv[1], NULL, 16);
    AFC_MagMotorSetPos(pos);
}
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(Shell_AFC_MagMotorSetPos,AFCMAGPOS,"Mag Motor Set Pos");
#endif
void AFC_MagMotorRunByStep(uint8_t dir,uint16_t _stepVal)//0x40,0x02
{
    uint8_t _afcCmd[5] = {0x40,0x02,0x00,0x00,0x00};
    _afcCmd[2] = dir;
    _afcCmd[3] = _stepVal;
    _afcCmd[4] = _stepVal >> 8;
    printf("dir:%d,stepVal:%d\r\n",dir,_stepVal);
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,5); 
}
void Shell_AFC_MagMotorRunByStep(int8_t argc, uint8_t **argv)//0x40,0x02
{
    uint8_t dir = strtol((char *)argv[1], NULL, 16);
    uint16_t _stepVal = (strtol((char *)argv[3], NULL, 16) << 8) | strtol((char *)argv[2], NULL, 16);   
    AFC_MagMotorRunByStep(dir,_stepVal);
}
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(Shell_AFC_MagMotorRunByStep,AFCMAGSTEP,"Mag Motor Run By Step");
#endif
void AFC_MagMotorGetEncValue(void)//0x40,0x03
{
    uint8_t _afcCmd[2] = {0x40,0x03};
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,2); 
}
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(AFC_MagMotorGetEncValue,AFCMAGENC,"Mag Motor Get Enc Value");
#endif
void AFC_MagMotorSetPresetPos(uint16_t pos)//0x40,0x04
{
    uint8_t _afcCmd[4] = {0x40,0x04,0x00,0x00};
    _afcCmd[2] = pos;
    _afcCmd[3] = pos >> 8;
    BGM_SendCmd(BGM_UART_AFC,UARTCmdType_CommandDown,_afcCmd,4); 
}
void Shell_AFC_MagMotorSetPresetPos(int8_t argc, uint8_t **argv)//0x40,0x04
{
    uint16_t pos = (strtol((char *)argv[2], NULL, 16) << 8) | strtol((char *)argv[1], NULL, 16);
    AFC_MagMotorSetPresetPos(pos);
}
#ifdef AFC_SHELL_CMD
MSH_CMD_EXPORT_ALIAS(Shell_AFC_MagMotorSetPresetPos,AFCMAGPRESET,"Mag Motor Set Preset Pos");
#endif
