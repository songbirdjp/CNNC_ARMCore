#include "drv_uart.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"

#define UART_SEND_SUCCEED_EVENT     (1<<0)

static void ErrorCallback(UART_HandleTypeDef *huart)
{
    DEVICE_UART *uart = (DEVICE_UART *)huart;

    printf("%s err", uart->name);
}

static void RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    osStatus_t ret = osOK;
    DEVICE_UART *uart = (DEVICE_UART *)huart;

    *(uint16_t *)&uart->rx_buf[uart->rx_buf_len] = size;  /* rx_buf last but two byte indicate valid data length, because uart use dma idle mode */

    ret = osMessageQueuePut(uart->rx_queue, uart->rx_buf, 0, 0);
    if (ret != osOK)
    {
        printf("%s queue put err:%d\r\n", uart->name, ret);
    }

    HAL_UARTEx_ReceiveToIdle_DMA((UART_HandleTypeDef *)uart, (uint8_t *)uart->rx_buf, uart->rx_buf_len);
}

static void TxCpltCallback(UART_HandleTypeDef *huart)
{
#ifndef ULOG_USING_ISR
    DEVICE_UART *uart = (DEVICE_UART *)huart;
    osEventFlagsSet(uart->tx_event, UART_SEND_SUCCEED_EVENT);
#endif
}

static int8_t uart_open(DEVICE_UART *uart)
{
    if (uart->open_state)
    {
        printf("device %s already opened\r\n", uart->name);
        return -1;
    }
    else
    {
        uart->open_state = 1;
    }

    HAL_UARTEx_ReceiveToIdle_DMA((UART_HandleTypeDef *)uart, (uint8_t *)uart->rx_buf, uart->rx_buf_len);

    return 0;
}

static int8_t uart_close(DEVICE_UART *uart)
{
    HAL_StatusTypeDef ret = HAL_OK;
    osStatus_t stat = osOK;

    if (uart->open_state)
    {
        ret = HAL_UART_DeInit((UART_HandleTypeDef *)uart);
        if (ret != HAL_OK)
        {
            printf("device %s deinit err:%d\r\n", uart->name, ret);
            return -1;
        }

        stat = osEventFlagsDelete(uart->tx_event);
        if (stat != osOK)
        {
            printf("device %s delete event err:%d\r\n", uart->name, stat);
            return -2;
        }

        stat = osMutexDelete(uart->tx_mutex);
        if (stat != osOK)
        {
            printf("device %s delete mutex err:%d\r\n", uart->name, stat);
            return -3;
        }

        uart->open_state = 0;
    }
    else
    {
        /* device already closed */
    }

    return 0;
}

static int8_t uart_write(DEVICE_UART *uart, uint8_t *buf, uint16_t size, uint32_t timeout)
{
    osStatus_t ret = osOK;
    HAL_StatusTypeDef status = HAL_OK;

    if (!uart->open_state)
    {
        printf("device %s is closed\r\n", uart->name);
        return -1;
    }

#ifndef ULOG_USING_ISR
    ret = osMutexAcquire(uart->tx_mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", uart->name, ret);
        return -2;
    }
#endif

#ifdef USING_UART_OPTION_FUNCTION
    if (uart->opt.before_write != NULL)
    {
        uart->opt.before_write(uart);
    }
#endif

    status = HAL_UART_Transmit_DMA((UART_HandleTypeDef *)uart, buf, size);
    if (status != HAL_OK)
    {
        printf("device %s write data err:%d\r\n", uart->name, status);
        ret = -3;
        goto err;
    }

#ifdef USING_UART_OPTION_FUNCTION
    if (uart->opt.after_write != NULL)
    {
        uart->opt.after_write(uart);
    }
#endif

#ifndef ULOG_USING_ISR
    uint32_t ret_val = osEventFlagsWait(uart->tx_event, UART_SEND_SUCCEED_EVENT, osFlagsWaitAny, timeout);
    if (ret_val != UART_SEND_SUCCEED_EVENT)
    {
        printf("device %s  wait event flag err: %#.8x\r\n", uart->name, ret_val);
        ret = -4;
        goto err;
    }
#endif

err:
#ifdef USING_UART_OPTION_FUNCTION
    if (uart->opt.complete_write != NULL)
    {
        uart->opt.complete_write(uart);
    }
#endif

#ifndef ULOG_USING_ISR
    osMutexRelease(uart->tx_mutex);
#endif

    return ret;
}

static int8_t uart_read(DEVICE_UART *uart, uint8_t *buf, uint32_t timeout)
{
    osStatus_t ret = osOK;

    if (!uart->open_state)
    {
        printf("device %s is closed\r\n", uart->name);
        return -1;
    }

#ifdef USING_UART_OPTION_FUNCTION
    if (uart->opt.before_read != NULL)
    {
        uart->opt.before_read(uart);
    }
#endif

#ifdef USING_UART_OPTION_FUNCTION
    if (uart->opt.after_read != NULL)
    {
        uart->opt.after_read(uart);
    }
#endif 

    ret = osMessageQueueGet(uart->rx_queue, buf, 0, timeout);
    if (ret != osOK)
    {
        printf("device %s read data err:%d\r\n", uart->name, ret);
        ret = -2;
    }

#ifdef USING_UART_OPTION_FUNCTION
    if (uart->opt.complete_read != NULL)
    {
        uart->opt.complete_read(uart);
    }
#endif

    return ret;
}


#ifdef USING_UART_OPTION_FUNCTION
static int8_t uart_opt_init(DEVICE_UART *uart, DEVICE_UART_OPT *opt_func)
{
    if (uart == NULL || opt_func == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    memcpy(&uart->opt, opt_func, sizeof(DEVICE_UART_OPT));

    return 0;
}
#endif

static int8_t uart_rx_queue_init(DEVICE_UART *uart, osMessageQueueId_t queue)
{
    if (uart == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    uart->rx_queue = queue;

    return 0;
}

static int8_t uart_dma_rx_buf_init(DEVICE_UART *uart, uint8_t *buf, uint16_t len)
{
    if (uart == NULL || buf == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }
    
    if (len == 0)
    {
        printf("len is zero\r\n");
        return -2;
    }
    
    uart->rx_buf = buf;
    uart->rx_buf_len = len;

    return 0;
}

static int8_t uart_ioctl(DEVICE_UART *uart, uint8_t cmd, void *arg)
{
    int8_t ret = 0;

    if (uart == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    switch (cmd)
    {
#ifdef USING_UART_OPTION_FUNCTION
    case UART_CMD_SET_OPT_FUNC:
        ret = uart_opt_init(uart, (DEVICE_UART_OPT *)arg);
        break;
#endif
    case UART_CMD_SET_DMA_RX_QUEUE:
        ret = uart_rx_queue_init(uart, (osMessageQueueId_t)arg);
        break;
    case UART_CMD_SET_DMA_RX_BUF:
    {
        uint32_t buf = *(uint32_t *)arg;
        uint16_t len = *(uint16_t *)((uint8_t *)arg + sizeof(buf));
        ret = uart_dma_rx_buf_init(uart, buf, len);
        break;
    }
    default:
        printf("uart ioctl cmd %d is not supported\r\n", cmd);
        return -2;
    }

    return ret;
}

/* default configure for uart is dma mode
* 1) queue 、mutex and event init, queue and/or event for rx, mutex for tx
* 2) add send function, and release mutex in complete callback function
* 3) add receive function, and put data to queue or send event
* 4) add port for app, to get data from queue with timeout
*/
int8_t uart_init(DEVICE_UART *uart, uint8_t *device_name)
{
    if (uart == NULL || device_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    if (uart->open_state)
    {
        printf("device %s is opened\r\n", uart->name);
        return -2;
    }

    /* 1. init hardware */
    if (!memcmp(device_name, DEVICE_NAME_UART1, sizeof(DEVICE_NAME_UART1)))
    {
        MX_USART1_UART_Init();
        memcpy(uart, &huart1, sizeof(UART_HandleTypeDef));
    }
    else
    {
        /* add other uart here */
    }

    /* 2. create queue 、event and mutex for device */
    // osMessageQueueAttr_t CmdQueue_attributes = {
    // .name = "uart_rx_queue"
    // };
    // uart->rx_queue = osMessageQueueNew (16, sizeof(struct CmdMessage), &CmdQueue_attributes);  /* modify queue size and count for different aim */

    osMutexAttr_t uart_tx_mutex_attributes = {
    .name = "uart_tx_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    uart->tx_mutex = osMutexNew(&uart_tx_mutex_attributes);

    const osEventFlagsAttr_t uart_tx_event_attributes = {
    .name = "uart_tx_event"
    };
    uart->tx_event = osEventFlagsNew(&uart_tx_event_attributes);
    

    /* 3. rx buf malloc for dma mode */
    // uart->rx_buf = (uint8_t *)pvPortMalloc(sizeof(struct CmdMessage));  /* here should check when use dma mode */
    // if (uart->rx_buf == NULL)
    // {
    //     return -3;
    // }

    // uart->rx_buf_len = sizeof(struct CmdMessage) - sizeof(uint16_t); /* indicate rx buf max len */
    
    /* 4. register callback function */
    HAL_UART_RegisterCallback((UART_HandleTypeDef *)uart, HAL_UART_ERROR_CB_ID, ErrorCallback);
    HAL_UART_RegisterCallback((UART_HandleTypeDef *)uart, HAL_UART_TX_COMPLETE_CB_ID, TxCpltCallback);
    // HAL_UART_RegisterCallback((UART_HandleTypeDef *)uart, HAL_UART_RX_COMPLETE_CB_ID, RxCpltCallback);
    HAL_UART_RegisterRxEventCallback((UART_HandleTypeDef *)uart, RxEventCallback);

    /* 5. device rename */
    memcpy(uart->name, device_name, DEVICE_NAME_LENGTH);

    /* 6. register operation function */
    uart->open = uart_open;
    uart->close = uart_close;
    uart->write = uart_write;
    uart->read = uart_read;
    uart->ioctl = uart_ioctl;

    /* 7. open device */
    return 0;//uart->open(uart);
}
