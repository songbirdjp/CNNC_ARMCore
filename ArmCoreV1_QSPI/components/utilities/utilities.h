#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

struct system_time
{
    uint32_t systick;   /* systick timer count */
    uint32_t ostick;    /* os count */
};

struct system_time *system_time_get(struct system_time *t);
uint32_t time_diff_us(struct system_time *begin, struct system_time *end);


int8_t split_string(uint8_t *str, char splitter, uint8_t **argv);




#ifdef __cplusplus
}
#endif

#endif