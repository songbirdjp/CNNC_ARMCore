#include "rtm_app.h"
#include "bgm_uart.h"
#include "ulog.h"
#include "init_call.h"

struct uart_cmd_set_get
{
    uint32_t id;
    uint8_t cmd;
    uint16_t len;
    uint8_t buf[UART_FRAME_SIZE_MAX];
};

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

static int8_t rtm_cmd_write(struct uart_cmd_set_get *obj)
{
    int8_t ret = 0;

    memmove(obj->buf + 5, obj->buf, obj->len);
    memcpy(obj->buf, &obj->id, sizeof(obj->id));
    obj->buf[4] = obj->cmd;
    obj->len += 5;

    struct cmd_object cmd = {0};
    cmd.id.byte = 0;
    cmd.type = 0;
    cmd.len = &obj->len;
    cmd.data = obj->buf;

    ret = uart_cmd_write(BGM_UART_RTM, &cmd);
    if (ret != 0)
    {
        LOG_E("uart cmd write err: %d\r\n", ret);
        return -4;
    }

    return 0;
}

static int8_t rtm_cmd_parse(enum uart_id id, struct cmd_object *cmd)
{
    int8_t ret = 0;
    uint32_t remote_id = cmd->id.byte;

#if 0
    LOG_I("recv %d data: ", remote_id);
    for (uint16_t i = 0; i < *cmd->len; i++)
    {
        LOG_I("%02x ", cmd->data[i]);
    }
    LOG_I("\r\n");
#endif

    if (*cmd->len == 0) /* cmd feedback */
    {
        return 0;
    }

    if (remote_id != 0x00 && (remote_id & RS422_BUS_MODULE_ID_BGM) != RS422_BUS_MODULE_ID_BGM)
    {
        LOG_E("cmd id err: %d\r\n", remote_id);
        return -1;
    }

    switch (cmd->type)
    {
    case UART_CMD_DATA_SET:
        switch (cmd->data[0])
        {
        case UART_DATA_CMD_RECV_BEAM_ID:
            break;
        case UART_DATA_CMD_RECV_RADIATION_INDEX:
            break;
        case UART_DATA_CMD_RECV_FSM_STATE:
            break;
        case UART_DATA_CMD_RECV_GMM:
            break;
        default:
            ret = -2;
            LOG_E("invalid sub cmd type: %d\r\n", cmd->data[0]);
            break;
        }
        break;
    case UART_CMD_DATA_GET:
        break;
    default:
        ret = -3;
        LOG_E("invalid cmd type: %d\r\n", cmd->type);
        break;
    }

    return ret;
}


static int8_t rtm_functions_init(void)
{
    int8_t ret = 0;

    ret = uart_cmd_parse_callback_register(BGM_UART_RTM, rtm_cmd_parse);
    if (ret != 0)
    {
        LOG_E("dose cmd parse callback register err: %d\r\n", ret);
        return -1;
    }

    ret = uart_init_callback_register(BGM_UART_RTM, NULL);
    if (ret != 0)
    {
        LOG_E("dose init callback register err: %d\r\n", ret);
        return -2;
    }

    return 0;
}
INIT_ENV_EXPORT(rtm_functions_init);


int8_t cmd_to_rtm_upload(uint32_t id, enum uart_subcmd_type type, uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;

    struct uart_cmd_set_get msg = 
    {
        .id = RS422_BUS_MODULE_ID_LOCAL | id,
        .cmd = type,
        .len = len,
    };

    memcpy(msg.buf, buf, len);

    ret = rtm_cmd_write(&msg);
    if (ret != 0)
    {
        LOG_E("uart send msg err: %d\r\n", ret);
    }

    return ret;
}


#ifndef RTM_APP_TEST
#include "shell.h"
static int8_t rtm_app_test(uint8_t argc, char *argv[])
{
    int8_t ret = 0;
    uint8_t buf[30] = {0};

    for (uint16_t i = 0; i < 24; i++)
    {
        buf[i] = i;
    }

    return cmd_to_rtm_upload(RS422_BUS_MODULE_ID_RTM_ON_PLC, UART_DATA_CMD_SEND_FSM_STATE, buf, 24);
}
MSH_CMD_EXPORT_ALIAS(rtm_app_test, rtm_app_test, rtm app test);
#endif