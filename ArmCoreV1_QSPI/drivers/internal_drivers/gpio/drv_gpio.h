#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME_LENGTH      16

enum ioctl_cmd
{
    IOCTL_CMD_MODE_SET = 0,
    IOCTL_CMD_IRQ_ENABLE_SET = 1,
    IOCTL_CMD_MAX
};

struct drv_gpio
{
    // uint8_t name[DEVICE_NAME_LENGTH];
    uint8_t open_state;

    int8_t (*open)(uint8_t *gpio_pin);
    int8_t (*close)(uint8_t *gpio_pin);
    int8_t (*write)(uint8_t *gpio_pin, uint8_t value);
    GPIO_PinState (*read)(uint8_t *gpio_pin);
    int8_t (*ioctl)(uint8_t *gpio_pin, enum ioctl_cmd cmd, uint32_t arg);
};

int8_t gpio_common_init(struct drv_gpio *gpio_config);
int8_t gpio_pin_irq_callback_register(uint8_t *gpio_pin, void (*callback)(void));
int8_t gpio_pin_irq_callback_unregister(uint8_t *gpio_pin);

#ifdef __cplusplus
}
#endif

#endif
