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