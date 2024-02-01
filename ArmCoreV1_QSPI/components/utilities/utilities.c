#include "utilities.h"

int8_t split_string(uint8_t *str, char splitter, uint8_t **argv)
{
    if (str == NULL || argv == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    uint8_t idx = 0, argc = 0;
    uint8_t len = strlen(str);


    while (idx < len)
    {
        while (str[idx] == splitter && idx < len)
        {
            str[idx] = '\0';
            idx++;
        }

        if (idx >= len)
        {
            break;
        }
        else
        {
            argv[argc] = &str[idx];
            argc++;

            while (str[idx] != splitter && idx < len)
            {
                idx++;
            }
        }
    }

    return argc;
}

struct system_time *system_time_get(struct system_time *t)
{
    t->systick = SysTick->VAL;
    t->ostick = osKernelGetTickCount();

    return t;
}

uint32_t time_diff_us(struct system_time *begin, struct system_time *end)
{
    uint64_t ostick_diff = ((uint64_t)end->ostick + UINT32_MAX - begin->ostick) % UINT32_MAX;
    uint32_t diff_systick = (ostick_diff * (SysTick->LOAD + 1) + begin->systick - end->systick);

    return diff_systick / (HAL_RCC_GetSysClockFreq() / 1000000);
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
int8_t gpio_pin_parse(uint8_t *gpio_pin, struct gpio_pin_info *info)
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
