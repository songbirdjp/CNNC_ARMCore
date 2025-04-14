/**
 * @file drv_uart.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-02-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "dev_uart.h"
#include "usart.h"
#include "ulog.h"
#include <string.h>
#include "init_call.h"

#define USING_UART_OPTION_FUNCTION

typedef struct uart_drv
{
    uart_dev_t dev;
    UART_HandleTypeDef *huart;

#ifdef USING_UART_OPTION_FUNCTION
    struct drv_opt *opt;
#endif
} uart_drv_t;

#ifdef USING_UART_OPTION_FUNCTION
typedef struct drv_opt
{
    int8_t (*before_write)(uart_drv_t *uart);
    int8_t (*after_write)(uart_drv_t *uart);
    int8_t (*complete_write)(uart_drv_t *uart);

    int8_t (*before_read)(uart_drv_t *uart);
    int8_t (*after_read)(uart_drv_t *uart);
    int8_t (*complete_read)(uart_drv_t *uart);

} drv_opt_t;
static int8_t drv_uart_write_before(uart_drv_t *uart);
static int8_t drv_uart_write_after(uart_drv_t *uart);
static int8_t drv_uart_write_complete(uart_drv_t *uart);
static int8_t drv_uart_read_before(uart_drv_t *uart);
static int8_t drv_uart_read_after(uart_drv_t *uart);
static int8_t drv_uart_read_complete(uart_drv_t *uart);
#endif

static uart_drv_t *uart_drv_get(UART_HandleTypeDef *huart);
static int8_t drv_uart_init(uart_dev_t *const self);
static void ErrorCallback(UART_HandleTypeDef *huart)
{
    uart_drv_t *uart_drv = uart_drv_get(huart);
    if (uart_drv == NULL)
    {
        return;
    }
    LOG_I("device %s err:%d\r\n", uart_drv->dev.name, huart->ErrorCode);
}

static void RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    uart_drv_t *uart_drv = uart_drv_get(huart);
    if (uart_drv == NULL)
    {
        return;
    }
    if (huart->RxEventType == HAL_UART_RXEVENT_HT)
    {
        return;
    }
    device_uart_recv_handler(&uart_drv->dev,
                             uart_drv->dev.rx_buf,
                             size);

    // __disable_irq();

    HAL_UARTEx_ReceiveToIdle_DMA(uart_drv->huart,
                                 uart_drv->dev.rx_buf,
                                 uart_drv->dev.rx_buf_len);

    __HAL_UART_DISABLE_IT(uart_drv->huart, UART_IT_ERR);

    // __enable_irq();
}

static void TxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_drv_t *uart_drv = uart_drv_get(huart);
    if (uart_drv == NULL)
    {
        return;
    }
    device_uart_send_handler(&uart_drv->dev);
}

static int8_t drv_uart_open(uart_dev_t *const self)
{
    if (self == NULL)
    {
        return -1;
    }
    uart_drv_t *uart_drv = (uart_drv_t *)self->user_data;
    if (uart_drv->huart == NULL)
    {
        return -2;
    }
    __disable_irq();
    HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_DMA(uart_drv->huart,
                                                            uart_drv->dev.rx_buf,
                                                            uart_drv->dev.rx_buf_len);
    if (status != HAL_OK)
    {
        return -3;
    }
    __HAL_UART_DISABLE_IT(uart_drv->huart, UART_IT_ERR);
    __HAL_UART_ENABLE(uart_drv->huart);
    __enable_irq();

    return 0;
}
static int8_t drv_uart_close(uart_dev_t *const self)
{
    if (self == NULL)
    {
        return -1;
    }
    uart_drv_t *uart_drv = (uart_drv_t *)self->user_data;
    if (uart_drv->huart == NULL)
    {
        return -2;
    }
    HAL_StatusTypeDef status = HAL_UART_DeInit(uart_drv->huart);
    if (status != HAL_OK)
    {
        return -3;
    }
    return 0;
}
static int8_t drv_uart_write(uart_dev_t *const self,
                             void const *const buffer,
                             uint32_t size,
                             uint32_t timeout)
{
    if (self == NULL || buffer == NULL || size == 0)
    {
        return -1;
    }
    uart_drv_t *uart_drv = (uart_drv_t *)self->user_data;

#ifdef USING_UART_OPTION_FUNCTION
    if (uart_drv->opt->before_write != NULL)
    {
        uart_drv->opt->before_write(uart_drv);
    }
#endif
    HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(uart_drv->huart,
                                                     (uint8_t const *)buffer,
                                                     size);
    if (status != HAL_OK)
    {
        return -2;
    }
#ifdef USING_UART_OPTION_FUNCTION
    if (uart_drv->opt->after_write != NULL)
    {
        uart_drv->opt->after_write(uart_drv);
    }
#endif

#ifdef USING_UART_OPTION_FUNCTION
    if (uart_drv->opt->complete_write != NULL)
    {
        uart_drv->opt->complete_write(uart_drv);
    }
#endif
    return 0;
}
static int8_t drv_uart_read(uart_dev_t *const self,
                            void *const buffer,
                            uint32_t size,
                            uint32_t timeout)
{
    if (self == NULL || buffer == NULL || size == 0)
    {
        return -1;
    }
    uart_drv_t *uart_drv = (uart_drv_t *)self->user_data;

#ifdef USING_UART_OPTION_FUNCTION
    if (uart_drv->opt->before_read != NULL)
    {
        uart_drv->opt->before_read(uart_drv);
    }
#endif

#ifdef USING_UART_OPTION_FUNCTION
    if (uart_drv->opt->after_read != NULL)
    {
        uart_drv->opt->after_read(uart_drv);
    }
#endif

#ifdef USING_UART_OPTION_FUNCTION
    if (uart_drv->opt->complete_read != NULL)
    {
        uart_drv->opt->complete_read(uart_drv);
    }
#endif
    return 0;
}
static int8_t drv_uart_ioctl(uart_dev_t *const self, uint8_t cmd, void *const arg)
{
    if (self == NULL || arg == NULL || cmd >= DEV_UART_IOCTL_MAX)
    {
        return -1;
    }

    uart_drv_t *uart_drv = (uart_drv_t *)self->user_data;

    switch (cmd)
    {
    default:
        break;
    }

    return 0;
}

static int32_t drv_uart_register(uart_drv_t *drv,
                                 UART_HandleTypeDef *huart,
                                 const char *name,
                                 uart_type_t uart_type)
{
    if ((drv == NULL) ||
        (huart == NULL) ||
        (name == NULL) ||
        ((uart_type != UART_TYPE_FULL_DUPLEX) &&
         (uart_type != UART_TYPE_HALF_DUPLEX_MASTER) &&
         (uart_type != UART_TYPE_HALF_DUPLEX_SLAVE)))
    {
        return -1;
    }

    memset(drv, 0, sizeof(uart_drv_t));

    drv->huart = huart;

#ifdef USING_UART_OPTION_FUNCTION
    static drv_opt_t uart_opt = {
        .before_write = drv_uart_write_before,
        .after_write = drv_uart_write_after,
        .complete_write = drv_uart_write_complete,

        .before_read = drv_uart_read_before,
        .after_read = drv_uart_read_after,
        .complete_read = drv_uart_read_complete,
    };
    drv->opt = &uart_opt;
#endif

    static device_uart_ops_t const uart_ops = {
        .init = drv_uart_init,
        .open = drv_uart_open,
        .close = drv_uart_close,
        .read = drv_uart_read,
        .write = drv_uart_write,
        .ioctl = drv_uart_ioctl,
    };

    int32_t ret = device_uart_register(&drv->dev,
                                       name,
                                       uart_type,
                                       (device_uart_ops_t *const)&uart_ops,
                                       drv);
    if (ret != 0)
    {
        return -2;
    }

    return 0;
}

/********************************************add user defined uart below******************************************************************/
static int8_t drv_uart_init(uart_dev_t *const self)
{
    if (self == NULL)
    {
        return -1;
    }
    uart_drv_t *uart_drv = (uart_drv_t *)self->user_data;
    if (uart_drv->huart == NULL)
    {
        return -2;
    }
    __disable_irq();
    if (!memcmp(uart_drv->dev.name, UART_DEV_NAME_CONSOLE, sizeof(UART_DEV_NAME_CONSOLE)))
    {
        MX_USART1_UART_Init();
    }
    else
    {
        /* add other uart here */ /**<------ add other uart here*/
    }
    __HAL_UART_DISABLE(uart_drv->huart);
    __HAL_UART_DISABLE_IT(uart_drv->huart, UART_IT_ERR);
    __HAL_UART_CLEAR_FLAG(uart_drv->huart, UART_CLEAR_PEF |
                                               UART_CLEAR_FEF |
                                               UART_CLEAR_NEF |
                                               UART_CLEAR_OREF |
                                               UART_CLEAR_IDLEF |
                                               UART_CLEAR_TXFECF |
                                               UART_CLEAR_TCF |
                                               UART_CLEAR_LBDF |
                                               UART_CLEAR_CTSF |
                                               UART_CLEAR_CMF |
                                               UART_CLEAR_WUF |
                                               UART_CLEAR_RTOF);
                                               
    HAL_UART_RegisterCallback(uart_drv->huart, HAL_UART_ERROR_CB_ID, ErrorCallback);
    HAL_UART_RegisterCallback(uart_drv->huart, HAL_UART_TX_COMPLETE_CB_ID, TxCpltCallback);
    HAL_UART_RegisterRxEventCallback(uart_drv->huart, RxEventCallback);

    __enable_irq();

    return 0;
}
static uart_drv_t usart1;
/*static uart_drv_t usartx;*/ /**<------ add other uart here*/

static uart_drv_t *uart_drv_get(UART_HandleTypeDef *huart)
{
    if (huart == NULL)
    {
        return NULL;
    }
    if (huart == &huart1)
    {
        return &usart1;
    }
    else
    {
        /* add other uart here */ /**<------ add other uart here*/
    }

    return NULL;
}

#ifdef USING_UART_OPTION_FUNCTION
static int8_t drv_uart_write_before(uart_drv_t *uart)
{
    if (uart == NULL)
    {
        return -1;
    }

    if (uart->huart == &huart1)
    {
        /* do nothing */
    }
    else
    {
        /* add other uart here */ /**<------ add other uart here*/
    }

    return 0;
}
static int8_t drv_uart_write_after(uart_drv_t *uart)
{
    if (uart == NULL)
    {
        return -1;
    }

    if (uart->huart == &huart1)
    {
        /* do nothing */
    }
    else
    {
        /* add other uart here */ /**<------ add other uart here*/
    }

    return 0;
}
static int8_t drv_uart_write_complete(uart_drv_t *uart)
{
    if (uart == NULL)
    {
        return -1;
    }

    if (uart->huart == &huart1)
    {
        /* do nothing */
    }
    else
    {
        /* add other uart here */ /**<------ add other uart here*/
    }

    return 0;
}
static int8_t drv_uart_read_before(uart_drv_t *uart)
{
    if (uart == NULL)
    {
        return -1;
    }

    if (uart->huart == &huart1)
    {
        /* do nothing */
    }
    else
    {
        /* add other uart here */ /**<------ add other uart here*/
    }

    return 0;
}
static int8_t drv_uart_read_after(uart_drv_t *uart)
{
    if (uart == NULL)
    {
        return -1;
    }

    if (uart->huart == &huart1)
    {
        /* do nothing */
    }
    else
    {
        /* add other uart here */ /**<------ add other uart here*/
    }

    return 0;
}
static int8_t drv_uart_read_complete(uart_drv_t *uart)
{
    if (uart == NULL)
    {
        return -1;
    }

    if (uart->huart == &huart1)
    {
        /* do nothing */
    }
    else
    {
        /* add other uart here */ /**<------ add other uart here*/
    }

    return 0;
}
#endif

int32_t drv_console_init(void)
{
    int32_t ret = drv_uart_register(uart_drv_get(&huart1),
                                    &huart1,
                                    UART_DEV_NAME_CONSOLE,
                                    UART_TYPE_FULL_DUPLEX);
    if (ret != 0)
    {
        return -1;
    }
    return 0;
}
static int32_t drv_uartx_init(void)
{

    /* add other uart here */ /**<------ add other uart here*/
    return 0;
}
INIT_DEVICE_EXPORT(drv_uartx_init);
