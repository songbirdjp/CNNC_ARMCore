/**
 * @file app_cpg.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-05-15
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef _APP_CPG_H_
#define _APP_CPG_H_
#include <stdint.h>
#include "dev_uart.h"
#include "app_manage.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct cpg_send_structure
    {
        uint16_t OffGantryUnitInfo;

        uint8_t BoardID;
        uint8_t HardwareVersion;
        uint32_t FirmWareVersion;

        uint32_t SequenceNum;
        uint8_t SystemCurrentState;
        
        uint32_t CpgLedBlink;
        uint8_t userPrompt;
        uint8_t Reserve[46];
    } __attribute__((aligned(1), packed)) cpg_send_structure_t;

    typedef struct cpg_recv_structure
    {
        uint16_t CPGUnitInfo;

        uint8_t BoardID;
        uint8_t HardwareVersion;
        uint32_t FirmWareVersion;

        uint32_t SequenceNum;

        uint32_t CpgButton;
        uint8_t Reserve[48];
    } __attribute__((aligned(1), packed)) cpg_recv_structure_t;

    typedef struct app_cpg
    {
#define CPG_INIT_BIT (0)
#define CPG_L_LINK_STATE_BIT (1)
#define CPG_R_LINK_STATE_BIT (2)
#define CPG_L_FAULT_STATE_BIT (3)
#define CPG_R_FAULT_STATE_BIT (4)
#define CPG_Y_BRAKE_STATE_BIT (5)
        manage_info_t manage_info;
    } app_cpg_t;

    void app_cpg_rx_thread(void *argument);
    void app_cpg_tx_thread(void *argument);
#ifdef __cplusplus
}
#endif

#endif /* _APP_CPG_H_ */