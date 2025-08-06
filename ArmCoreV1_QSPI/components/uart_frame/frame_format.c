/**
 * @file frame_format.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-02-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "frame_format.h"
#include "hw_crc.h"
#include <string.h>
#include "FreeRTOS.h"
#include "ulog.h"

#define FRAME_HEADER_OFFSET 0
#define FRAME_HEADER_LEN 2

#define FRAME_COUNT_OFFSET (FRAME_HEADER_OFFSET + FRAME_HEADER_LEN)
#define FRAME_COUNT_LEN 2

#define FRAME_DATA_LEN_OFFSET (FRAME_COUNT_OFFSET + FRAME_COUNT_LEN)
#define FRAME_DATA_LEN 2

#define FRAME_DATA_OFFSET (FRAME_DATA_LEN_OFFSET + FRAME_DATA_LEN)

#define FRAME_CRC_LEN 4
#define FRAME_EXTRA_LEN (FRAME_DATA_OFFSET + FRAME_CRC_LEN)

#define FRAME_ACTUAL_CRC_LEN (FRAME_COUNT_OFFSET + FRAME_CRC_LEN)
typedef struct uart_frame
{
    uint16_t header;
    uint16_t count;
    uint16_t data_len;
    uint32_t crc;
} __attribute__((packed)) uart_frame_t;

static void frame_format_set_format(frame_format_t *self,
                                    uint16_t header,
                                    crc_type_t crc_type,
                                    uint32_t crc_polynomial,
                                    uint32_t crc_init,
                                    uint32_t crc_xor,
                                    crc_func_t crc_func)
{
    if (self == NULL || crc_func == NULL)
    {
        return;
    }
    self->format.header = header;
    self->format.crc_type = crc_type;
    self->format.crc_polynomial = crc_polynomial;
    self->format.crc_init = crc_init;
    self->format.crc_xor = crc_xor;

    self->format.crc_func = crc_func;
}
static int32_t crc_calculate_callback(void *self, uint32_t *crc, uint8_t *data, uint32_t size)
{
    if (self == NULL || data == NULL || size == 0 || crc == NULL)
    {
        return -1;
    }
    /*TODO:根据self调用相应的硬件crc计算函数*/
    *crc = hardware_crc_calculate(CRC32, data, size) ^ 0xFFFFFFFF;
    return 0;
}
static void frame_format_timer_callback(void *arg)
{
    frame_format_t *self = (frame_format_t *)arg;
    osStatus_t status = osOK;
    int32_t ret = 0;
    if (self == NULL)
    {
        return;
    }
    if (self->tx_retry_count >= self->retry_count)
    {
        self->frame_format_statistics.send_error_count++;
        return;
    }

    ret = self->send_func(self->tx_buffer, self->tx_retry_data_len, 1000, self->send_arg);
    if (ret != 0)
    {
        return;
    }

    if (self->tx_retry_count == 0)
    {
        self->frame_format_statistics.send_retry_count++;
    }

    self->tx_retry_count++;
    status = osTimerStart(self->osTimerId, self->timeout_ms / portTICK_RATE_MS);
    if (status != osOK)
    {
        return;
    }

#if 0
    LOG_I("send repeat count: %d\r\n", self->tx_retry_count);
    LOG_I("recv_response_count: %#.4x\r\n", self->recv_response_count);
    for (uint8_t i = 0; i < self->tx_retry_data_len; i++)
    {
        LOG_I("%.2x ", self->tx_buffer[i]);
    }
    LOG_I("\r\n");
#endif

}

int32_t frame_format_init(frame_format_t *self,
                          bool crc_check_state,
                          uint8_t retry_count,
                          uint32_t timeout_ms,
                          uint32_t rx_buffer_size,
                          uint32_t tx_buffer_size)
{
    if (self == NULL || rx_buffer_size == 0 || tx_buffer_size == 0)
    {
        return -1;
    }
    memset(self, 0, sizeof(frame_format_t));
    self->osTimerId = osTimerNew(frame_format_timer_callback,
                                 osTimerOnce,
                                 (void *)self,
                                 NULL);
    if (self->osTimerId == NULL)
    {
        return -2;
    }
    self->osSemaphoreId = osSemaphoreNew(1, 0, NULL);

    self->rx_buffer = pvPortMalloc(rx_buffer_size);
    if (self->rx_buffer == NULL)
    {
        return -3;
    }
    self->rx_buffer_size = rx_buffer_size;

    self->tx_buffer = pvPortMalloc(tx_buffer_size);
    if (self->tx_buffer == NULL)
    {
        return -4;
    }
    self->tx_response_buffer = pvPortMalloc(tx_buffer_size);
    if (self->tx_response_buffer == NULL)
    {
        return -5;
    }
    self->tx_buffer_size = tx_buffer_size;
    const osMutexAttr_t tx_retry_mutex_attr =
        {
            .name = "uart_protocol_tx_retry_mutex",
            .attr_bits = osMutexPrioInherit | osMutexRecursive,
            .cb_mem = NULL,
            .cb_size = 0,
        };
    self->tx_mutex = osMutexNew(&tx_retry_mutex_attr);
    if (self->tx_mutex == NULL)
    {
        return -6;
    }
    const osMutexAttr_t tx_mutex_attr =
        {
            .name = "uart_protocol_tx_mutex",
            .attr_bits = osMutexPrioInherit | osMutexRecursive,
            .cb_mem = NULL,
            .cb_size = 0,
        };
    self->tx_response_mutex = osMutexNew(&tx_mutex_attr);
    if (self->tx_response_mutex == NULL)
    {
        return -7;
    }
    frame_format_set_format(self,
                            0xAA55,
                            CRC_TYPE_CRC_32,
                            0x04C11DB7,
                            0xFFFFFFFF,
                            0xFFFFFFFF,
                            crc_calculate_callback);
    self->crc_check_state = crc_check_state;
    self->retry_count = retry_count;
    self->timeout_ms = timeout_ms;
    return 0;
}
int32_t frame_format_send_func_register(frame_format_t *self, uart_xfer_func_t send_func, void *arg)
{
    if (self == NULL || send_func == NULL)
    {
        return -1;
    }
    self->send_func = send_func;
    self->send_arg = arg;
    return 0;
}
int32_t frame_format_recv_func_register(frame_format_t *self, uart_xfer_func_t recv_func, void *arg)
{
    if (self == NULL || recv_func == NULL)
    {
        return -1;
    }
    self->recv_func = recv_func;
    self->recv_arg = arg;
    return 0;
}
int32_t frame_format_crc_error_handle_func_register(frame_format_t *self, uart_crc_error_handle_func_t crc_error_handle_func, void *arg)
{
    if (self == NULL)
    {
        return -1;
    }
    self->crc_error_handle_func = crc_error_handle_func;
    self->crc_error_handle_arg = arg;
    return 0;
}
int32_t frame_format_retry_judge_handle_func_register(frame_format_t *self, uart_crc_error_handle_func_t retry_judge_handle_func, void *arg)
{
    if (self == NULL)
    {
        return -1;
    }
    self->retry_judge_handle_func = retry_judge_handle_func;
    self->retry_judge_handle_arg = arg;
    return 0;
}
int32_t frame_format_send(frame_format_t *self, uint8_t *data, uint16_t data_len, uint32_t timeout)
{
    int32_t ret = 0;
    uint32_t crc = 0;
    osStatus_t status = osOK;
    if (self == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }

    if (data[4] & 0x80) // send a response frame
    {
        status = osMutexAcquire(self->tx_response_mutex, timeout);
        if (status != osOK)
        {
            return -2;
        }
        self->frame_format_statistics.send_total_count++;

        memcpy(self->tx_response_buffer + FRAME_DATA_OFFSET, data, data_len);
        *(uint16_t *)&(self->tx_response_buffer[FRAME_HEADER_OFFSET]) = self->format.header;
        *(uint16_t *)&(self->tx_response_buffer[FRAME_COUNT_OFFSET]) = self->recv_count;
        *(uint16_t *)&(self->tx_response_buffer[FRAME_DATA_LEN_OFFSET]) = data_len;

        self->tx_data_len = FRAME_EXTRA_LEN + data_len;
        ret = self->format.crc_func(&self->format, &crc, self->tx_response_buffer + FRAME_COUNT_OFFSET, self->tx_data_len - FRAME_ACTUAL_CRC_LEN);
        if (ret != 0)
        {
            ret = -3;
            goto err;
        }
        *(uint32_t *)&(self->tx_response_buffer[FRAME_DATA_OFFSET + data_len]) = crc;

        ret = self->send_func(self->tx_response_buffer, data_len + FRAME_EXTRA_LEN, timeout, self->send_arg);
        if (ret != 0)
        {
            ret = -4;
            goto err;
        }

    err:
        osMutexRelease(self->tx_response_mutex);
    }
    else
    {
        status = osMutexAcquire(self->tx_mutex, timeout);
        if (status != osOK)
        {
            return -5;
        }
        self->frame_format_statistics.send_total_count++;

        memcpy(self->tx_buffer + FRAME_DATA_OFFSET, data, data_len);
        *(uint16_t *)&(self->tx_buffer[FRAME_HEADER_OFFSET]) = self->format.header;
        *(uint16_t *)&(self->tx_buffer[FRAME_COUNT_OFFSET]) = self->send_count;
        *(uint16_t *)&(self->tx_buffer[FRAME_DATA_LEN_OFFSET]) = data_len;
        self->tx_retry_data_len = FRAME_EXTRA_LEN + data_len;
        ret = self->format.crc_func(&self->format, &crc, self->tx_buffer + FRAME_COUNT_OFFSET, self->tx_retry_data_len - FRAME_ACTUAL_CRC_LEN);
        if (ret != 0)
        {
            ret = -6;
            goto error;
        }
        *(uint32_t *)&(self->tx_buffer[FRAME_DATA_OFFSET + data_len]) = crc;

        self->recv_response_count = *(uint16_t *)&(self->tx_buffer[FRAME_COUNT_OFFSET]);
        ret = self->send_func(self->tx_buffer, data_len + FRAME_EXTRA_LEN, timeout, self->send_arg);
        if (ret != 0)
        {
            ret = -7;
            goto error;
        }

        if (data[0] & 0x01) // send a request frame
        {
            if (self->retry_count > 0)
            {
                self->tx_retry_count = 0;
                status = osTimerStart(self->osTimerId, self->timeout_ms / portTICK_RATE_MS);
                if (status != osOK)
                {
                    ret = -8;
                    goto error;
                }
            }
            self->semaphore_lock++;
            status = osSemaphoreAcquire(self->osSemaphoreId, timeout); /* TODO: 此处未接收到反馈包，则一直阻塞等待直至超时，在等待超时的过程中是可以重发的。可以使用队列将返回状态给到应用层 */
            self->semaphore_lock--;
            if (status != osOK)
            {
                self->send_count++;
                ret = -9;
                goto error;
            }
            /* TODO： 收到应答帧后，此处可以返回应答状态给到应用层，在应用层可以选择是否等待该状态 */
            if (osTimerIsRunning(self->osTimerId) != 0)
            {
                status = osTimerStop(self->osTimerId);
                if (status != osOK)
                {
                    self->send_count++;
                    ret = -10;
                    goto error;
                }
            }
        }

        self->send_count++;

    error:
        osMutexRelease(self->tx_mutex);
    }
    return ret;
}

int32_t frame_format_recv(frame_format_t *self, uint8_t *data, uint16_t *data_len, uint32_t timeout)
{
    uint32_t crc = 0;
    int32_t ret = 0;
    osStatus_t status = osOK;
    uint32_t recv_len = 0;
    if (self == NULL || data == NULL || data_len == NULL)
    {
        return -1;
    }
wait_recv:
    ret = self->recv_func(self->rx_buffer, 0xFFFF, timeout, self->recv_arg);
    if (ret != 0)
    {
        return -2;
    }
    recv_len = *(uint16_t *)&(self->rx_buffer[self->rx_buffer_size - sizeof(uint16_t)]);

    if (recv_len < sizeof(uart_frame_t))
    {
        return -3;
    }

    self->frame_format_statistics.recv_total_count++;

    if (self->crc_check_state == true)
    {
        self->format.crc_func(&self->format, &crc, self->rx_buffer + FRAME_COUNT_OFFSET, recv_len - FRAME_ACTUAL_CRC_LEN);
        if (crc != *(uint32_t *)&(self->rx_buffer[recv_len - FRAME_CRC_LEN]))
        {
            self->frame_format_statistics.recv_crc_error_count++;

            if (self->crc_error_handle_func != NULL)
            {
                ret = self->crc_error_handle_func(self->rx_buffer, recv_len, self->crc_error_handle_arg);
                if (ret != 0)
                {
                    return -4;
                }
                goto wait_recv;
            }
            return -5;
        }
    }
    if (self->retry_judge_handle_func != NULL)
    {
        if (self->retry_judge_handle_func(self->rx_buffer, recv_len, self->retry_judge_handle_arg) == 0) // 接收到重发帧
        {
            // 接收到CRC错误帧，重试
            if (osTimerIsRunning(self->osTimerId) != 0)
            {
                status = osTimerStop(self->osTimerId);
                if (status != osOK)
                {
                    self->send_count++;
                    return -6;
                }
            }
            if (self->tx_retry_count >= self->retry_count)
            {
                self->frame_format_statistics.send_error_count++;
                 goto wait_recv;
            }

            status = osTimerStart(self->osTimerId, self->timeout_ms / portTICK_RATE_MS);
            if (status != osOK)
            {
                return -7;
            }

            ret = self->send_func(self->tx_buffer, self->tx_retry_data_len, 100, self->send_arg);
            if (ret != 0)
            {
                return -8;
            }

            self->tx_retry_count++;

            if (self->tx_retry_count == 0)
            {
                self->frame_format_statistics.send_retry_count++;
            }
            goto wait_recv;
        }
    }
    *data_len = recv_len - FRAME_EXTRA_LEN;
    memcpy(data, self->rx_buffer + FRAME_DATA_OFFSET, recv_len - FRAME_EXTRA_LEN);

#if 0
#include "ulog.h"
    LOG_I("recv: ");
    for (int i = 0; i < recv_len; i++)
    {
        LOG_I("%02X ", self->rx_buffer[i]);
    }
    LOG_I("\r\n");
#endif

    if (self->rx_buffer[FRAME_DATA_OFFSET + 4] & 0x80) // receive a response frame
    {
        if (self->semaphore_lock)
        {
            if (self->recv_response_count != ((uart_frame_t *)self->rx_buffer)->count)
            {
				LOG_E("recv response count error: %d  %d\r\n", self->recv_response_count, ((uart_frame_t *)self->rx_buffer)->count);
                return -9;
            }
            status = osSemaphoreRelease(self->osSemaphoreId);
            if (status != osOK)
            {
                return -10;
            }
        }
        else
        {
            LOG_E("recv response without lock\r\n");
        }
    }
    else
    {
        uint16_t count = *(uint16_t *)(self->rx_buffer + FRAME_COUNT_OFFSET);

        if (count > self->recv_count)
        {
            self->frame_format_statistics.recv_lose_count += count - self->recv_count - 1;
        }
        else if (count < self->recv_count)
        {
            self->frame_format_statistics.recv_lose_count += 0xFFFF - self->recv_count + count;
        }

        self->recv_count = count;
    }
    return 0;
}

int32_t frame_format_get_statistics(frame_format_t *self, frame_format_statistics_t *statistics)
{
    if (self == NULL || statistics == NULL)
    {
        return -1;
    }
    self->frame_format_statistics.send_lose_rate =
        (float)self->frame_format_statistics.send_error_count / (float)self->frame_format_statistics.send_total_count;

    self->frame_format_statistics.recv_lose_rate =
        (float)self->frame_format_statistics.recv_lose_count /
        (float)(self->frame_format_statistics.recv_total_count + self->frame_format_statistics.recv_lose_count);

    memcpy(statistics, &self->frame_format_statistics, sizeof(frame_format_statistics_t));
    return 0;
}

void frame_format_clear_statistics(frame_format_t *self)
{
    if (self == NULL)
    {
        return;
    }
    self->frame_format_statistics.send_total_count = 0;
    self->frame_format_statistics.send_retry_count = 0;
    self->frame_format_statistics.send_error_count = 0;
    self->frame_format_statistics.send_lose_rate = 0.0;

    self->frame_format_statistics.recv_total_count = 0;
    self->frame_format_statistics.recv_lose_count = 0;
    self->frame_format_statistics.recv_crc_error_count = 0;
    self->frame_format_statistics.recv_lose_rate = 0.0;
}
