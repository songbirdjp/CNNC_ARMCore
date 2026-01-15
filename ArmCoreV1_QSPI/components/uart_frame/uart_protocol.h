/**
 * @file uart_protocol.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-03-28
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __UART_PROTOCOL_H__
#define __UART_PROTOCOL_H__

#include <stdint.h>
#include "stdbool.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "dev_uart.h"
#include "frame_format.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UART_PROTOCOL_DATA_MAX_LENGTH (128)

    /**************************************private*****************************/

    typedef enum
    {
        UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID = 0, /*!< heartbeat rx timeout callback id */
        UART_PROTOCOL_HEARTBEAT_RX_CB_ID,             /*!< heartbeat rx callback id */
        UART_PROTOCOL_PNT_RX_CB_ID,                   /*!< pnt rx callback id */
        UART_PROTOCOL_CONFIG_SET_RX_CB_ID,            /*!< config set rx callback id */
        UART_PROTOCOL_CONFIG_GET_RX_CB_ID,            /*!< config get rx callback id */
        UART_PROTOCOL_SET_RX_CB_ID,                   /*!< set rx callback id */
        UART_PROTOCOL_GET_RX_CB_ID,                   /*!< get rx callback id */
        UART_PROTOCOL_REBOOT_RX_CB_ID,                /*!< reset rx callback id */
        UART_PROTOCOL_MAX_RX_CB_ID,

        UART_PROTOCOL_HEARTBEAT_TX_CB_ID = 0, /*!< heartbeat tx callback id */
        UART_PROTOCOL_PNT_TX_CB_ID,           /*!< pnt tx callback id */
        UART_PROTOCOL_MAX_TX_CB_ID,
    } uart_protocol_CallbackIDTypeDef;

    typedef int32_t (*uart_protocol_rx_callback_t)(struct uart_protocol *const self,
                                                   uint32_t ID,
                                                   const uint8_t *data,
                                                   uint16_t *len,
                                                   void *arg); /*接收数据传入*/
    typedef int32_t (*uart_protocol_tx_callback_t)(struct uart_protocol *const self,
                                                   uint8_t *data,
                                                   uint16_t *len,
                                                   void *arg); /*发送数据传出*/
    typedef struct
    {
        uart_protocol_rx_callback_t pCallback;
        void *arg;
    } uart_protocol_rx_callback_arg_t;
    typedef struct
    {
        uart_protocol_tx_callback_t pCallback;
        void *arg;
    } uart_protocol_tx_callback_arg_t;
    typedef struct uart_protocol
    {
        uart_dev_t *uart_dev;

        frame_format_t frame_format;

        osTimerId_t uart_protocol_heartbeat_timer;
        osTimerId_t uart_protocol_heartbeat_timeout_timer;
        uint32_t uart_protocol_heartbeat_rx_timeout;

        osTimerId_t uart_protocol_pnt_timer;
        uint32_t uart_protocol_pnt_timeout;

        uart_protocol_rx_callback_arg_t uart_protocol_rx_callback[UART_PROTOCOL_MAX_RX_CB_ID];
        uart_protocol_tx_callback_arg_t uart_protocol_tx_callback[UART_PROTOCOL_MAX_TX_CB_ID];
        osMessageQueueId_t get_rx_response_queue;
        osMessageQueueId_t config_get_response_queue;
    } uart_protocol_t;

    /**************************************public******************************/

    /*TODO:one leader with multi-followers realize*/

    typedef struct
    {
        uint8_t board_id : 3;
        uint8_t HardwareVersion : 5;
        uint32_t FirmWareVersion;
    } __attribute__((aligned(1), packed)) heartbeat_t;

    int32_t uart_protocol_init(uart_protocol_t *const self,
                               const char *name,
                               uint32_t heartbeat_tx_timeout,
                               uint32_t heartbeat_rx_timeout,
                               uint32_t pnt_timeout);
    int32_t uart_protocol_open(uart_protocol_t *const self);
    int32_t uart_protocol_send(uart_protocol_t *const self,
                               const uint8_t *data,
                               uint16_t len,
                               uint32_t timeout);

    int32_t uart_protocol_recv(uart_protocol_t *const self,
                               uint8_t *data,
                               uint16_t *len,
                               uint32_t timeout);

    int32_t uart_protocol_rx_RegisterCallback(uart_protocol_t *const self,
                                              uart_protocol_CallbackIDTypeDef CallbackID,
                                              uart_protocol_rx_callback_t pCallback,
                                              void *arg);
    int32_t uart_protocol_tx_RegisterCallback(uart_protocol_t *const self,
                                              uart_protocol_CallbackIDTypeDef CallbackID,
                                              uart_protocol_tx_callback_t pCallback,
                                              void *arg);
    int32_t uart_protocol_config_set(uart_protocol_t *const self,
                                     uint32_t ID,
                                     const uint8_t *data,
                                     uint32_t len,
                                     uint32_t timeout);
    int32_t uart_protocol_config_get(uart_protocol_t *const self,
                                     uint32_t ID,
                                     uint8_t *data,
                                     uint32_t *len,
                                     uint32_t timeout);
    int32_t uart_protocol_config_get_response(uart_protocol_t *const self,
                                              uint32_t ID,
                                              uint8_t *data,
                                              uint32_t len,
                                              uint32_t timeout);
    int32_t uart_protocol_set(uart_protocol_t *const self,
                              uint32_t ID,
                              uint8_t cmd,
                              const uint8_t *data,
                              uint16_t len,
                              uint32_t timeout);
    int32_t uart_protocol_get(uart_protocol_t *const self,
                              uint32_t ID,
                              uint8_t cmd,
                              const uint8_t *data,
                              uint16_t *len,
                              uint32_t timeout);
    int32_t uart_protocol_reboot(uart_protocol_t *const self,
                                 uint32_t ID,
                                 uint32_t timeout);
#ifdef __cplusplus
}
#endif

#endif /* __UART_PROTOCOL_H__ */