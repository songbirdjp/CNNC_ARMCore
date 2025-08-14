#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALIGN(size, align)          (((size) + (align) - 1) & ~((align) - 1))


struct system_time
{
    uint32_t systick;   /* systick timer count */
    uint32_t ostick;    /* os count */
};

struct system_time *system_time_get(struct system_time *t);
uint32_t time_diff_us(struct system_time *begin, struct system_time *end);

int8_t split_string(uint8_t *str, char splitter, uint8_t **argv);


struct gpio_pin_info
{
    GPIO_TypeDef *port;
    int32_t pin;
    IRQn_Type irq_line;
    void (*callback)(void)
};

int8_t gpio_pin_parse(uint8_t *gpio_pin, struct gpio_pin_info *info);

#ifdef __cplusplus
}
#endif

#endif