/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/
/**
 * \addtogroup lan9252_app lan9252_app
 * @{
 */

/**
\file lan9252_app.h
\brief lan9252_app function prototypes and defines

\version 1.0.0.11
 */

 
 #ifndef _LAN9252_APP_H_
#define _LAN9252_APP_H_


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#include "ecatappl.h"

#include "9252_HW.h"

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

#endif //_LAN9252_APP_H_

//include custom application object dictionary 
#include "lan9252_appObjects.h"


#if defined(_LAN9252_APP_) && (_LAN9252_APP_ == 1)
    #define PROTO
#else
    #define PROTO extern
#endif


PROTO void APPL_Application(void);
#if EXPLICIT_DEVICE_ID
PROTO UINT16 APPL_GetDeviceID(void);
#endif

PROTO void   APPL_AckErrorInd(UINT16 stateTrans);
PROTO UINT16 APPL_StartMailboxHandler(void);
PROTO UINT16 APPL_StopMailboxHandler(void);
PROTO UINT16 APPL_StartInputHandler(UINT16 *pIntMask);
PROTO UINT16 APPL_StopInputHandler(void);
PROTO UINT16 APPL_StartOutputHandler(void);
PROTO UINT16 APPL_StopOutputHandler(void);

PROTO UINT16 APPL_GenerateMapping(UINT16 *pInputSize,UINT16 *pOutputSize);
PROTO void APPL_InputMapping(UINT16* pData);
PROTO void APPL_OutputMapping(UINT16* pData);

/* user defined below */
#include "cmsis_os2.h"

typedef struct 
{
    /* internal init */
    osMessageQueueId_t pdo_output_queue;
    osEventFlagsId_t pdo_output_event;
    uint32_t event_flag;
    osMutexId_t pdo_output_update_mutex;
    osMutexId_t pdo_input_update_mutex;
    
    // void (*appl_input_update)(void);
    void (*appl_output_update)(uint16_t *pData);

    /* user init */
    void (*appl_cb)(void);
}LAN9252_APPL_OPS;

LAN9252_APPL_OPS *lan9252_app_ops_get(void);
int8_t lan9252_app_ops_init(void);
int8_t lan9252_app_ops_register(osEventFlagsId_t output_event, uint32_t event_flag, void (*appl_cb)(void));


#undef PROTO
/** @}*/

