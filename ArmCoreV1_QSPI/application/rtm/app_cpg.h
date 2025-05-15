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
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct cpg_send_structure
    {
        uint16_t OffGantryUnitInfo;

        uint8_t BoardID;
        uint8_t HardwareVersion;
        uint8_t SystemCurrentState;
        uint32_t FirmWareVersion;

        uint32_t CpgLedBlink;
        uint8_t Vibration;
        uint8_t Backlight;
        uint8_t buzzer;
        uint8_t Reserve[8];
    } __attribute__((aligned(1), packed)) cpg_send_structure_t;

    typedef struct cpg_recv_structure
    {
        uint16_t CPGUnitInfo;
        uint8_t BoardID;
        uint8_t HardwareVersion;
        uint32_t FirmWareVersion;

        uint32_t CpgButton;
        uint8_t Reserve[8];
    } __attribute__((aligned(1), packed)) cpg_recv_structure_t;

#ifdef __cplusplus
}
#endif

#endif /* _APP_CPG_H_ */