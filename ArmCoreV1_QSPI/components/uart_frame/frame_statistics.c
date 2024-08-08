#include "frame_statistics.h"

int8_t frame_stats_recv_update(struct frame_statistics *stats, enum frame_update_type type, uint16_t value)
{
    if (type >= FRAME_UPDATE_MAX)
    {
        return -1;
    }

    int8_t ret = 0;
    uint32_t ostick_now = osKernelGetTickCount();
    uint32_t ostick_used = 0;

#ifdef FRAME_TIMEOUT_THREAD_DETECT
    osMutexAcquire(stats->mutex, osWaitForever);
#endif

    switch (type)
    {
    case FRAME_UPDATE_CNT:
        stats->recv_cnt++;

#if DEVICE_IS_MASTER && FRAME_IS_ACK_MODE
        ostick_used = (ostick_now + UINT32_MAX - stats->ostick_begin) % UINT32_MAX;
        stats->total_time_cnt += ostick_used;

        stats->max_time_cnt = (stats->max_time_cnt < ostick_used) ? ostick_used : stats->max_time_cnt;
        stats->min_time_cnt = (stats->min_time_cnt > ostick_used) ? ostick_used : stats->min_time_cnt;
#endif
        break;

    case FRAME_UPDATE_CRC:
        stats->crc_err_cnt += value ? 0 : 1;
        break;

    case FRAME_UPDATE_CMD:
        stats->cmd_invalid_cnt += value ? 0 : 1;
        break;

    case FRAME_UPDATE_LOST:
        if (value != (uint16_t)stats->recv_cnt)
        {
            stats->lost_cnt = ((uint32_t)value + UINT16_MAX - (uint16_t)stats->recv_cnt) % UINT16_MAX;
        }
        break;

    case FRAME_UPDATE_TIMEOUT:
#if DEVICE_IS_MASTER || DEVICE_IS_SLAVE_PERIODIC
        stats->timeout_cnt += value ? 0 : 1;
#endif
        break;

    default:
        ret = -2;
        goto exit;
        break;
    }

exit:

#ifdef FRAME_TIMEOUT_THREAD_DETECT
    osMutexRelease(stats->mutex);
#endif

    return ret;
}

int8_t frame_stats_send_update(struct frame_statistics *stats)
{
#ifdef FRAME_TIMEOUT_THREAD_DETECT
    osMutexAcquire(stats->mutex, osWaitForever);
#endif

    stats->send_cnt++;

#if DEVICE_IS_MASTER && FRAME_IS_ACK_MODE
    stats->ostick_begin = osKernelGetTickCount();
#endif

#ifdef FRAME_TIMEOUT_THREAD_DETECT
    osMutexRelease(stats->mutex);
#endif

    return 0;
}


#ifdef FRAME_TIMEOUT_THREAD_DETECT
int8_t frame_stats_polling(void *argument)
{
#if DEVICE_IS_MASTER || DEVICE_IS_SLAVE_PERIODIC
    uint32_t ostick_end = 0, ostick_now = 0;
    struct frame_statistics *stats = (struct frame_statistics *)argument;

    osMutexAcquire(stats->mutex, osWaitForever);

    ostick_end = stats->ostick_begin + stats->timeout;
    ostick_now = osKernelGetTickCount();
    
    if (ostick_now >= ostick_end)
    {
        stats->timeout_cnt++;
        stats->ostick_begin = ostick_now;
    }

    osMutexRelease(stats->mutex);
#endif

    return 0;
}
#endif





