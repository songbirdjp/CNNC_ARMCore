#include "fpga_port.h"
#include "drv_spi.h"


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
    // printf("after write\r\n");

    return 0;
}
static int8_t fpga_opt_complete_write(DEVICE_SPI *spi)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    return 0;
}
static int8_t fpga_opt_before_read(DEVICE_SPI *spi)
{
    // printf("before read\r\n");

    return 0;
}
static int8_t fpga_opt_after_read(DEVICE_SPI *spi)
{
    // printf("after read\r\n");

    return 0;
}
static int8_t fpga_opt_complete_read(DEVICE_SPI *spi)
{
    // printf("complete read\r\n");

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

    return spi_opt_init(spi, spi_opt);
}
#endif

int8_t device_send_to_fpga_init(uint8_t *device_name)
{
    if (device_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

#ifdef USING_SPI_OPTION_FUNCTION
    int8_t ret = 0;
    ret = device_send_to_fpga_opt_init(device_send_to_fpga_get(), &device_send_to_fpga_opt);
    if (ret != 0)
    {
        printf("device send to fpga init err:%d\r\n", ret);
        return ret;
    }
#endif

    return spi_init(device_send_to_fpga_get(), device_name, SPI_MASTER);
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
        printf("ptr is null\r\n");
        return -1;
    }

    return spi_init(device_recv_from_fpga_get(), device_name, SPI_SLAVE);
}

int8_t device_recv_from_fpga_buffer_init(uint8_t *buf, uint16_t len)
{
    return spi_dma_rx_buf_init(device_recv_from_fpga_get(), buf, len);
}

int8_t device_recv_from_fpga_queue_init(osMessageQueueId_t queue, int8_t (*cb)(void *arg))
{
    return spi_rx_queue_init(device_recv_from_fpga_get(), queue, cb);
}

int8_t device_recv_from_fpga_open(void)
{
    return device_recv_from_fpga_get()->open(device_recv_from_fpga_get());
}