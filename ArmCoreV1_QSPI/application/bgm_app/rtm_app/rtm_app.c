#include "rtm_app.h"
#include "bgm_uart.h"
#include "ulog.h"
#include "init_call.h"
#include "bgm_app.h"
#include "plan_data.h"
#include "bgm_def.h"
#include "dose_error.h"
#include "bgm_error.h"

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
            {
                struct bgm_data_info *obj = bgm_data_info_get();
                osMutexAcquire(obj->mutex, osWaitForever);
                obj->beam_id = cmd->data[1];
                osMutexRelease(obj->mutex);
            }
            break;
        case UART_DATA_CMD_RECV_RADIATION_INDEX:
            {
                static uint16_t last_radiation_index = 0;
                struct bgm_data_info *obj = bgm_data_info_get();
                osMutexAcquire(obj->mutex, osWaitForever);
                if (obj->fsm_state != BGM_STATE_WORK && obj->fsm_state != BGM_STATE_COMPLETE)
                {
                    obj->radiation_index = cmd->data[1] | cmd->data[2] << 8;
                }
                else
                {
                    switch (obj->deliver_type)
                    {
                    case DELIVER_TYPE_VMAT:
                    case DELIVER_TYPE_HiMAT:
                    case DELIVER_TYPE_SURVIEW:
                    case DELIVER_TYPE_CT:
                    case DELIVER_TYPE_SSIMRT:
                        obj->radiation_index = cmd->data[1] | cmd->data[2] << 8;
                        if (last_radiation_index != obj->radiation_index)
                        {
                            last_radiation_index = obj->radiation_index;

                            ret = dose_radiation_index_set(BGM_UART_DOSE1, last_radiation_index, 0);
                            ret |= dose_radiation_index_set(BGM_UART_DOSE2, last_radiation_index, 0);
                        }
                        break;
                    case DELIVER_TYPE_SWIMRT:
                    case DELIVER_TYPE_CRT:
                        /* radiation index is updated by dose board */
                        obj->radiation_index = dose_radiation_index_get(BGM_UART_DOSE1);
                        break;
                    default:
                        LOG_E("invalid deliver type: %d\r\n", obj->deliver_type);
                        break;
                    }
                }
                osMutexRelease(obj->mutex);
            }
            break;
        case UART_DATA_CMD_RECV_FAULT_CLEAR:
            if (cmd->data[1] == 1)
            {
                ret = dose_fault_clear(BGM_UART_DOSE1);
                ret |= dose_fault_clear(BGM_UART_DOSE2);
                ret |= dose_err_info_clear();
                ret |= bgm_error_info_clear();
            }
            break;
        case UART_DATA_CMD_RECV_SYSTEM_FSM_STATE:
            /* TODO */
            break;
        case UART_DATA_CMD_RECV_FSM_STATE:
            {
                struct bgm_data_info *obj = bgm_data_info_get();
                osMutexAcquire(obj->mutex, osWaitForever);
                obj->interlock_override = cmd->data[3] | cmd->data[4] << 8 | cmd->data[5] << 16 | cmd->data[6] << 24;
                obj->unready_override = cmd->data[7] | cmd->data[8] << 8 | cmd->data[9] << 16 | cmd->data[10] << 24;
                osMutexRelease(obj->mutex);
            }
            break;
        case UART_DATA_CMD_RECV_GMM:
            break;
        case UART_DATA_CMD_RECV_STATE_SYNC:
            ret = dose_radiation_enable_set(BGM_UART_DOSE1, &cmd->data[1]);
            ret |= dose_radiation_enable_set(BGM_UART_DOSE2, &cmd->data[1]);
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