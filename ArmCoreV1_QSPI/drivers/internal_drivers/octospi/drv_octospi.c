#include "drv_octospi.h"
#include "octospi.h"
#include "utilities.h"

#define OSPI_SEND_SUCCEED_EVENT      (1<<0)
#define OSPI_RECV_SUCCEED_EVENT      (1<<1)

static void ErrorCallback(OSPI_HandleTypeDef *hospi)
{
    DEVICE_OSPI *ospi = (DEVICE_OSPI *)hospi;
    
    printf("%s err\r\n", ospi->name);
}
static void CmdCpltCallback(OSPI_HandleTypeDef *hospi)
{
    DEVICE_OSPI *ospi = (DEVICE_OSPI *)hospi;
    osEventFlagsSet(ospi->tx_event, OSPI_SEND_SUCCEED_EVENT);
}
static void TxCpltCallback(OSPI_HandleTypeDef *hospi)
{
    DEVICE_OSPI *ospi = (DEVICE_OSPI *)hospi;
    osEventFlagsSet(ospi->tx_event, OSPI_SEND_SUCCEED_EVENT);
}
static void RxCpltCallback(OSPI_HandleTypeDef *hospi)
{
    DEVICE_OSPI *ospi = (DEVICE_OSPI *)hospi;
    osEventFlagsSet(ospi->rx_event, OSPI_RECV_SUCCEED_EVENT);
}

static int8_t spi_open(DEVICE_OSPI *ospi)
{
    if (ospi->open_state)
    {
        printf("device %s already opened\r\n", ospi->name);
        return -1;
    }
    else
    {
        ospi->open_state = 1;
    }

    return 0;
}

static int8_t spi_close(DEVICE_OSPI *ospi)
{
    HAL_StatusTypeDef ret = HAL_OK;
    osStatus_t stat;

    if (ospi->open_state)
    {
        ret = HAL_OSPI_DeInit((OSPI_HandleTypeDef *)ospi);
        if (ret != HAL_OK)
        {
            printf("device %s deinit err:%d\r\n", ospi->name, ret);
            return -1;
        }

        stat = osEventFlagsDelete(ospi->tx_event);
        if (stat != osOK)
        {
            printf("device %s delete event err:%d\r\n", ospi->name, stat);
            return -2;
        }

        stat = osMutexDelete(ospi->tx_mutex);
        if (stat != osOK)
        {
            printf("device %s delete mutex err:%d\r\n", ospi->name, stat);
            return -3;
        }

        stat = osEventFlagsDelete(ospi->rx_event);
        if (stat != osOK)
        {
            printf("device %s delete event err:%d\r\n", ospi->name, stat);
            return -4;
        }

        ospi->open_state = 0;
    }
    else
    {
        /* device already closed */
    }

    return 0;
}

static int8_t spi_write(DEVICE_OSPI *ospi, OSPI_RegularCmdTypeDef *cmd_buf, uint8_t *data_buf, uint32_t timeout)
{
    osStatus_t ret = osOK;

    if (!ospi->open_state)
    {
        printf("device %s is closed\r\n", ospi->name);
        return -1;
    }

    if (cmd_buf == NULL)
    {
        printf("cmd or data ptr is null\r\n");
        return -2;
    }
    
    ret = osMutexAcquire(ospi->tx_mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", ospi->name, ret);
        return -3;
    }

#ifdef USING_OSPI_OPTION_FUNCTION
    if (ospi->opt.before_write != NULL)
    {
        ospi->opt.before_write(ospi);
    }
#endif

    if (data_buf != NULL)
    {
        ret = HAL_OSPI_Command((OSPI_HandleTypeDef *)ospi, cmd_buf, timeout);
        if (ret != HAL_OK)
        {
            printf("device %s write cmd err:%d\r\n", ospi->name, ret);
            return -4;
        }

#ifdef USING_OSPI_DMA_MODE
        if (cmd_buf->NbData != sizeof(uint32_t))
        {
            ret = HAL_OSPI_Transmit_DMA((OSPI_HandleTypeDef *)ospi, data_buf);
        }
        else
        {
            ret = HAL_OSPI_Transmit((OSPI_HandleTypeDef *)ospi, data_buf, timeout);
        }
#else
        ret = HAL_OSPI_Transmit((OSPI_HandleTypeDef *)ospi, data_buf, timeout);
#endif

        if (ret != HAL_OK)
        {
            printf("device %s write data err:%d\r\n", ospi->name, ret);
            return -5;
        }

    }
    else
    {
        ret = HAL_OSPI_Command_IT((OSPI_HandleTypeDef *)ospi, cmd_buf);
        if (ret != HAL_OK)
        {
            printf("device %s write cmd err:%d\r\n", ospi->name, ret);
            return -4;
        }

#ifndef USING_OSPI_DMA_MODE
        ret = osEventFlagsWait(ospi->tx_event, OSPI_SEND_SUCCEED_EVENT, osFlagsWaitAny, timeout);
        if (ret != OSPI_SEND_SUCCEED_EVENT)
        {
            printf("device %s wait event flag err:%d\r\n", ospi->name, ret);
            return -5;
        }
#endif

    }

#ifdef USING_OSPI_OPTION_FUNCTION
    if (ospi->opt.after_write != NULL)
    {
        ospi->opt.after_write(ospi);
    }
#endif

#ifdef USING_OSPI_DMA_MODE
    if (cmd_buf->NbData != sizeof(uint32_t))
    {
        ret = osEventFlagsWait(ospi->tx_event, OSPI_SEND_SUCCEED_EVENT, osFlagsWaitAny, timeout);
        if (ret != OSPI_SEND_SUCCEED_EVENT)
        {
            printf("device %s wait event flag err:%d\r\n", ospi->name, ret);
            return -6;
        }
    }
#endif

#ifdef USING_OSPI_OPTION_FUNCTION
    if (ospi->opt.complete_write != NULL)
    {
        ospi->opt.complete_write(ospi);
    }
#endif

    osMutexRelease(ospi->tx_mutex);

    return 0;
}

static int8_t spi_read(DEVICE_OSPI *ospi, OSPI_RegularCmdTypeDef *cmd_buf, uint8_t *data_buf, uint32_t timeout)
{
    osStatus_t ret = osOK;

    if (!ospi->open_state)
    {
        printf("device %s is closed\r\n", ospi->name);
        return -1;
    }

    if (cmd_buf == NULL || data_buf == NULL)
    {
        printf("cmd or data buf is null\r\n");
        return -2;
    }

    ret = osMutexAcquire(ospi->tx_mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", ospi->name, ret);
        return -3;
    }

#ifdef USING_OSPI_OPTION_FUNCTION
    if (ospi->opt.before_read != NULL)
    {
        ospi->opt.before_read(ospi);
    }
#endif

    ret = HAL_OSPI_Command((OSPI_HandleTypeDef *)ospi, cmd_buf, timeout);
    if (ret != HAL_OK)
    {
        printf("device %s write cmd err:%d\r\n", ospi->name, ret);
        return -4;
    }

#ifdef USING_OSPI_DMA_MODE
    if (cmd_buf->NbData != sizeof(uint32_t))
    {
        ret = HAL_OSPI_Receive_DMA(&ospi->hospi, data_buf);
    }
    else
    {
        ret = HAL_OSPI_Receive((OSPI_HandleTypeDef *)ospi, data_buf, timeout);
    }
#else
    ret = HAL_OSPI_Receive((OSPI_HandleTypeDef *)ospi, data_buf, timeout);
#endif

    if (ret != HAL_OK)
    {
        printf("device %s receive dma err:%d\r\n", ospi->name, ret);
        return -5;
    }

#ifdef USING_OSPI_OPTION_FUNCTION
    if (ospi->opt.after_read != NULL)
    {
        ospi->opt.after_read(ospi);
    }
#endif

#ifdef USING_OSPI_DMA_MODE
    if (cmd_buf->NbData != sizeof(uint32_t))
    {
        ret = osEventFlagsWait(ospi->rx_event, OSPI_RECV_SUCCEED_EVENT, osFlagsWaitAny, timeout);
        if (ret != OSPI_RECV_SUCCEED_EVENT)
        {
            printf("device %s wait event flag err:%d\r\n", ospi->name, ret);
            return -6;
        }
    }
#endif

#ifdef USING_OSPI_OPTION_FUNCTION
    if (ospi->opt.complete_read != NULL)
    {
        ospi->opt.complete_read(ospi);
    }
#endif

    osMutexRelease(ospi->tx_mutex);

    return 0;
}

/* default configure for spi is dma mode
* 1) queue 、mutex and event init, queue and/or event for rx, mutex for tx
* 2) add send function, and release mutex in complete callback function
* 3) add receive function, and put data to queue or send event
* 4) add port for app, to get data from queue with timeout
*/
int8_t ospi_init(DEVICE_OSPI *ospi, uint8_t *device_name)
{
    if (ospi == NULL || device_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    if (ospi->open_state)
    {
        printf("device %s is opened\r\n", ospi->name);
        return -2;
    }

    /* 1. init hardware */
    if (!memcmp(device_name, DEVICE_NAME_OSPI1, sizeof(DEVICE_NAME_OSPI1)))
    {
        MX_OCTOSPI1_Init();
        memcpy(ospi, &hospi1, sizeof(OSPI_HandleTypeDef));
    }
    else
    {
        /* add other spi here */
    }

    /* 2. create queue 、event and mutex for device */
    // osMessageQueueAttr_t CmdQueue_attributes = {
    // .name = "spi_rx_queue"
    // };
    // ospi->rx_queue = osMessageQueueNew (16, sizeof(struct CmdMessage), &CmdQueue_attributes);  /* modify queue size and count for different aim */

    osMutexAttr_t ospi_tx_mutex_attributes = {
    .name = "ospi_tx_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    ospi->tx_mutex = osMutexNew(&ospi_tx_mutex_attributes);

    const osEventFlagsAttr_t ospi_tx_event_attributes = {
    .name = "ospi_tx_event"
    };
    ospi->tx_event = osEventFlagsNew(&ospi_tx_event_attributes);
    
    const osEventFlagsAttr_t ospi_rx_event_attributes = {
    .name = "ospi_rx_event"
    };
    ospi->rx_event = osEventFlagsNew(&ospi_rx_event_attributes);

    /* 3. rx buf malloc for dma mode */
    // ospi->rx_buf = (uint8_t *)pvPortMalloc(sizeof(struct CmdMessage));  /* here should check when use dma mode */
    // if (ospi->rx_buf == NULL)
    // {
    //     return -3;
    // }

    // ospi->rx_buf_len = sizeof(struct CmdMessage) - sizeof(uint16_t); /* indicate rx buf max len */
    
    /* 4. register callback function */
    HAL_OSPI_RegisterCallback((OSPI_HandleTypeDef *)ospi, HAL_OSPI_ERROR_CB_ID, ErrorCallback);
    HAL_OSPI_RegisterCallback((OSPI_HandleTypeDef *)ospi, HAL_OSPI_CMD_CPLT_CB_ID, CmdCpltCallback);
    HAL_OSPI_RegisterCallback((OSPI_HandleTypeDef *)ospi, HAL_OSPI_TX_CPLT_CB_ID, TxCpltCallback);
    HAL_OSPI_RegisterCallback((OSPI_HandleTypeDef *)ospi, HAL_OSPI_RX_CPLT_CB_ID, RxCpltCallback);
    // HAL_OSPI_RegisterRxEventCallback((OSPI_HandleTypeDef *)ospi, RxEventCallback);

    /* 5. device rename */
    memcpy(ospi->name, device_name, DEVICE_NAME_LENGTH);

    /* 6. device mode */
    // ospi->master_or_slave = mode;

    /* 7. register operation function */
    ospi->open = spi_open;
    ospi->close = spi_close;
    ospi->write = spi_write;
    ospi->read = spi_read;
    ospi->ioctl = NULL;
    ospi->rx_cb = NULL;

    /* 8. open device */
    return 0;//ospi->open(spi);
}

int8_t ospi_rx_queue_init(DEVICE_OSPI *ospi, osMessageQueueId_t queue)
{
    if (ospi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    ospi->rx_queue = queue;

    return 0;
}

int8_t ospi_rx_callback_register(DEVICE_OSPI *ospi, int8_t (*cb)(void *arg))
{
    if (ospi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    ospi->rx_cb = cb;

    return 0;
}

int8_t ospi_dma_rx_buf_init(DEVICE_OSPI *ospi, uint8_t *buf, uint16_t len)
{
    if (ospi == NULL || buf == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }
    
    if (len == 0)
    {
        printf("len is zero\r\n");
        return -2;
    }
    
    ospi->rx_buf = buf;
    ospi->rx_buf_len = len;

    return 0;
}

#ifdef USING_OSPI_OPTION_FUNCTION
int8_t ospi_opt_init(DEVICE_OSPI *ospi, DEVICE_OSPI_OPT *opt_func)
{
    if (ospi == NULL || opt_func == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    memcpy(&ospi->opt, opt_func, sizeof(DEVICE_OSPI_OPT));

    return 0;
}
#endif

#ifdef USING_OSPI_SLAVE_TO_MASTER_INTERRUPT
int8_t device_ospi_irq_node_add(DEVICE_OSPI *ospi, IRQ_INFO_NODE *node)
{
    if (ospi == NULL || node == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    if (ospi->irq_list == NULL)
    {
        ospi->irq_list = (DEVICE_IRQ_LIST *)pvPortMalloc(sizeof(DEVICE_IRQ_LIST));
        if (ospi->irq_list == NULL)
        {
            printf("device irq %s malloc err\r\n", node->node_name);
            return -2;
        }
        
        ospi->irq_list->next = NULL;
        ospi->irq_list->node_data = node;

        return 0;
    }

    DEVICE_IRQ_LIST *ptr_pre = ospi->irq_list;
    DEVICE_IRQ_LIST *ptr = ptr_pre;

    while (ptr != NULL)
    {
        ptr_pre = ptr;
        ptr = ptr->next;
    }

    if (ptr == NULL)
    {
        ptr = (DEVICE_IRQ_LIST *)pvPortMalloc(sizeof(DEVICE_IRQ_LIST));
        if (ptr == NULL)
        {
            printf("device irq %s malloc err\r\n", node->node_name);
            return -3;
        }
    }

    ptr->node_data = node;
    ptr->next = NULL;

    ptr_pre->next = ptr;

    return 0;
}

DEVICE_IRQ_LIST *device_ospi_irq_node_find(DEVICE_OSPI *ospi, uint8_t *node_name)
{
    if (ospi == NULL || node_name == NULL)
    {
        printf("ptr is null\r\n");
        return NULL;
    }

    DEVICE_IRQ_LIST *header = ospi->irq_list;
    DEVICE_IRQ_LIST *ptr = header;

    while (ptr != NULL && strcmp(ptr->node_data->node_name, node_name) != 0)
    {
        ptr = ptr->next;
    }

    if (ptr == NULL)
    {
        printf("can't find target node\r\n");
    }

    return ptr;
}

int8_t device_ospi_irq_node_delete(DEVICE_OSPI *ospi, uint8_t *node_name)
{
    if (ospi == NULL || node_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    DEVICE_IRQ_LIST *ptr = ospi->irq_list;
    DEVICE_IRQ_LIST *ptr_pre = NULL;
    
    while (ptr != NULL && strcmp(ptr->node_data->node_name, node_name) != 0)
    {
        ptr_pre = ptr;
        ptr = ptr->next;
    }

    if (ptr == NULL)
    {
        printf("can't find target node\r\n");
        return -2;
    }

    if (ptr_pre == NULL)    /* first node */
    {
        ospi->irq_list = ptr->next;
    }
    else
    {
        ptr_pre->next = ptr->next;
    }

    vPortFree(ptr);

    return 0;
}

int8_t device_ospi_irq_list_clear(DEVICE_OSPI *ospi)
{
    if (ospi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    DEVICE_IRQ_LIST *header = ospi->irq_list;
    DEVICE_IRQ_LIST *ptr = header;

    while (ptr != NULL && ptr->node_data == NULL)
    {
        ptr = ptr->next;
    }

    if (ptr != NULL)
    {
        printf("irq node is valid, so must free it first\r\n");
        return -2;
    }

    ptr = header;
    while (ptr != NULL)
    {
        header = header->next;

        ptr->node_data = NULL;
        ptr->next = NULL;
        vPortFree(ptr);
        ptr = header;
    }

    ospi->irq_list = NULL;

    return 0;
}

int8_t device_ospi_irq_list_list(DEVICE_OSPI *ospi)
{
    if (ospi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    DEVICE_IRQ_LIST *header = ospi->irq_list;
    DEVICE_IRQ_LIST *ptr = header;

    while (ptr != NULL)
    {
        printf("ptr addr:%p  node_name:%s node_data addr %p\r\n", ptr, ptr->node_data->node_name, ptr->node_data);
        ptr = ptr->next;
    }

    return 0;
}

int8_t device_ospi_irq_wait_with_block(DEVICE_OSPI *ospi, uint8_t *node_name, char splitter, uint32_t timeout)
{
    if (ospi == NULL || node_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    uint32_t ret = 0;   

    uint8_t name_buf[50] = {0};
    uint8_t *argv[5] = {NULL};
    memcpy(name_buf, node_name, strlen(node_name));

    uint8_t name_num = split_string(name_buf, splitter, argv);

    DEVICE_IRQ_LIST *node_res = NULL;
    osEventFlagsId_t event = NULL;
    uint32_t flag = 0;

    for (uint8_t i = 0; i < name_num; i++)
    {
        node_res = device_ospi_irq_node_find(ospi, argv[i]);
        if (node_res == NULL)
        {
            printf("%s node not find\r\n", argv[i]);
        }
        else
        {
            flag |= node_res->node_data->irq_event_flag;
            event = node_res->node_data->irq_event;
        }
        // printf("[%d]: %s\r\n", i, argv[i]);
    }
 
    ret = osEventFlagsWait(event, flag, osFlagsWaitAny, timeout);
    if ((ret & flag) != ret)
    {
        printf("device %s wait irq err:%u\r\n", ospi->name, ret);
    }

    return ret;
}

#endif