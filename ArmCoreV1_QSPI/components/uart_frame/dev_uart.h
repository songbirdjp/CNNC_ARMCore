/**
 * @file dev_uart.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-02-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __DEV_UART_H__
#define __DEV_UART_H__

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os2.h"
#include "dev_base.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /**************************************private*********************************/
    typedef enum uart_type
    {
        UART_TYPE_NULL = 0,
        UART_TYPE_FULL_DUPLEX,
        UART_TYPE_HALF_DUPLEX_MASTER,
        UART_TYPE_HALF_DUPLEX_SLAVE
    } uart_type_t;
    typedef enum uart_mode_t
    {
        UART_MODE_NULL = 0,
        UART_MODE_DMA,
        UART_MODE_BLOCKING,
    } uart_mode_t;
    typedef enum uart_state
    {
        UART_STATE_INIT = 0,
        UART_STATE_READY,
        UART_STATE_BUSY
    } uart_state_t;

#define USE_FRAME_FORMAT
#ifdef USE_FRAME_FORMAT
#include "frame_format.h"
#endif
    typedef struct uart_dev
    {
        const char *name;
        uart_type_t device_type;

        uart_mode_t device_mode;
        osMutexId_t mutex_rx;
        osMutexId_t mutex_tx;

        osMessageQueueId_t osMessageQueueId_rx;
        osSemaphoreId_t osSemaphoreId_tx;

        uint8_t *rx_buf; /* used for dma */
        uint16_t rx_buf_len;

        uint8_t *rx_buf_tmp;

        bool open_state;
        uint16_t open_count;
        uart_state_t state;

        struct device_uart_ops const *ops; /**< Device operations */
        void *user_data;              /**< Private data */
#ifdef USE_FRAME_FORMAT
        bool use_frame_format;
        frame_format_t frame_format;
#endif
    } uart_dev_t;

    typedef enum uart_cmd
    {
        DEV_UART_IOCTL_MIN = 0,
        DEV_UART_IOCTL_USED_FRAME,
/* Add user ioctl commands here */
#ifdef USE_FRAME_FORMAT
        DEV_UART_IOCTL_SET_FRAME_FORMAT,
#endif
        DEV_UART_IOCTL_DRIVER_INIT,
        DEV_UART_IOCTL_MAX,
    } uart_cmd_t;

    typedef struct device_uart_ops
    {
        device_err_t (*init)(uart_dev_t *const self);
        device_err_t (*open)(uart_dev_t *const self);
        device_err_t (*close)(uart_dev_t *const self);
        device_err_t (*read)(uart_dev_t *const self, void *const buffer, uint32_t size, uint32_t timeout);
        device_err_t (*write)(uart_dev_t *const self, void const *const buffer, uint32_t size, uint32_t timeout);
        device_err_t (*ioctl)(uart_dev_t *const self, uint8_t cmd, void *const arg);
    } device_uart_ops_t;
    int32_t device_uart_register(uart_dev_t *const self,
                                 const char *name,
                                 uart_type_t uart_type,
                                 device_uart_ops_t const *const device_ops,
                                 void *const user_data);
    void device_uart_recv_handler(uart_dev_t *const self, void *const buffer, uint32_t size);
    void device_uart_send_handler(uart_dev_t *const self);
/****************************************public********************************/
#define UART_DEV_NAME_CONSOLE "console(USART1)"
#define UART_DEV_NAME_USART2 "USART2"
#define UART_DEV_NAME_USART3 "USART3"
#define UART_DEV_NAME_UART4 "UART4"
#define UART_DEV_NAME_UART5 "UART5"
#define UART_DEV_NAME_USART6 "USART6"
#define UART_DEV_NAME_UART7 "UART7"
#define UART_DEV_NAME_UART8 "UART8"
#define UART_DEV_NAME_UART9 "UART9"
#define UART_DEV_NAME_USART10 "USART10"

    uart_dev_t *device_uart_find(char const *name);

#define DEV_UART_IOCTL_USE_DMA 0x01      /* dma mode, non-blocking mode */
#define DEV_UART_IOCTL_USE_BLOCKING 0x02 /* blocking mode, non-dma mode，not yet supported*/

    device_err_t dev_uart_init(uart_dev_t *dev, uint16_t oflags, uint32_t queueSpace, uint32_t queueMsgSize);
    device_err_t dev_uart_open(uart_dev_t *dev);
    device_err_t dev_uart_close(uart_dev_t *dev);
    device_err_t dev_uart_send(uart_dev_t *dev, uint8_t *buf, uint16_t len, uint32_t timeout);
    device_err_t dev_uart_recv(uart_dev_t *dev, uint8_t *buf, uint16_t len, uint32_t timeout);

#define DEV_UART_CMD_USED_FRAME_FORMAT DEV_UART_IOCTL_USED_FRAME
    typedef struct frame_format_arg
    {
        bool use_frame_format; /* enable or disable frame format */
    } frame_used_arg_t;
    device_err_t dev_uart_config(uart_dev_t *dev, uint8_t cmd, void *arg);
#ifdef __cplusplus
}
#endif

#endif /* __DEV_UART_H__ */