/**
 * @file frame_format.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-02-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __FRAME_FORMAT_H__
#define __FRAME_FORMAT_H__

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* frame format :   | header | count | data len | data | crc32 |
     *
     *   字节序: LSB first
     *   比特序: lsb first
     *
     *   header: 2 bytes，固定为0x55 0xAA
     *   count:  2 bytes，表示当前帧序号，从0开始，每发送一帧递增1，溢出时归零继续计数
     *   data len: 2 bytes，表示数据长度
     *   data: 表示数据内容，见下述说明
     *   crc32: 4 bytes，表示数据校验值，采用CRC32算法， polynomial=0x04C11DB7, init=0xFFFFFFFF, xor=0xFFFFFFFF
     *                   计算时，包含count、data len、data三个字段
     *
     *   data segment:  | ack | id | protocol data |
     *
     *   id 与 ack 共占用 1 byte -> id：7 bits, ack：1 bit
     *   id: 标识访问的总线上设备的id标号，范围0~127，0：广播    1~127：具体设备
     *   ack: 表示是否需要应答，0：不需要，1：需要
     *   protocol data: 协议数据，占用字节数由具体协议确定
     *
     * hardware configuration:
     *   - UART: 500kbps, 8-N-1, no flow control;  DMA mode, no fifo, idle interrupt
     */
    typedef enum crc_type
    {
        CRC_TYPE_CRC_8 = 0,
        CRC_TYPE_CRC_16,
        CRC_TYPE_CRC_32,
    } crc_type_t;

    typedef int32_t (*crc_func_t)(void *, uint32_t *, uint8_t *, uint32_t);
    typedef struct format
    {
        uint16_t header;

        crc_type_t crc_type;
        uint32_t crc_polynomial;
        uint32_t crc_init;
        uint32_t crc_xor;

        crc_func_t crc_func;
    } format_t;
    typedef struct frame_format_statistics
    {
        uint32_t send_total_count;
        uint32_t recv_total_count;
        uint32_t send_error_count;     /*< 重试次数超过最大值，超过最大值后仍然失败 */
        uint32_t recv_crc_error_count; /*< 接收到的数据帧CRC校验失败 */
        uint32_t send_retry_count;     /*< 重试后成功，同一数据包多次尝试记为一次 */
        uint32_t recv_lose_count;      /*< 接收到的数据帧count值不连续，丢失的包数 */
        float send_lose_rate;          /*< 发送丢包率 = send_error_count / send_total_count */
        float recv_lose_rate;          /*< 接收丢包率 = recv_lose_count / (recv_total_count +  recv_lose_count)*/
    } frame_format_statistics_t;
    typedef int32_t (*uart_xfer_func_t)(uint8_t *data, uint16_t data_len, uint32_t timeout, void *arg);
    typedef struct frame_format
    {
        format_t format;

        bool crc_check_state;
        uint8_t retry_count;
        uint32_t timeout_ms;
        osTimerId_t osTimerId;
        osSemaphoreId_t osSemaphoreId;

        osMutexId_t tx_mutex;
        uint8_t *tx_buffer;
        uint32_t tx_retry_data_len;

        osMutexId_t tx_response_mutex;
        uint8_t *tx_response_buffer;

        uint32_t tx_data_len;
        uint32_t tx_buffer_size;
        uart_xfer_func_t send_func;
        uint8_t tx_retry_count;
        void *send_arg;

        uint8_t *rx_buffer;
        uint32_t rx_buffer_size;
        uart_xfer_func_t recv_func;
        void *recv_arg;
        uint16_t recv_response_count;

        uint16_t send_count;
        uint16_t recv_count;

        frame_format_statistics_t frame_format_statistics;
    } frame_format_t;

    int32_t frame_format_init(frame_format_t *self,
                              bool crc_check_state,
                              uint8_t retry_count,
                              uint32_t timeout_ms,
                              uint32_t rx_buffer_size,
                              uint32_t tx_buffer_size);

    int32_t frame_format_send_func_register(frame_format_t *self, uart_xfer_func_t send_func, void *arg);
    int32_t frame_format_recv_func_register(frame_format_t *self, uart_xfer_func_t recv_func, void *arg);

    int32_t frame_format_send(frame_format_t *self, uint8_t *data, uint16_t data_len, uint32_t timeout);

    int32_t frame_format_recv(frame_format_t *self, uint8_t *data, uint16_t data_len, uint32_t timeout);

    int32_t frame_format_get_statistics(frame_format_t *self, frame_format_statistics_t *statistics);

    void frame_format_clear_statistics(frame_format_t *self);

#ifdef __cplusplus
}
#endif

#endif /* __FRAME_FORMAT_H__ */