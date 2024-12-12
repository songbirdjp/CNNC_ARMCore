#include "AFC_uart.h"
#include "uart_port.h"
#include "init_call.h"
#include "sys_cfg.h"
// #include "mcu_adc.h"
#include "ulog.h"
#include "drv_flash.h"
#include "motorctrl.h"
#include "shell.h"
#include "stdint.h"
const uint8_t AFC_Version[4] = {0x19,0,0,1};

int8_t control_data_pointer_get(void **ptr)
{
    if (ptr == NULL)
    {
        return -1;
    }

    *ptr = (void *)control_data_get();
    
    return 0;
}


osMessageQueueId_t AFC_uart_send_queue = NULL;
static int8_t AFC_uart_cmd_write(struct AFC_object *cmd)
{
    osStatus_t stat = osOK;
    struct AFC_uart send_buf = {0};
    uint8_t offset = sizeof(struct AFC_object) - sizeof(uint8_t *);

    memcpy(send_buf.buf, cmd, sizeof(struct AFC_object));
    memcpy(&send_buf.buf[offset], cmd->data, cmd->len);
    send_buf.len = offset + cmd->len;
    stat = osMessageQueuePut(AFC_uart_send_queue, &send_buf, 0, 0);
    if (stat != osOK)
    {
        LOG_E("AFC uart send queue put err: %d\r\n", stat);
        return -1;
    }
    return 0;
}
void AFC_UART_SendCmd(UARTCmdType_t cmdType, uint8_t *cmdData,uint8_t len)
{
    struct cmd_object BGMCmdToSend;
    BGMCmdToSend.id.byte = 0x80;
    BGMCmdToSend.type = cmdType;
    BGMCmdToSend.len = len;
    BGMCmdToSend.data = cmdData;
    AFC_uart_cmd_write(&BGMCmdToSend);
}

static int8_t AFC_handshake_frame_parse(struct AFC_object *cmd)
{
    int8_t ret = 0;



#if 1
    LOG_I("bgm arm core handshake frame parse: %d\r\n", cmd->len);
    LOG_I("hardware version: %#.2x\r\n", cmd->data[0]);
    LOG_I("software version: %u.%u.%u\r\n", cmd->data[1], cmd->data[2], cmd->data[3]);
    LOG_I("AFC id: %u\r\n", cmd->data[4]);
#endif

    uint8_t *fw_ver = system_info_get()->fw_version;

    cmd->len = 4;
    cmd->data[0] = 0x01;    /* 0: dose1     1: dose2 */
    cmd->data[1] = strtoul(&fw_ver[0], NULL, 10);  /* software version */
    cmd->data[2] = strtoul(&fw_ver[3], NULL, 10);
    cmd->data[3] = strtoul(&fw_ver[6], NULL, 10);

    return ret;
}


static int8_t  AFC_ParaSet_parse(struct AFC_object *cmd)
{
   return 0;
}
static int8_t  AFC_MagMotorCmd_parse(struct AFC_object *cmd)
{   
    int8_t ret = 0;
    MotorCtrlParam_TypeDef *obj = MAG_motorParam_get();
    switch (cmd->data[1])
    {
        case 0x00:// Mag Motor find zero ok ACK
            cmd->len = 0x03;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x00;
            cmd->data[2] = obj->motorFindZeroOK;
            break;
        case 0x01://Mag Motor set position 2 bytes no ACK
            obj->encoderValTarget = (cmd->data[0] << 8) | cmd->data[1];
            break;
        case 0x02://Mag Motor run by step no ACK
            if(cmd->data[0] == 0x01)
            {
                obj->encoderValTarget = obj->encoderValTarget + cmd->data[1];
            }
            else if(cmd->data[0] == 0x02)
            {
                obj->encoderValTarget = obj->encoderValTarget - cmd->data[1];
            }
            else
            {
                LOG_E("Mag Motor run by step err: %d\r\n", cmd->data[0]);
            }
            break;
        case 0x03:
            cmd->len = 0x04;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x03;
            cmd->data[2] = obj->encoderValCurrent & 0xFF;
            cmd->data[3] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x04:
            obj->presetPos = (cmd->data[0] << 8) | cmd->data[1];
            break;
        default:
            break;
    }
    return ret;
}   
static int8_t  AFC_AFTMotorCmd_parse(struct AFC_object *cmd)
{
    int8_t ret = 0;
    MotorCtrlParam_TypeDef *obj = AFT_motorParam_get();
    switch (cmd->data[1])
    {
         case 0x00:
            obj->motorBrakeStatus = cmd->data[0];
            break;
        case 0x01://enable AFT Motor to init
            obj->motorInitEnable = cmd->data[0];
            break;
        case 0x02://enable AFT Motor to find zero
            cmd->len = 0x01;
            cmd->data[0] = obj->motorFindZeroOK;
            break;
        case 0x03://Mag Motor set position 2 bytes no ACK
            obj->encoderValTarget = (cmd->data[0] << 8) | cmd->data[1];
            break;
        case 0x04://Mag Motor run by step no ACK
            if(cmd->data[0] == 0x01)
            {
                obj->encoderValTarget = obj->encoderValCurrent + cmd->data[1];
            }
            else if(cmd->data[0] == 0x02)
            {
                obj->encoderValTarget = obj->encoderValCurrent - cmd->data[1];
            }
            else
            {
                LOG_E("Mag Motor run by step err: %d\r\n", cmd->data[0]);
            }
            break;
        case 0x05:
            cmd->len = 0x02;
            cmd->data[0] = obj->encoderValCurrent & 0xFF;
            cmd->data[1] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x06:
            obj->presetPos = (cmd->data[0] << 8) | cmd->data[1];
            break;
        default:
            // ret = -1;
            break;
    }
    return ret;
    // return 0;
}    

extern DEVICE_FLASH *flash;
static int8_t  AFC_ADCSampleSet_parse(struct AFC_object *cmd)//0x60
{
    int8_t ret = 0;
    switch (cmd->data[1])
    {
    case 0x01:
        break;
    case 0x05:
        cmd->len = 0x12;
        cmd->type = 0x02;
        cmd->data[0] = 0x60;
        cmd->data[1] = 0x05;
        flash->read(flash, 0, &cmd->data[2], cmd->len - 2, 1000);
        // LOG_E("flash read len: %d\r\n", cmd->len);
        break;
    default:
        break;
    }   
   return ret;
}   
  

static int8_t AFC_command_frame_parse(struct AFC_object *cmd)
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

static int8_t AFC_realtime_frame_parse(struct AFC_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {

    }

    return ret;
}

static int8_t AFC_cmd_parse(struct AFC_object *cmd)
{
    if (cmd == NULL)
    {
        LOG_E("cmd is NULL\r\n");
        return -1;
    }
    /* 1. check cmd id */
    if (cmd->id.bits.cmd_id != AFC_UART_ID)
    {
        return 0;
    }

    int8_t ret = 0;

    /* 2. parse cmd type */
    switch (cmd->type)
    {
    case 0x01:  /* handshake frame */
        ret = AFC_handshake_frame_parse(cmd);
        if (ret != 0)
        {
            LOG_E("AFC_handshake_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x02:  /* command frame */
        ret = AFC_command_frame_parse(cmd);
        if (ret != 0)
        {
            LOG_E("AFC_command_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x03:  /* realtime frame */
        ret = AFC_realtime_frame_parse(cmd);
        if (ret != 0)
        {
            LOG_E("AFC_realtime_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    default:
        LOG_E("invalid cmd type: %d\r\n", cmd->type);
        return -2;
        break;
    }

    if (cmd->id.bits.cmd_ack != 0)  /* need ack */
    {
        cmd->id.bits.cmd_ack = 0;

        cmd->type |= 0x80;
        
        ret = AFC_uart_cmd_write(cmd);
        if (ret != 0)
        {
            LOG_E("AFC uart cmd write err: %d\r\n", ret);
        }
    }

    return ret;
}



static int8_t AFC_uart_send_entry(void *argument)
{
    int8_t ret = 0;
    struct AFC_uart send_buf = {0};

    for (;;)
    {
        osMessageQueueGet(AFC_uart_send_queue, &send_buf, NULL, osWaitForever);

#if 1
        printf("send_buf len: %d\r\n", send_buf.len);
        for (uint8_t i = 0; i < send_buf.len; i++)
        {
            printf("%02x ", send_buf.buf[i]);
        }
        printf("\r\n");
#endif

        ret = device_AFC_uart_data_write(&send_buf, send_buf.len, 1000);
        if (ret != 0)
        {
            LOG_E("device_AFC_uart_data_write err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t AFC_uart_cmd_process(struct AFC_uart *buf)
{
    if (buf == NULL)
    {
        return -1;
    }

#if 0
    LOG_I("recv_buf len: %d\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        LOG_I("%02x ", buf->buf[i]);
    }
    LOG_I("\r\n");
#endif

    struct AFC_object cmd = {0};
    memcpy(&cmd, buf->buf, sizeof(struct AFC_object));
    cmd.data = &buf->buf[sizeof(struct AFC_object) - sizeof(uint8_t *)];
    return AFC_cmd_parse(&cmd);
}

static int8_t AFC_uart_init(void)
{
    int8_t ret = device_AFC_uart_init(DEVICE_AFC_UART_NAME_DEFAULT);
    if (ret != 0)
    {
        LOG_E("device_AFC_uart_init err: %d\r\n", ret);
        return -1;
    }

    ret = device_AFC_uart_open();
    if (ret != 0)
    {
        LOG_E("device_AFC_uart_open err: %d\r\n", ret);
        return -2;
    }

    return 0;
}

static int8_t AFC_uart_recv_entry(void *argument)
{
    int8_t ret = 0;
    struct AFC_uart recv_buf = {0};

    ret = AFC_uart_init();
    if (ret != 0)
    {
        LOG_E("AFC_uart_init err: %d\r\n", ret);
        return -1;
    }

    for (;;)
    {
        ret = device_AFC_uart_data_read(&recv_buf, osWaitForever);
        if (ret != 0)
        {
            LOG_E("device_AFC_uart_data_read err: %d\r\n", ret);
            continue;
        }

        ret = AFC_uart_cmd_process(&recv_buf);
        if (ret!= 0)
        {
            LOG_E("AFC_uart_cmd_process err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t AFC_uart_thread_init(void)
{
    osThreadAttr_t thread_recv_attr = {
    .name = "AFC_uart_recv_thread",
    .stack_size = 2048 * 4,
    .priority = osPriorityAboveNormal,
    };

    osThreadId_t thread_id = osThreadNew(AFC_uart_recv_entry, NULL, &thread_recv_attr);
    if (thread_id == NULL)
    {
        LOG_E("thread AFC uart create failed\r\n");
        return -1;
    }

    AFC_uart_send_queue = osMessageQueueNew(5, sizeof(struct AFC_uart), NULL);
    if (AFC_uart_send_queue == NULL)
    {
        LOG_E("message queue create failed\r\n");
        return -2;
    }

    osThreadAttr_t thread_send_attr = {
    .name = "AFC_uart_send_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityAboveNormal,
    };

    thread_id = osThreadNew(AFC_uart_send_entry, NULL, &thread_send_attr);
    if (thread_id == NULL)
    {
        LOG_E("thread AFC uart create failed\r\n");
        return -3;
    }

    osMutexAttr_t mutex_attributes = {
    .name = "data_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };


    return 0;
}
INIT_APP_EXPORT(AFC_uart_thread_init);


