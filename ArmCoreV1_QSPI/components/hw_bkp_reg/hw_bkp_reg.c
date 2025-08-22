#include "hw_bkp_reg.h"
#include "rtc.h"

int8_t bkp_reg_write(enum bkp_reg reg, uint32_t value)
{
    if (reg >= REG_VALID_MAX)
    {
        return -1;
    }

    HAL_RTCEx_BKUPWrite(&hrtc, reg, value);

    return 0;
}

int8_t bkp_reg_read(enum bkp_reg reg, uint32_t *value)
{
    if (reg >= REG_VALID_MAX)
    {
        return -1;
    }

    *value = HAL_RTCEx_BKUPRead(&hrtc, reg);

    return 0;
}