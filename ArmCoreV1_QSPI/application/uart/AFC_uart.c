#include "afc_uart.h"
#include "uart_port.h"
#include "init_call.h"
#include "sys_cfg.h"
#include "ulog.h"
#include "motorctrl.h"
#include "tim.h"
#include "AFCapp.h"

static int8_t AFC_ParaSet_parse(struct afc_object *cmd)
{
    int8_t ret = 0;
    AFCConfigParam_TypeDef *objConfig = AFC_ConfigParam_get();
    AFCApplicationParam_t *objAFCApp = AFCApplicationParamGet();
    uint16_t temp_data[16] = {0};
    switch (cmd->data[1])//0x01
    {
        case 0x00://Set AFC Control Mode
            objConfig->AFCControlmode = cmd->data[2];
            switch(objConfig->AFCControlmode)
            {
                case 0x00:
                    MagMotorState = MotorFSM_StayAtPresetPos;
                    // objAFCApp->whichData =1;
                    printf("MotorFSM_StayAtPresetPos\r\n");
                    break;
                case 0x01:
                    MagMotorState = MotorFSM_ManualControl;
                    // objAFCApp->whichData =2;
                    printf("MotorFSM_ManualControl\r\n");
                    break;
                case 0x02:
                    MagMotorState = MotorFSM_AutoControl;
                    objAFCApp->positionCalculated = __HAL_TIM_GET_COUNTER(&htim2);
                    printf("MotorFSM_AutoControl\r\n");
                    break;
                default:
                    ret = -1;
                    break;
            }
            break;
        case 0x01://Set AFC Sample Mode
            objConfig->AFCSampleMode = cmd->data[2];
            break;
        case 0x02://Set AFC Sample Delay
            objConfig->AFCSampleDelay = (cmd->data[3] << 8) | cmd->data[2];
            //LOG_E("AFC Sample Delay: %d\r\n", obj->AFCSampleDelay);
            __HAL_TIM_SET_COUNTER(&htim4, objConfig->AFCSampleDelay);
            break;
        case 0x03://delete sample data
            break;
        case 0x04:
            *cmd->len = 0x02;
            cmd->data[0] = 0x01;
            cmd->data[1] = 0x04;

            //memcpy(&cmd->data[2],AFC_ADCSampleRecvProcess(), sizeof(uint8_t) * 16 * 2);
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}

static int8_t AFC_MagMotorCmd_parse(struct afc_object *cmd)
{
    int8_t ret = 0;
    MotorCtrlParam_TypeDef *obj = MAG_motorParam_get();
    obj->encoderValCurrent = __HAL_TIM_GET_COUNTER(&htim2);
    switch (cmd->data[1])
    {
        case 0x00:// Mag Motor find zero ok ACK
            *cmd->len = 0x03;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x00;
            cmd->data[2] = obj->motorFindZeroOK;
            break;
        case 0x01://Mag Motor set position 2 bytes no ACK
            obj->encoderValTarget = (cmd->data[3] << 8) | cmd->data[2];
            *cmd->len = 0x04;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x01;
            cmd->data[2] = obj->encoderValTarget & 0xFF;
            cmd->data[3] = (obj->encoderValTarget >> 8) & 0xFF;
            // printf("obj->encoderValCurrent = %d\r\n",obj->encoderValTarget);
            break;
        case 0x02://Mag Motor run by step no ACK
            if (cmd->data[2] != 0x01 && cmd->data[2] != 0x02)
            {
                //LOG_E("Mag Motor run by step err: %d\r\n", cmd->data[0]);
                break;
            }
            uint16_t step_value = (cmd->data[4] << 8) | cmd->data[3];
            if (cmd->data[2] == 0x01)
            {
                obj->encoderValTarget += step_value;
                if(obj->encoderValCurrent> 35200)//1024*4.75*5.18 = 25195.5200;
                {
                    obj->encoderValTarget = 35200;
                }
            }
            else if (cmd->data[2] == 0x02)
            {
                obj->encoderValTarget -= step_value;
                if(obj->encoderValCurrent < 10000)
                {
                    obj->encoderValTarget = 10000;
                }
            }
            *cmd->len = 0x04;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x02;
            cmd->data[2] = obj->encoderValCurrent & 0xFF;
            cmd->data[3] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x03:
            *cmd->len = 0x04;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x03;
            cmd->data[2] = obj->encoderValCurrent & 0xFF;
            cmd->data[3] = (obj->encoderValCurrent >> 8) & 0xFF;
            // printf("obj->encoderValCurrent\r\n");
            break;
        case 0x04:
            obj->presetPos = (cmd->data[3] << 8) | cmd->data[2];
            // printf("obj->presetPos = %d\r\n",obj->presetPos);
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}

static int8_t AFC_AFTMotorCmd_parse(struct afc_object *cmd)
{
    int8_t ret = 0;
    MotorCtrlParam_TypeDef *obj = AFT_motorParam_get();
     obj->encoderValCurrent = __HAL_TIM_GET_COUNTER(&htim3);
    switch (cmd->data[1])
    {
         case 0x00:
            obj->motorBrakeStatus = cmd->data[0];
            break;
        case 0x01://enable AFT Motor to init
            obj->motorInitEnable = cmd->data[0];
            break;
        case 0x0F://enable AFT Motor to find zero
            *cmd->len = 0x01;
            cmd->data[0] = obj->motorFindZeroOK;
            break;
        case 0x03://Mag Motor set position 2 bytes no ACK
            obj->encoderValTarget = (cmd->data[0] << 8) | cmd->data[1];
            break;
        case 0x02://Mag Motor run by step no ACK
          if (cmd->data[2] != 0x01 && cmd->data[2] != 0x02)
            {
                LOG_E("AFT Motor run by step err: %d\r\n", cmd->data[0]);
                break;
            }
            uint16_t step_value = (cmd->data[4] << 8) | cmd->data[3];
            // printf("step_value = %d\r\n",step_value);
            if (cmd->data[2] == 0x01)
            {
                // obj->encoderValTarget += step_value;
                // if(obj->encoderValCurrent> 65535)//1024*4.75*5.18 = 25195.5200;
                // {
                //     obj->encoderValTarget = 65535;
                // }
                motorCtrlByPWM(MOTOR_AFT, 60);
                osDelay(200);
                motorCtrlByPWM(MOTOR_AFT, 0);
                // printf("A1111111111111\r\n");
            }
            else if (cmd->data[2] == 0x02)
            {
                // obj->encoderValTarget -= step_value;
                // if(obj->encoderValCurrent < 100)
                // {
                //     obj->encoderValTarget = 100;
                // }
                motorCtrlByPWM(MOTOR_AFT, -60);
                osDelay(200);
                motorCtrlByPWM(MOTOR_AFT, 0);
                printf("A2222222222222\r\n");
            }
            printf("obj->encoderValTarget = %d\r\n",obj->encoderValTarget);
            *cmd->len = 0x04;
            cmd->data[0] = 0x41;
            cmd->data[1] = 0x02;
            cmd->data[2] = obj->encoderValCurrent & 0xFF;
            cmd->data[3] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x05:
            *cmd->len = 0x02;
            cmd->data[0] = obj->encoderValCurrent & 0xFF;
            cmd->data[1] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x06:
            obj->presetPos = (cmd->data[0] << 8) | cmd->data[1];
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}

static int8_t AFC_ADCSampleSet_parse(struct afc_object *cmd)//0x60
{
    int8_t ret = 0;
    switch (cmd->data[1])
    {
    case 0x01:
        break;
    case 0x05:
        *cmd->len = 0x12;
        cmd->type = 0x02;
        cmd->data[0] = 0x60;
        cmd->data[1] = 0x05;
        break;
    default:
        break;
    }
   return ret;
}

static int8_t afc_handshake_frame_parse(struct afc_object *cmd)
{
    int8_t ret = 0;
    AFCConfigParam_TypeDef *obj = AFC_ConfigParam_get();
    obj->AFCHandShakeOK = 0x01;

#if 1
    LOG_I("bgm arm core handshake frame parse: %d\r\n", *cmd->len);
    LOG_I("hardware version: %#.2x\r\n", cmd->data[0]);
    LOG_I("software version: %u.%u.%u\r\n", cmd->data[1], cmd->data[2], cmd->data[3]);
    LOG_I("afc id: %u\r\n", cmd->data[4]);
#endif

    uint8_t *fw_ver = system_info_get()->fw_version;

    *cmd->len = 5;
    cmd->data[0] = 0x00;    /* unused */
    cmd->data[1] = 0x19;    /* hardware version */
    cmd->data[2] = strtoul(&fw_ver[0], NULL, 10);  /* software version */
    cmd->data[3] = strtoul(&fw_ver[3], NULL, 10);
    cmd->data[4] = strtoul(&fw_ver[6], NULL, 10);

    return ret;
}

static int8_t afc_command_frame_parse(struct afc_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])   /* first cmd */
    {
    case 0x01:
        ret = AFC_ParaSet_parse(cmd);
        break;
    case 0x40:
        ret = AFC_MagMotorCmd_parse(cmd);
        break;
    case 0x41:
        ret = AFC_AFTMotorCmd_parse(cmd);//recv original: 16 bytes 55 aa 00 00 06 00 80 02 02 00 60 05 ea 71 0b fe 
        break;
    case 0x60:
        ret = AFC_ADCSampleSet_parse(cmd);
    case 0xEE:
        HAL_NVIC_SystemReset();
        break;
    default:
        ret = -1;
        break;
    }

    if (ret != 0)
    {
        printf("AFC uart command [%.2x, %.2x] parse err: %d\r\n", cmd->data[0], cmd->data[1], ret);
    }

    return ret;
}

static int8_t afc_realtime_frame_parse(struct afc_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    default:
        LOG_I("invalid realtime frmae type: %d\r\n", cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}

static int8_t afc_cmd_parse(struct afc_object *obj)
{
    if (obj == NULL)
    {
        LOG_E("cmd is NULL\r\n");
        return -1;
    }

    /* 0. get valid data in object */
    struct afc_object cmd = {0};

    cmd.id.byte = obj->data[1];
    cmd.type = obj->data[2];
    cmd.len = (uint16_t *)&obj->data[3];
    cmd.data = obj->data + 5;

#if 0
    LOG_I("id: %02x\r\n", cmd.id.byte);
    LOG_I("type: %02x\r\n", cmd.type);
    LOG_I("len: %d\r\n", *cmd.len);
    for (uint8_t i = 0; i < *(cmd.len); i++)
    {
        LOG_I("%02x ", cmd.data[i]);
    }
    LOG_I("\r\n");
#endif

    /* 1. check cmd id */
    if (cmd.id.bits.cmd_id != AFC_UART_ID)
    {
        LOG_E("cmd id err: %d\r\n", cmd.id.bits.cmd_id);
        return 0;
    }

    int8_t ret = 0;

    /* 2. parse cmd type */
    switch (cmd.type)
    {
    case 0x01:  /* handshake frame */
        ret = afc_handshake_frame_parse(&cmd);
        if (ret != 0)
        {
            LOG_E("afc_handshake_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x02:  /* command frame */
        ret = afc_command_frame_parse(&cmd);
        if (ret != 0)
        {
            LOG_E("afc_command_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x03:  /* realtime frame */
        ret = afc_realtime_frame_parse(&cmd);
        if (ret != 0)
        {
            LOG_E("afc_realtime_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    default:
        LOG_E("invalid cmd type: %d\r\n", cmd.type);
        return -2;
        break;
    }

    *obj->len = *cmd.len + 5;

    if (cmd.id.bits.cmd_ack != 0)  /* need ack */
    {
        obj->data[1] &= ~(1 << 7);

        obj->data[2] |= 0x80;

        // ret = afc_uart_cmd_write(cmd);
        // if (ret != 0)
        // {
        //     LOG_E("afc uart cmd write err: %d\r\n", ret);
        // }
    }

    return ret;
}

static int8_t afc_uart_cmd_process(struct afc_object *obj)
{
    if (obj == NULL)
    {
        return -1;
    }

#if 0
    LOG_I("recv len: %d\r\n", *obj->len);
    for (uint8_t i = 0; i < *obj->len; i++)
    {
        LOG_I("%02x ", obj->data[i]);
    }
    LOG_I("\r\n");
#endif

    return afc_cmd_parse(obj);
}

static osMessageQueueId_t afc_uart_send_queue = NULL;
int8_t afc_uart_cmd_write(struct afc_object *cmd)
{
    uint8_t buf[AFC_UART_FRAME_SIZE_MAX] = {0};

    buf[0] = cmd->id.byte;
    buf[1] = cmd->type;
    buf[2] = *cmd->len & 0xff;
    buf[3] = *cmd->len >> 8;
    memcpy(&buf[4], cmd->data, *cmd->len);

    osStatus_t stat = osMessageQueuePut(afc_uart_send_queue, buf, 0, 0);
    if (stat != osOK)
    {
        LOG_E("afc uart send queue put err: %d\r\n", stat);
    }

    return stat;
}

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

static osEventFlagsId_t uart_rx_event_id = NULL;
#define UART_RX_HEARTBEAT_TIMEOUT_EVENT (1 << 0)
#define UART_RX_HEARTBEAT_CMD_EVENT     (1 << 1)
#define UART_RX_REBOOT_CMD_EVENT        (1 << 2)
static int8_t uart_recv_heartbeat_timeout_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return osEventFlagsSet(uart_rx_event_id, UART_RX_HEARTBEAT_TIMEOUT_EVENT);
}
static int8_t uart_recv_heartbeat_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return osEventFlagsSet(uart_rx_event_id, UART_RX_HEARTBEAT_CMD_EVENT);
}
static int8_t uart_recv_time_sync_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return 0;//timestamp_ns_set(*(uint64_t *)data);
}
static int8_t uart_recv_set_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    struct afc_object obj = {.id.byte = id, .type = UART_CMD_DATA_SET, .len = len, .data = data};
    return afc_uart_cmd_process(&obj);
}
static int8_t uart_recv_get_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    struct afc_object obj = {.id.byte = id, .type = UART_CMD_DATA_GET, .len = len, .data = data};
    return afc_uart_cmd_process(&obj);
}
static int8_t uart_recv_reboot_cmd_callback(struct uart_protocol *const self, uint32_t id, const uint8_t *data, uint16_t *len, void *arg)
{
    return osEventFlagsSet(uart_rx_event_id, UART_RX_REBOOT_CMD_EVENT);
}
static int8_t uart_send_heartbeat_cmd_callback(struct uart_protocol *const self, uint8_t *data, uint16_t *len, void *arg)
{
    uint8_t *fw_ver = system_info_get()->fw_version;

    *len = 5;
    data[0] = 0xFF; /* TODO: board id + hardware version */
    data[1] = 0x00; /* software version */
    data[2] = strtoul(&fw_ver[0], NULL, 10);
    data[3] = strtoul(&fw_ver[3], NULL, 10);
    data[4] = strtoul(&fw_ver[6], NULL, 10);

    return 0;
}

static int8_t afc_uart_recv_entry(void *argument)
{
    int8_t ret = 0;
    uint8_t buf[AFC_UART_FRAME_SIZE_MAX] = {0};

    // ret = uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID, uart_recv_heartbeat_timeout_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_HEARTBEAT_RX_CB_ID, uart_recv_heartbeat_cmd_callback,  NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_PNT_RX_CB_ID, uart_recv_time_sync_cmd_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_SET_RX_CB_ID, uart_recv_set_cmd_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_GET_RX_CB_ID, uart_recv_get_cmd_callback, NULL);
    ret |= uart_protocol_rx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_REBOOT_RX_CB_ID, uart_recv_reboot_cmd_callback, NULL);
    // ret |= uart_protocol_tx_RegisterCallback(uart_protocal_get(), UART_PROTOCOL_HEARTBEAT_TX_CB_ID, uart_send_heartbeat_cmd_callback, NULL);
    if (ret != 0)
    {
        LOG_E("uart_protocol_rx_RegisterCallback err: %d\r\n", ret);
        osThreadExit();
    }

    ret = device_afc_uart_open();
    if (ret != 0)
    {
        LOG_E("device dos uart open err: %d\r\n", ret);
        osThreadExit();
    }

    for (;;)
    {
        ret = device_afc_uart_data_recv_with_block(buf, sizeof(buf), osWaitForever);
        if (ret != 0)
        {
            LOG_E("device afc uart data recv with block err: %d\r\n", ret);
            continue;
        }
    }

    return 0;
}

static int8_t link_status_entry(void *argument)
{
    int8_t ret = 0;
    int32_t event_flags = 0;

    for (;;)
    {
        event_flags = osEventFlagsWait(uart_rx_event_id, UART_RX_HEARTBEAT_TIMEOUT_EVENT | UART_RX_HEARTBEAT_CMD_EVENT | UART_RX_REBOOT_CMD_EVENT, osFlagsWaitAny, osWaitForever);
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

static int8_t afc_uart_send_entry(void *argument)
{
    int8_t ret = 0;
    uint8_t buf[AFC_UART_FRAME_SIZE_MAX] = {0};
    struct uart_data obj = {0};

    for (;;)
    {
        osMessageQueueGet(afc_uart_send_queue, buf, NULL, osWaitForever);

        obj.id = 0;
        obj.cmd = 0;
        obj.len = (buf[2] | buf[3] << 8) + 4;
        obj.data = buf;

#if 0
        LOG_I("send id: %d, cmd: %d, len: %d\r\n", obj.id, obj.cmd, obj.len);
        for (uint8_t i = 0; i < obj.len; i++)
        {
            LOG_I("%02x ", obj.data[i]);
        }
        LOG_I("\r\n");
#endif

        ret = device_afc_uart_data_write(&obj, 100);
        if (ret != 0)
        {
            LOG_E("device afc uart data write err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t afc_uart_thread_init(void)
{
    uart_rx_event_id = osEventFlagsNew(NULL);
    if (uart_rx_event_id == NULL)
    {
        LOG_E("osEventFlagsNew err\r\n");
        return -1;
    }

    osThreadAttr_t attr = {
    .name = "afc_uart_recv_thread",
    .stack_size = 4096 * 4,
    .priority = osPriorityAboveNormal,
    };

    osThreadId_t thread_id = osThreadNew(afc_uart_recv_entry, NULL, &attr);
    if (thread_id == NULL)
    {
        LOG_E("thread afc uart create failed\r\n");
        return -2;
    }

    attr.name = "link_status_thread";
    thread_id = osThreadNew(link_status_entry, NULL, &attr);
    if (thread_id == NULL)
    {
        LOG_E("thread link status create failed\r\n");
        return -3;
    }

    afc_uart_send_queue = osMessageQueueNew(16, AFC_UART_FRAME_SIZE_MAX, NULL);
    if (afc_uart_send_queue == NULL)
    {
        LOG_E("queue afc uart send create failed\r\n");
        return -4;
    }

    attr.name = "afc_uart_send_thread";
    thread_id = osThreadNew(afc_uart_send_entry, NULL, &attr);
    if (thread_id == NULL)
    {
        LOG_E("thread afc uart send create failed\r\n");
        return -5;
    }

    return 0;
}
INIT_APP_EXPORT(afc_uart_thread_init);