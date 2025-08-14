#include "drv_fdcan.h"
#include "fdcan.h"

static void ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
    struct device_fdcan *fdcan = (struct device_fdcan *)hfdcan;

    printf("device %s error code: %#.8x\r\n", fdcan->name, hfdcan->ErrorCode);  /* refer to HAL_FDCAN_Error_Code */
}

static void RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    osStatus_t ret = osOK;
    struct device_fdcan *fdcan = (struct device_fdcan *)hfdcan;
    struct fdcan_rx_msg msg = {0};//{.header = {0}, .buf = fdcan->rx_buf};
    HAL_StatusTypeDef status = HAL_OK;

    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        status = HAL_FDCAN_GetRxMessage((FDCAN_HandleTypeDef *)fdcan, FDCAN_RX_FIFO0, (FDCAN_RxHeaderTypeDef *)&msg, msg.buf);
        if (status != HAL_OK)
        {
            printf("HAL_FDCAN_GetRxMessage error: %d\r\n", status);
            return;
        }
#if 0
        printf("recv: ");
        for (uint8_t i = 0; i < 64; i++)
        {
            printf("%.2x ", msg.buf[i]);
        }
        printf("\r\n");
#endif
        if (fdcan->rx_queue != NULL)
        {
            ret = osMessageQueuePut(fdcan->rx_queue, &msg, 0, 0);
            if (ret != osOK)
            {
                printf("device %s put data err: %d\r\n", fdcan->name, ret);
                return;
            }
        }
    }

    if (fdcan->rx_cb != NULL)
    {
        fdcan->rx_cb((void *)fdcan);
    }

    status = HAL_FDCAN_ActivateNotification((FDCAN_HandleTypeDef *)fdcan, RxFifo0ITs, 0);
    if (status != HAL_OK)
    {
        printf("HAL_FDCAN_ActivateNotification error: %d\r\n", status);
    }

}

static void RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    printf("---RxFifo1Callback---\r\n");
}

static void ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
    struct device_fdcan *fdcan = (struct device_fdcan *)hfdcan;

    printf("device %s error status: %#.8x\r\n", fdcan->name, ErrorStatusITs);   /* only indicate EP、EW and BO in ErrorStatusITs */
}


static int8_t fdcan_open(struct device_fdcan *fdcan)
{
    if (fdcan == NULL)
    {
        printf("ptr is NULL\r\n");
        return -1;
    }

    if (fdcan->open_state)
    {
        printf("fdcan is already opened\r\n");
        return -2;
    }

    fdcan->open_state = 1;

    return 0;
}

static int8_t fdcan_close(struct device_fdcan *fdcan)
{
    if (fdcan == NULL)
    {
        printf("ptr is NULL\r\n");
        return -1;
    }

    if (!fdcan->open_state)
    {
        printf("fdcan is already closed\r\n");
        return -2;
    }

    fdcan->open_state = 0;

    HAL_StatusTypeDef status = HAL_FDCAN_Stop((FDCAN_HandleTypeDef *)fdcan);
    if (status != HAL_OK)
    {
        printf("HAL_FDCAN_Stop error: %d\r\n", status);
        return -3;
    }

    osStatus_t stat = osMutexDelete(fdcan->tx_mutex);
    if (stat != osOK)
    {
        printf("device %s delete mutex err: %d\r\n", fdcan->name, stat);
        return -4;
    }

    return 0;
}

static int8_t fdcan_write(struct device_fdcan *fdcan, struct fdcan_tx_msg *msg, uint32_t timeout)
{
    if (fdcan == NULL || msg == NULL)
    {
        printf("ptr is NULL\r\n");
        return -1;
    }

    if (!fdcan->open_state)
    {
        printf("fdcan is not open\r\n");
        return -2;
    }

    if (HAL_FDCAN_GetTxFifoFreeLevel((FDCAN_HandleTypeDef *)fdcan) > 0)
    {
        osStatus_t ret = osMutexAcquire(fdcan->tx_mutex, timeout);
        if (ret != osOK)
        {
            printf("device %s acquire mutex err: %d\r\n", fdcan->name, ret);
            return -3;
        }

        HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ((FDCAN_HandleTypeDef *)fdcan, &msg->header, msg->buf);
        if (status != HAL_OK)
        {
            printf("HAL_FDCAN_AddMessageToTxFifoQ error: %d\r\n", status);
            osMutexRelease(fdcan->tx_mutex);
            return -4;
        }

        osMutexRelease(fdcan->tx_mutex);
    }
    else
    {
        printf("tx fifo is full\r\n");
        return -3;
    }

    return 0;
}

static int8_t fdcan_read(struct device_fdcan *fdcan, struct fdcan_rx_msg *msg, uint32_t timeout)
{
    if (fdcan == NULL || msg == NULL)
    {
        printf("ptr is NULL\r\n");
        return -1;
    }

    if (!fdcan->open_state)
    {
        printf("fdcan is not open\r\n");
        return -2;
    }

    osStatus_t ret = osMessageQueueGet(fdcan->rx_queue, msg, 0, timeout);
    if (ret != osOK)
    {
        printf("device %s read data err: %d\r\n", fdcan->name, ret);
        return -3;
    }

    return 0;
}

static int8_t fdcan_ioctl(struct device_fdcan *fdcan, uint32_t cmd, void *arg)
{
    if (fdcan == NULL || arg == NULL)
    {
        printf("ptr is NULL\r\n");
        return -1;
    }

    int8_t ret = 0;
    HAL_StatusTypeDef status = HAL_OK;

    switch (cmd)
    {
    case FDCAN_CMD_SET_RX_GLOBAL_FILTER:
    {
        struct fdcan_filter *ft = (struct fdcan_filter *)arg;
        status = HAL_FDCAN_ConfigGlobalFilter((FDCAN_HandleTypeDef *)fdcan, ft->global_filter.NonMatchingStd, ft->global_filter.NonMatchingExt, ft->global_filter.RejectRemoteStd, ft->global_filter.RejectRemoteExt);
        if (status != HAL_OK)
        {
            printf("%s set global filter err: %d\r\n", fdcan->name, status);
            ret = -2;
        }
        break;
    }
    case FDCAN_CMD_SET_RX_FILTER:
    {
        struct fdcan_filter *ft = (struct fdcan_filter *)arg;
        status = HAL_FDCAN_ConfigFilter((FDCAN_HandleTypeDef *)fdcan, (FDCAN_FilterTypeDef *)ft);
        if (status != HAL_OK)
        {
            printf("%s set filter err: %d\r\n", fdcan->name, status);
            ret = -2;
        }
        break;
    }
    case FDCAN_CMD_SET_RX_QUEUE:
        fdcan->rx_queue = (osMessageQueueId_t)arg;
        break;

    case FDCAN_CMD_SET_RX_CALLBACK:
        fdcan->rx_cb = (int8_t (*)(void *))arg;
        break;

    case FDCAN_CMD_SET_IRQ_LINE:
    {
        uint32_t irq_type = *(uint32_t *)arg;
        uint32_t irq_line = *((uint32_t *)arg + 1);
        status = HAL_FDCAN_ConfigInterruptLines((FDCAN_HandleTypeDef *)fdcan, irq_type, irq_line);
        if (status != HAL_OK)
        {
            printf("HAL_FDCAN_ConfigInterruptLines error: %d\r\n", status);
            ret = -2;
        }
        break;
    }
    case FDCAN_CMD_SET_IRQ_ENABLE:
    {
        uint32_t *ptr = (uint32_t *)arg;
        uint32_t irq_switch = *ptr;
        uint32_t irq_type = *(ptr + 1);
        uint32_t tx_buf_idx = *(ptr + 2);
        if (irq_switch == 0)
        {
            status = HAL_FDCAN_DeactivateNotification((FDCAN_HandleTypeDef *)fdcan, irq_type);
            if (status != HAL_OK)
            {
                printf("HAL_FDCAN_DeactivateNotification error: %d\r\n", status);
                ret = -2;
            }
        }
        else
        {
            status = HAL_FDCAN_ActivateNotification((FDCAN_HandleTypeDef *)fdcan, irq_type, tx_buf_idx);
            if (status != HAL_OK)
            {
                printf("HAL_FDCAN_ActivateNotification error: %d\r\n", status);
                ret = -2;
            }
        }
        break;
    }
    case FDCAN_CMD_SET_START:
    {
        uint32_t enable = *(uint32_t *)arg;
        if (enable)
        {
            status = HAL_FDCAN_Start((FDCAN_HandleTypeDef *)fdcan);
            if (status != HAL_OK)
            {
                printf("HAL_FDCAN_Start error: %d\r\n", status);
                ret = -2;
            }
        }
        else
        {
            status = HAL_FDCAN_Stop((FDCAN_HandleTypeDef *)fdcan);
            if (status != HAL_OK)
            {
                printf("HAL_FDCAN_Stop error: %d\r\n", status);
                ret = -2;
            }
        }
        break;
    }
    case FDCAN_CMD_GET_ERR_CNT:
        status = HAL_FDCAN_GetErrorCounters((FDCAN_HandleTypeDef *)fdcan, (FDCAN_ErrorCountersTypeDef *)arg);
        if (status != HAL_OK)
        {   
            printf("HAL_FDCAN_GetErrorCounters error: %d\r\n", status);
            ret = -2;
        }
        break;
    
    case FDCAN_CMD_GET_PROTO_STAT:
        status = HAL_FDCAN_GetProtocolStatus((FDCAN_HandleTypeDef *)fdcan, (FDCAN_ProtocolStatusTypeDef *)arg);
        if (status != HAL_OK)
        {   
            printf("HAL_FDCAN_GetProtocolStatus error: %d\r\n", status);
            ret = -2;
        }
        break;
    
    default:
        return -3;
    }

    return ret;
}

int8_t fdcan_init(struct device_fdcan *fdcan, uint8_t *device_name)
{
    if (fdcan == NULL || device_name == NULL)
    {
        printf("ptr is NULL\r\n");
        return -1;
    }

    if (fdcan->open_state)
    {
        printf("device %s is already opened\r\n", fdcan->name);
        return -2;
    }

    /* 1. init hardware  */
    if (!memcmp(device_name, DEVICE_NAME_FDCAN1, sizeof(DEVICE_NAME_FDCAN1)))
    {
        MX_FDCAN1_Init();
        memcpy(fdcan, &hfdcan1, sizeof(FDCAN_HandleTypeDef));
    }
    else if (!memcmp(device_name, DEVICE_NAME_FDCAN2, sizeof(DEVICE_NAME_FDCAN2)))
    {

    }
    else if (!memcmp(device_name, DEVICE_NAME_FDCAN3, sizeof(DEVICE_NAME_FDCAN3)))
    {

    }
    else
    {
        printf("device %s is not supported\r\n", device_name);
        return -3;
    }

    /* 2. create tx mutex */
    osMutexAttr_t fdcan_tx_mutex_attributes = {
    .name = "fdcan_tx_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    fdcan->tx_mutex = osMutexNew(&fdcan_tx_mutex_attributes);
    if (fdcan->tx_mutex == NULL)
    {
        printf("create tx mutex err\r\n");
        return -4;
    }

    /* 3. register callback function */
    HAL_StatusTypeDef status = HAL_OK;
    // status = HAL_FDCAN_RegisterCallback((FDCAN_HandleTypeDef *)fdcan, HAL_FDCAN_TIMEOUT_OCCURRED_CB_ID, NULL);
    status |= HAL_FDCAN_RegisterCallback((FDCAN_HandleTypeDef *)fdcan, HAL_FDCAN_ERROR_CALLBACK_CB_ID, ErrorCallback);
    status |= HAL_FDCAN_RegisterRxFifo0Callback((FDCAN_HandleTypeDef *)fdcan, RxFifo0Callback);
    status |= HAL_FDCAN_RegisterRxFifo1Callback((FDCAN_HandleTypeDef *)fdcan, RxFifo1Callback);
    status |= HAL_FDCAN_RegisterErrorStatusCallback((FDCAN_HandleTypeDef *)fdcan, ErrorStatusCallback);
    if (status != HAL_OK)
    {
        printf("register callback function err: %d\r\n", status);
        return -5;
    }

    /* 4. device rename */
    memcpy(fdcan->name, device_name, DEVICE_NAME_LENGTH);

    /* 5. register operation function */
    fdcan->open = fdcan_open;
    fdcan->close = fdcan_close;
    fdcan->write = fdcan_write;
    fdcan->read = fdcan_read;
    fdcan->ioctl = fdcan_ioctl;

    return 0;
}
