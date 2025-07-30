/**
 * @file app_manage.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-04-14
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __APP_MANAGE_H__
#define __APP_MANAGE_H__

#include "stdint.h"
#include "cmsis_os2.h"
typedef struct manage_info
{
    uint32_t status_word;
    uint32_t control_word;
} manage_info_t;

#define bit_set(value, bit) ((value) |= (1 << (bit)))
#define bit_clean(value, bit) ((value) &= ~(1 << (bit)))
#define bit_get(value, bit) ((value) & (1 << (bit)))

/************************************thread flag***************************/
extern osThreadId_t app_rtm_main_threadId;
#define APP_RTM_THREAD_FLAG_ETHERCAT_READY (1 << 0)
#define APP_RTM_THREAD_FLAG_DI_READY (1 << 1)
#define APP_RTM_THREAD_FLAG_DO_READY (1 << 2)
#define APP_RTM_THREAD_FLAG_RTM_OFF_READY (1 << 3)
#define APP_RTM_THREAD_FLAG_ICM_READY (1 << 4)
#define APP_RTM_THREAD_FLAG_BGM_READY (1 << 5)
#define APP_RTM_THREAD_FLAG_QAM_READY (1 << 6)
#define APP_RTM_THREAD_FLAG_BSM_READY (1 << 7)

#define APP_RTM_THREAD_FLAG_ALL (APP_RTM_THREAD_FLAG_ETHERCAT_READY |  \
                                 APP_RTM_THREAD_FLAG_DI_READY |        \
                                 APP_RTM_THREAD_FLAG_DO_READY |        \
                                 APP_RTM_THREAD_FLAG_RTM_OFF_READY | \
                                 APP_RTM_THREAD_FLAG_ICM_READY | \
                                 APP_RTM_THREAD_FLAG_BGM_READY | \
                                 APP_RTM_THREAD_FLAG_QAM_READY)

static inline void app_rtm_thread_flag_set(uint32_t flag)
{
    osThreadFlagsSet(app_rtm_main_threadId, flag);
}
static inline int32_t app_rtm_thread_flag_get(uint32_t timeout)
{
    uint32_t flags = osThreadFlagsWait(APP_RTM_THREAD_FLAG_ALL, osFlagsWaitAll | osFlagsNoClear, timeout);
    if (flags & 0x80000000)
    {
        return -1;
    }
    return flags;
}
#endif /* __APP_MANAGE_H__ */