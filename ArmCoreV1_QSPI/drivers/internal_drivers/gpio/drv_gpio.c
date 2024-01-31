#include "drv_gpio.h"
#include "utilities.h"

__weak void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}
__weak void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI1_IRQn 1 */

  /* USER CODE END EXTI1_IRQn 1 */
}
__weak void EXTI2_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_IRQn 0 */

  /* USER CODE END EXTI2_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
  /* USER CODE BEGIN EXTI2_IRQn 1 */

  /* USER CODE END EXTI2_IRQn 1 */
}
__weak void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */

  /* USER CODE END EXTI3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
  /* USER CODE BEGIN EXTI3_IRQn 1 */

  /* USER CODE END EXTI3_IRQn 1 */
}
__weak void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */

  /* USER CODE END EXTI4_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
  /* USER CODE BEGIN EXTI4_IRQn 1 */

  /* USER CODE END EXTI4_IRQn 1 */
}
__weak void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}
__weak void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}


static GPIO_TypeDef *gpio_handle_get(uint8_t *str)
{
    uint8_t str_len = strlen(str);

    if (str_len == 0 || str_len > 5) /* gpio port length */
    {
        printf("gpio port err:%s\r\n", str);
        return NULL;
    }

    GPIO_TypeDef *gpio_port = NULL;
    if (!memcmp(str, "GPIOA", str_len))
    {
        gpio_port = GPIOA;
    }
    else if (!memcmp(str, "GPIOB", str_len))
    {
        gpio_port = GPIOB;
    }
    else if (!memcmp(str, "GPIOC", str_len))
    {
        gpio_port = GPIOC;
    }
    else if (!memcmp(str, "GPIOD", str_len))
    {
        gpio_port = GPIOD;
    }
    else if (!memcmp(str, "GPIOE", str_len))
    {
        gpio_port = GPIOE;
    }
    else if (!memcmp(str, "GPIOF", str_len))
    {
        gpio_port = GPIOF;
    }
    else if (!memcmp(str, "GPIOG", str_len))
    {
        gpio_port = GPIOG;
    }
    else if (!memcmp(str, "GPIOH", str_len))
    {
        gpio_port = GPIOH;
    }
    else if (!memcmp(str, "GPIOI", str_len))
    {
        // gpio_port = GPIOI;
    }
    else if (!memcmp(str, "GPIOJ", str_len))
    {
        gpio_port = GPIOJ;
    }
    else if (!memcmp(str, "GPIOK", str_len))
    {
        gpio_port = GPIOK;
    }
    else
    {
        /* do nothing here */
    }

    return gpio_port;
}
static int16_t gpio_pin_index_get(uint8_t *str)
{
    uint8_t str_len = strlen(str);

    if (str_len == 0 || str_len > 2) /* gpio pin num length */
    {
        printf("gpio port err:%s\r\n", str);
        return -1;
    }

    uint16_t pin_num = atoi(str);

    return ((uint16_t)1 << pin_num);
    
}
static IRQn_Type gpio_line_irq_index_get(uint16_t gpio_pin)
{
    IRQn_Type gpio_line_irq = -1;

    if (gpio_pin >= 0 && gpio_pin <= 4)
    {
        gpio_line_irq = gpio_pin + 6;
    }
    else if (gpio_pin >= 5 && gpio_pin <= 9)
    {
        gpio_line_irq = 23;
    }
    else if (gpio_pin >= 10 && gpio_pin <= 15)
    {
        gpio_line_irq = 40;
    }
    else
    {
        /* do nothing */
    }

    return gpio_line_irq;

}

struct gpio_pin_info
{
    GPIO_TypeDef *port;
    int32_t pin;
    IRQn_Type irq_line
};

static int8_t gpio_pin_parse(uint8_t *gpio_pin, struct gpio_pin_info *info)
{
    if (gpio_pin == NULL || info == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    uint8_t *argv[3] = {NULL};
    uint8_t str_buf[10] = {0};

    memcpy(str_buf, gpio_pin, strlen(gpio_pin));
    int8_t ret = split_string(str_buf, '_', argv);

    info->port = gpio_handle_get(argv[0]);
    info->pin = gpio_pin_index_get(argv[1]);
    info->irq_line = gpio_line_irq_index_get(atoi(argv[1]));

    if (info->port == NULL || info->pin == -1 || info->irq_line == -1)
    {
        printf("gpio info err\r\n");
        return -2;
    }

    return ret;
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
    return ret;
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
