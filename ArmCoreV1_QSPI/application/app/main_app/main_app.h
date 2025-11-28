#ifndef __MAIN_APP_H__
#define __MAIN_APP_H__
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BANKA
//#define TEST
#ifdef BANKA
#define BANK_NO 1 //A=1 B=2
#else
#define BANK_NO 2 //A=1 B=2
#endif

struct ethercat_data_recv     /* master -> slave */
{
    /* data */
};

struct ethercat_data_send    /* slave -> master */
{
    /* data */
};

#define IS_TCP_SERVER
enum serverFsmStates {
    FSM_NOSTATE,//0
    FSM_INIT,//1
    FSM_IDLE,//2
    FSM_PRELIMINARY,//3
    FSM_PREPARE,//4
    FSM_READY,//5
    FSM_SERVO,//6
    GAP1,
    GAP2,
    GAP3,
    FSM_PARK,//10
    FSM_MANUAL,//11
    FSM_COMPLETE,//12
    FSM_SHUTDOWN,//13
    FSM_POWERSAVE,//14
    FSM_TERMINATE,//15
    FSM_INTERRUPT,//16
    UART_DEBUG,//17
    TOTAL_FSM
};
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_APP_H__ */