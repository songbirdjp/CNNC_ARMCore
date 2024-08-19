#include "drv_spi.h"
#include "utilities.h"
#include "spi.h"

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
        if (spi->rx_queue != NULL)
        {
            ret = osMessageQueuePut(spi->rx_queue, spi->rx_buf, 0, 0);
            if (ret != osOK)
            {
                printf("%s queue put err:%d\r\n", spi->name, ret);
            }
        }

        if (spi->rx_cb != NULL)
        {
            spi->rx_cb((void *)spi);
        }
    }
}
static void TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    osStatus_t ret = osOK;
    DEVICE_SPI *spi = (DEVICE_SPI *)hspi;

    if (spi->master_or_slave == SPI_MASTER)
    {
        osEventFlagsSet(spi->rx_event, SPI_RECV_SUCCEED_EVENT);

        if (spi->rx_queue != NULL)
        {
            ret = osMessageQueuePut(spi->rx_queue, spi->rx_buf, 0, 0);
            if (ret != osOK)
            {
                printf("%s queue put err:%d\r\n", spi->name, ret);
            }
        }

        if (spi->rx_cb != NULL)
        {
            spi->rx_cb((void *)spi);
        }
    }
    else
    {
        /* do nothing */
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
        HAL_StatusTypeDef status = HAL_SPI_Receive_DMA((SPI_HandleTypeDef *)spi, (uint8_t *)spi->rx_buf, spi->rx_buf_len);
        if (status != HAL_OK)
        {
            printf("device %s receive dma err:%d\r\n", spi->name, status);
            return -2;
        }

        __HAL_SPI_ENABLE(&spi->hspi);
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
    HAL_StatusTypeDef status = HAL_OK;

    if (!spi->open_state)
    {
        printf("device %s is closed\r\n", spi->name);
        return -1;
    }

    ret = osMutexAcquire(spi->tx_mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", spi->name, ret);
        return -2;
    }

#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.before_write != NULL)
    {
        spi->opt.before_write(spi);
    }
#endif

    status = HAL_SPI_Transmit_DMA((SPI_HandleTypeDef *)spi, buf, size);
    if (status != HAL_OK)
    {
        printf("device %s write data err:%d\r\n", spi->name, status);
        ret = -3;
        goto err;
    }

#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.after_write != NULL)
    {
        spi->opt.after_write(spi);
    }
#endif

    uint32_t ret_val = osEventFlagsWait(spi->tx_event, SPI_SEND_SUCCEED_EVENT, osFlagsWaitAny, timeout);
    if (ret_val != SPI_SEND_SUCCEED_EVENT)
    {
        printf("device %s wait event flag err: %#.8x\r\n", spi->name, ret_val);
        ret = -4;
        goto err;
    }

err:
#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.complete_write != NULL)
    {
        spi->opt.complete_write(spi);
    }
#endif

    osMutexRelease(spi->tx_mutex);

    return ret;
}

static int8_t spi_read(DEVICE_SPI *spi, uint8_t *buf, uint16_t size, uint32_t timeout)
{
    osStatus_t ret = osOK;

    if (!spi->open_state)
    {
        printf("device %s is closed\r\n", spi->name);
        return -1;
    }

    if (spi->master_or_slave == SPI_MASTER)
    {
        ret = osMutexAcquire(spi->tx_mutex, timeout);
        if (ret != osOK)
        {
            printf("device %s acquire mutex err:%d\r\n", spi->name, ret);
            return ret;
        }
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
            ret = -2;
            goto err;
        }

#ifdef USING_SPI_OPTION_FUNCTION
        if (spi->opt.after_read != NULL)
        {
            spi->opt.after_read(spi);
        }
#endif

        uint32_t ret_val = osEventFlagsWait(spi->rx_event, SPI_RECV_SUCCEED_EVENT, osFlagsWaitAny, timeout);
        if (ret_val != SPI_RECV_SUCCEED_EVENT)
        {
            printf("device %s wait event flag err: %#.8x\r\n", spi->name, ret_val);
            ret = -3;
            goto err;
        }
    }
    else
    {
#ifdef USING_SPI_OPTION_FUNCTION
        if (spi->opt.after_read != NULL)
        {
            spi->opt.after_read(spi);
        }
#endif

        ret = osMessageQueueGet(spi->rx_queue, buf, 0, timeout);
        if (ret != osOK)
        {
            printf("device %s read data err:%d\r\n", spi->name, ret);
            ret = -2;
            goto err;
        }
    }

err:
#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.complete_read != NULL)
    {
        spi->opt.complete_read(spi);
    }
#endif

    if (spi->master_or_slave == SPI_MASTER)
    {
        osMutexRelease(spi->tx_mutex);
    }

    return ret;
}

static int8_t spi_write_and_read(DEVICE_SPI *spi, uint8_t *send_buf, uint8_t *recv_buf, uint16_t size, uint32_t timeout)
{
    osStatus_t ret = osOK;

    if (!spi->open_state)
    {
        printf("device %s is closed\r\n", spi->name);
        return -1;
    }

    if (spi->master_or_slave == SPI_MASTER)
    {
        ret = osMutexAcquire(spi->tx_mutex, timeout);
        if (ret != osOK)
        {
            printf("device %s acquire mutex err:%d\r\n", spi->name, ret);
            return ret;
        }
    }

#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.before_read != NULL)
    {
        spi->opt.before_read(spi);
    }
#endif

    if (spi->master_or_slave == SPI_MASTER)
    {
        ret = HAL_SPI_TransmitReceive_DMA((SPI_HandleTypeDef *)spi, send_buf, recv_buf, size);
        if (ret != HAL_OK)
        {
            printf("device %s receive dma err:%d\r\n", spi->name, ret);
            ret = -2;
            goto err;
        }

#ifdef USING_SPI_OPTION_FUNCTION
        if (spi->opt.after_read != NULL)
        {
            spi->opt.after_read(spi);
        }
#endif

        uint32_t ret_val = osEventFlagsWait(spi->rx_event, SPI_RECV_SUCCEED_EVENT, osFlagsWaitAny, timeout);
        if (ret_val != SPI_RECV_SUCCEED_EVENT)
        {
            printf("device %s wait event flag err: %#.8x\r\n", spi->name, ret_val);
            ret = -3;
            goto err;
        }
    }
    else
    {
        ret = -2;
        goto err;
    }

err:
#ifdef USING_SPI_OPTION_FUNCTION
    if (spi->opt.complete_read != NULL)
    {
        spi->opt.complete_read(spi);
    }
#endif

    if (spi->master_or_slave == SPI_MASTER)
    {
        osMutexRelease(spi->tx_mutex);
    }

    return ret;
}

#ifdef USING_SPI_OPTION_FUNCTION
static int8_t spi_opt_init(DEVICE_SPI *spi, DEVICE_SPI_OPT *opt_func)
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

static int8_t spi_rx_queue_init(DEVICE_SPI *spi, osMessageQueueId_t queue)
{
    if (spi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    spi->rx_queue = queue;

    return 0;
}

static int8_t spi_dma_rx_buf_init(DEVICE_SPI *spi, uint8_t *buf, uint16_t len)
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

static int8_t spi_rx_callback_register(DEVICE_SPI *spi, int8_t (*cb)(void *arg))
{
    if (spi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    spi->rx_cb = cb;

    return 0;
}

#ifdef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
static int8_t device_irq_node_add(DEVICE_SPI *spi, IRQ_INFO_NODE *node)
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

static int8_t device_irq_node_find(DEVICE_SPI *spi, uint8_t *node_name, DEVICE_IRQ_LIST **ptr)
{
    if (spi == NULL || node_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    DEVICE_IRQ_LIST *header = spi->irq_list;
    *ptr = header;

    while (*ptr != NULL && strcmp((*ptr)->node_data->node_name, node_name) != 0)
    {
        *ptr = (*ptr)->next;
    }

    if (*ptr == NULL)
    {
        printf("can't find target node\r\n");
    }

    return 0;
}

static int8_t device_irq_node_delete(DEVICE_SPI *spi, uint8_t *node_name)
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

static int8_t device_irq_list_clear(DEVICE_SPI *spi)
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

static int8_t device_irq_list_list(DEVICE_SPI *spi)
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

static int8_t device_irq_wait_with_block(DEVICE_SPI *spi, uint8_t *node_name, char splitter, uint32_t timeout)
{
    if (spi == NULL || node_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    int8_t ret = 0;   

    uint8_t name_buf[50] = {0};
    uint8_t *argv[5] = {NULL};
    memcpy(name_buf, node_name, strlen(node_name));

    uint8_t name_num = split_string(name_buf, splitter, argv);

    DEVICE_IRQ_LIST *node_res = NULL;
    osEventFlagsId_t event = NULL;
    uint32_t flag = 0;

    for (uint8_t i = 0; i < name_num; i++)
    {
        ret = device_irq_node_find(spi, argv[i], &node_res);
        if (ret == 0 && node_res != NULL) 
        {
            flag |= node_res->node_data->irq_event_flag;
            event = node_res->node_data->irq_event;
        }
        else
        {
            printf("device %s irq node find err\r\n", argv[i]);
            return -2;
        }

        // printf("[%d]: %s\r\n", i, argv[i]);
    }
 
    uint32_t ret_val = osEventFlagsWait(event, flag, osFlagsWaitAny, timeout);
    if ((ret_val & flag) != ret_val)
    {
        printf("device %s wait irq err:%u\r\n", spi->name, ret_val);
        return -3;
    }

    return ret_val;
}

#endif

static int8_t spi_ioctl(DEVICE_SPI *spi, uint8_t cmd, void *arg)
{
    if (spi == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    int8_t ret = 0;
    uint32_t offset = 0;

    switch (cmd)
    {
#ifdef USING_SPI_OPTION_FUNCTION
    case SPI_CMD_SET_OPT_FUNC:
        ret = spi_opt_init(spi, (DEVICE_SPI_OPT *)arg);
        if (ret != 0)
        {
            printf("device %s set opt func err:%d\r\n", spi->name, ret);
        }
        break;
#endif

    case SPI_CMD_SET_DMA_RX_QUEUE:
        ret = spi_rx_queue_init(spi, (osMessageQueueId_t)arg);
        if (ret != 0)
        {
            printf("device %s set rx queue err:%d\r\n", spi->name, ret);
        }
        break;

    case SPI_CMD_SET_DMA_RX_BUF:
    {
        uint32_t buf = *(uint32_t *)((uint8_t *)arg + offset);

        offset += ALIGN(sizeof(buf), 4);
        uint16_t len = *(uint16_t *)((uint8_t *)arg + offset);

        ret = spi_dma_rx_buf_init(spi, buf, len);
        if (ret != 0)
        {
            printf("device %s set rx buf err:%d\r\n", spi->name, ret);
        }
        break;
    }
    case SPI_CMD_SET_RX_CALLBACK:
        ret = spi_rx_callback_register(spi, (int8_t (*)(void *arg))arg);
        if (ret != 0)
        {
            printf("device %s set rx callback err:%d\r\n", spi->name, ret);
        }
        break;

#ifdef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
    case SPI_CMD_IRQ_NODE_ADD:
        ret = device_irq_node_add(spi, (IRQ_INFO_NODE *)arg);
        if (ret != 0)
        {
            printf("device %s irq node add err:%d\r\n", spi->name, ret);
        }
        break;
    
    case SPI_CMD_IRQ_NODE_DEL:
        ret = device_irq_node_delete(spi, (uint8_t *)arg);
        if (ret != 0)
        {
            printf("device %s irq node del err:%d\r\n", spi->name, ret);
        }
        break;

    case SPI_CMD_IRQ_NODE_FIND:
    {
        uint32_t name = *(uint32_t *)((uint8_t *)arg + offset);
        offset += ALIGN(sizeof(name), 4);

        ret = device_irq_node_find(spi, name, (DEVICE_IRQ_LIST **)((uint8_t *)arg + offset));
        if (ret != 0)
        {
            printf("device %s irq node find err\r\n", spi->name);
            ret = -1;
        }
        break;
    }
    case SPI_CMD_IRQ_LIST_LIST:
        ret = device_irq_list_list(spi);
        if (ret != 0)
        {
            printf("device %s irq list list err:%d\r\n", spi->name, ret);
        }
        break;

    case SPI_CMD_IRQ_LIST_CLEAR:
        ret = device_irq_list_clear(spi);
        if (ret != 0)
        {
            printf("device %s irq list clear err:%d\r\n", spi->name, ret);
        }
        break;

    case SPI_CMD_IRQ_WAIT_WITH_BLOCK:
    {
        uint32_t name = *(uint32_t *)((uint8_t *)arg + offset);
        offset += ALIGN(sizeof(name), 4);

        char splitter = *(char *)((uint8_t *)arg + offset);
        offset += ALIGN(sizeof(splitter), 4);

        uint32_t timeout = *(uint32_t *)((uint8_t *)arg + offset);

        ret = device_irq_wait_with_block(spi, (uint8_t *)name, splitter, timeout);
        if (ret < 0)
        {
            printf("device %s irq wait with block err:%d\r\n", spi->name, ret);
        }
        break;
    }
#endif
    default:
        break;
    }


    return ret;
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
    __disable_irq();

    if (!memcmp(device_name, DEVICE_NAME_SPI1, sizeof(DEVICE_NAME_SPI1)))
    {
        MX_SPI1_Init();
        memcpy(spi, &hspi1, sizeof(SPI_HandleTypeDef));
        // if (hspi1.hdmarx->Init.Mode == DMA_CIRCULAR)
        // {
        //     extern DMA_HandleTypeDef hdma_spi1_rx;
        //     hdma_spi1_rx.Parent = (void *)spi;
        // }
    }
    else if (!memcmp(device_name, DEVICE_NAME_SPI2, sizeof(DEVICE_NAME_SPI2)))
    {
        MX_SPI2_Init();
        memcpy(spi, &hspi2, sizeof(SPI_HandleTypeDef));
        if (hspi2.hdmarx->Init.Mode == DMA_CIRCULAR)
        {
            extern DMA_HandleTypeDef hdma_spi2_rx;
            hdma_spi2_rx.Parent = (void *)spi;
        }
    }
    else if (!memcmp(device_name, DEVICE_NAME_SPI4, sizeof(DEVICE_NAME_SPI4)))
    {
        MX_SPI4_Init();
        memcpy(spi, &hspi4, sizeof(SPI_HandleTypeDef));
        if (hspi4.hdmarx->Init.Mode == DMA_CIRCULAR)
        {
            extern DMA_HandleTypeDef hdma_spi4_rx;
            hdma_spi4_rx.Parent = (void *)spi;
        }
    }
    else
    {
        /* add other spi here */
    }

    __HAL_SPI_DISABLE(&spi->hspi);

    __enable_irq();

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
    HAL_SPI_RegisterCallback((SPI_HandleTypeDef *)spi, HAL_SPI_TX_RX_COMPLETE_CB_ID, TxRxCpltCallback);
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
    spi->write_and_read = spi_write_and_read;
    spi->ioctl = spi_ioctl;
    spi->rx_cb = NULL;

    /* 8. open device */
    return 0;//spi->open(spi);
}

