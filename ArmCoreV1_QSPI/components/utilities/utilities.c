#include "utilities.h"
#include "stdint.h"
#include "stdio.h"
#include "stm32h7xx_hal.h"

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
