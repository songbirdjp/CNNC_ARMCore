#include "bgm_uart.h"
#include "init_call.h"
#include "cmsis_os2.h"
#include "ulog.h"

static int8_t (*uart_cmd_parse[BGM_UART_MAX])(enum uart_id id, struct cmd_object *cmd) = {NULL};

int8_t uart_cmd_parse_callback_register(enum uart_id id, int8_t (*callback)(enum uart_id id, struct cmd_object *cmd))
{
    if (id >= BGM_UART_MAX)
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
        return uart_cmd_parse[id](id, cmd);
    }

    return 0;
}

static int8_t uart_init(enum uart_id id)
{
    if (id >= BGM_UART_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    int8_t ret = device_uart_init(id);
    if (ret != 0)
    {
        LOG_E("uart[%d] init err: %d\r\n", id, ret);
        return -2;
    }

    ret = device_uart_open(id);
    if (ret != 0)
    {
        LOG_E("uart[%d] open err: %d\r\n", id, ret);
        return -3;
    }

    return 0;
}

static int8_t uart_cmd_process(enum uart_id id, struct bgm_uart *buf)
{
    if (id >= BGM_UART_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    if (buf == NULL)
    {
        LOG_E("buf is NULL\r\n");
        return -2;
    }

#if 0
    LOG_I("recv_buf len: %d\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        LOG_I("%02x ", buf->buf[i]);
    }
    LOG_I("\r\n");
#endif

    struct cmd_object cmd = {0};

    if (id == BGM_UART_EPS || id == BGM_UART_VPS)
    {
        cmd.id.byte = buf->buf[0];
        cmd.type = buf->buf[1];
        cmd.len = buf->len - 2;
        cmd.data = &buf->buf[2];
#if 0
        for (uint8_t i = 0; i < cmd.len; i++)
        {
            LOG_I("%02x ", cmd.data[i]);
        }
        LOG_I("\r\n");
#endif
    }
    else
    {
        memcpy(&cmd, buf->buf, sizeof(struct cmd_object));
        cmd.data = &buf->buf[sizeof(struct cmd_object) - sizeof(uint8_t *)];
    }

    return uart_cmd_parse_func(id, &cmd);
}

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

osMessageQueueId_t uart_send_queue[BGM_UART_MAX] = {NULL};
int8_t uart_cmd_write(enum uart_id id, struct cmd_object *cmd)
{
    if (id >= BGM_UART_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    osStatus_t stat = osOK;
    struct bgm_uart send_buf = {0};

    if (id == BGM_UART_EPS || id == BGM_UART_VPS)
    {
        send_buf.buf[0] = cmd->id.byte;
        send_buf.buf[1] = cmd->type;

        memcpy(&send_buf.buf[2], cmd->data, cmd->len);
        uint16_t crc = modbus_crc16_cal(send_buf.buf, 2 + cmd->len);
        send_buf.buf[2 + cmd->len] = crc >> 8;
        send_buf.buf[3 + cmd->len] = crc & 0xFF;
        send_buf.len = 2 + cmd->len + sizeof(uint16_t);
    }
    else
    {
        uint8_t offset = sizeof(struct cmd_object) - sizeof(uint8_t *);

        memcpy(send_buf.buf, cmd, sizeof(struct cmd_object));
        memcpy(&send_buf.buf[offset], cmd->data, cmd->len);
        send_buf.len = offset + cmd->len;
    }

    stat = osMessageQueuePut(uart_send_queue[id], &send_buf, 0, 1000);
    if (stat != osOK)
    {
        LOG_E("dose uart send queue put err: %d\r\n", stat);
        return -2;
    }

    return 0;
}

int8_t uart_modbus_cmd_write(enum uart_id id, struct modbus_cmd_object *buf)
{
    if (id >= BGM_UART_MAX)
    {
        LOG_E("invalid uart id: %d\r\n", id);
        return -1;
    }

    int8_t ret = 0;

    ret = uart_cmd_write(id, buf);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -2;
    }

    return 0;
}

static int8_t (*uart_init_callback[BGM_UART_MAX])(void) = {NULL};
int8_t uart_init_callback_register(enum uart_id id, int8_t (*callback)(void))
{
    if (id >= BGM_UART_MAX)
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
        return uart_init_callback[id]();
    }

    return 0;
}

static int8_t uart_recv_entry(void *argument)
{
    int8_t ret = 0;
    struct bgm_uart recv_buf = {0};
    enum uart_id uart_id = *(enum uart_id *)argument;

    ret = uart_init(uart_id);
    if (ret != 0)
    {
        LOG_E("uart init err: %d\r\n", ret);
        return -1;
    }

    for (;;)
    {
        ret = device_uart_data_read(uart_id, &recv_buf, osWaitForever);
        if (ret != 0)
        {
            LOG_E("uart[%d] data read err: %d\r\n", uart_id, ret);
            continue;
        }

        ret = uart_cmd_process(uart_id, &recv_buf);
        if (ret!= 0)
        {
            LOG_E("uart cmd process err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t uart_send_entry(void *argument)
{
    int8_t ret = 0;
    struct bgm_uart send_buf = {0}, recv_buf = {0};
    enum uart_id uart_id = *(enum uart_id *)argument;

    ret = uart_init(uart_id);
    if (ret != 0)
    {
        LOG_E("uart[%d] init err: %d\r\n", uart_id, ret);
        return -1;
    }

    ret = uart_init_func(uart_id);
    if (ret != 0)
    {
        LOG_E("uart[%d] init function err: %d\r\n", uart_id, ret);
        return -2;
    }

    for (;;)
    {
        osMessageQueueGet(uart_send_queue[uart_id], &send_buf, NULL, osWaitForever);

#if 0
        LOG_I("[uart%d] send_buf len: %d -> ", uart_id, send_buf.len);
        for (uint8_t i = 0; i < send_buf.len; i++)
        {
            LOG_I("%02x ", send_buf.buf[i]);
        }
        LOG_I("\r\n");
#endif

send_data:
        #define TIMEOUT_MS  1000
        ret = device_uart_data_write(uart_id, &send_buf, send_buf.len, TIMEOUT_MS);
        if (ret != 0)
        {
            LOG_E("uart[%d] data write err: %d\r\n", uart_id, ret);
        }

        ret = device_uart_data_read(uart_id, &recv_buf, TIMEOUT_MS);
        if (ret != 0)
        {
            LOG_E("uart[%d] data read err: %d\r\n", uart_id, ret);
            continue;// goto send_data;
        }

        ret = uart_cmd_process(uart_id, &recv_buf);
        if (ret!= 0)
        {
            LOG_E("uart[%d] cmd process err: %d\r\n", uart_id, ret);
        }
    }

    return 0;
}

static int8_t bgm_uart_thread_init(void)
{
    static enum uart_id uart_id[BGM_UART_MAX] = {BGM_UART_AFC, BGM_UART_DOSE1, BGM_UART_DOSE2, BGM_UART_EPS, BGM_UART_VPS};

    osThreadAttr_t thread_attr = {
    .name = "bgm_uart_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityAboveNormal7,
    };

    // osThreadId_t thread_id = osThreadNew(bgm_uart_recv_entry, NULL, &thread_attr);
    // if (thread_id == NULL)
    // {
    //     LOG_E("thread dose uart create failed\r\n");
    //     return -1;
    // }

    // thread_id = osThreadNew(bgm_uart_recv_entry, NULL, &thread_attr);
    // if (thread_id == NULL)
    // {
    //     LOG_E("thread dose uart create failed\r\n");
    //     return -1;
    // }

    // thread_id = osThreadNew(bgm_uart_recv_entry, NULL, &thread_attr);
    // if (thread_id == NULL)
    // {
    //     LOG_E("thread dose uart create failed\r\n");
    //     return -1;
    // }

    osThreadId_t thread_id = NULL;

    for (uint8_t i = BGM_UART_AFC; i < BGM_UART_MAX; i++)
    {
        uart_send_queue[i] = osMessageQueueNew(16, sizeof(struct bgm_uart), NULL);
        if (uart_send_queue[i] == NULL)
        {
            LOG_E("message queue create failed\r\n");
            return -1;
        }

        thread_id = osThreadNew(uart_send_entry, &uart_id[i], &thread_attr);
        if (thread_id == NULL)
        {
            LOG_E("thread uart send create failed\r\n");
            return -2;
        }
    }

    return 0;
}
INIT_APP_EXPORT(bgm_uart_thread_init);


#ifdef BGM_UART_TEST
#include "shell.h"

static int8_t bgm_uart_cmd_send(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct cmd_object cmd = {0};
    uint8_t data[10] = {0};

    #define CMD_ID_AFC      0
    #define CMD_ID_DOSE1    0
    #define CMD_ID_DOSE2    0

    cmd.id.bits.cmd_id = CMD_ID_AFC;
    cmd.id.bits.cmd_ack = 1;
    cmd.type = 0x40;
    cmd.len = 10;
    cmd.data = data;

    for (uint8_t i = 0; i < cmd.len; i++)
    {
        cmd.data[i] = i;
    }

    ret = uart_cmd_write(BGM_UART_DOSE1, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_uart_cmd_send, bgm_uart_cmd_send, send bgm uart cmd);

static int8_t bgm_uart_modbus_cmd_vps(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct cmd_object cmd = {0};
    uint8_t data[10] = {0x01, 0xF4, 0x00, 0x01};

    cmd.id.byte = DEVICE_ADDRESS_VPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;

    ret = uart_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 1. set 205 is remote */
    cmd.type = WRITE_SINGLE_REGISTER;
    cmd.len = 4;
    cmd.data = data;
    data[0] = 0x00;
    data[1] = 0xCC;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 2. set 203 is remote */
    cmd.type = WRITE_SINGLE_REGISTER;
    cmd.len = 4;
    cmd.data = data;
    data[0] = 0x00;
    data[1] = 0xCA;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 3. set 207 is remote voltage */
    // cmd.type = WRITE_SINGLE_REGISTER;
    // cmd.len = 4;
    // cmd.data = data;
    // data[0] = 0x00;
    // data[1] = 0xCE;
    // data[2] = 0x0F;
    // data[3] = 0xA0;

    // ret = uart_cmd_write(BGM_UART_VPS, &cmd);
    // if (ret != 0)
    // {
    //     LOG_E("uart cmd write err: %d\r\n", ret);
    //     return -1;
    // }

    /* 4. set 202 is remote start */
    cmd.type = WRITE_SINGLE_REGISTER;
    cmd.len = 4;
    cmd.data = data;
    data[0] = 0x00;
    data[1] = 0xC9;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_uart_modbus_cmd_vps, bgm_uart_modbus_cmd_vps, vps bgm uart modbus cmd);

static int8_t bgm_uart_modbus_cmd_eps(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct cmd_object cmd = {0};
    uint8_t data[10] = {0x01, 0x2C, 0x00, 0x01};

    cmd.id.byte = DEVICE_ADDRESS_EPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;

    ret = uart_cmd_write(BGM_UART_EPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 1. read 001: status */
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_cmd_write(BGM_UART_EPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 2. read 002: voltage */
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;
    data[0] = 0x00;
    data[1] = 0x01;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_cmd_write(BGM_UART_EPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 3. read 003: current */
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;
    data[0] = 0x00;
    data[1] = 0x02;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_cmd_write(BGM_UART_EPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_uart_modbus_cmd_eps, bgm_uart_modbus_cmd_eps, eps bgm uart modbus cmd);
#endif