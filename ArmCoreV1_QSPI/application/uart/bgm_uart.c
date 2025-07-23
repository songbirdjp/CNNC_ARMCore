#include "bgm_uart.h"
#include "cmsis_os2.h"
#include "timestamp.h"
#include "sys_cfg.h"
#include "ulog.h"
#include "init_call.h"

uint16_t modbus_crc16_cal(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    uint16_t i, j;

    for (i = 0; i < length; i++)
    {
        crc ^= data[i];

        for (j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc = (crc >> 1) ^ 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return (crc >> 8 | crc << 8);
}

static int8_t (*uart_cmd_parse[BGM_UART_MAX + UART_DEV_MAX])(enum uart_id id, struct cmd_object *cmd) = {NULL};

int8_t uart_cmd_parse_callback_register(enum uart_id id, int8_t (*callback)(enum uart_id id, struct cmd_object *cmd))
{
    if (id >= BGM_UART_MAX + UART_DEV_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    uart_cmd_parse[id] = callback;

    return 0;
}
static int8_t uart_cmd_parse_func(enum uart_id id, struct cmd_object *cmd)
{
    if (id >= BGM_UART_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    if (uart_cmd_parse[id] != NULL)
    {
        if (id == BGM_UART_EPS_VPS)
        {
            switch (cmd->id.byte)
            {
            case 0x01:  /* eps */
                return uart_cmd_parse[id](id, cmd);
                break;
            case 0x02:  /* vps */
                return uart_cmd_parse[id + 1](id, cmd);
                break;
            default:
                LOG_E("invalid modbus addr: %d\r\n", cmd->id.byte);
                return -2;
                break;
            }
        }
        return uart_cmd_parse[id](id, cmd);
    }

    return 0;
}

static int8_t (*uart_init_callback[BGM_UART_MAX + UART_DEV_MAX])(void) = {NULL};
int8_t uart_init_callback_register(enum uart_id id, int8_t (*callback)(void))
{
    if (id >= BGM_UART_MAX + UART_DEV_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    uart_init_callback[id] = callback;

    return 0;
}
static int8_t uart_init_func(enum uart_id id)
{
    if (id >= BGM_UART_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    if (uart_init_callback[id] != NULL)
    {
        uart_init_callback[id]();
    }

    if (id == BGM_UART_EPS_VPS)
    {
        if (uart_init_callback[id + 1] != NULL)
        {
            uart_init_callback[id + 1]();
        }
    }

    return 0;
}

static int8_t uart_cmd_process(enum uart_id id, struct cmd_object *obj)
{
    if (id >= BGM_UART_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    if (obj == NULL)
    {
        LOG_E("[%d]: obj is NULL\r\n", id);
        return -2;
    }

#if 0
    LOG_I("[%d]: recv id: %d, type: %d, len: %d\r\n", id, obj->id.byte, obj->type, *obj->len);
    for (uint8_t i = 0; i < *obj->len; i++)
    {
        LOG_I("%02x ", obj->data[i]);
    }
    LOG_I("\r\n");
#endif

    struct cmd_object cmd = {0};

    switch (id)
    {
    case BGM_UART_AFC:
    case BGM_UART_DOSE1:
    case BGM_UART_DOSE2:
        cmd.id.byte = obj->data[1];
        cmd.type = obj->data[2];
        cmd.len = (uint16_t *)&obj->data[3];
        cmd.data = obj->data + 5;

        if (cmd.id.bits.cmd_ack != 0)   /* need ack */
        {
            obj->data[1] &= ~(1 << 7);
            obj->data[2] |= 0x80;
            *obj->len = *cmd.len + 5;
        }
        break;
    case BGM_UART_RTM:
        memcpy(&cmd, obj, sizeof(struct cmd_object));
        break;
    case BGM_UART_EPS_VPS:
        *obj->len -= 2;
        memcpy(&cmd, obj, sizeof(struct cmd_object));
        break;
    default:
        LOG_E("invalid uart id: %d\r\n", id);
        return -3;
        break;
    }

    return uart_cmd_parse_func(id, &cmd);
}

/* ***************************************************************************************************************************************** */


enum uart_cmd_type
{
    UART_CMD_HEAERBEAT = 1,
    UART_CMD_TIME_SYNC,
    UART_CMD_PARA_SET,
    UART_CMD_PARA_GET,
    UART_CMD_DATA_SET,
    UART_CMD_DATA_GET,
    UART_CMD_PARA_SET_ACK = 0x80 | UART_CMD_PARA_SET,
    UART_CMD_PARA_GET_ACK = 0x80 | UART_CMD_PARA_GET,
    UART_CMD_DATA_SET_ACK = 0x80 | UART_CMD_DATA_SET,
    UART_CMD_DATA_GET_ACK = 0x80 | UART_CMD_DATA_GET,
    UART_CMD_SYSTEM_RESRT = 0xEB,
};

static osEventFlagsId_t uart_rx_event_id[UART_PROTOCOL_NUM] = {NULL};
static osMessageQueueId_t uart_send_queue[BGM_UART_MAX] = {NULL};
#define UART_RX_HEARTBEAT_TIMEOUT_EVENT (1 << 0)
#define UART_RX_HEARTBEAT_CMD_EVENT     (1 << 1)
#define UART_RX_REBOOT_CMD_EVENT        (1 << 2)
static enum uart_id uart_protocol_id_get(struct uart_protocol *const self)
{
    uint8_t *name = self->uart_dev->name;

    if (strcmp(name, UART_DEV_NAME_UART7) == 0)
    {
        return BGM_UART_AFC;
    }
    else if (strcmp(name, UART_DEV_NAME_UART5) == 0)
    {
        return BGM_UART_DOSE1;
    }
    else if (strcmp(name, UART_DEV_NAME_USART2) == 0)
    {
        return BGM_UART_DOSE2;
    }
    else if (strcmp(name, UART_DEV_NAME_USART3) == 0)
    {
        return BGM_UART_RTM;
    }

    return BGM_UART_MAX;
}
static int8_t uart_recv_heartbeat_timeout_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return osEventFlagsSet(uart_rx_event_id[uart_protocol_id_get(self)], UART_RX_HEARTBEAT_TIMEOUT_EVENT);
}
static int8_t uart_recv_heartbeat_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return osEventFlagsSet(uart_rx_event_id[uart_protocol_id_get(self)], UART_RX_HEARTBEAT_CMD_EVENT);
}
static int8_t uart_recv_time_sync_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    uint64_t timestamp_ns = data[0] | (uint64_t)data[1] << 8 | (uint64_t)data[2] << 16 | (uint64_t)data[3] << 24 | 
                                (uint64_t)data[4] << 32 | (uint64_t)data[5] << 40 | (uint64_t)data[6] << 48 | (uint64_t)data[7] << 56;
    return timestamp_ns_set(timestamp_ns);
}
static int8_t uart_recv_set_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    struct cmd_object obj = {.id.byte = id, .type = UART_CMD_DATA_SET, .len = len, .data = data};
    return uart_cmd_process(uart_protocol_id_get(self), &obj);
}
static int8_t uart_recv_get_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    struct cmd_object obj = {.id.byte = id, .type = UART_CMD_DATA_GET, .len = len, .data = data};
    return uart_cmd_process(uart_protocol_id_get(self), &obj);
}
static int8_t uart_recv_reboot_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return osEventFlagsSet(uart_rx_event_id[uart_protocol_id_get(self)], UART_RX_REBOOT_CMD_EVENT);
}
static int8_t uart_send_heartbeat_cmd_callback(struct uart_protocol *const self, uint8_t *data, uint16_t *len, void *arg)
{
    enum uart_id id = uart_protocol_id_get(self);

    /* TODO：只对rtm发送心跳包 */

    uint8_t *fw_ver = system_info_get()->fw_version;

    *len = 5;
    data[0] = 0xFF; /* TODO: board id + hardware version */
    data[1] = 0x00; /* software version */
    data[2] = strtoul(&fw_ver[0], NULL, 10);
    data[3] = strtoul(&fw_ver[3], NULL, 10);
    data[4] = strtoul(&fw_ver[6], NULL, 10);

    return 0;
}

static int8_t uart_recv_entry(void *argument)
{
    int8_t ret = 0;
    enum uart_id id = *(enum uart_id *)argument;
    uint8_t buf[UART_FRAME_SIZE_MAX] = {0};

    // ret = uart_protocol_rx_RegisterCallback(bgm_uart_protocol_get(id), UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID, uart_recv_heartbeat_timeout_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(bgm_uart_protocol_get(id), UART_PROTOCOL_HEARTBEAT_RX_CB_ID, uart_recv_heartbeat_cmd_callback,  NULL);
    ret |= uart_protocol_rx_RegisterCallback(bgm_uart_protocol_get(id), UART_PROTOCOL_PNT_RX_CB_ID, uart_recv_time_sync_cmd_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(bgm_uart_protocol_get(id), UART_PROTOCOL_SET_RX_CB_ID, uart_recv_set_cmd_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(bgm_uart_protocol_get(id), UART_PROTOCOL_GET_RX_CB_ID, uart_recv_get_cmd_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(bgm_uart_protocol_get(id), UART_PROTOCOL_REBOOT_RX_CB_ID, uart_recv_reboot_cmd_callback, NULL);
    // ret |= uart_protocol_tx_RegisterCallback(bgm_uart_protocol_get(id), UART_PROTOCOL_HEARTBEAT_TX_CB_ID, uart_send_heartbeat_cmd_callback, NULL);
    if (ret != 0)
    {
        LOG_E("[%d]: uart_protocol_rx_RegisterCallback err: %d\r\n", id, ret);
        osThreadExit();
    }

    ret = uart_open(id);
    if (ret != 0)
    {
        LOG_E("[%d]: uart open err: %d\r\n", id, ret);
        osThreadExit();
    }

    ret = uart_init_func(id);
    if (ret != 0)
    {
        LOG_E("[%d]: init function err: %d\r\n", id, ret);
        osThreadExit();
    }

    for (;;)
    {
        ret = uart_data_recv_with_block(id, buf, sizeof(buf), osWaitForever);
        if (ret != 0)
        {
            LOG_E("[%d]: uart data recv with block err: %d\r\n", id, ret);
        }
    }

    return 0;
}

static int8_t link_status_entry(void *argument)
{
    int8_t ret = 0;
    enum uart_id id = *(enum uart_id *)argument;
    int32_t event_flags = 0;

    for (;;)
    {
        event_flags = osEventFlagsWait(uart_rx_event_id[id], UART_RX_HEARTBEAT_TIMEOUT_EVENT | UART_RX_HEARTBEAT_CMD_EVENT | UART_RX_REBOOT_CMD_EVENT, osFlagsWaitAny, osWaitForever);
        if (event_flags > 0)
        {
            if (event_flags & UART_RX_HEARTBEAT_TIMEOUT_EVENT)
            {

            }
            else if (event_flags & UART_RX_HEARTBEAT_CMD_EVENT)
            {

            }
            else if (event_flags & UART_RX_REBOOT_CMD_EVENT)
            {
                HAL_NVIC_SystemReset();
            }
        }
    }

    return 0;
}

int8_t uart_cmd_write(enum uart_id id, struct cmd_object *cmd)
{
    if (id >= BGM_UART_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    uint8_t buf[UART_FRAME_SIZE_MAX] = {0};

    buf[0] = cmd->id.byte;
    buf[1] = cmd->type;
    buf[2] = *cmd->len & 0xFF;
    buf[3] = *cmd->len >> 8;
    memcpy(&buf[4], cmd->data, *cmd->len);

    osStatus_t stat = osMessageQueuePut(uart_send_queue[id], buf, 0, 100);
    if (stat != osOK)
    {
        LOG_E("[%d]: uart send queue put err: %d\r\n", id, stat);
    }

    return stat;
}

int8_t uart_modbus_cmd_write(enum uart_id id, struct modbus_cmd_object *obj)
{
    if (id >= BGM_UART_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    int8_t ret = 0;
    uint8_t buf[UART_FRAME_SIZE_MAX] = {0};

    buf[0] = obj->addr;
    buf[1] = obj->type;
    memcpy(&buf[2], obj->data, obj->len);
    uint16_t crc = modbus_crc16_cal(buf, 2 + obj->len);
    buf[2 + obj->len] = crc >> 8;
    buf[3 + obj->len] = crc & 0xFF;
    buf[4 + obj->len] = obj->cmd_id;
    buf[5 + obj->len] = obj->cmd_id >> 8;
    uint16_t len = obj->len + 4;

    struct cmd_object cmd = {0};
    cmd.id.byte = obj->addr;
    cmd.type = obj->type;
    cmd.len = &len;
    cmd.data = &buf[2];

    ret = uart_cmd_write(id, &cmd);
    if (ret != 0)
    {
        LOG_E("[%d]: uart cmd write err: %d\r\n", id, ret);
        return -2;
    }

    return 0;
}

static int8_t uart_send_entry(void *argument)
{
    int8_t ret = 0;
    enum uart_id id = *(enum uart_id *)argument;
    uint16_t modbus_cmd_id = 0;

    uint8_t buf[UART_FRAME_SIZE_MAX] = {0};
    struct uart_data obj = {0};

    if (id == BGM_UART_EPS_VPS)
    {
        osDelay(5000);

        ret = uart_open(id);
        if (ret != 0)
        {
            LOG_E("[%d]: uart open err: %d\r\n", id, ret);
            osThreadExit();
        }

        ret = uart_init_func(id);
        if (ret != 0)
        {
            LOG_E("[%d]: init function err: %d\r\n", id, ret);
            osThreadExit();
        }
    }


    for (;;)
    {
        osMessageQueueGet(uart_send_queue[id], buf, NULL, osWaitForever);

        /* NOTE:
         * 1. 针对rtm连接，需要按照协议格式进行组包
         * 2. 针对bgm内部连接，需要将所发送数据打包到data字段中
         * 3. 针对modbus连接，需要将所发送数据打包到data字段中
         */

        switch (id)
        {
        case BGM_UART_AFC:
        case BGM_UART_DOSE1:
        case BGM_UART_DOSE2:
            obj.id = 0;
            obj.cmd = 0;
            obj.len = (buf[2] | buf[3] << 8) + 4;
            obj.data = buf;
            break;
        case BGM_UART_RTM:
            obj.id = *(uint32_t *)&buf[4];
            obj.cmd = buf[8];
            obj.len = (buf[2] | buf[3] << 8) - 5;
            obj.data = &buf[9];
            break;
        case BGM_UART_EPS_VPS:
            obj.id = 0;
            obj.cmd = 0;
            obj.len = (buf[2] | buf[3] << 8) - 2;
            memcpy(&buf[2], &buf[4], obj.len);
            obj.len += 2;
            obj.data = buf;
            modbus_cmd_id = buf[2 + obj.len] | buf[3 + obj.len] << 8;
            // LOG_I("modbus cmd id: %d\r\n", modbus_cmd_id);
            break;
        default:
            LOG_E("invalid uart id: %d\r\n", id);
            continue;
            break;
        }

#if 0
        LOG_I("current id: %d\r\n", id);
        LOG_I("send id: %d, cmd: %d, len: %d\r\n", obj.id, obj.cmd, obj.len);
        for (uint8_t i = 0; i < obj.len; i++)
        {
            LOG_I("%02x ", obj.data[i]);
        }
        LOG_I("\r\n");
#endif

repeat:
        ret = uart_data_write(id, &obj, obj.len, 100);
        if (ret != 0)
        {
            LOG_E("[%d]: uart data write err: %d\r\n", id, ret);
        }

        if (id == BGM_UART_EPS_VPS) /* modbus连接 */
        {
            #define UART_MODBUS_TIMEOUT 100
            #define UART_MODBUS_RETRY_TIMES 50
            #define UART_MODBUS_RETRY_INTERVAL 50
            static uint8_t retry_times = 0;

            ret = uart_data_recv_with_block(id, buf, sizeof(buf), UART_MODBUS_TIMEOUT);
            if (ret != 0)
            {
                LOG_E("[%d]: uart data recv with block err: %d, retry_times: %d\r\n", id, ret, retry_times);
            }
            else
            {
#if 0
                uint16_t len = *(uint16_t *)&buf[UART_FRAME_SIZE_MAX - sizeof(uint16_t)];
                LOG_I("modbus recv len: %d\r\n", len);
                for (uint8_t i = 0; i < len; i++)
                {
                    LOG_I("%02x ", buf[i]);
                }
                LOG_I("\r\n");
#endif
                struct cmd_object obj = {0};
                obj.id.byte = buf[0];
                obj.type = buf[1];
                obj.len = (uint16_t *)&buf[UART_FRAME_SIZE_MAX - sizeof(uint16_t)];
                memmove(&buf[4], &buf[2], *obj.len);
                buf[2] = modbus_cmd_id;
                buf[3] = modbus_cmd_id >> 8;
                obj.data = &buf[2];
                ret = uart_cmd_process(id, &obj);
                if (ret != 0)
                {
                    LOG_E("[%d]: uart cmd process err: %d\r\n", id, ret);
                }
            }

            if (ret != 0)
            {
                if (retry_times++ < UART_MODBUS_RETRY_TIMES)
                {
                    osDelay(UART_MODBUS_RETRY_INTERVAL);
                    goto repeat;
                }
                else
                {
                    retry_times = 0;
                }
            }
            else
            {
                retry_times = 0;
            }

            osDelay(50);
        }
    }

    return 0;
}

static int8_t uart_thread_init(void)
{
    static enum uart_id uart_id[BGM_UART_MAX] = {BGM_UART_AFC, BGM_UART_DOSE1, BGM_UART_DOSE2, BGM_UART_RTM, BGM_UART_EPS_VPS};

    osThreadId_t thread_id = NULL;
    osThreadAttr_t attr = {0};

    for (uint8_t i = 0; i < UART_PROTOCOL_NUM; i++)
    {
        uart_rx_event_id[i] = osEventFlagsNew(NULL);
        if (uart_rx_event_id[i] == NULL)
        {
            LOG_E("osEventFlagsNew err\r\n");
            return -1;
        }

        attr.name = "uart_link_status_thread";
        attr.stack_size = 256 * 4,
        attr.priority = osPriorityNormal,
        thread_id = osThreadNew(link_status_entry, &uart_id[i], &attr);
        if (thread_id == NULL)
        {
            LOG_E("thread link status create failed\r\n");
            return -2;
        }

        attr.name = "uart_recv_thread";
        attr.stack_size = 1024 * 4;
        attr.priority = osPriorityAboveNormal1;
        thread_id = osThreadNew(uart_recv_entry, &uart_id[i], &attr);
        if (thread_id == NULL)
        {
            LOG_E("thread uart recv create failed\r\n");
            return -3;
        }
    }

    for (uint8_t i = 0; i < BGM_UART_MAX; i++)
    {
        if (i < UART_PROTOCOL_NUM)
        {
            uart_send_queue[i] = osMessageQueueNew(5, UART_FRAME_SIZE_MAX, NULL);
        }
        else
        {
            uart_send_queue[i] = osMessageQueueNew(16, UART_FRAME_SIZE_MAX, NULL);
        }
        if (uart_send_queue[i] == NULL)
        {
            LOG_E("queue uart send create failed\r\n");
            return -4;
        }

        attr.name = "uart_send_thread";
        attr.stack_size = 1024 * 4;
        attr.priority = osPriorityAboveNormal;
        thread_id = osThreadNew(uart_send_entry, &uart_id[i], &attr);
        if (thread_id == NULL)
        {
            LOG_E("thread uart tx create failed\r\n");
            return -5;
        }
    }

    return 0;
}
INIT_APP_EXPORT(uart_thread_init);