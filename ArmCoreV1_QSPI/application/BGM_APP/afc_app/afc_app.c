#include "afc_app.h"
#include <stddef.h>
#include "bgm_uart.h"
#include "ulog.h"
#include "init_call.h"

struct afc_info_t
{
    uint8_t hw_version;
    uint8_t sw_version[6];
};

static struct afc_info_t afc_info = {0};

static int8_t afc_handshake_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;

    LOG_I("AFC Handshake success\r\n");
    afc_info.hw_version = cmd->data[1];
    afc_info.sw_version[0] = cmd->data[2];
    afc_info.sw_version[1] = '.';
    afc_info.sw_version[2] = cmd->data[3];
    afc_info.sw_version[3] = '.';
    afc_info.sw_version[4] = cmd->data[4];
    LOG_I("AFC hw version: %d\r\n", afc_info.hw_version);
    LOG_I("AFC sw version: %s\r\n", afc_info.sw_version);

    return ret;
}

static int8_t afc_command_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;
    uint16_t offset = 0;

    switch (cmd->data[0])   /* first cmd */
    {
    case 0x40:
        break;
    case 0x41:
        break;
    case 0x42:
        break;
    case 0x60:
        switch (cmd->data[1])
        {
        case 0x05:
            for (int i = 2; i <= 18; i++) 
            {
                LOG_I("Data[%d]: %x\r\n", i, cmd->data[i]);
            }
            // ret = flash->write(flash, offset, &cmd->data[2], 16, 1000);
            // if(ret != 0)
            // {
            //     LOG_I("flash write err: %d\r\n", ret);
            // }
            offset += 16;
            break;
        default:
            ret = -1;
            break;
        }
        break;
    default:
        break;
    }

    return ret;
}

static int8_t afc_realtime_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;

    return ret;
}

static int8_t afc_cmd_parse(enum uart_id id, struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        LOG_E("cmd is NULL\r\n");
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.bits.cmd_id != BGM_UART_ID)
    {
        return 0;
    }

    int8_t ret = 0;

    /* 2. parse cmd type */
    switch (cmd->type)
    {
    case 0x81:  /* handshake frame */
        ret = afc_handshake_frame_parse(cmd);
        if (ret != 0)
        {
            LOG_E("afc_handshake_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x82:  /* command frame */
        ret = afc_command_frame_parse(cmd);
        if (ret != 0)
        {
            LOG_E("afc_command_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x83:  /* realtime frame */
        ret = afc_realtime_frame_parse(cmd);
        if (ret != 0)
        {
            LOG_E("afc_realtime_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    default:
        LOG_E("invalid cmd type: %d\r\n", cmd->type);
        return -2;
        break;
    }

    return ret;
}

static int8_t afc_functions_init(void)
{
    int8_t ret = 0;

    ret = uart_cmd_parse_callback_register(BGM_UART_AFC, afc_cmd_parse);
    if (ret != 0)
    {
        LOG_E("afc cmd parse callback register err: %d\r\n", ret);
        return -1;
    }

    ret = uart_init_callback_register(BGM_UART_AFC, NULL);
    if (ret != 0)
    {
        LOG_E("afc init callback register err: %d\r\n", ret);
        return -2;
    }

    return 0;
}
INIT_ENV_EXPORT(afc_functions_init);