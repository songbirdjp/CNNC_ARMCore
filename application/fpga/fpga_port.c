#include "drv_spi.h"
#include "ulog.h"

#undef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
static DEVICE_SPI device_send_to_fpga = {0};
DEVICE_SPI *device_send_to_fpga_get(void)
{
    return &device_send_to_fpga;
}

static DEVICE_SPI device_recv_from_fpga = {0};

DEVICE_SPI *device_recv_from_fpga_get(void)
{
    return &device_recv_from_fpga;
}

void SPI2_IRQHandler(void)
{
  /* USER CODE BEGIN SPI2_IRQn 0 */

  /* USER CODE END SPI2_IRQn 0 */
  HAL_SPI_IRQHandler((SPI_HandleTypeDef *)device_recv_from_fpga_get());
  /* USER CODE BEGIN SPI2_IRQn 1 */

  /* USER CODE END SPI2_IRQn 1 */
}
void SPI3_IRQHandler(void)
{
  /* USER CODE BEGIN SPI3_IRQn 0 */

  /* USER CODE END SPI3_IRQn 0 */
  HAL_SPI_IRQHandler((SPI_HandleTypeDef *)device_send_to_fpga_get());
  /* USER CODE BEGIN SPI3_IRQn 1 */

  /* USER CODE END SPI3_IRQn 1 */
}

#ifdef USING_SPI_OPTION_FUNCTION
static DEVICE_SPI_OPT device_send_to_fpga_opt = {0};
static int8_t fpga_opt_before_write(DEVICE_SPI *spi)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    return 0;
}
static int8_t fpga_opt_after_write(DEVICE_SPI *spi)
{
    // LOG_I("after write\r\n");

    return 0;
}
static int8_t fpga_opt_complete_write(DEVICE_SPI *spi)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    return 0;
}
static int8_t fpga_opt_before_read(DEVICE_SPI *spi)
{
    // LOG_I("before read\r\n");

    return 0;
}
static int8_t fpga_opt_after_read(DEVICE_SPI *spi)
{
    // LOG_I("after read\r\n");

    return 0;
}
static int8_t fpga_opt_complete_read(DEVICE_SPI *spi)
{
    // LOG_I("complete read\r\n");

    return 0;
}
static int8_t device_send_to_fpga_opt_init(DEVICE_SPI *spi, DEVICE_SPI_OPT *spi_opt)
{
    spi_opt->before_write = fpga_opt_before_write;
    spi_opt->after_write = fpga_opt_after_write;
    spi_opt->complete_write = fpga_opt_complete_write;
    spi_opt->before_read = fpga_opt_before_read;
    spi_opt->after_read = fpga_opt_after_read;
    spi_opt->complete_read = fpga_opt_complete_read;

    return spi->ioctl(spi, SPI_CMD_SET_OPT_FUNC, spi_opt);
}
#endif

int8_t device_send_to_fpga_init(uint8_t *device_name)
{
    if (device_name == NULL)
    {
        LOG_E("ptr is null\r\n");
        return -1;
    }

    int8_t ret = spi_init(device_send_to_fpga_get(), device_name, SPI_MASTER);
    if (ret != 0)
    {
        LOG_E("device send to fpga init err:%d\r\n", ret);
    }

#ifdef USING_SPI_OPTION_FUNCTION
    ret = device_send_to_fpga_opt_init(device_send_to_fpga_get(), &device_send_to_fpga_opt);
    if (ret != 0)
    {
        LOG_E("device send to fpga init err:%d\r\n", ret);
        return ret;
    }
#endif

    return ret;
}

int8_t device_send_to_fpga_open(void)
{
    return device_send_to_fpga_get()->open(device_send_to_fpga_get());
}

int8_t device_send_to_fpga_write(uint8_t *buf, uint16_t size, uint32_t timeout)
{
    return device_send_to_fpga_get()->write(device_send_to_fpga_get(), buf, size, timeout);
}

#undef USING_SPI_OPTION_FUNCTION

int8_t device_recv_from_fpga_init(uint8_t *device_name)
{
    if (device_name == NULL)
    {
        LOG_E("ptr is null\r\n");
        return -1;
    }

    return spi_init(device_recv_from_fpga_get(), device_name, SPI_SLAVE);
}

int8_t device_recv_from_fpga_buffer_init(uint8_t *buf, uint16_t len)
{
    struct dma_rx_buf_info
    {
        uint8_t *buf;
        uint16_t len;
    } info = {buf, len};

    return device_recv_from_fpga_get()->ioctl(device_recv_from_fpga_get(), SPI_CMD_SET_DMA_RX_BUF, (void *)&info);
}

int8_t device_recv_from_fpga_queue_init(osMessageQueueId_t queue)
{
    return device_recv_from_fpga_get()->ioctl(device_recv_from_fpga_get(), SPI_CMD_SET_DMA_RX_QUEUE, (void *)queue);
}

int8_t device_recv_from_fpga_callback_register(int8_t (*cb)(void *arg))
{
    return device_recv_from_fpga_get()->ioctl(device_recv_from_fpga_get(), SPI_CMD_SET_RX_CALLBACK, (void *)cb);
}

int8_t device_recv_from_fpga_open(void)
{
    return device_recv_from_fpga_get()->open(device_recv_from_fpga_get());
}