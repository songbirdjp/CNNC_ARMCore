#include "fdcan_port.h"
#include "init_call.h"

static struct device_fdcan fdcan1 = {0};

static struct device_fdcan *device_fdcan1_get(void)
{
    return &fdcan1;
}

/**
  * @brief This function handles FDCAN1 interrupt 0.
  */
void FDCAN1_IT0_IRQHandler(void)
{
  /* USER CODE BEGIN FDCAN1_IT0_IRQn 0 */

  /* USER CODE END FDCAN1_IT0_IRQn 0 */
  HAL_FDCAN_IRQHandler((FDCAN_HandleTypeDef *)device_fdcan1_get());
  /* USER CODE BEGIN FDCAN1_IT0_IRQn 1 */

  /* USER CODE END FDCAN1_IT0_IRQn 1 */
}

/**
  * @brief This function handles FDCAN1 interrupt 1.
  */
void FDCAN1_IT1_IRQHandler(void)
{
  /* USER CODE BEGIN FDCAN1_IT1_IRQn 0 */

  /* USER CODE END FDCAN1_IT1_IRQn 0 */
  HAL_FDCAN_IRQHandler((FDCAN_HandleTypeDef *)device_fdcan1_get());
  /* USER CODE BEGIN FDCAN1_IT1_IRQn 1 */

  /* USER CODE END FDCAN1_IT1_IRQn 1 */
}

static int8_t fdcan_dlc_to_index(uint8_t len)
{
    switch (len)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return len;
    case 12:
        return FDCAN_DLC_BYTES_12;
    case 16:
        return FDCAN_DLC_BYTES_16;
    case 20:
        return FDCAN_DLC_BYTES_20;
    case 24:
        return FDCAN_DLC_BYTES_24;
    case 32:
        return FDCAN_DLC_BYTES_32;
    case 48:
        return FDCAN_DLC_BYTES_48;
    case 64:
        return FDCAN_DLC_BYTES_64;
    default:
        return -1;
    }

    return 0;
}

static int8_t device_fdcan_init(struct device_fdcan *fdcan, uint8_t *device_name)
{
    int8_t ret = 0;

    /* 1. device init */
    ret = fdcan_init(fdcan, device_name);
    if (ret != 0)
    {
        printf("device %s init err: %d\r\n", device_name, ret);
        return ret;
    }

    /* 2. set rx filter */
    struct fdcan_filter filter = {
        .filter.IdType = FDCAN_STANDARD_ID,
        .filter.FilterIndex = 0,
        .filter.FilterType = FDCAN_FILTER_MASK,
        .filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
        .filter.FilterID1 = 0,
        .filter.FilterID2 = 0,
        .global_filter.NonMatchingStd = FDCAN_REJECT,
        .global_filter.NonMatchingExt = FDCAN_REJECT,
        .global_filter.RejectRemoteStd = FDCAN_REJECT_REMOTE,
        .global_filter.RejectRemoteExt = FDCAN_REJECT_REMOTE
    };
    ret = fdcan->ioctl(fdcan, FDCAN_CMD_SET_RX_GLOBAL_FILTER, (void *)&filter);
    if (ret != 0)
    {
        printf("device %s set rx filter err: %d\r\n", device_name, ret);
        return ret;
    }
    ret = fdcan->ioctl(fdcan, FDCAN_CMD_SET_RX_FILTER, (void *)&filter);
    if (ret != 0)
    {
        printf("device %s set rx filter err: %d\r\n", device_name, ret);
        return ret;
    }

    /* 3. register rx queue */
    osMessageQueueId_t fdcan_QueueHandle = osMessageQueueNew(10, sizeof(struct fdcan_rx_msg), NULL);
    if (fdcan_QueueHandle == NULL)
    {
        printf("device %s create rx queue failed\r\n", device_name);
        return -1;
    }

    ret = fdcan->ioctl(fdcan, FDCAN_CMD_SET_RX_QUEUE, (void *)fdcan_QueueHandle);
    if (ret != 0)
    {
        printf("device %s set rx queue err: %d\r\n", device_name, ret);
        return ret;
    }

    /* 4. irq line config */
    struct fdcan_irq_line_config
    {
        uint32_t irq_idx;
        uint32_t line;
    }line_info = {0, FDCAN_INTERRUPT_LINE0};

    ret = fdcan->ioctl(fdcan, FDCAN_CMD_SET_IRQ_LINE, (void *)&line_info);
    if (ret != 0)
    {
        printf("device %s set irq line err: %d\r\n", device_name, ret);
        return ret;
    }

    /* 5. irq enable */
    struct fdcan_irq_enable
    {
        uint32_t en;
        uint32_t irq_idx;
        uint32_t tx_buf_idx
    }irq_info = {.en = 1, .irq_idx = 0, .tx_buf_idx = 0};

    irq_info.irq_idx = FDCAN_IT_RX_FIFO0_MESSAGE_LOST | FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO0_NEW_MESSAGE | \
                   FDCAN_IT_RX_FIFO1_MESSAGE_LOST | FDCAN_IT_RX_FIFO1_FULL | FDCAN_IT_RX_FIFO1_NEW_MESSAGE | \
                   FDCAN_IT_RAM_ACCESS_FAILURE | FDCAN_IT_ERROR_LOGGING_OVERFLOW | FDCAN_IT_ARB_PROTOCOL_ERROR | \
                   FDCAN_IT_DATA_PROTOCOL_ERROR | FDCAN_IT_ERROR_PASSIVE | FDCAN_IT_ERROR_WARNING | FDCAN_IT_BUS_OFF;


    ret = fdcan->ioctl(fdcan, FDCAN_CMD_SET_IRQ_ENABLE, (void *)&irq_info);
    if (ret != 0)
    {
        printf("device %s set irq enable err: %d\r\n", device_name, ret);
        return ret;
    }

    /* 6. open device */
    ret = fdcan->open(fdcan);
    if (ret != 0)
    {
        printf("device %s open err: %d\r\n", device_name, ret);
        return ret;
    }

    /* 7. start device */
    uint32_t en = 1;
    ret = fdcan->ioctl(fdcan, FDCAN_CMD_SET_START, (void *)&en);
    if (ret != 0)
    {
        printf("device %s start err: %d\r\n", device_name, ret);
        return ret;
    }

    return 0;
}

static int8_t fdcan1_init(void)
{
    return device_fdcan_init(device_fdcan1_get(), DEVICE_NAME_FDCAN1);
}
INIT_APP_EXPORT(fdcan1_init);


int8_t fdcan1_data_write(uint32_t id, uint8_t *buf, uint8_t len)
{
    if (buf == NULL)
    {
        return -1;
    }

    int8_t dlc = fdcan_dlc_to_index(len);
    if (dlc < 0)
    {
        printf("buf len is not support: %d\r\n", len);
        return -2;
    }

    struct fdcan_tx_msg msg = {0};

    msg.header.Identifier = id;
    msg.header.IdType = FDCAN_STANDARD_ID;
    msg.header.TxFrameType = FDCAN_DATA_FRAME;
    msg.header.DataLength = dlc;
    msg.header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    msg.header.BitRateSwitch = FDCAN_BRS_OFF;
    msg.header.FDFormat = FDCAN_FD_CAN;
    msg.header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    msg.header.MessageMarker = 0;

    msg.buf = buf;

    return device_fdcan1_get()->write(device_fdcan1_get(), &msg, 1000);
}

int8_t fdcan1_data_read(struct fdcan_rx_msg *msg, uint32_t timeout)
{
    if (msg == NULL)
    {
        return -1;
    }

    return device_fdcan1_get()->read(device_fdcan1_get(), msg, timeout);
}

#ifdef FDCAN1_TEST
#include "shell.h"

static int8_t fdcan1_test(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: <cmd><run times>\r\n");
        return -1;
    }


    uint8_t tx_buf[64] = {0};
    struct fdcan_rx_msg msg = {0};
    int8_t ret = 0;

    uint32_t run_times = strtoul(argv[1], NULL, 10);

    for (uint32_t cnt = 0; cnt < run_times; cnt++)
    {
        for (uint8_t i = 0; i < sizeof(tx_buf); i++)
        {
            tx_buf[i] = (uint8_t)cnt + i;
        }

        ret = fdcan1_data_write(0, tx_buf, sizeof(tx_buf));
        if (ret != 0)
        {
            printf("fdcan1_data_write err: %d\r\n", ret);
            return ret;
        }

        ret = fdcan1_data_read(&msg, osWaitForever);
        if (ret != 0)
        {
            printf("fdcan1_data_read err: %d\r\n", ret);
            return ret;
        }

        if (memcmp(tx_buf, msg.buf, sizeof(tx_buf)))
        {
            printf("rx_buf: ");
            for (uint8_t i = 0; i < sizeof(msg.buf); i++)
            {
                printf("%02x ", msg.buf[i]);
            }
            printf("\r\n");
        }

        printf("count: %u\r\n", cnt);
    }

    FDCAN_ErrorCountersTypeDef err_cnt = {0};
    ret = device_fdcan1_get()->ioctl(device_fdcan1_get(), FDCAN_CMD_GET_ERR_CNT, (void *)&err_cnt);
    if (ret != 0)
    {
        printf("device %s get err cnt err: %d\r\n", DEVICE_NAME_FDCAN1, ret);
        return ret;
    }

    FDCAN_ProtocolStatusTypeDef status = {0};
    ret = device_fdcan1_get()->ioctl(device_fdcan1_get(), FDCAN_CMD_GET_PROTO_STAT, (void *)&status);
    if (ret != 0)
    {
        printf("device %s get proto status err: %d\r\n", DEVICE_NAME_FDCAN1, ret);
        return ret;
    }

    printf("TxErrorCnt: %u\r\n", err_cnt.TxErrorCnt);
    printf("RxErrorCnt: %u\r\n", err_cnt.RxErrorCnt);
    printf("RxErrorPassive: %u\r\n", err_cnt.RxErrorPassive);
    printf("ErrorLogging: %u\r\n", err_cnt.ErrorLogging);

    printf("Activity: %u\r\n", status.Activity);
    printf("BusOff: %u\r\n", status.BusOff);
    printf("DataLastErrorCode: %u\r\n", status.DataLastErrorCode);
    printf("ErrorPassive: %u\r\n", status.ErrorPassive);
    printf("LastErrorCode: %u\r\n", status.LastErrorCode);
    printf("ProtocolException: %u\r\n", status.ProtocolException);
    printf("RxBRSflag: %u\r\n", status.RxBRSflag);
    printf("RxESIflag: %u\r\n", status.RxESIflag);
    printf("RxFDFflag: %u\r\n", status.RxFDFflag);
    printf("TDCvalue: %u\r\n", status.TDCvalue);
    printf("Warning: %u\r\n", status.Warning);


    return 0;
}
MSH_CMD_EXPORT_ALIAS(fdcan1_test, fdcan1_test, test fdcan1);
#endif