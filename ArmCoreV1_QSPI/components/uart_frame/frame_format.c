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

#define FRAME_HEADER_OFFSET 0
#define FRAME_HEADER_LEN 2

#define FRAME_COUNT_OFFSET (FRAME_HEADER_OFFSET + FRAME_HEADER_LEN)
#define FRAME_COUNT_LEN 2

#define FRAME_DATA_LEN_OFFSET (FRAME_COUNT_OFFSET + FRAME_COUNT_LEN)
#define FRAME_DATA_LEN 2

#define FRAME_DATA_OFFSET (FRAME_DATA_LEN_OFFSET + FRAME_DATA_LEN)

#define FRAME_CRC_LEN 4
#define FRAME_EXTRA_LEN (FRAME_DATA_OFFSET + FRAME_CRC_LEN)
typedef struct uart_frame
{
    uint16_t header;
    uint16_t count;
    uint16_t data_len;
    uint8_t data;
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
    *crc = hardware_crc_calculate(CRC32, data, size);
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
        self->frame_format_statistics.send_lose_rate =
            (float)self->frame_format_statistics.send_error_count / (float)self->frame_format_statistics.send_total_count;
        return;
    }

    ret = self->send_func(self->tx_buffer, self->tx_data_len, 1000, self->send_arg);
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

    self->rx_buffer = pvPortMalloc(rx_buffer_size + FRAME_EXTRA_LEN);
    if (self->rx_buffer == NULL)
    {
        return -3;
    }
    self->rx_buffer_size = rx_buffer_size;

    self->tx_buffer = pvPortMalloc(tx_buffer_size + FRAME_EXTRA_LEN);
    if (self->tx_buffer == NULL)
    {
        return -4;
    }
    self->tx_buffer_size = tx_buffer_size;

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
    self->uart_state = UART_STATE_NORMAL;
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
int32_t frame_format_send(frame_format_t *self, uint8_t *data, uint16_t data_len, uint32_t timeout)
{
    int32_t ret = 0;
    uint32_t crc = 0;
    osStatus_t status = osOK;
    if (self == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }

    self->frame_format_statistics.send_total_count++;
    self->frame_format_statistics.send_lose_rate =
        (float)self->frame_format_statistics.send_error_count / (float)self->frame_format_statistics.send_total_count;

    memcpy(self->tx_buffer + FRAME_DATA_OFFSET, data, data_len);
    *(uint16_t *)&(self->tx_buffer[FRAME_HEADER_OFFSET]) = self->format.header;
    *(uint16_t *)&(self->tx_buffer[FRAME_COUNT_OFFSET]) = self->send_count;
    *(uint16_t *)&(self->tx_buffer[FRAME_DATA_LEN_OFFSET]) = data_len;
    ret = self->format.crc_func(&self->format, &crc, data, data_len);
    if (ret != 0)
    {
        return -2;
    }
    *(uint32_t *)&(self->tx_buffer[FRAME_DATA_OFFSET + data_len]) = crc;
    self->tx_data_len = FRAME_EXTRA_LEN + data_len;

    ret = self->send_func(self->tx_buffer, data_len + FRAME_EXTRA_LEN, timeout, self->send_arg);
    if (ret != 0)
    {
        return -3;
    }
    self->tx_retry_count = 0;
    status = osTimerStart(self->osTimerId, self->timeout_ms / portTICK_RATE_MS);
    if (status != osOK)
    {
        return -4;
    }

    status = osSemaphoreAcquire(self->osSemaphoreId, timeout);

    self->send_count++;

    if (status != osOK)
    {
        return -5;
    }
    
    return 0;
}

int32_t frame_format_recv(frame_format_t *self, uint8_t *data, uint16_t data_len, uint32_t timeout)
{
    uint32_t crc = 0;
    int32_t ret = 0;
    osStatus_t status = osOK;
    uint32_t recv_len = 0;
    if (self == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }
recv_again:
    ret = self->recv_func(self->rx_buffer, recv_len, timeout, self->recv_arg);
    if (ret != 0)
    {
        return -2;
    }
    recv_len = *(uint16_t *)&(self->rx_buffer[self->rx_buffer_size - sizeof(uint16_t)]);

    if (recv_len < sizeof(uart_frame_t))
    {
        return -3;
    }
    if (recv_len == sizeof(uart_frame_t)) // receive a response frame
    {
        self->format.crc_func(&self->format, &crc, self->rx_buffer + FRAME_DATA_OFFSET, recv_len - FRAME_EXTRA_LEN);
        if (crc != *(uint32_t *)&(self->rx_buffer[recv_len - sizeof(uint32_t)]))
        {
            return -4;
        }
        if (self->send_count != ((uart_frame_t *)self->rx_buffer)->count)
        {
            return -5;
        }
        status = osTimerStop(self->osTimerId);
        if (status != osOK)
        {
            return -6;
        }
        status = osSemaphoreRelease(self->osSemaphoreId);
        if (status != osOK)
        {
            return -7;
        }

        goto recv_again;
    }
    else
    {
        self->frame_format_statistics.recv_total_count++;
        self->frame_format_statistics.recv_lose_rate =
            (float)self->frame_format_statistics.recv_lose_count /
            (float)(self->frame_format_statistics.recv_total_count + self->frame_format_statistics.recv_lose_count);
            
        if (self->crc_check_state == true)
        {
            self->format.crc_func(&self->format, &crc, self->rx_buffer + FRAME_DATA_OFFSET, recv_len - FRAME_EXTRA_LEN);
            if (crc != *(uint32_t *)&(self->rx_buffer[recv_len - FRAME_CRC_LEN]))
            {
                self->frame_format_statistics.recv_crc_error_count++;
                return -8;
            }
        }

        uart_frame_t response_frame = {
            .header = *(uint16_t *)(self->rx_buffer + FRAME_HEADER_OFFSET),
            .count = *(uint16_t *)(self->rx_buffer + FRAME_COUNT_OFFSET),
            .data_len = 1,
            .data = self->rx_buffer[FRAME_DATA_OFFSET],
            .crc = 0,
        };
        self->format.crc_func(&self->format,
                              &response_frame.crc,
                              (uint8_t *)&response_frame + FRAME_DATA_OFFSET,
                              sizeof(uart_frame_t) - FRAME_EXTRA_LEN);
        ret = self->send_func((uint8_t *)&response_frame, sizeof(uart_frame_t), timeout, (void *)self->send_arg);
        if (ret != 0)
        {
            return -9;
        }
        if(response_frame.count > self->recv_count)
        {
            self->frame_format_statistics.recv_lose_count +=  response_frame.count - self->recv_count - 1;
        }
        else if(response_frame.count < self->recv_count)
        {
            self->frame_format_statistics.recv_lose_count +=  0xFFFF - self->recv_count + response_frame.count;
        }
        self->frame_format_statistics.recv_lose_rate =
            (float)self->frame_format_statistics.recv_lose_count /
            (float)(self->frame_format_statistics.recv_total_count + self->frame_format_statistics.recv_lose_count);

        self->recv_count = response_frame.count;
    }
    memcpy(data, self->rx_buffer + FRAME_DATA_OFFSET, recv_len - FRAME_EXTRA_LEN);
    return 0;
}

int32_t frame_format_get_statistics(frame_format_t *self, frame_format_statistics_t *statistics)
{
    if (self == NULL || statistics == NULL)
    {
        return -1;
    }
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
