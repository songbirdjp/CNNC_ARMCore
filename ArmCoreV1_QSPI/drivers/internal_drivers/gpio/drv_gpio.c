#include "drv_gpio.h"
#include "utilities.h"

static struct gpio_pin_info gpio_info_irq[16] = {0};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint8_t pin_index = POSITION_VAL(GPIO_Pin);

    if (pin_index >= 16)
    {
        printf("gpio exti err:%d\r\n", pin_index);
        return;
    }

    if (gpio_info_irq[pin_index].callback != NULL)
    {
        gpio_info_irq[pin_index].callback();
    }
}

static int8_t gpio_ioctl(uint8_t *gpio_pin, enum ioctl_cmd cmd, uint32_t arg)
{
    if (gpio_pin == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    int8_t ret = 0;
    struct gpio_pin_info gpio_info = {0};

    ret = gpio_pin_parse(gpio_pin, &gpio_info);
    if (ret < 0)
    {
        printf("gpio pin parse err:%d\r\n", ret);
        return -2;
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    switch (cmd)
    {
    case IOCTL_CMD_MODE_SET:
        GPIO_InitStruct.Pin = gpio_info.pin;
        GPIO_InitStruct.Mode = arg;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(gpio_info.port, &GPIO_InitStruct);
        break;

    case IOCTL_CMD_IRQ_ENABLE_SET:
        if (arg == 0)
        {
            HAL_NVIC_DisableIRQ(gpio_info.irq_line);
        }
        else
        {
            HAL_NVIC_EnableIRQ(gpio_info.irq_line);
        }
        break;

    default:
        break;
    }

    return ret;
}

static int8_t gpio_open(uint8_t *gpio_pin)
{
    return 0;
}

static int8_t gpio_close(uint8_t *gpio_pin)
{
    return 0;
}

static int8_t gpio_write(uint8_t *gpio_pin, uint8_t value)
{
    if (gpio_pin == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    struct gpio_pin_info gpio_info = {0};
    int8_t ret = gpio_pin_parse(gpio_pin, &gpio_info);
    if (ret < 0)
    {
        printf("gpio pin parse err:%d\r\n", ret);
        return -2;
    }

    HAL_GPIO_WritePin(gpio_info.port, gpio_info.pin, value);
    return 0;
}

static GPIO_PinState gpio_read(uint8_t *gpio_pin)
{
    if (gpio_pin == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    struct gpio_pin_info gpio_info = {0};
    int8_t ret = gpio_pin_parse(gpio_pin, &gpio_info);
    if (ret < 0)
    {
        printf("gpio pin parse err:%d\r\n", ret);
        return -2;
    }

    return HAL_GPIO_ReadPin(gpio_info.port, gpio_info.pin);
}

int8_t gpio_common_init(struct drv_gpio *gpio_config)
{
    if (gpio_config == NULL)
    {
        printf("gpio common init failed\r\n");
        return -1;
    }

    gpio_config->open_state = 1;
    gpio_config->open = gpio_open;
    gpio_config->close = gpio_close;
    gpio_config->write = gpio_write;
    gpio_config->read = gpio_read;
    gpio_config->ioctl = gpio_ioctl;

    return 0;
}

int8_t gpio_pin_irq_callback_register(uint8_t *gpio_pin, void (*callback)(void))
{
    if (gpio_pin == NULL || callback == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    int8_t ret = 0;
    struct gpio_pin_info gpio_info = {0};

    ret = gpio_pin_parse(gpio_pin, &gpio_info);
    if (ret < 0)
    {
        printf("gpio pin parse err:%d\r\n", ret);
        return -2;
    }

    uint8_t pin_index = POSITION_VAL(gpio_info.pin);

    if (pin_index >= 16)
    {
        printf("gpio pin index err\r\n");
        return -3;
    }

    gpio_info_irq[pin_index].port = gpio_info.port;
    gpio_info_irq[pin_index].pin = gpio_info.pin;
    gpio_info_irq[pin_index].irq_line = gpio_info.irq_line;
    gpio_info_irq[pin_index].callback = callback;

    return 0;
}

int8_t gpio_pin_irq_callback_unregister(uint8_t *gpio_pin)
{
    if (gpio_pin == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    int8_t ret = 0;
    struct gpio_pin_info gpio_info = {0};

    ret = gpio_pin_parse(gpio_pin, &gpio_info);
    if (ret < 0)
    {
        printf("gpio pin parse err:%d\r\n", ret);
        return -2;
    }

    uint8_t pin_index = POSITION_VAL(gpio_info.pin);

    if (pin_index >= 16)
    {
        printf("gpio pin index err\r\n");
        return -3;
    }

    gpio_info_irq[pin_index].port = gpio_info.port;
    gpio_info_irq[pin_index].pin = gpio_info.pin;
    gpio_info_irq[pin_index].irq_line = gpio_info.irq_line;
    gpio_info_irq[pin_index].callback = NULL;

    return 0;
}
