#ifndef __BGM_ERROR_H__
#define __BGM_ERROR_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum bgm_error_type
{
    BGM_ERROR_PLAN_DATA = 0,
    BGM_ERROR_ALL,
};

int8_t bgm_error_info_clear(void);
int32_t bgm_error_info_get(enum bgm_error_type type);
int8_t bgm_error_info_set(enum bgm_error_type type, uint8_t value);


#ifdef __cplusplus
}
#endif

#endif /* __BGM_ERROR_H__ */