//
// Created by hdsyc on 2023/8/29.
//
//
// Created by Caijiaheng on 2023/8/29.
// Please copy all contents if you want to use uart to control MLC
// Please use putty to control it
//
#include "freertos.h"
#include "usart.h"
#include "main.h"
#include "stdio.h"
#include "httpserver.h"
#include "ecatappl.h"
struct CmdMessage CmdMsg;
UINT16 state_setting;
void ExecuteConsoleCmd(uint16_t _consoleCmd,uint16_t _consolePara)
{
    switch(_consoleCmd)
    {
        case CMD_HELP_MODE :
            Entry2HelpMode();
            break;
        case CMD_CLEAR_MODE:
            printf("all data already clear\r\n");
            break;
        case CMD_RECV_DATA_MODE:

        case CMD_SEND_DATA_MODE:
            if(SendEtherNetData == _consolePara)
            {
                //send to getBuff[];
            }
            else if(SendEtherCATData == _consolePara)
            {
                sDOOutputs.DataOut1[1] = 20;//only demo
            }
            break;
        case CMD_STATE_MACHINE_MODE:
            state_setting = _consolePara;
            break;
        default:
            printf("\r\nunknown command.\r\n");
            break;

    }
}
void Entry2HelpMode(void)
{
    printf("\r\nhelp\thelp\r\n");
    printf("cls\tclean screen\r\n");
}