#include "ltc2632_port.h"
#include "drv_spi.h"

#undef USING_SPI_OPTION_FUNCTION
#undef USING_SPI_SLAVE_TO_MASTER_INTERRUPT

static DEVICE_SPI device_ltc2632 = {0};

static DEVICE_SPI *device_ltc2632_get(void)
{
    return &device_ltc2632;
}

void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */

  /* USER CODE END SPI1_IRQn 0 */
  HAL_SPI_IRQHandler((SPI_HandleTypeDef *)device_ltc2632_get());
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}

int8_t device_ltc2632_init(uint8_t *device_name)
{
    if (device_name == NULL)
    {
        return -1;
    }

    return spi_init(device_ltc2632_get(), device_name, SPI_MASTER);
}

int8_t device_ltc2632_open(void)
{
    return device_ltc2632_get()->open(device_ltc2632_get());
}

int8_t device_ltc2632_write(uint8_t *buf, uint16_t size, uint32_t timeout)
{
    return device_ltc2632_get()->write(device_ltc2632_get(), buf, size, timeout);
}

int8_t device_ltc2632_buffer_init(uint8_t *buf, uint16_t len)
{
    struct dma_rx_buf_info
    {
        uint8_t *buf;
        uint16_t len;
    } info = {buf, len};

    return device_ltc2632_get()->ioctl(device_ltc2632_get(), SPI_CMD_SET_DMA_RX_BUF, (void *)&info); 
}

int8_t device_ltc2632_queue_init(osMessageQueueId_t queue)
{
    return device_ltc2632_get()->ioctl(device_ltc2632_get(), SPI_CMD_SET_DMA_RX_QUEUE, (void *)queue);
}

int8_t device_ltc2632_callback_register(int8_t (*cb)(void *arg))
{
    return device_ltc2632_get()->ioctl(device_ltc2632_get(), SPI_CMD_SET_RX_CALLBACK, (void *)cb);
}


#ifdef LTC2632_TEST
#include "shell.h"
static int8_t ltc2632_test(int8_t argc, char **argv)
{
    int8_t ret = 0;

    ret = device_ltc2632_init(DEVICE_LTC2632_NAME_DEFAULT);
    if (ret != 0)
    {
        printf("device_ltc2632_init failed\r\n");
        return -1;
    }

    ret = device_ltc2632_open();
    if (ret != 0)
    {
        printf("device_ltc2632_open failed\r\n");
        return -2;
    }

    static uint32_t tx_buf[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};

    ret = device_ltc2632_write(tx_buf, sizeof(tx_buf) / sizeof(tx_buf[0]), 1000);
    if (ret != 0)
    {
        printf("device_ltc2632_write failed\r\n");
        return -3;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(ltc2632_test, ltc2632_test, test ltc2632);
#endif
