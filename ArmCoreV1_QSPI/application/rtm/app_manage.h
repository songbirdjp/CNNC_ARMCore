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

#define ETHERCAT_SLAVE_INIT_BIT (0)
#define ETHERCAT_STATE_BIT (1)

/************************************thread flag***************************/
extern osThreadId_t app_rtm_main_threadId;
#define APP_RTM_THREAD_FLAG_ETHERCAT (1 << 0)
#define APP_RTM_THREAD_FLAG_DI (1 << 1)
#define APP_RTM_THREAD_FLAG_DO (1 << 2)
#define APP_RTM_THREAD_FLAG_SERIAL_RX (1 << 3)
#define APP_RTM_THREAD_FLAG_SERIAL_TX (1 << 4)
#define APP_RTM_THREAD_FLAG_INTERLOCK (1 << 5)
#define APP_RTM_THREAD_FLAG_FAULT_DETECT (1 << 6)

#define APP_RTM_THREAD_FLAG_ALL (APP_RTM_THREAD_FLAG_ETHERCAT |  \
                             APP_RTM_THREAD_FLAG_DI |        \
                             APP_RTM_THREAD_FLAG_DO |        \
                             APP_RTM_THREAD_FLAG_SERIAL_RX | \
                             APP_RTM_THREAD_FLAG_SERIAL_TX | \
                             APP_RTM_THREAD_FLAG_FAULT_DETECT)
inline void app_rtm_thread_flag_set(uint32_t flag)
{
    osThreadFlagsSet(app_rtm_main_threadId, flag);
}
#endif /* __APP_MANAGE_H__ */