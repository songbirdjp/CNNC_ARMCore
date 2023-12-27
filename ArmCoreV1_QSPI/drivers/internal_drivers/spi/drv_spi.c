#include "drv_spi.h"

#define SPI_SEND_SUCCEED_EVENT      (1<<0)
#define SPI_RECV_SUCCEED_EVENT      (1<<1)

static void ErrorCallback(SPI_HandleTypeDef *hspi)
{
    DEVICE_SPI *spi = (DEVICE_SPI *)hspi;
    
    printf("%s err\r\n", spi->name);
}
static void TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    DEVICE_SPI *spi = (DEVICE_SPI *)hspi;
    osEventFlagsSet(spi->tx_event, SPI_SEND_SUCCEED_EVENT);
}
static void RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    osStatus_t ret = osOK;
    DEVICE_SPI *spi = (DEVICE_SPI *)hspi;

    if (spi->master_or_slave == SPI_MASTER)
    {
        osEventFlagsSet(spi->rx_event, SPI_RECV_SUCCEED_EVENT);
    }
    else
    {
        ret = osMessageQueuePut(spi->rx_queue, spi->rx_buf, 0, 0);
        if (ret != osOK)
        {
            printf("%s queue put err:%d\r\n", spi->name, ret);
        }
    }
}

static int8_t spi_open(DEVICE_SPI *spi)
{
    if (spi->open_state)
    {
        printf("device %s already opened\r\n", spi->name);
        return -1;
    }
    else
    {
        spi->open_state = 1;
    }

    if (spi->master_or_slave == SPI_MASTER)
    {
        /* do nothing here */
    }
    else
    {
        HAL_SPI_Receive_DMA((SPI_HandleTypeDef *)spi, (uint8_t *)spi->rx_buf, spi->rx_buf_len);
    }

    return 0;
}

static int8_t spi_close(DEVICE_SPI *spi)
{
    HAL_StatusTypeDef ret = HAL_OK;
    osStatus_t stat;

    if (spi->open_state)
    {
        ret = HAL_SPI_DeInit((SPI_HandleTypeDef *)spi);
        if (ret != HAL_OK)
        {
            printf("device %s deinit err:%d\r\n", spi->name, ret);
            return -1;
        }

        stat = osEventFlagsDelete(spi->tx_event);
        if (stat != osOK)
        {
            printf("device %s delete event err:%d\r\n", spi->name, stat);
            return -2;
        }

        stat = osMutexDelete(spi->tx_mutex);
        if (stat != osOK)
        {
            printf("device %s delete mutex err:%d\r\n", spi->name, stat);
            return -3;
        }

        stat = osEventFlagsDelete(spi->rx_event);
        if (stat != osOK)
        {
            printf("device %s delete event err:%d\r\n", spi->name, stat);
            return -4;
        }

        spi->open_state = 0;
    }
    else
    {
        /* device already closed */
    }

    return 0;
}

static int8_t spi_write(DEVICE_SPI *spi, uint8_t *buf, uint16_t size, uint32_t timeout)
{
    osStatus_t ret = osOK;

    if (!spi->open_state)
    {
        printf("device %s is closed\r\n", spi->name);
        return -1;
    }

#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.before_write != NULL)
    {
        spi->opt.before_write(spi);
    }
#endif
    
    ret = osMutexAcquire(spi->tx_mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", spi->name, ret);
        return -2;
    }

    ret = HAL_SPI_Transmit_DMA((SPI_HandleTypeDef *)spi, buf, size);
    if (ret != HAL_OK)
    {
        printf("device %s write data err:%d\r\n", spi->name, ret);
        return -3;
    }

#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.after_write != NULL)
    {
        spi->opt.after_write(spi);
    }
#endif

    ret = osEventFlagsWait(spi->tx_event, SPI_SEND_SUCCEED_EVENT, osFlagsWaitAny, timeout);
    if (ret != SPI_SEND_SUCCEED_EVENT)
    {
        printf("device %s wait event flag err:%d\r\n", spi->name, ret);
        return -4;
    }
    osMutexRelease(spi->tx_mutex);

#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.complete_write != NULL)
    {
        spi->opt.complete_write(spi);
    }
#endif

    return 0;
}

static int8_t spi_read(DEVICE_SPI *spi, uint8_t *buf, uint16_t size, uint32_t timeout)
{
    osStatus_t ret = osOK;

    if (!spi->open_state)
    {
        printf("device %s is closed\r\n", spi->name);
        return -1;
    }

#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.before_read != NULL)
    {
        spi->opt.before_read(spi);
    }
#endif


    if (spi->master_or_slave == SPI_MASTER)
    {
        ret = HAL_SPI_Receive_DMA((SPI_HandleTypeDef *)spi, buf, size);
        if (ret != HAL_OK)
        {
            printf("device %s receive dma err:%d\r\n", spi->name, ret);
            return -2;
        }

        ret = osEventFlagsWait(spi->rx_event, SPI_RECV_SUCCEED_EVENT, osFlagsWaitAny, timeout);
        if (ret != SPI_RECV_SUCCEED_EVENT)
        {
            printf("device %s wait event flag err:%d\r\n", spi->name, ret);
            return -4;
        }
    }
    else
    {
        ret = osMessageQueueGet(spi->rx_queue, buf, 0, timeout);
        if (ret != osOK)
        {
            printf("device %s read data err:%d\r\n", spi->name, ret);
            return -2;
        }
    }


#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.complete_read != NULL)
    {
        spi->opt.complete_read(spi);
    }
#endif

    return 0;
}

/* default configure for spi is dma mode
* 1) queue 、mutex and event init, queue and/or event for rx, mutex for tx
* 2) add send function, and release mutex in complete callback function
* 3) add receive function, and put data to queue or send event
* 4) add port for app, to get data from queue with timeout
*/
int8_t spi_init(DEVICE_SPI *spi, uint8_t *device_name, SPI_MODE mode)
{
    if (spi == NULL || device_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    if (spi->open_state)
    {
        printf("device %s is opened\r\n", spi->name);
        return -2;
    }

    /* 1. init hardware */
    if (!memcmp(device_name, DEVICE_NAME_SPI1, sizeof(DEVICE_NAME_SPI1)))
    {
        MX_SPI1_Init((SPI_HandleTypeDef *)spi);
    }
    else if (!memcmp(device_name, DEVICE_NAME_SPI2, sizeof(DEVICE_NAME_SPI2)))
    {
        MX_SPI2_Init((SPI_HandleTypeDef *)spi);
    }
    else if (!memcmp(device_name, DEVICE_NAME_SPI3, sizeof(DEVICE_NAME_SPI3)))
    {
        MX_SPI3_Init((SPI_HandleTypeDef *)spi);
    }
        else if (!memcmp(device_name, DEVICE_NAME_SPI6, sizeof(DEVICE_NAME_SPI6)))
    {
        MX_SPI6_Init((SPI_HandleTypeDef *)spi);
    }
    else
    {
        /* add other spi here */
    }

    /* 2. create queue 、event and mutex for device */
    // osMessageQueueAttr_t CmdQueue_attributes = {
    // .name = "spi_rx_queue"
    // };
    // spi->rx_queue = osMessageQueueNew (16, sizeof(struct CmdMessage), &CmdQueue_attributes);  /* modify queue size and count for different aim */

    osMutexAttr_t spi_tx_mutex_attributes = {
    .name = "spi_tx_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    spi->tx_mutex = osMutexNew(&spi_tx_mutex_attributes);

    const osEventFlagsAttr_t spi_tx_event_attributes = {
    .name = "spi_tx_event"
    };
    spi->tx_event = osEventFlagsNew(&spi_tx_event_attributes);
    
    const osEventFlagsAttr_t spi_rx_event_attributes = {
    .name = "spi_rx_event"
    };
    spi->rx_event = osEventFlagsNew(&spi_rx_event_attributes);

    /* 3. rx buf malloc for dma mode */
    // spi->rx_buf = (uint8_t *)pvPortMalloc(sizeof(struct CmdMessage));  /* here should check when use dma mode */
    // if (spi->rx_buf == NULL)
    // {
    //     return -3;
    // }

    // spi->rx_buf_len = sizeof(struct CmdMessage) - sizeof(uint16_t); /* indicate rx buf max len */
    
    /* 4. register callback function */
    HAL_SPI_RegisterCallback((SPI_HandleTypeDef *)spi, HAL_SPI_ERROR_CB_ID, ErrorCallback);
    HAL_SPI_RegisterCallback((SPI_HandleTypeDef *)spi, HAL_SPI_TX_COMPLETE_CB_ID, TxCpltCallback);
    HAL_SPI_RegisterCallback((SPI_HandleTypeDef *)spi, HAL_SPI_RX_COMPLETE_CB_ID, RxCpltCallback);
    // HAL_spi_RegisterRxEventCallback((SPI_HandleTypeDef *)spi, RxEventCallback);

    /* 5. device rename */
    memcpy(spi->name, device_name, DEVICE_NAME_LENGTH);

    /* 6. device mode */
    spi->master_or_slave = mode;

    /* 7. register operation function */
    spi->open = spi_open;
    spi->close = spi_close;
    spi->write = spi_write;
    spi->read = spi_read;
    spi->ioctl = NULL;

    /* 8. open device */
    return spi->open(spi);
}


int8_t spi_rx_queue_init(DEVICE_SPI *spi, osMessageQueueId_t queue)
{
    if (spi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    spi->rx_queue = queue;

    return 0;
}

int8_t spi_dma_rx_buf_init(DEVICE_SPI *spi, uint8_t *buf, uint16_t len)
{
    if (spi == NULL || buf == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }
    
    if (len == 0)
    {
        printf("len is zero\r\n");
        return -2;
    }
    
    spi->rx_buf = buf;
    spi->rx_buf_len = len;

    return 0;
}

#ifdef USING_SPI_OPTION_FUNCTION
int8_t spi_opt_init(DEVICE_SPI *spi, DEVICE_SPI_OPT *opt_func)
{
    if (spi == NULL || opt_func == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    memcpy(&spi->opt, opt_func, sizeof(DEVICE_SPI_OPT));

    return 0;
}
#endif

#ifdef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
int8_t device_irq_node_add(DEVICE_SPI *spi, IRQ_INFO_NODE *node)
{
    if (spi == NULL || node == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    if (spi->irq_list == NULL)
    {
        spi->irq_list = (DEVICE_IRQ_LIST *)pvPortMalloc(sizeof(DEVICE_IRQ_LIST));
        if (spi->irq_list == NULL)
        {
            printf("device irq %s malloc err\r\n", node->node_name);
            return -2;
        }
        
        spi->irq_list->next = NULL;
        spi->irq_list->node_data = node;

        return 0;
    }

    DEVICE_IRQ_LIST *ptr_pre = spi->irq_list;
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

DEVICE_IRQ_LIST *device_irq_node_find(DEVICE_SPI *spi, uint8_t *node_name)
{
    if (spi == NULL || node_name == NULL)
    {
        printf("ptr is null\r\n");
        return NULL;
    }

    DEVICE_IRQ_LIST *header = spi->irq_list;
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

int8_t device_irq_node_delete(DEVICE_SPI *spi, uint8_t *node_name)
{
    if (spi == NULL || node_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    DEVICE_IRQ_LIST *ptr = spi->irq_list;
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
        spi->irq_list = ptr->next;
    }
    else
    {
        ptr_pre->next = ptr->next;
    }

    vPortFree(ptr);

    return 0;
}

int8_t device_irq_list_clear(DEVICE_SPI *spi)
{
    if (spi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    DEVICE_IRQ_LIST *header = spi->irq_list;
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

    spi->irq_list = NULL;

    return 0;
}

int8_t device_irq_list_list(DEVICE_SPI *spi)
{
    if (spi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    DEVICE_IRQ_LIST *header = spi->irq_list;
    DEVICE_IRQ_LIST *ptr = header;

    while (ptr != NULL)
    {
        printf("ptr addr:%p  node_name:%s node_data addr %p\r\n", ptr, ptr->node_data->node_name, ptr->node_data);
        ptr = ptr->next;
    }

    return 0;
}

int8_t device_irq_wait_with_block(DEVICE_SPI *spi, uint32_t timeout)
{
    if (spi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    uint32_t ret;
    IRQ_INFO_NODE *node = spi->irq_list->node_data;

    ret = osEventFlagsWait(node->irq_event, node->irq_event_flag, osFlagsWaitAny, timeout);
    if (ret != node->irq_event_flag)
    {
        printf("device %s wait irq err:%u\r\n", spi->name, ret);
        return ret;
    }

    return 0;
}

#endif