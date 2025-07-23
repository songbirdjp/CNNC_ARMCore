/**
 * @file app_data_record.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-07-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "app_data_record.h"
#include "tcp_tasks.h"
#include "stdlib.h"
#include "uart_protocol.h"
#include "timestamp.h"
#include "fmc_sdram_port.h"
#include "ulog.h"
#include "rtm_main.h"

static uint8_t data_record_buf[DATA_RECORD_DATA_ITEM][DATA_RECORD_DATA_SIZE] __attribute__((section(".sdram_ext"))) = {0};

static int32_t init_data_fifo(data_fifo_t *self, uint8_t *fifo)
{
    self->fifo = fifo;
    memset(self->fifo, 0, DATA_RECORD_DATA_ITEM * DATA_RECORD_DATA_SIZE);
    memset(self->fifo_item_size, 0, DATA_RECORD_DATA_ITEM * sizeof(uint32_t));
    self->fifo_out = 0;
    self->fifo_in = 0;
    self->fifo_space = DATA_RECORD_DATA_ITEM;
    osMutexAttr_t attr = {
        .name = "data_fifo_mutex",
        .attr_bits = osMutexRecursive | osMutexPrioInherit};
    self->mutex = osMutexNew(&attr);
    if (self->mutex == NULL)
    {
        return -1;
    }
}
static int32_t data_fifo_is_empty(data_fifo_t *self)
{
    if (self == NULL)
    {
        return -1;
    }
    return (self->fifo_space == DATA_RECORD_DATA_ITEM);
}
static int32_t data_fifo_is_full(data_fifo_t *self)
{
    if (self == NULL)
    {
        return -1;
    }
    return (self->fifo_space == 0);
}

static int32_t data_fifo_put(data_fifo_t *self, uint8_t *data, uint32_t len)
{
    if (self == NULL || data == NULL || len == 0 || len > DATA_RECORD_DATA_SIZE)
    {
        return -1;
    }
    osMutexAcquire(self->mutex, osWaitForever);
    if (data_fifo_is_full(self))
    {
        self->fifo_out = (self->fifo_out + 1) % DATA_RECORD_DATA_ITEM;
        memcpy(self->fifo + (self->fifo_in * DATA_RECORD_DATA_SIZE), data, len);
    }
    else
    {
        memcpy(self->fifo + (self->fifo_in * DATA_RECORD_DATA_SIZE), data, len);
    }
    self->fifo_item_size[self->fifo_in] = len;
    self->fifo_in = (self->fifo_in + 1) % DATA_RECORD_DATA_ITEM;
    self->fifo_space--;
    osMutexRelease(self->mutex);
    return 0;
}
static int32_t data_fifo_get(data_fifo_t *self, uint8_t *data, uint32_t *len)
{
    if (self == NULL || data == NULL || len == NULL)
    {
        return -1;
    }
    osMutexAcquire(self->mutex, osWaitForever);
    if (data_fifo_is_empty(self))
    {
        osMutexRelease(self->mutex);
        return -2;
    }

    memcpy(data, self->fifo + (self->fifo_out * DATA_RECORD_DATA_SIZE), self->fifo_item_size[self->fifo_out]);
    *len = self->fifo_item_size[self->fifo_out];

    self->fifo_out = (self->fifo_out + 1) % DATA_RECORD_DATA_ITEM;
    self->fifo_space++;
    osMutexRelease(self->mutex);
    return 0;
}
#define MAX_DATA_LEN (UART_PROTOCOL_DATA_MAX_LENGTH + sizeof(uint64_t) + sizeof(uint32_t))

typedef struct
{
    uint32_t id_ack;
    uint8_t type;
    uint16_t length;
    uint8_t data[UART_PROTOCOL_DATA_MAX_LENGTH];
} __attribute__((aligned(1), packed)) module_payload_t;

typedef struct
{
    uint8_t require_state;
    uint8_t require_ctrl_mode;
    uint32_t interlock_override;
    uint32_t unready_override;
} __attribute__((aligned(1), packed)) module_state_require_t;

typedef struct
{
    uint8_t fsm_state_current;
    uint8_t ctrl_mode_cur;
    uint16_t reserved;
    uint8_t beam_ID;
    uint8_t reserved1;
    uint16_t radiation_index;
    uint32_t not_ready_event;
    uint32_t warning_interlock;
    uint32_t minor_interlock;
    uint32_t serious_interlock;
} __attribute__((aligned(1), packed)) module_current_status_t;

static module_current_status_t rtm_off_current_status = {0};
static module_current_status_t psm_current_status = {0};
static module_current_status_t gmm_current_status = {0};

static dido_structure_t  rtm_off_dido_structure = {0};
static struct
{
    uint8_t require_state;
    uint8_t require_ctrl_mode;
    uint16_t rtm_off_plc_info;
    uint32_t interlock_override;
    uint32_t unready_override;
    uint16_t led_belt;
} __attribute__((aligned(1), packed)) rtm_off_state_require = {0};
static module_state_require_t psm_state_require = {0};
static module_state_require_t gmm_state_require = {0};

#define RTM_OFF_ID_FILTER (RTM_ON_PLC_ID | RTM_ON_ARM_ID | ICM_ID | BGM_ID | QAM_ID | BSM_ID)
#define PSM_ID_FILTER (PSM_ID)
#define GMM_ID_FILTER (GMM_ID)

static int32_t app_data_record_filter(uint32_t ID, void *data, uint32_t len)
{
    if (data == NULL || len == 0)
    {
        goto error;
    }
    module_payload_t *payload = (module_payload_t *)data;
    uint8_t cmd = payload->data[0];
    if (payload->type != 0x05)
    {
        goto ret;
    }
    switch (ID)
    {
    case RTM_OFF_ID_FILTER:
    {
        switch (cmd)
        {
        case SEND_RTM_OFF_ARM_CURRENT_STATE_CMD:
            if (memcmp(&rtm_off_current_status, payload->data + 1, sizeof(module_current_status_t)) == 0)
            {
                goto ret;
            }
            memcpy(&rtm_off_current_status, payload->data + 1, sizeof(module_current_status_t));
            break;
        case RECEIVE_RTM_OFF_ARM_REQUIRE_STATE_CMD:
            if (memcmp(&rtm_off_state_require, payload->data + 1, sizeof(module_state_require_t) + sizeof(uint16_t)) == 0)
            {
                goto ret;
            }
            memcpy(&rtm_off_state_require, payload->data + 1, sizeof(module_state_require_t) + sizeof(uint16_t));
            break;
        case SEND_RTM_OFF_ARM_DIDO_CMD:
            if (memcmp(&rtm_off_dido_structure, payload->data + 1, sizeof(dido_structure_t)) == 0)
            {
                goto ret;
            }
            memcpy(&rtm_off_dido_structure, payload->data + 1, sizeof(dido_structure_t));
            break;
        default:
            goto ret;
            break;
        }
    }
    break;
    case PSM_ID_FILTER:
    {
        switch (cmd)
        {
        case SEND_PSM_CURRENT_STATE_CMD:
            if (memcmp(&psm_current_status, payload->data + 1, sizeof(module_current_status_t)) == 0)
            {
                goto ret;
            }
            memcpy(&psm_current_status, payload->data + 1, sizeof(module_current_status_t));
            break;
        case RECEIVE_PSM_REQUIRE_STATE_CMD:
            if (memcmp(&psm_state_require, payload->data + 1, sizeof(module_state_require_t)) == 0)
            {
                goto ret;
            }
            memcpy(&psm_state_require, payload->data + 1, sizeof(module_state_require_t));
            break;
        default:
            goto ret;
            break;
        }
    }
    break;
    case GMM_ID_FILTER:
    {
        switch (cmd)
        {
        case SEND_GMM_CURRENT_STATE_CMD:
            if (memcmp(&gmm_current_status, payload->data + 1, sizeof(module_current_status_t)) == 0)
            {
                goto ret;
            }
            memcpy(&gmm_current_status, payload->data + 1, sizeof(module_current_status_t));
            break;
        case RECEIVE_GMM_REQUIRE_STATE_CMD:
            if (memcmp(&gmm_state_require, payload->data + 1, sizeof(module_state_require_t)) == 0)
            {
                goto ret;
            }
            memcpy(&gmm_state_require, payload->data + 1, sizeof(module_state_require_t));
            break;
        default:
            goto ret;
            break;
        }
    }
    break;
    default:
        return -1;
        break;
    }
    return 0;
ret:
    return 1;
error:
    return -1;
}
static osThreadId_t tid;
#define TID_FLAG 0x01
int32_t app_data_record(app_data_record_t *self, uint32_t ID, void *data, uint32_t len)
{
    if (self == NULL || data == NULL || len == 0 || len > UART_PROTOCOL_DATA_MAX_LENGTH)
    {
        return -1;
    }
    uint32_t ret = app_data_record_filter(ID, data, len);
    if (ret < 0)
    {
        return -2;
    }
    if (ret > 0)
    {
        return 0;
    }
    uint8_t *pdata = (uint8_t *)data;
    uint32_t datalen = len;
    uint32_t totallen = datalen + sizeof(uint64_t) + sizeof(uint32_t);
    uint8_t *pbuf = self->txdata;
    memcpy(pbuf, &ID, sizeof(uint32_t));
    pbuf += sizeof(uint32_t);
    uint64_t timestamp = timestamp_ns_get();
    memcpy(pbuf, &timestamp, sizeof(uint64_t));
    pbuf += sizeof(uint64_t);
    memcpy(pbuf, data, datalen);

    ret = data_fifo_put(&self->data_fifo, self->txdata, totallen);
    if (ret != 0)
    {
        return -3;
    }
    osThreadFlagsSet(tid, TID_FLAG);
    return 0;
}

void app_data_record_thread(void *arg)
{
    app_data_record_t *self = (app_data_record_t *)arg;
    uint8_t dataBuf[MAX_DATA_LEN] = {0};
    uint32_t len = 0;
    int8_t sn[MAX_CLIENT_NUM] = {0};
    while (1)
    {
        uint32_t ret = osThreadFlagsWait(TID_FLAG, osFlagsWaitAny, 1000);
        // if ((ret & 0x80000000) || ((ret & TID_FLAG) == 0U))
        // {
        //     continue;
        // }
        int32_t retval = get_sn(sn, RAM_DATA);
        if (retval != 0)
        {
            continue;
        }
        while (1)
        {
            ret = data_fifo_get(&self->data_fifo, dataBuf, &len);
            if (ret != 0)
            {
                break;
            }
            for (uint8_t i = 0; i < MAX_CLIENT_NUM; i++)
            {
                if (sn[i] == -1)
                {
                    continue;
                }
                self->sn = sn[i];
                ret = ws_send(self->sn, dataBuf, len, true, false, WDT_BINDATA);
                if (ret != 0)
                {
                    continue;
                }
            }
        }
    }
}
int32_t app_data_record_init(app_data_record_t *self)
{
    if (self == NULL)
    {
        return -1;
    }
    memset(self, 0, sizeof(app_data_record_t));

    init_data_fifo(&self->data_fifo, data_record_buf);

    self->txdata = (uint8_t *)pvPortMalloc(MAX_DATA_LEN);
    if (self->txdata == NULL)
    {
        return -2;
    }
    osThreadAttr_t attr = {
        .name = "app_data_record_thread",
        .stack_size = 1024 * 4,
        .priority = osPriorityNormal,
    };
    tid = osThreadNew(app_data_record_thread, self, &attr);
    if (tid == NULL)
    {
        free(self->txdata);
        return -3;
    }
    return 0;
}