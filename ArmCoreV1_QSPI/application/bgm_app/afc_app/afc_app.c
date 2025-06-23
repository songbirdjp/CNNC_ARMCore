#include "afc_app.h"
#include <stddef.h>
#include "bgm_uart.h"
#include "ulog.h"
#include "init_call.h"

struct afc_info_t
{
    uint8_t hw_version;
    uint8_t sw_version[6];

    uint16_t encoder_value; /* mag encoder value */

    osMutexId_t mutex;
};

static struct afc_info_t afc_info = {0};
static struct afc_info_t *afc_info_obj_get(void)
{
    return &afc_info;
}

static int8_t afc_handshake_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;
    struct afc_info_t *info = afc_info_obj_get();

    osMutexAcquire(info->mutex, osWaitForever);

    info->hw_version = cmd->data[1];
    info->sw_version[0] = cmd->data[2] + 0x30;
    info->sw_version[1] = '.';
    info->sw_version[2] = cmd->data[3] + 0x30;
    info->sw_version[3] = '.';
    info->sw_version[4] = cmd->data[4] + 0x30;

    osMutexRelease(info->mutex);

    LOG_I("AFC hw version: %#.2x\r\n", info->hw_version);
    LOG_I("AFC sw version: %s\r\n", info->sw_version);

    return ret;
}

static int8_t afc_command_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;
    uint16_t offset = 0;
    struct afc_info_t *info = afc_info_obj_get();

    osMutexAcquire(info->mutex, osWaitForever);

    switch (cmd->data[0])   /* first cmd */
    {
    case 0x40:
        switch (cmd->data[1])
        {
        case 0x01:
            LOG_I("set afc position: %u\r\n", cmd->data[2] | cmd->data[3] << 8);
            break;
        case 0x02:
            break;
        case 0x03:
            info->encoder_value = cmd->data[2] | cmd->data[3] << 8;
            // LOG_I("Encoder value: %d\r\n", info->encoder_value);
            break;
        case 0x04:
            LOG_I("set afc preset position: %u\r\n", cmd->data[2] | cmd->data[3] << 8);
            break;
        default:
            ret = -1;
            break;
        }
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

    osMutexRelease(info->mutex);

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
        LOG_E("invalid cmd id: %d\r\n", cmd->id.bits.cmd_id);
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

    osMutexAttr_t mutex_attributes = {
    .name = "afc_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    afc_info_obj_get()->mutex = osMutexNew(&mutex_attributes);
    if (afc_info_obj_get()->mutex == NULL)
    {
        LOG_E("afc mutex create failedd\r\n");
        return -1;
    }

    ret = uart_cmd_parse_callback_register(BGM_UART_AFC, afc_cmd_parse);
    if (ret != 0)
    {
        LOG_E("afc cmd parse callback register err: %d\r\n", ret);
        return -2;
    }

    ret = uart_init_callback_register(BGM_UART_AFC, NULL);
    if (ret != 0)
    {
        LOG_E("afc init callback register err: %d\r\n", ret);
        return -3;
    }

    return 0;
}
INIT_ENV_EXPORT(afc_functions_init);

float afc_info_get(enum afc_info_index index, void *data)
{
    float value = 0.0f;
    struct afc_info_t *info = afc_info_obj_get();

    osMutexAcquire(info->mutex, osWaitForever);

    switch (index)
    {
    case AFC_INFO_VERSION:
        memcpy(data, &info->hw_version, 6);
        break;
    case AFC_INFO_MAG_POSITION:
        value = info->encoder_value;
        break;

    default:
        break;
    }

    osMutexRelease(info->mutex);

    return value;
}