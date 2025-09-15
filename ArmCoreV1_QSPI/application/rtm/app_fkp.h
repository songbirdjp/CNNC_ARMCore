/**
 * @file app_fkp.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-11-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef _APP_FKP_H_
#define _APP_FKP_H_
#include <stdint.h>
#include "dev_uart.h"
#include "app_manage.h"
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct serial_frame_format
    {
        uint16_t header;
        uint16_t count;
        uint16_t data_len;

        uint8_t id : 7;
        uint8_t ack : 1;
    } __attribute__((aligned(1), packed)) serial_frame_format_t;

    typedef struct fkp_send_structure
    {
        uint16_t OffGantryUnitInfo;

        uint8_t BoardID;
        uint8_t HardwareVersion;
        uint8_t SystemCurrentState;
        uint32_t FirmWareVersion;

        float TotalDose;
        float DeliveredDose;
        uint8_t FkpLedBlink;
        uint8_t Vibration;
        uint8_t beep;
        uint8_t power_off;
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t totalFractions;
        uint8_t fractions;
    } __attribute__((aligned(1), packed)) fkp_send_structure_t;

    typedef struct fkp_recv_structure
    {
        uint16_t FKPUnitInfo;
        uint8_t BoardID;
        uint8_t HardwareVersion;
        uint32_t FirmWareVersion;

        uint16_t FkpButton;
        uint8_t Reserve[7];
    } __attribute__((aligned(1), packed)) fkp_recv_structure_t;

void app_fkp_rx_thread(void *argument);
void app_fkp_tx_thread(void *argument);
#ifdef __cplusplus
}
#endif

#endif /* _APP_FKP_H_ */