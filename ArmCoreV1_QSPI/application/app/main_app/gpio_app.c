#include "gpio_app.h"
#include "gpio_port.h"

int8_t wdt_reset_set(uint8_t en)
{
    return gpio_common_get()->write("GPIOD_1", (en == 0) ? 1 : 0);
}

int8_t dose_hv_enable_set(uint8_t en)
{
    return gpio_common_get()->write("GPIOB_8", (en == 0) ? 0 : 1);
}

int8_t dose_trigger_out_set(uint8_t en)
{
    return gpio_common_get()->write("GPIOB_9", (en == 0) ? 0 : 1);
}

#ifndef GPIO_TEST
#include "shell.h"
#include "ulog.h"
static int8_t gpio_trigger_out_set(uint8_t argc, char **argv)
{
    return dose_trigger_out_set(atoi(argv[1]));
}
MSH_CMD_EXPORT_ALIAS(gpio_trigger_out_set, gpio_trigger_out_set, set trigger out gpio);

static int8_t gpio_trigger_out_get(uint8_t argc, char **argv)
{
    LOG_I("trigger out gpio: %d\r\n", gpio_common_get()->read("GPIOB_9"));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(gpio_trigger_out_get, gpio_trigger_out_get, get trigger out gpio);

static int8_t gpio_hv_enable_set(uint8_t argc, char **argv)
{
    return dose_hv_enable_set(atoi(argv[1]));
}
MSH_CMD_EXPORT_ALIAS(gpio_hv_enable_set, gpio_hv_enable_set, set hv enable gpio);

static int8_t gpio_hv_enable_get(uint8_t argc, char **argv)
{
    LOG_I("hv enable gpio: %d\r\n", gpio_common_get()->read("GPIOB_8"));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(gpio_hv_enable_get, gpio_hv_enable_get, get hv enable gpio);

#endif