/**
 * @file app_search.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _APP_SEARCH_H_
#define _APP_SEARCH_H_

#include "app_dido.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define SEARCH_MIN_TIMEOUT_MS (60000)
int32_t search_timeout_set(uint32_t timeout_tick);
int32_t search_state_machine(app_dido_t *app_dido);

#ifdef __cplusplus
}
#endif

#endif