/**
 * @file uart_protocol.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-03-28
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "uart_protocol.h"
#include "ulog.h"
#include <string.h>
#include <stdlib.h>
#include "init_call.h"
typedef struct
{
    uint8_t data[UART_PROTOCOL_DATA_MAX_LENGTH];
    uint16_t len;
} uart_protocol_rx_msg_t;

#define UART_PROTOCOL_RX_DATA_QUEUE_SIZE (sizeof(uart_protocol_rx_msg_t))

typedef struct
{
    uint32_t id_ack;
    uint8_t type;
    uint16_t length;
    uint8_t data[UART_PROTOCOL_DATA_MAX_LENGTH];
} __attribute__((aligned(1), packed)) uart_protocol_payload_t;

#define UART_PROTOCOL_PAYLOAD_QUEUE_SIZE (sizeof(uart_protocol_payload_t))

typedef struct
{
    uint16_t len;
    uint16_t sof : 1;
    uint16_t eof : 1;
    uint16_t sn : 14;
} __attribute__((aligned(1), packed)) config_msg_t;

typedef struct
{
    uint16_t header;
    uint16_t count;
    uint16_t length;
    uart_protocol_payload_t payload;
    uint32_t crc32;
} __attribute__((aligned(1), packed)) uart_protocol_frame_t;

typedef struct uart_crc_error_frame
{
    uint16_t header;
    uint16_t count;
    uint16_t length;
    uint32_t id_ack;
    uint8_t type;
    uint16_t len;
    uint32_t data;
    uint32_t crc;
} __attribute__((aligned(1), packed)) uart_crc_error_frame_t;

uart_crc_error_frame_t uart_crc_error_frame = {
    .header = 0xAA55,
    .count = 0x0000,
    .length = 0x0B,
    .id_ack = 0x00,
    .type = 0xFF,
    .len = 0x0004,
    .data = 0x65637263,
    .crc = 0x3453FD02};

#define UART_PROTOCOL_FRAME_QUEUE_SIZE (sizeof(uart_protocol_frame_t) + sizeof(uint16_t))

static void uart_protocol_heartbeat_tx_timer_callback(void *arg);
static void uart_protocol_heartbeat_rx_timeout_timer_callback(void *arg);
static void uart_protocol_pnt_timer_callback(void *arg);

static int32_t uart_send_func(uint8_t *data, uint16_t data_len, uint32_t timeout, void *arg)
{
    if (arg == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }
    uart_protocol_t *uart_protocol = (uart_protocol_t *)arg;
    int32_t ret = 0;
    ret = dev_uart_send(uart_protocol->uart_dev, data, data_len, timeout);
    if (ret < 0)
    {
        return -1;
    }

#if 0
    LOG_I("send: ");
    for (int i = 0; i < data_len; i++)
    {
        LOG_I("%02x ", data[i]);
    }
    LOG_I("\r\n");
#endif

    return 0;
}
static int32_t uart_recv_func(uint8_t *data, uint16_t data_len, uint32_t timeout, void *arg)
{
    if (arg == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }
    uart_protocol_t *uart_protocol = (uart_protocol_t *)arg;
    int32_t ret = 0;
    ret = dev_uart_recv(uart_protocol->uart_dev, data, data_len, timeout);
    if (ret < 0)
    {
        return -1;
    }
    return 0;
}
static int32_t uart_crc_error_func(uint8_t *data, uint16_t data_len, void *arg)
{
    if (arg == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }
    int32_t ret = 0;
    uart_protocol_t *uart_protocol = (uart_protocol_t *)arg;
    uart_protocol_frame_t *frame = data;
    // if ((frame->header == 0xAA55) &&
    //     (frame->payload.id_ack & 0x01) &&
    //     ((frame->payload.type == 0x03) || (frame->payload.type == 0x04) || (frame->payload.type == 0x05) || (frame->payload.type == 0x06)))
    {
        ret = dev_uart_send(uart_protocol->uart_dev, &uart_crc_error_frame, sizeof(uart_crc_error_frame), 100);
        if (ret != 0)
        {
            return -2;
        }
    }
    return 0;
}
static int32_t uart_retry_judge_func(uint8_t *data, uint16_t data_len, void *arg)
{
    if (arg == NULL || data == NULL || data_len == 0)
    {
        return -1;
    }
    if(data_len != sizeof(uart_crc_error_frame_t))
    {
        return -2;
    }
    if(memcmp(data, &uart_crc_error_frame, sizeof(uart_crc_error_frame_t)) != 0)
    {
        return -3;
    }
    return 0;
}
int32_t uart_protocol_init(uart_protocol_t *const self,
                           const char *name,
                           uint32_t heartbeat_tx_timeout,
                           uint32_t heartbeat_rx_timeout,
                           uint32_t pnt_timeout)
{
    device_err_t err;
    osStatus_t osStatus = osOK;
    int32_t ret = 0;
    osMessageQueueAttr_t queue_attributes = {0};
    if (self == NULL || name == NULL || heartbeat_tx_timeout == 0 || heartbeat_rx_timeout == 0 || pnt_timeout == 0)
    {
        return -1;
    }
    memset(self, 0, sizeof(uart_protocol_t));
    self->uart_dev = device_uart_find(name);
    if (self->uart_dev == NULL)
    {
        return -2;
    }
    frame_used_arg_t frame_used_arg = {true};
    err = dev_uart_config(self->uart_dev, DEV_UART_IOCTL_USED_FRAME, (void *)&frame_used_arg);
    if (err != DEV_EOK)
    {
        return -3;
    }
    err = dev_uart_init(self->uart_dev, DEV_UART_IOCTL_USE_DMA, 16, UART_PROTOCOL_FRAME_QUEUE_SIZE);
    if (err != DEV_EOK)
    {
        return -4;
    }

    ret = frame_format_init(&self->frame_format,
                            true,
                            3,
                            10,
                            UART_PROTOCOL_FRAME_QUEUE_SIZE,
                            UART_PROTOCOL_FRAME_QUEUE_SIZE);
    if (ret != 0)
    {
        return -5;
    }

    ret = frame_format_send_func_register(&self->frame_format, uart_send_func, self);
    if (ret != 0)
    {
        return -6;
    }

    ret = frame_format_recv_func_register(&self->frame_format, uart_recv_func, self);
    if (ret != 0)
    {
        return -7;
    }
    ret = frame_format_crc_error_handle_func_register(&self->frame_format, uart_crc_error_func, self);
    if (ret != 0)
    {
        return -8;
    }
    ret = frame_format_retry_judge_handle_func_register(&self->frame_format, uart_retry_judge_func, self);
    if (ret != 0)
    {
        return -9;
    }

    queue_attributes.name = "get_rx_response_queue";
    self->get_rx_response_queue = osMessageQueueNew(1, UART_PROTOCOL_RX_DATA_QUEUE_SIZE, &queue_attributes);

    queue_attributes.name = "config_get_response_queue";
    self->config_get_response_queue = osMessageQueueNew(1, UART_PROTOCOL_RX_DATA_QUEUE_SIZE, &queue_attributes);

    if ((self->get_rx_response_queue == NULL) || (self->config_get_response_queue == NULL))
    {
        return -10;
    }

    self->uart_protocol_heartbeat_timer = osTimerNew(uart_protocol_heartbeat_tx_timer_callback,
                                                     osTimerPeriodic,
                                                     self,
                                                     NULL);
    if (self->uart_protocol_heartbeat_timer == NULL)
    {
        return -11;
    }
    osStatus = osTimerStart(self->uart_protocol_heartbeat_timer, heartbeat_tx_timeout / portTICK_PERIOD_MS);
    if (osStatus != osOK)
    {
        return -12;
    }

    self->uart_protocol_heartbeat_timeout_timer = osTimerNew(uart_protocol_heartbeat_rx_timeout_timer_callback,
                                                             osTimerPeriodic,
                                                             self,
                                                             NULL);
    if (self->uart_protocol_heartbeat_timeout_timer == NULL)
    {
        return -13;
    }
    osStatus = osTimerStart(self->uart_protocol_heartbeat_timeout_timer, heartbeat_rx_timeout / portTICK_PERIOD_MS);
    if (osStatus != osOK)
    {
        return -14;
    }
    self->uart_protocol_heartbeat_rx_timeout = heartbeat_rx_timeout;
    self->uart_protocol_pnt_timer = osTimerNew(uart_protocol_pnt_timer_callback,
                                               osTimerPeriodic,
                                               self,
                                               NULL);
    if (self->uart_protocol_pnt_timer == NULL)
    {
        return -15;
    }
    osStatus = osTimerStart(self->uart_protocol_pnt_timer, pnt_timeout / portTICK_PERIOD_MS);
    if (osStatus != osOK)
    {
        return -16;
    }

    return 0;
}
int32_t uart_protocol_open(uart_protocol_t *const self)
{
    device_err_t err;
    if (self == NULL || self->uart_dev == NULL)
    {
        return -1;
    }
    err = dev_uart_open(self->uart_dev);
    if (err != DEV_EOK)
    {
        return -2;
    }
    return 0;
}
int32_t uart_protocol_send(uart_protocol_t *const self,
                           const uint8_t *data,
                           uint16_t len,
                           uint32_t timeout)
{
    if (self == NULL || data == NULL || len == 0)
    {
        return -1;
    }

    int32_t ret = frame_format_send(&self->frame_format, data, len, timeout);
    if (ret != 0)
    {
        return -2;
    }
    return 0;
}
int32_t uart_protocol_recv(uart_protocol_t *const self,
                           uint8_t *data,
                           uint16_t *len,
                           uint32_t timeout)
{
    osStatus_t osStatus = osOK;
    int32_t ret = 0;
    if (self == NULL || data == NULL || len == NULL)
    {
        return -1;
    }
    uart_protocol_payload_t *payload = (uart_protocol_payload_t *)data;
    ret = frame_format_recv(&self->frame_format, payload, len, timeout);
    if (ret != 0)
    {
        LOG_E("frame_format_recv error: %d\r\n", ret);
        return -2;
    }

    switch (payload->type)
    {
    case 0x01: /*心跳帧*/
        if (osTimerIsRunning(self->uart_protocol_heartbeat_timeout_timer) != 0)
        {
            osStatus = osTimerStart(self->uart_protocol_heartbeat_timeout_timer,
                                    self->uart_protocol_heartbeat_rx_timeout / portTICK_PERIOD_MS);
            if (osStatus != osOK)
            {
                return -3;
            }
        }

        if (self->uart_protocol_rx_callback[UART_PROTOCOL_HEARTBEAT_RX_CB_ID].pCallback != NULL)
        {
            ret = self->uart_protocol_rx_callback[UART_PROTOCOL_HEARTBEAT_RX_CB_ID].pCallback(self,
                                                                                              payload->id_ack >> 1,
                                                                                              payload->data,
                                                                                              &payload->length,
                                                                                              self->uart_protocol_rx_callback[UART_PROTOCOL_HEARTBEAT_RX_CB_ID].arg);
            if (ret != 0)
            {
                return -4;
            }
        }
        break;
    case 0x02: /*授时帧*/
        if (self->uart_protocol_rx_callback[UART_PROTOCOL_PNT_RX_CB_ID].pCallback != NULL)
        {
            ret = self->uart_protocol_rx_callback[UART_PROTOCOL_PNT_RX_CB_ID].pCallback(self,
                                                                                        payload->id_ack >> 1,
                                                                                        payload->data,
                                                                                        &payload->length,
                                                                                        self->uart_protocol_rx_callback[UART_PROTOCOL_PNT_RX_CB_ID].arg);
            if (ret != 0)
            {
                return -4;
            }
        }
        break;
    case 0x03: /*参数配置帧*/
    {
        /*TODO:组包实现 */
        if (self->uart_protocol_rx_callback[UART_PROTOCOL_CONFIG_SET_RX_CB_ID].pCallback != NULL)
        {
            ret = self->uart_protocol_rx_callback[UART_PROTOCOL_CONFIG_SET_RX_CB_ID].pCallback(self,
                                                                                               payload->id_ack >> 1,
                                                                                               payload->data,
                                                                                               &payload->length,
                                                                                               self->uart_protocol_rx_callback[UART_PROTOCOL_CONFIG_SET_RX_CB_ID].arg);
            if (ret != 0)
            {
                return -5;
            }
            struct
            {
                uint32_t id_ack;
                uint8_t type;
                uint16_t length;
            } __attribute__((packed)) format = {(payload->id_ack & 0xFFFFFFFE), 0x83, 0x0000};
            int32_t ret = uart_protocol_send(self, (uint8_t *)&format, sizeof(format), 100); /* TODO: 参数配置帧是否需要添加状态回复，以判断参数配置有效性、正确性 */
            if (ret < 0)
            {
                return -6;
            }
        }
    }
    break;
    case 0x04: /*参数获取帧*/
    {
        if (self->uart_protocol_rx_callback[UART_PROTOCOL_CONFIG_GET_RX_CB_ID].pCallback != NULL)
        {
            ret = self->uart_protocol_rx_callback[UART_PROTOCOL_CONFIG_GET_RX_CB_ID].pCallback(self,
                                                                                               payload->id_ack >> 1,
                                                                                               payload->data,
                                                                                               &payload->length,
                                                                                               self->uart_protocol_rx_callback[UART_PROTOCOL_CONFIG_GET_RX_CB_ID].arg);
            if (ret != 0)
            {
                return -7;
            }
        }
    }
    break;
    case 0x05: /*SET帧*/
    {
        if (self->uart_protocol_rx_callback[UART_PROTOCOL_SET_RX_CB_ID].pCallback != NULL)
        {
            ret = self->uart_protocol_rx_callback[UART_PROTOCOL_SET_RX_CB_ID].pCallback(self,
                                                                                        payload->id_ack >> 1,
                                                                                        payload->data,
                                                                                        &payload->length,
                                                                                        self->uart_protocol_rx_callback[UART_PROTOCOL_SET_RX_CB_ID].arg);
            // if (ret != 0)
            // {
            //     return -8;
            // }
        }

        // struct
        // {
        //     uint32_t id_ack;
        //     uint8_t type;
        //     uint16_t length;
        // } __attribute__((packed)) format = {(payload->id_ack & 0xFFFFFFFE), 0x85, 0x0000};
        // int32_t ret = uart_protocol_send(self, (uint8_t *)&format, sizeof(format), 100); /* TODO: 参数配置帧是否需要添加状态回复，以判断参数配置有效性、正确性 */
        // if (ret < 0)
        // {
        //     return -9;
        // }
        uart_protocol_payload_t send_data = {0};
        send_data.id_ack = payload->id_ack & 0xFFFFFFFE;
        send_data.type = payload->type | 0x80;
        send_data.length = payload->length;
        memcpy(send_data.data, payload->data, send_data.length);
        ret = uart_protocol_send(self, (uint8_t *)&send_data, send_data.length + sizeof(uart_protocol_payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH, 100);
        if (ret < 0)
        {
            return -9;
        }
    }
    break;
    case 0x06: /*GET帧*/
    {
        if (self->uart_protocol_rx_callback[UART_PROTOCOL_GET_RX_CB_ID].pCallback != NULL)
        {
            ret = self->uart_protocol_rx_callback[UART_PROTOCOL_GET_RX_CB_ID].pCallback(self,
                                                                                        payload->id_ack >> 1,
                                                                                        payload->data,
                                                                                        &payload->length,
                                                                                        self->uart_protocol_rx_callback[UART_PROTOCOL_GET_RX_CB_ID].arg);
            // if (ret != 0)
            // {
            //     return -10;
            // }
        }
        uart_protocol_payload_t send_data = {0};
        send_data.id_ack = payload->id_ack & 0xFFFFFFFE;
        send_data.type = payload->type | 0x80;
        send_data.length = payload->length;
        memcpy(send_data.data, payload->data, send_data.length);
        ret = uart_protocol_send(self, (uint8_t *)&send_data, send_data.length + sizeof(uart_protocol_payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH, 100);
        if (ret < 0)
        {
            return -11;
        }
    }
    break;
    case 0x83: /*参数配置应答*/
        break;
    case 0x84: /*参数获取应答*/
        osStatus = osMessageQueuePut(self->config_get_response_queue, payload->data, 0, 100);
        if (osStatus != osOK)
        {
            return -12;
        }
        break;
    case 0x85: /*数据set应答*/
        if (self->uart_protocol_rx_callback[UART_PROTOCOL_SET_RX_CB_ID].pCallback != NULL)
        {
            ret = self->uart_protocol_rx_callback[UART_PROTOCOL_SET_RX_CB_ID].pCallback(self,
                                                                                        payload->id_ack >> 1,
                                                                                        payload->data,
                                                                                        &payload->length,
                                                                                        self->uart_protocol_rx_callback[UART_PROTOCOL_SET_RX_CB_ID].arg);
            if (ret != 0)
            {
                return -13;
            }
        }
        break;
    case 0x86: /*数据get应答*/
        osStatus = osMessageQueuePut(self->get_rx_response_queue, payload->data + 1, 0, 100);
        if (osStatus != osOK)
        {
            return -14;
        }
        break;
    case 0xeb: /* 复位帧 */
        if (self->uart_protocol_rx_callback[UART_PROTOCOL_REBOOT_RX_CB_ID].pCallback != NULL)
        {
            ret = self->uart_protocol_rx_callback[UART_PROTOCOL_REBOOT_RX_CB_ID].pCallback(self,
                                                                                           payload->id_ack >> 1,
                                                                                           payload->data,
                                                                                           &payload->length,
                                                                                           self->uart_protocol_rx_callback[UART_PROTOCOL_REBOOT_RX_CB_ID].arg);
            if (ret != 0)
            {
                return -15;
            }
        }
        break;
    default:
        ret = -16;
        LOG_E("invalid frame type: %d\r\n", payload->type);
        break;
    }

    return ret;
}

int32_t uart_protocol_rx_RegisterCallback(uart_protocol_t *const self,
                                          uart_protocol_CallbackIDTypeDef CallbackID,
                                          uart_protocol_rx_callback_t pCallback,
                                          void *arg)
{
    if (self == NULL || pCallback == NULL)
    {
        return -1;
    }
    if (CallbackID >= UART_PROTOCOL_MAX_RX_CB_ID)
    {
        return -2;
    }
    self->uart_protocol_rx_callback[CallbackID].pCallback = pCallback;
    self->uart_protocol_rx_callback[CallbackID].arg = arg;
    return 0;
}
int32_t uart_protocol_tx_RegisterCallback(uart_protocol_t *const self,
                                          uart_protocol_CallbackIDTypeDef CallbackID,
                                          uart_protocol_tx_callback_t pCallback,
                                          void *arg)
{
    if (self == NULL || pCallback == NULL)
    {
        return -1;
    }
    if (CallbackID >= UART_PROTOCOL_MAX_TX_CB_ID)
    {
        return -2;
    }
    self->uart_protocol_tx_callback[CallbackID].pCallback = pCallback;
    self->uart_protocol_tx_callback[CallbackID].arg = arg;
    return 0;
}
int32_t uart_protocol_config_set(uart_protocol_t *const self,
                                 uint32_t ID,
                                 const uint8_t *data,
                                 uint32_t len,
                                 uint32_t timeout)
{
    uart_protocol_payload_t payload = {0};
    if (self == NULL || data == NULL || len == 0)
    {
        return -1;
    }
    payload.id_ack = (ID << 1) | 0x01;
    payload.type = 0x03;

    if (len > (UART_PROTOCOL_DATA_MAX_LENGTH - sizeof(config_msg_t)))
    {
    }
    else
    {
        payload.length = len + sizeof(config_msg_t); /*lenght, len+ 2BYTE(包整体长度) + 2BYTE(SOF + EOF + SN)*/

        config_msg_t config_msg = {
            .len = len,
            .sof = 0x01,
            .eof = 0x01,
            .sn = 0x0000,
        };
        *(uint32_t *)payload.data = *(uint32_t *)&config_msg;
        memcpy(payload.data + sizeof(config_msg_t), data, len);
        int32_t ret = uart_protocol_send(self, &payload, payload.length + sizeof(uart_protocol_payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH, timeout);
        if (ret < 0)
        {
            return -2;
        }
    }
    return 0;
}
int32_t uart_protocol_config_get(uart_protocol_t *const self,
                                 uint32_t ID,
                                 uint8_t *data,
                                 uint32_t *len,
                                 uint32_t timeout)
{
    return 0;
}
int32_t uart_protocol_set(uart_protocol_t *const self,
                          uint32_t ID,
                          uint8_t cmd,
                          const uint8_t *data,
                          uint16_t len,
                          uint32_t timeout)
{
    uart_protocol_payload_t payload = {0};

    if (self == NULL || data == NULL || len == 0 || len > UART_PROTOCOL_DATA_MAX_LENGTH)
    {
        return -1;
    }
    payload.id_ack = (ID << 1) | 0x01; /*(ID << 1) | 0x01，需要应答*/
    payload.type = 0x05;               /*0x05, 数据set帧*/
    payload.data[0] = cmd;
    memcpy(&payload.data[1], data, len);
    payload.length = len + 1;

    int32_t ret = uart_protocol_send(self, &payload, payload.length + sizeof(uart_protocol_payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH, timeout);
    if (ret < 0)
    {
        return -2;
    }
    return 0;
}
int32_t uart_protocol_get(uart_protocol_t *const self,
                          uint32_t ID,
                          uint8_t cmd,
                          const uint8_t *data,
                          uint16_t *len,
                          uint32_t timeout)
{
    osStatus_t osStatus = osOK;
    uart_protocol_payload_t payload = {0};

    if (self == NULL || data == NULL || len == NULL)
    {
        return -1;
    }
    payload.id_ack = (ID << 1) | 0x01; /*(ID << 1) | 0x01，需要应答*/
    payload.type = 0x06;               /*0x06, 数据get帧*/
    payload.data[0] = cmd;
    payload.length = 1;
    int32_t ret = uart_protocol_send(self, &payload, payload.length + sizeof(uart_protocol_payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH, timeout);
    if (ret < 0)
    {
        return -2;
    }
    osStatus = osMessageQueueGet(self->get_rx_response_queue, data, 0, timeout);
    if (osStatus != osOK)
    {
        return -3;
    }
    return 0;
}
int32_t uart_protocol_reboot(uart_protocol_t *const self,
                             uint32_t ID,
                             uint32_t timeout)
{
    uart_protocol_payload_t payload = {0};
    if (self == NULL)
    {
        return -1;
    }
    payload.id_ack = (ID << 1) | 0x00; /*(ID << 1) | 0x00，不需要应答*/
    payload.type = 0xeb;               /*0xeb, 复位帧*/
    payload.length = 0x0000;           /*lenght*/

    return uart_protocol_send(self, &payload, payload.length + sizeof(uart_protocol_payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH, timeout);
}

static int32_t uart_protocol_get_response(uart_protocol_t *const self,
                                          uint32_t ID,
                                          uint8_t cmd,
                                          const uint8_t *data,
                                          uint16_t len,
                                          uint32_t timeout)
{
    uart_protocol_payload_t payload = {0};

    if (self == NULL || data == NULL || len == 0 || len > (UART_PROTOCOL_DATA_MAX_LENGTH - 1))
    {
        return -1;
    }
    payload.id_ack = (ID << 1) | 0x00; /*(ID << 1) | 0x00，不需要应答*/
    payload.type = 0x86;               /*0x86, 数据get 应答帧*/
    payload.data[0] = cmd;
    memcpy(&payload.data[1], data, len);
    payload.length = len + 1;

    int32_t ret = uart_protocol_send(self, &payload, payload.length + sizeof(uart_protocol_payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH, timeout);
    if (ret < 0)
    {
        return -2;
    }

    return 0;
}

static osMessageQueueId_t uart_protocol_queue = NULL;
#define UART_PROTOCOL_HEARTBEAT_TX_EVENT (1 << 0)
#define UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_EVENT (1 << 1)
#define UART_PROTOCOL_PNT_TX_EVENT (1 << 2)
struct msg_queue
{
    uint8_t event;
    uart_protocol_t *uart_protocol;
};

static void uart_protocol_heartbeat_tx_timer_callback(void *arg)
{
    osStatus_t stat = osOK;
    struct msg_queue queue = {.event = UART_PROTOCOL_HEARTBEAT_TX_EVENT, .uart_protocol = (uart_protocol_t *)arg};

    stat = osMessageQueuePut(uart_protocol_queue, &queue, 0, 0);
    if (stat != osOK)
    {
        LOG_E("uart_protocol_queue put err: %d\r\n", stat);
    }
}
static void uart_protocol_heartbeat_rx_timeout_timer_callback(void *arg)
{
    osStatus_t stat = osOK;
    struct msg_queue queue = {.event = UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_EVENT, .uart_protocol = (uart_protocol_t *)arg};

    stat = osMessageQueuePut(uart_protocol_queue, &queue, 0, 0);
    if (stat != osOK)
    {
        LOG_E("uart_protocol_queue put err: %d\r\n", stat);
    }
}
static void uart_protocol_pnt_timer_callback(void *arg)
{
    osStatus_t stat = osOK;
    struct msg_queue queue = {.event = UART_PROTOCOL_PNT_TX_EVENT, .uart_protocol = (uart_protocol_t *)arg};

    stat = osMessageQueuePut(uart_protocol_queue, &queue, 0, 0);
    if (stat != osOK)
    {
        LOG_E("uart_protocol_queue put err: %d\r\n", stat);
    }
}

static void heartbeat_tx_timer_callback(void *arg)
{
    uart_protocol_payload_t payload = {0};
    if (arg == NULL)
    {
        return;
    }
    uart_protocol_t *uart_protocol = (uart_protocol_t *)arg;
    if (uart_protocol->uart_protocol_tx_callback[UART_PROTOCOL_HEARTBEAT_TX_CB_ID].pCallback != NULL)
    {
        payload.id_ack = 0x00000000; /*(0x00000000 << 1) | 0x00, 广播，不需要应答*/
        payload.type = 0x01;         /*0x01, 心跳帧*/

        if (0 != uart_protocol->uart_protocol_tx_callback[UART_PROTOCOL_HEARTBEAT_TX_CB_ID].pCallback(uart_protocol,
                                                                                                      payload.data,
                                                                                                      &payload.length,
                                                                                                      uart_protocol->uart_protocol_tx_callback[UART_PROTOCOL_HEARTBEAT_TX_CB_ID].arg))
        {
            return;
        }

        int32_t ret = uart_protocol_send(uart_protocol, &payload, payload.length + sizeof(uart_protocol_payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH, 100);
        if (ret < 0)
        {
            return;
        }
    }
}
static void heartbeat_rx_timeout_timer_callback(void *arg)
{
    if (arg == NULL)
    {
        return;
    }
    uart_protocol_t *uart_protocol = (uart_protocol_t *)arg;
    if (uart_protocol->uart_protocol_rx_callback[UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID].pCallback != NULL)
    {

        if (0 != uart_protocol->uart_protocol_rx_callback[UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID].pCallback(uart_protocol,
                                                                                                              0,
                                                                                                              NULL,
                                                                                                              0,
                                                                                                              uart_protocol->uart_protocol_rx_callback[UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID].arg))
        {
            return;
        }
    }
}
static void pnt_timer_callback(void *arg)
{
    uart_protocol_payload_t payload = {0};
    if (arg == NULL)
    {
        return;
    }
    uart_protocol_t *uart_protocol = (uart_protocol_t *)arg;
    if (uart_protocol->uart_protocol_tx_callback[UART_PROTOCOL_PNT_TX_CB_ID].pCallback != NULL)
    {
        payload.id_ack = 0x00000000; /*(0x00000000 << 1) | 0x00, 广播，不需要应答*/
        payload.type = 0x02;         /*0x02, 授时帧*/

        if (0 != uart_protocol->uart_protocol_tx_callback[UART_PROTOCOL_PNT_TX_CB_ID].pCallback(uart_protocol,
                                                                                                payload.data,
                                                                                                &payload.length,
                                                                                                uart_protocol->uart_protocol_tx_callback[UART_PROTOCOL_PNT_TX_CB_ID].arg))
        {
            return;
        }
        int32_t ret = uart_protocol_send(uart_protocol, &payload, payload.length + sizeof(uart_protocol_payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH, 100);
        if (ret < 0)
        {
            return;
        }
    }
}

static int8_t uart_protocol_thread_entry(void *argument)
{
    struct msg_queue msg = {0};

    for (;;)
    {
        osMessageQueueGet(uart_protocol_queue, &msg, NULL, osWaitForever);

        if (msg.event & UART_PROTOCOL_HEARTBEAT_TX_EVENT)
        {
            heartbeat_tx_timer_callback(msg.uart_protocol);
        }
        if (msg.event & UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_EVENT)
        {
            heartbeat_rx_timeout_timer_callback(msg.uart_protocol);
        }
        if (msg.event & UART_PROTOCOL_PNT_TX_EVENT)
        {
            pnt_timer_callback(msg.uart_protocol);
        }
    }

    return 0;
}

static int8_t uart_protocol_timerout_init(void)
{

    uart_protocol_queue = osMessageQueueNew(16, sizeof(struct msg_queue), NULL);
    if (uart_protocol_queue == NULL)
    {
        LOG_E("queue uart protocol create failed\r\n");
        return -1;
    }

    osThreadAttr_t attr = {
        .name = "uart_protocol_timer",
        .stack_size = 1024 * 4,
        .priority = osPriorityNormal,
    };
    osThreadId_t thread_id = osThreadNew(uart_protocol_thread_entry, NULL, &attr);
    if (thread_id == NULL)
    {
        LOG_E("thread uart protocol create failed\r\n");
        return -2;
    }

    return 0;
}
INIT_COMPONENT_EXPORT(uart_protocol_timerout_init);

// #define UART_DEV_TEST

#ifdef UART_DEV_TEST
#include "shell.h"
static uart_protocol_t uart_protocol;
int8_t uart_test_statistics(uint8_t argc, uint8_t **argv)
{
    if (argc < 1 || argc > 2)
    {
        LOG_E("uart_test_statistics error\r\n");
        goto Usage;
    }

    frame_format_statistics_t statistics;
    if (strcmp(argv[1], "clear") == 0)
    {
        frame_format_clear_statistics(&uart_protocol.frame_format);
    }
    else if (strcmp(argv[1], "get") == 0)
    {
        frame_format_get_statistics(&uart_protocol.frame_format, &statistics);
        LOG_I("frame_format_statistics:\r\n");
        LOG_I("\tsend_total_count: %d\r\n", statistics.send_total_count);
        LOG_I("\tsend_error_count: %d\r\n", statistics.send_error_count);
        LOG_I("\tsend_retry_count: %d\r\n", statistics.send_retry_count);
        LOG_I("\tsend_lose_rate:%f\r\n", statistics.send_lose_rate);
        LOG_I("\trecv_total_count: %d\r\n", statistics.recv_total_count);
        LOG_I("\trecv_lose_count: %d\r\n", statistics.recv_lose_count);
        LOG_I("\trecv_crc_error_count: %d\r\n", statistics.recv_crc_error_count);
        LOG_I("\trecv_lose_rate:%f\r\n", statistics.recv_lose_rate);
    }
    else
    {
        LOG_E("uart_test_statistics option error\r\n");
        goto Usage;
    }
    return 0;
Usage:
    LOG_I("Usage: uart_test_statistics [option]\r\n");

    LOG_I("\r\n");
    LOG_I("option:<get> or <clear> \r\n");
    return 0;
}
MSH_CMD_EXPORT_ALIAS(uart_test_statistics, uart_test_statistics, uart test statistics);
static void uart_test_send_thread(void *argument)
{
    uint8_t data[UART_PROTOCOL_DATA_MAX_LENGTH] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    uint16_t len = 10;
    uint16_t length = 0;
    uint32_t timeout = 1000;
    int32_t ret = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        data[i] = i;
    }
    osDelay(100);
    LOG_I("uart_test_send_thread start\r\n");
    ret = uart_protocol_open(&uart_protocol);
    if (ret != 0)
    {
        LOG_E("uart_protocol_open error: %d\r\n", ret);
        goto thread_exit;
    }
    for (;;)
    {
        ret = uart_protocol_config_set(&uart_protocol, 0x01, data, len, timeout);
        if (ret != DEV_EOK)
        {
            LOG_E("config set error: %d\r\n", ret);
            continue;
        }
        ret = uart_protocol_set(&uart_protocol, 0x02, 0x01, data, len, timeout);
        if (ret != DEV_EOK)
        {
            LOG_E("set error: %d\r\n", ret);
            continue;
        }
        ret = uart_protocol_get(&uart_protocol, 0x02, 0x01, data, &length, timeout);
        if (ret != DEV_EOK)
        {
            LOG_E("get error: %d\r\n", ret);
            continue;
        }
        osDelay(1);
    }
thread_exit:
    osThreadExit();
}

int32_t uart_protocol_config_rx_callback(struct uart_protocol *const self,
                                         uint32_t ID,
                                         const uint8_t *data,
                                         uint16_t *len,
                                         void *arg)
{
    return 0;
}
int32_t uart_protocol_heartbeat_rx_callback(struct uart_protocol *const self,
                                            uint32_t ID,
                                            const uint8_t *data,
                                            uint16_t *len,
                                            void *arg)
{
    heartbeat_t *heartbeat = (heartbeat_t *)data;
    LOG_I("recv heartbeat:%x board_id:%x HardwareVersion:%x FirmWareVersion:%x\r\n",
          ID,
          heartbeat->board_id,
          heartbeat->HardwareVersion,
          heartbeat->FirmWareVersion);

    return 0;
}
int32_t uart_protocol_heartbeat_tx_callback(struct uart_protocol *const self,
                                            const uint8_t *data,
                                            uint16_t *len,
                                            void *arg)
{
    heartbeat_t heartbeat = {.board_id = 0x01, .HardwareVersion = 0x01, .FirmWareVersion = 0x01234567};
    memcpy(data, &heartbeat, sizeof(heartbeat));
    *len = sizeof(heartbeat);
    return 0;
}
int32_t uart_protocol_heartbeat_rx_timeout_callback(struct uart_protocol *const self,
                                                    const uint8_t *data,
                                                    uint16_t *len,
                                                    void *arg)
{
    LOG_E("heartbeat timeout!\r\n");
    return 0;
}
int32_t uart_protocol_set_rx_callback(struct uart_protocol *const self,
                                      uint32_t ID,
                                      const uint8_t *data,
                                      uint16_t *len,
                                      void *arg)
{
    return 0;
}
int32_t uart_protocol_get_rx_callback(struct uart_protocol *const self,
                                      uint32_t ID,
                                      uint8_t *data,
                                      uint16_t *len,
                                      void *arg)
{
    uint8_t cmd = data[0];
    switch (cmd)
    {
    case 0x01:
        for (uint8_t i = 1; i < 128; i++)
        {
            data[i] = i;
        }
        *len = 128;
        break;
    default:
        break;
    }

    return 0;
}

static void uart_test_recv_thread(void *argument)
{
    uint8_t data[UART_PROTOCOL_DATA_MAX_LENGTH] = {0x00};
    uint16_t len = UART_PROTOCOL_DATA_MAX_LENGTH;
    uint32_t timeout = 3000;
    int32_t ret = 0;
    ret = uart_protocol_rx_RegisterCallback(&uart_protocol,
                                            UART_PROTOCOL_CONFIG_SET_RX_CB_ID,
                                            uart_protocol_config_rx_callback,
                                            NULL);
    ret = uart_protocol_rx_RegisterCallback(&uart_protocol,
                                            UART_PROTOCOL_SET_RX_CB_ID,
                                            uart_protocol_set_rx_callback,
                                            NULL);
    ret = uart_protocol_rx_RegisterCallback(&uart_protocol,
                                            UART_PROTOCOL_GET_RX_CB_ID,
                                            uart_protocol_get_rx_callback,
                                            NULL);
    ret = uart_protocol_rx_RegisterCallback(&uart_protocol,
                                            UART_PROTOCOL_HEARTBEAT_RX_CB_ID,
                                            uart_protocol_heartbeat_rx_callback,
                                            NULL);
    ret = uart_protocol_rx_RegisterCallback(&uart_protocol,
                                            UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID,
                                            uart_protocol_heartbeat_rx_timeout_callback,
                                            NULL);
    ret = uart_protocol_tx_RegisterCallback(&uart_protocol,
                                            UART_PROTOCOL_HEARTBEAT_TX_CB_ID,
                                            uart_protocol_heartbeat_tx_callback,
                                            NULL);
    ret = uart_protocol_open(&uart_protocol);
    if (ret != 0)
    {
        LOG_E("uart_protocol_open error: %d\r\n", ret);
        goto thread_exit;
    }
    LOG_I("uart_test_recv_thread start\r\n");
    for (;;)
    {
        ret = uart_protocol_recv(&uart_protocol, data, len, timeout);
        if (ret != 0)
        {
            LOG_E("uart_test_recv_thread error: %d\r\n", ret);
            continue;
        }
    }
thread_exit:
    osThreadExit();
}

int uart_test(void)
{
    device_err_t err = DEV_EOK;
    int32_t ret = 0;
    osThreadAttr_t thread_attributes = {
        .name = "uart_test_send_thread",
        .stack_size = 1024 * 4,
        .priority = (osPriority_t)osPriorityAboveNormal,
    };

    ret = uart_protocol_init(&uart_protocol,
                             UART_DEV_NAME_UART5,
                             1000,
                             10000,
                             10000);
    if (ret != 0)
    {
        LOG_E("uart_protocol_init error: %d\r\n", ret);
        return -1;
    }
    osThreadId_t thread_id;
    thread_id = osThreadNew(uart_test_send_thread, &uart_protocol, &thread_attributes);
    if (thread_id == NULL)
    {
        return -4;
    }

    thread_attributes.name = "uart_test_recv_thread";
    thread_id = osThreadNew(uart_test_recv_thread, &uart_protocol, &thread_attributes);
    if (thread_id == NULL)
    {
        return -5;
    }
    return 0;
}

INIT_COMPONENT_EXPORT(uart_test);
#endif