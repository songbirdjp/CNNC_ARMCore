#include "gpio_port.h"
#include "init_call.h"

static struct drv_gpio gpio_common = {0};

struct drv_gpio *gpio_common_get(void)
{
    return &gpio_common;
}

static int8_t gpio_port_init(void)
{
    return gpio_common_init(gpio_common_get());
}
INIT_DEVICE_EXPORT(gpio_port_init);

#ifdef DRV_TEST
#include "shell.h"
static void drv_gpio_test(void)
{
    printf("GPIOB ODR:%#x\r\n", GPIOB->ODR);
    printf("GPIOB IDR:%#x\r\n", GPIOB->IDR);
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_0"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_4"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_8"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_12"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_13"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_14"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_15"));


    gpio_common_get()->write("GPIOB_0", GPIO_PIN_SET);
    gpio_common_get()->write("GPIOB_4", GPIO_PIN_SET);
    gpio_common_get()->write("GPIOB_8", GPIO_PIN_SET);
    gpio_common_get()->write("GPIOB_12", GPIO_PIN_SET);

    printf("GPIOB ODR:%#x\r\n", GPIOB->ODR);
    printf("GPIOB IDR:%#x\r\n", GPIOB->IDR);
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_0"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_4"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_8"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_12"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_13"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_14"));
    printf("read:%#x\r\n", gpio_common_get()->read("GPIOB_15"));
}
MSH_CMD_EXPORT_ALIAS(drv_gpio_test, gpio_test, test gpio driver);
#endif