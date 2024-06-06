#include <stdint.h>
#include "init_call.h"

void system_fun_init(void)
{
    const struct init_desc *desc;
    int result = 0;
    uint32_t fun_num = 1;

    extern uint32_t __init_call_start;
    extern uint32_t __init_call_end;

    printf("\r\n########## function initialize begin ##########\r\n");

    for (desc = &__init_call_start; desc < &__init_call_end; desc++, fun_num++)
    {
        result = desc->init_fn();
        printf("done_%-2u [%-32s %-2d]\r\n", fun_num, desc->fn_name, result);
    }

    printf("########## function initialize end   ##########\r\n\r\n");
}