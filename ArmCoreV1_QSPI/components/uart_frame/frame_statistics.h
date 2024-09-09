#ifndef __FRAME_STATISTICS_H__
#define __FRAME_STATISTICS_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USING_FRAME_STATISTICS

#ifdef USING_FRAME_STATISTICS
#define DEVICE_IS_MASTER    0
#define DEVICE_IS_SLAVE_PERIODIC    0
#define FRAME_IS_ACK_MODE   1
// #define FRAME_TIMEOUT_THREAD_DETECT
#endif

struct frame_statistics 
{
#if DEVICE_IS_MASTER || DEVICE_IS_SLAVE_PERIODIC
    uint16_t timeout;   /* unit: os ticks */
    uint16_t timeout_cnt;
#endif

    uint16_t crc_err_cnt;
    uint16_t cmd_invalid_cnt;
    uint16_t lost_cnt;

#if DEVICE_IS_MASTER && FRAME_IS_ACK_MODE
    uint16_t max_time_cnt;  /* from send to recv used time */
    uint16_t min_time_cnt;
    uint64_t total_time_cnt;
#endif

    struct 
    {
        uint16_t bytes;
        union
        {
            uint16_t send : 1;
            uint16_t reserved : 15;
        }bits;
    }flags;

    uint32_t ostick_begin;

    uint64_t send_cnt;
    uint64_t recv_cnt;
    uint64_t ack_cnt;

#ifdef FRAME_TIMEOUT_THREAD_DETECT
    osMutexId_t mutex
#endif
};

enum frame_update_type
{
    FRAME_UPDATE_CNT,
    FRAME_UPDATE_CRC,
    FRAME_UPDATE_CMD,
    FRAME_UPDATE_LOST,
    FRAME_UPDATE_TIMEOUT,

    FRAME_UPDATE_MAX
};

int8_t frame_stats_send_update(struct frame_statistics *stats);
int8_t frame_stats_recv_update(struct frame_statistics *stats, enum frame_update_type type, uint16_t value);

#ifdef FRAME_TIMEOUT_THREAD_DETECT
int8_t frame_stats_polling(void *argument);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FRAME_STATISTICS_H__ */