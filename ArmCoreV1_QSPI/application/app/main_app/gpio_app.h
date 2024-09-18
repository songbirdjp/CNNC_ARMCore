#ifndef __GPIO_APP_H__
#define __GPIO_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int8_t wdt_reset_set(uint8_t en);
int8_t dose_hv_enable_set(uint8_t en);
int8_t dose_trigger_out_set(uint8_t en);

#ifdef __cplusplus
}
#endif


#endif /* __GPIO_APP_H__ */