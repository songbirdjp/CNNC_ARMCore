#include "stdint.h"
#include "stdio.h"

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