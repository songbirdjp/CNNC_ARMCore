#include "bootloader.h"
#include "stm32h7xx.h"

/* 
 *  1. Set the vector table offset to the specified value.
 *  2. modify the xxx_FLASH.ld file -> MEMORY item -> flash origin and length. origin addr must multiple of 0x400
 */

int8_t vector_table_offset_set(uint32_t offset)
{
    if (offset % 0x400 != 0)    /* must be a multiple of 0x400 */
    {
        return -1;
    }

    SCB->VTOR = FLASH_BASE | offset;

    return 0;
}

uint32_t vector_table_offset_get(void)
{
    return SCB->VTOR;
}



