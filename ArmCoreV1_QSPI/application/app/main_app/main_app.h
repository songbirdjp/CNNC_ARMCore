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

#define MAX_BEAM_NUM    30

struct ethercat_data_recv     /* master -> slave */
{
    /* data */
};

struct ethercat_data_send    /* slave -> master */
{
    /* data */
};

#define IS_TCP_SERVER
enum serverFsmStates {FSM_NOSTATE,FSM_INIT,FSM_IDLE,FSM_PREPARE,FSM_READY,FSM_SERVO,FSM_MANUAL,FSM_FAULT,FSM_SHUTDOWN};
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_APP_H__ */