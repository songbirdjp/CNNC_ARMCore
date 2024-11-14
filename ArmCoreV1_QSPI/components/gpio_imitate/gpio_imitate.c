#include "tim.h"
#include "utilities.h"
#include "init_call.h"
#include "shell.h"
#include "cmsis_os2.h"

extern DMA_HandleTypeDef hdma_tim7_up;

static uint32_t *io_buf = NULL; /* io buffer for dma transfer */
static osMutexId_t io_mutex = NULL;
static osEventFlagsId_t io_event = NULL;

#define GPIO_IMITATE_SEND_SUCCEED_EVENT     (1<<0)

static void GpioImitateCpltCallback(DMA_HandleTypeDef *hdma)
{
    osEventFlagsSet(io_event, GPIO_IMITATE_SEND_SUCCEED_EVENT);
}

static int8_t gpio_imitate_init(void)
{
    MX_TIM7_Init();
    HAL_DMA_RegisterCallback(&hdma_tim7_up, HAL_DMA_XFER_CPLT_CB_ID, GpioImitateCpltCallback);

    /* os mutex and event flag init */
    osMutexAttr_t mutex_attributes = {
    .name = "gpio_send_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    io_mutex = osMutexNew(&mutex_attributes);
    if (io_mutex == NULL)
    {
        return -1;
    }

    osEventFlagsAttr_t gpio_send_event_attributes = {
    .name = "gpio_send_event"
    };
    io_event = osEventFlagsNew(&gpio_send_event_attributes);
    if (io_event == NULL)
    {
        return -2;
    }

    return 0;
}
INIT_APP_EXPORT(gpio_imitate_init);

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

    HAL_StatusTypeDef status = HAL_OK;
    int8_t ret = 0;
    uint32_t data_len = len * 8;

    osMutexAcquire(io_mutex, osWaitForever);

    /* 1. parse gpio_pin info */
    struct gpio_pin_info gpio = {0};
    ret = gpio_pin_parse(gpio_pin, &gpio);
    if (ret < 0)
    {
        printf("gpio parse err\r\n");
        ret = -2;
        goto out;
    }

    /* 2. generate array for gpio pin according to buffer data */
    io_buf = (uint32_t *)pvPortMalloc(data_len * sizeof(uint32_t));
    if (io_buf == NULL)
    {
        printf("malloc err\r\n");
        ret = -3;
        goto out;
    }

    ret = gpio_array_generate(buf, len, gpio.pin, io_buf);
    if (ret != 0)
    {
        printf("gpio array generate err:%d\r\n", ret);
        goto err;
    }

#if 0
    for(uint16_t i = 0; i < data_len; i++)
    {
        printf("io_buf[%d]:%#.8x\r\n", i, io_buf[i]);
    }
    printf("\r\n");
#endif

    /* 3. start dma transfer with tim7 period */
    /* TODO: must wait for dma transfer complete, but not implement here, add mutex if necessary */
    status = HAL_TIM_Base_Stop(&htim7);
    if (status != HAL_OK)
    {
        printf("tim stop err:%d\r\n", status);
        ret = -4;
        goto err;
    }

    status = HAL_DMA_Start_IT(&hdma_tim7_up, io_buf, &gpio.port->BSRR, data_len);
    if (status != HAL_OK)
    {
        printf("dma start err:%d\r\n", status);
        ret = -5;
        goto err;
    }

    __HAL_TIM_ENABLE_DMA(&htim7, TIM_DMA_UPDATE);
    status = HAL_TIM_Base_Start(&htim7);
    if (status != HAL_OK)
    {
        printf("tim start err:%d\r\n", status);
        ret = -6;
        goto err;
    }

    uint32_t ret_val = osEventFlagsWait(io_event, GPIO_IMITATE_SEND_SUCCEED_EVENT, osFlagsWaitAny, osWaitForever);
    if (ret_val != GPIO_IMITATE_SEND_SUCCEED_EVENT)
    {
        printf("gpio imitate wait event flag err: %#.8x\r\n", ret_val);
        ret = -7;
        goto err;
    }

err:
    if (io_buf != NULL)
    {
        vPortFree(io_buf);
        io_buf = NULL;
    }

out:
    osMutexRelease(io_mutex);

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

    gpio_imitate_start("GPIOD_7", &buf, 1);
}
MSH_CMD_EXPORT_ALIAS(gpio_imitate_test, gpio_imitate, test gpio imitate);
#endif