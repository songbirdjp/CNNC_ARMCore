/**
 * @file app_data_record.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __APP_DATA_RECORD_H__
#define __APP_DATA_RECORD_H__

#include "websocket.h"

#define DATA_RECORD_DATA_ITEM 1024
#define DATA_RECORD_DATA_SIZE (UART_PROTOCOL_DATA_MAX_LENGTH + sizeof(uint64_t) + sizeof(uint32_t))
typedef struct {
    uint8_t *fifo;
    uint32_t fifo_item_size[DATA_RECORD_DATA_ITEM];
    uint32_t fifo_out;
    uint32_t fifo_in;
    uint32_t fifo_space;
    osMutexId_t mutex;
}data_fifo_t;

typedef struct {
    data_fifo_t data_fifo;
    uint8_t *txdata;
    uint8_t sn;
} app_data_record_t;

int32_t app_data_record_init(app_data_record_t *self);
int32_t app_data_record(app_data_record_t *self,uint32_t ID, void *data, uint32_t len);
#endif /* __APP_DATA_RECORD_H__ */