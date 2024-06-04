#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include "stm32h7xx_hal.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME_LENGTH      16

#define DEVICE_NAME_UART1       "uart1"
#define DEVICE_NAME_UART2       "uart2"

#define USING_UART_OPTION_FUNCTION

#ifdef USING_UART_OPTION_FUNCTION
typedef struct drv_opt
{
    int8_t (*before_write)(struct drv_uart *uart);
    int8_t (*after_write)(struct drv_uart *uart);
    int8_t (*complete_write)(struct drv_uart *uart);

    int8_t (*before_read)(struct drv_uart *uart);
    int8_t (*after_read)(struct drv_uart *uart);
    int8_t (*complete_read)(struct drv_uart *uart);

}DEVICE_UART_OPT;
#endif

struct drv_uart
{
    UART_HandleTypeDef huart;
    uint8_t name[DEVICE_NAME_LENGTH];
    uint8_t open_state;
    osMessageQueueId_t rx_queue;
    osMutexId_t tx_mutex;
    osEventFlagsId_t tx_event;

    uint8_t *rx_buf;  /* used for dma */ 
    uint16_t rx_buf_len;

    int8_t (*open)(struct drv_uart *uart);
    int8_t (*close)(struct drv_uart *uart);
    int8_t (*write)(struct drv_uart *uart, uint8_t *buf, uint16_t size, uint32_t timeout);
    int8_t (*read)(struct drv_uart *uart, uint8_t *buf, uint32_t timeout);
    int8_t (*ioctl)(struct drv_uart *uart, uint8_t cmd, void *arg);

#ifdef USING_UART_OPTION_FUNCTION
    struct drv_opt opt;
#endif

};

typedef struct drv_uart DEVICE_UART;


int8_t uart_init(DEVICE_UART *uart, uint8_t *device_name);
int8_t uart_rx_queue_init(DEVICE_UART *uart, osMessageQueueId_t queue);
int8_t uart_dma_rx_buf_init(DEVICE_UART *uart, uint8_t *buf, uint16_t len);

#ifdef USING_UART_OPTION_FUNCTION
int8_t uart_opt_init(DEVICE_UART *uart, DEVICE_UART_OPT *opt_func);
#endif


#ifdef __cplusplus
}
#endif

#endif

