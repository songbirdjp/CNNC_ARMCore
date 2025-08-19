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

/************************************thread flag***************************/
extern osThreadId_t app_rtm_main_threadId;
#define APP_RTM_THREAD_FLAG_ETHERCAT_READY (1 << 0)
#define APP_RTM_THREAD_FLAG_DI_READY (1 << 1)
#define APP_RTM_THREAD_FLAG_DO_READY (1 << 2)
#define APP_RTM_THREAD_FLAG_RTM_ON_READY (1 << 3)
#define APP_RTM_THREAD_FLAG_PSM_READY (1 << 4)
// #define APP_RTM_THREAD_FLAG_GMM_READY (1 << 5)
#define APP_RTM_THREAD_FLAG_FKP_READY (1 << 6)
#define APP_RTM_THREAD_FLAG_CPG_READY (1 << 7)

#define APP_RTM_THREAD_FLAG_ALL (APP_RTM_THREAD_FLAG_ETHERCAT_READY |  \
                                 APP_RTM_THREAD_FLAG_DI_READY |        \
                                 APP_RTM_THREAD_FLAG_DO_READY |        \
                                 APP_RTM_THREAD_FLAG_RTM_ON_READY | \
                                 APP_RTM_THREAD_FLAG_PSM_READY | \
                                 APP_RTM_THREAD_FLAG_FKP_READY | \
                                 APP_RTM_THREAD_FLAG_CPG_READY)

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