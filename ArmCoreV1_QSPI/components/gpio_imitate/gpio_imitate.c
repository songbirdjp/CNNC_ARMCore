#include "tim.h"
#include "utilities.h"
#include "init_call.h"
#include "shell.h"

static uint32_t *io_buf = NULL; /* io buffer for dma transfer */

static void GpioImitateCpltCallback(DMA_HandleTypeDef *hdma)
{
    if(io_buf != NULL)    
    {
        vPortFree(io_buf);
        io_buf = NULL;
    }
}

static void gpio_imitate_init(void)
{
    MX_TIM7_Init();
    HAL_DMA_RegisterCallback(&hdma_tim7_up, HAL_DMA_XFER_CPLT_CB_ID, GpioImitateCpltCallback);
}
INIT_BOARD_EXPORT(gpio_imitate_init);

static int8_t gpio_array_generate(uint8_t *buf, uint16_t len, uint32_t pin, uint32_t *gen_buf)
{
    if (buf == NULL || len == 0 || gen_buf == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    for (uint16_t i = 0; i < len; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            if (buf[i] & (1 << j))
            {
                gen_buf[i * 8 + j] = pin;
            }
            else
            {
                gen_buf[i * 8 + j] = pin << 16;
            }
        }
    }

    return 0;
}

/*
* @brief  start gpio imitate, lsb first, 8 bit per byte
* @note   only support gpio port A-K, pin 0-15
* @param  gpio_pin: gpio pin string, such as "GPIOD_5"
* @param  buf: buffer data for gpio imitate
* @param  len: buffer data length
* @return 0: success, others: fail
*/
int8_t gpio_imitate_start(uint8_t *gpio_pin, uint8_t *buf, uint16_t len)
{
    if (gpio_pin == NULL || buf == NULL || len == 0)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    HAL_StatusTypeDef hal_status = HAL_OK;
    int8_t ret = 0;
    uint32_t data_len = len * 8;

    /* 1. parse gpio_pin info */
    struct gpio_pin_info gpio = {0};
    ret = gpio_pin_parse(gpio_pin, &gpio);
    if (ret < 0)
    {
        printf("gpio parse err\r\n");
        return -2;
    }

    /* 2. generate array for gpio pin according to buffer data */
    io_buf = (uint32_t *)pvPortMalloc(data_len * sizeof(uint32_t));
    if (io_buf == NULL)
    {
        printf("malloc err\r\n");
        return -3;
    }

    ret = gpio_array_generate(buf, len, gpio.pin, io_buf);
    if (ret != 0)
    {
        printf("gpio array generate err:%d\r\n", ret);
        goto err;
    }

    /* 3. start dma transfer with tim7 period */
    hal_status = HAL_TIM_Base_Stop(&htim7);
    if (hal_status != HAL_OK)
    {
        printf("tim stop err:%d\r\n", hal_status);
        goto err;
    }

    hal_status = HAL_DMA_Start_IT(&hdma_tim7_up, io_buf, &gpio.port->BSRR, data_len);
    if (hal_status != HAL_OK)
    {
        printf("dma start err:%d\r\n", hal_status);
        goto err;
    }

    __HAL_TIM_ENABLE_DMA(&htim7, TIM_DMA_UPDATE);
    hal_status = HAL_TIM_Base_Start(&htim7);
    if (hal_status != HAL_OK)
    {
        printf("tim start err:%d\r\n", hal_status);
        goto err;
    }

err:
    if (io_buf != NULL)
    {
        vPortFree(io_buf);
        io_buf = NULL;
    }

    return ret;
}

#ifdef COMPONENT_TEST
static int8_t gpio_imitate_test(uint8_t argc, uint8_t *argv[])
{
    if (argc != 2)
    {
        printf("usage: gpio_imitate_test <gpio_pin> <buf>\r\n");
        return -1;
    }

    uint8_t buf = 0xAA;

    gpio_imitate_start("GPIOD_5", &buf, 1);
}
MSH_CMD_EXPORT_ALIAS(gpio_imitate_test, gpio_imitate, test gpio imitate);
#endif