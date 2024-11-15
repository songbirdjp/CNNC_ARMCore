#include "adcs7476_port.h"
#include "drv_spi.h"
#include "lptim.h"
#include "ulog.h"
#undef USING_SPI_OPTION_FUNCTION
#undef USING_SPI_SLAVE_TO_MASTER_INTERRUPT

/* master indicates the spi port is used as master, slave indicates the spi port is used as slave */
static DEVICE_SPI device_adcs7476_master = {0}, device_adcs7476_slave = {0};

static DEVICE_SPI *device_adcs7476_master_get(void)
{
    return &device_adcs7476_master;
}

static DEVICE_SPI *device_adcs7476_slave_get(void)
{
    return &device_adcs7476_slave;
}

void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */

  /* USER CODE END SPI1_IRQn 0 */
   HAL_SPI_IRQHandler((SPI_HandleTypeDef *)device_adcs7476_master_get());
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}
void SPI6_IRQHandler(void)
{
  /* USER CODE BEGIN SPI6_IRQn 0 */

  /* USER CODE END SPI6_IRQn 0 */
   HAL_SPI_IRQHandler((SPI_HandleTypeDef *)device_adcs7476_slave_get());
  /* USER CODE BEGIN SPI6_IRQn 1 */

  /* USER CODE END SPI6_IRQn 1 */
}
int8_t device_adcs7476_init(uint8_t *device_name)
{
    if (device_name == NULL)
    {
        printf("device name is NULL\r\n");
        return -1;
    }

    int8_t ret = 0;

    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        ret = spi_init(device_adcs7476_master_get(), device_name, SPI_MASTER);
        //printf("111ret: %d\r\n", ret); 
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {

       
        ret = spi_init(device_adcs7476_slave_get(), device_name, SPI_SLAVE);
    }
    else
    {
        printf("device name is not correct: %s\r\n", device_name);
        return -2;
    }

    return ret;
}

int8_t device_adcs7476_open(uint8_t *device_name)
{
    if (device_name == NULL)
    {
        printf("device name is NULL\r\n");
        return -1;
    }

    int8_t ret = 0;

    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        ret = device_adcs7476_master_get()->open(device_adcs7476_master_get());
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
        ret = device_adcs7476_slave_get()->open(device_adcs7476_slave_get());
    }
    else
    {
        printf("device name is not correct: %s\r\n", device_name);
        return -2;
    }

    return ret;
}
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi6;
static int8_t device_adcs7476_data_buf_init(uint8_t *device_name, uint8_t *rx_buf, uint16_t len)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t *tx_buf = NULL;

    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        tx_buf = (uint8_t *)pvPortMalloc(sizeof(uint16_t) * len);
        if (tx_buf == NULL)
        {
            printf("malloc tx_buf failed\r\n");
            return -1;
        }
        status = HAL_SPI_TransmitReceive_DMA(device_adcs7476_master_get(), tx_buf, rx_buf, len); 
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
        // status = HAL_SPI_Receive_DMA(device_adcs7476_slave_get(), rx_buf, len);
    }
    else
    {
        printf("device name is not correct: %s\r\n", device_name);
        return -2;
    }

    if (status != HAL_OK)
    {
        printf("device %s data buf init err: %d\r\n", device_name, status);
        if (tx_buf != NULL)
        {
            vPortFree(tx_buf);
        }

        return -3;
    }
    return 0;
}

int8_t device_adcs7476_buffer_init(uint8_t *device_name, uint8_t *buf, uint16_t len)
{
    if (device_name == NULL || buf == NULL)
    {
        printf("device name or buf is NULL\r\n");
        return -1;
    }

    if (len == 0)
    {
        printf("buffer length is 0\r\n");
        return -2;
    }

    struct dma_rx_buf_info
    {
        uint8_t *buf;
        uint16_t len;
    } info = {buf, len};

    int8_t ret = 0;

    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        ret = device_adcs7476_master_get()->ioctl(device_adcs7476_master_get(), SPI_CMD_SET_DMA_RX_BUF, (void *)&info);
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
        ret = device_adcs7476_slave_get()->ioctl(device_adcs7476_slave_get(), SPI_CMD_SET_DMA_RX_BUF, (void *)&info);
    }
    else
    {
        printf("device name is not correct: %s\r\n", device_name);
        return -3;
    }

    if (ret != 0)
    {
        printf("device %s buffer init err: %d\r\n", device_name, ret);
        return -4;
    }
    return device_adcs7476_data_buf_init(device_name, buf, len);
}

int8_t device_adcs7476_queue_init(uint8_t *device_name, osMessageQueueId_t queue)
{
    if (device_name == NULL || queue == NULL)
    {
        printf("device name or queue is NULL\r\n");
        return -1;
    }

    int8_t ret = 0;

    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        ret = device_adcs7476_master_get()->ioctl(device_adcs7476_master_get(), SPI_CMD_SET_DMA_RX_QUEUE, (void *)queue);
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
        ret = device_adcs7476_slave_get()->ioctl(device_adcs7476_slave_get(), SPI_CMD_SET_DMA_RX_QUEUE, (void *)queue);
    }
    else
    {
        printf("device name is not correct: %s\r\n", device_name);
        return -2;
    }

    return ret;
}

int8_t device_adcs7476_callback_register(uint8_t *device_name, int8_t (*cb)(void *arg))
{
    if (device_name == NULL || cb == NULL)
    {
        printf("device name or callback is NULL\r\n");
        return -1;
    }

    int8_t ret = 0;

    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        ret = device_adcs7476_master_get()->ioctl(device_adcs7476_master_get(), SPI_CMD_SET_RX_CALLBACK, (void *)cb);
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
        ret = device_adcs7476_slave_get()->ioctl(device_adcs7476_slave_get(), SPI_CMD_SET_RX_CALLBACK, (void *)cb);
    }
    else
    {
        printf("device name is not correct: %s\r\n", device_name);
        return -2;
    }

    if (ret != 0)
    {
        printf("device %s callback register err: %d\r\n", device_name, ret);
        return -3;
    }

    return 0;
}
static int callback_counter = 0;
static void AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
#if 1
   // HAL_LPTIM_Counter_Stop_IT(hlptim);
    //LOG_E("AutoReloadMatchCallback\r\n");
    HAL_StatusTypeDef status = HAL_LPTIM_SetOnce_Stop_IT(hlptim);
    if (status != HAL_OK)
    {
    LOG_E("HAL_LPTIM_SetOnce_Stop_IT err: %d\r\n", status);
    }
#endif
}

int8_t device_adcs7476_sample_interval_set(uint16_t sample_interval_10ns)
{
    if (sample_interval_10ns > 0xFFFF)
    {
        printf("sample interval is too large: (sample_interval_10ns <= 0xFFFF)\r\n");
        return -1;
    }
    //LOG_E("sample_interval_10ns: %d\r\n", sample_interval_10ns);
    HAL_StatusTypeDef status = HAL_OK;

    if (HAL_LPTIM_GetState(&hlptim3) == HAL_LPTIM_STATE_RESET)
    {
        MX_LPTIM3_Init();
    }
    
    status = HAL_LPTIM_RegisterCallback(&hlptim3, HAL_LPTIM_AUTORELOAD_MATCH_CB_ID, AutoReloadMatchCallback);
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_RegisterCallback err: %d\r\n", status);
        return -2;
    }

    status = HAL_LPTIM_Counter_Stop(&hlptim3);
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_Counter_Stop err: %d\r\n", status);
        return -3;
    }

    __HAL_LPTIM_AUTORELOAD_SET(&hlptim3, sample_interval_10ns);

    return 0;
}

int8_t device_adcs7476_sample_enable(uint8_t en)
{
    HAL_StatusTypeDef status = HAL_OK;

    uint32_t period = HAL_LPTIM_ReadAutoReload(&hlptim3);

   status = (en == 0) ? HAL_LPTIM_Counter_Stop(&hlptim3) : HAL_LPTIM_Counter_Start(&hlptim3, period);
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_Counter_Start/Stop err: %d\r\n", status);
        return -1; 
    }

#if 0
    // status = HAL_LPTIM_Counter_Start(&hlptim2, period);   /* 100M / 1分频 */
    // if (status != HAL_OK)
    // {
    //     printf("HAL_LPTIM_Counter_Start err: %d\r\n", status);
    //     return -1;
    // }

    status = HAL_LPTIM_SetOnce_Start_IT(&hlptim2, period, period);   /* 100M / 1分频 */
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_SetOnce_Start_IT err: %d\r\n", status);
        return -1;
    }
#endif
    return 0;
}


#ifdef ADCS7476_TEST
#include "shell.h"
static osMessageQueueId_t queue_master = NULL, queue_slave = NULL;
static osEventFlagsId_t adcs7476_event_flag = NULL;
#define ADC7476_MASTER_FLAG (1 << 0)
#define ADC7476_SLAVE_FLAG  (1 << 1)
#define BUF_LEN 100
static uint16_t recv_buf_master[BUF_LEN] = {0xA5};
static uint16_t recv_buf_slave[BUF_LEN] = {0x5A};
static int8_t adcs7476_master_callback(SPI_HandleTypeDef *hspi)
{
    osStatus_t status = osEventFlagsSet(adcs7476_event_flag, ADC7476_MASTER_FLAG);
    if (status != osOK)
    {
        printf("osEventFlagsSet failed: %d\r\n", status);
    }
    else
    {
        printf("osEventFlagsSet succeeded\r\n");
    }
    return 0;
}

static int8_t adcs7476_slave_callback(SPI_HandleTypeDef *hspi)
{
    osStatus_t status = osEventFlagsSet(adcs7476_event_flag, ADC7476_SLAVE_FLAG);
    if (status != osOK)
    {
        printf("osEventFlagsSet failed: %d\r\n", status);
    }
    else
    {
        printf("osEventFlagsSet succeeded\r\n");
    }
    return 0;
}

static int8_t recv_buf_printf(uint16_t *recv_buf)
{
    for (int i = 0; i < BUF_LEN; i++)
    {
        printf("%#.2x ", recv_buf[i]);
    }
    printf("\r\n");

    return 0;
}

static int8_t adcs7476_test_cfg(uint8_t *device_name)
{
    int8_t ret = 0;

    ret = device_adcs7476_init(device_name);
    if (ret != 0)
    {
        printf("device adcs7476 %s init failed\r\n", device_name);
        return -1;
    }

    uint8_t *recv_buf = NULL;
    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        recv_buf = (uint8_t *)recv_buf_master;
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
        recv_buf = (uint8_t *)recv_buf_slave;
    }
    else
    {
        printf("device name is not correct\r\n");
        return -2;
    }

    ret = device_adcs7476_buffer_init(device_name, recv_buf, BUF_LEN);
    if (ret != 0)
    {
        printf("device adcs7476 %s buffer init failed\r\n", device_name);
        return -3;
    }

    osMessageQueueId_t queue = osMessageQueueNew(5, sizeof(uint16_t) * BUF_LEN, NULL);
    if (queue == NULL)
    {
        printf("queue create failed\r\n");
        return -4;
    }

    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        queue_master = queue;
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
        queue_slave = queue;
    }

    ret = device_adcs7476_queue_init(device_name, queue);
    if (ret != 0)
    {
        printf("device adcs7476 %s queue init failed\r\n", device_name);
        return -5;
    }

    int8_t (*cb)(void *arg) = NULL;
    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        cb = adcs7476_master_callback;
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
        cb = adcs7476_slave_callback;
    }
    else
    {
        printf("device name is not correct\r\n");
        return -6;
    }
    
    ret = device_adcs7476_callback_register(device_name, cb);
    if (ret != 0)
    {
        printf("device adcs7476 %s callback register failed\r\n", device_name);
        return -7;
    }

    ret = device_adcs7476_open(device_name);
    if (ret != 0)
    {
        printf("device adcs7476 %s open failed\r\n", device_name);
        return -8;
    }

    return 0;
}

// static void dma_generator0_callback(DMA_HandleTypeDef *hdma)
// {
//     // printf("dma_generator0_callback\r\n");
//     osEventFlagsSet(event_flags_master, 0x01);
// }

static int8_t adcs7476_test(int8_t argc, char **argv)
{
    int8_t ret = 0;

    adcs7476_event_flag = osEventFlagsNew(NULL);
    if (adcs7476_event_flag == NULL)
    {
        printf("osEventFlagsNew failed\r\n");
        return -1;
    }


    ret = adcs7476_test_cfg(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT);
    if (ret != 0)
    {
        printf("adcs7476 test cfg failed\r\n");
        return -2;
    }

    ret = adcs7476_test_cfg(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT);
    if (ret != 0)
    {
        printf("adcs7476 test cfg failed\r\n");
        return -3;
    }


    // HAL_DMA_RegisterCallback(&hdma_dma_generator0, HAL_DMA_XFER_CPLT_CB_ID, dma_generator0_callback);

    // HAL_DMAEx_EnableMuxRequestGenerator(&hdma_dma_generator0);

    // HAL_DMA_Start_IT(&hdma_dma_generator0, (uint32_t)&SPI4->RXDR, (uint32_t)recv_buf_master, BUF_LEN);


    extern SPI_HandleTypeDef hspi1;

    device_adcs7476_sample_interval_set(10000);

    device_adcs7476_sample_enable(1);   /* 1us * 100 = 10kHz */

    static uint16_t recv_tmp[BUF_LEN] = {0};
    
    for (;;)
    {

        osEventFlagsWait(adcs7476_event_flag, ADC7476_MASTER_FLAG | ADC7476_SLAVE_FLAG, osFlagsWaitAny, osWaitForever);

        osMessageQueueGet(queue_master, recv_tmp, NULL, 0);

        recv_buf_printf(recv_tmp);

        // printf("RXDR: %#.2x\r\n", hspi4.Instance->RXDR);

        // memset(recv_tmp, 0x55, sizeof(uint16_t) * BUF_LEN);

    }

    return ret;
}
MSH_CMD_EXPORT_ALIAS(adcs7476_test, adcs7476_test, test adcs7476);
#endif

