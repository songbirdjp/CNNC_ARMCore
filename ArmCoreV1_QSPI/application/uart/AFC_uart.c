#include "AFC_uart.h"
#include "uart_port.h"
#include "init_call.h"
#include "sys_cfg.h"
#include "mcu_adc.h"
#include "ulog.h"
const uint8_t AFC_Version[4] = {0x19,0,0,1};
static struct control_para control_data = 
{
    .calibration = {.adc_factor = {2376000, 2376000, 2376000, 2376000, 2376000}, 
                    .dac_factor = 30,
                    .trig_interval_min = 4000},
    .treatment = {.prf_hz = 1},
    .interlock = {.threshold_dose_rate = {10}, 
                  .threshold_dose_cp = {10}, 
                  .one_pulse = {.threshold_low = 10, .threshold_high = 10}, 
                  .threshold_symmetry = 10, 
                  .communication_timeout = 5000},
};
static struct control_para *control_data_get(void)
{
    return &control_data;
}

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
/*
HANSHAKE 下行
Byte1	帧类型	0x01
Byte2~3	帧长度	0x0004
Byte4	数据1	BGM ARM IO 板卡硬件ID/版本 0x19
Byte5	数据2	BGM ARM IO 固件版本号(xx.yy.zz)xx
Byte6	数据3	BGM ARM IO 固件版本号yy
Byte7	数据4	BGM ARM IO 固件版本号zz
HANSHAKE 上行
Byte1	帧类型	0x81
Byte2~3	帧长度	0x0004
Byte4	数据1	AFC板硬件版本
Byte5	数据2	AFC板固件版本号xx
Byte6	数据3	AFC板固件版本号yy
Byte7	数据4	AFC板固件版本号zz

 */
static int8_t AFC_handshake_frame_parse(struct AFC_object *cmd)
{
    int8_t ret = 0;

    struct control_para *obj = control_data_get();

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

}
static int8_t  AFC_MagMotorCmd_parse(struct AFC_object *cmd)
{

}   
static int8_t  AFC_AFTMotorCmd_parse(struct AFC_object *cmd)
{

}    
static int8_t  AFC_ADCSampleSet_parse(struct AFC_object *cmd)
{
    
}   
static int8_t  AFC_ADCParaSet_parse(struct AFC_object *cmd)
{

}   
static int8_t  AFC_ADCPowerFeedback_parse(struct AFC_object *cmd)
{

}   
static int8_t  AFC_Interlock_parse(struct AFC_object *cmd)
{

}   
static int8_t  AFC_FlashState_parse(struct AFC_object *cmd)
{

}   
static int8_t  AFC_StateControl_parse(struct AFC_object *cmd)
{

}       
static int8_t  AFC_CtrlStatus_parse(struct AFC_object *cmd)
{

}   
static int8_t  AFC_ResetCtrl_parse(struct AFC_object *cmd)
{

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
        ret = AFC_AFTMotorCmd_parse(cmd);
        break;
    case 0x60:
        ret = AFC_ADCSampleSet_parse(cmd);
        break;
    case 0x80:
        ret = AFC_ADCParaSet_parse(cmd);
        break;
    case 0x81:
        ret = AFC_ADCPowerFeedback_parse(cmd);
        break;
    case 0x82:
        ret = AFC_Interlock_parse(cmd);
        break;  
    case 0x83:
        ret = AFC_FlashState_parse(cmd);
        break;
    case 0xC0:
        ret = AFC_StateControl_parse(cmd);
        break;
    case 0xC1:
        ret = AFC_CtrlStatus_parse(cmd);
        break;
    case 0xC2:
        ret = AFC_ResetCtrl_parse(cmd);
        break;
    default:
        break;
    }

    if (ret != 0)
    {
        LOG_E("AFC uart command [%.2x, %.2x] parse err: %d\r\n", cmd->data[0], cmd->data[1], ret);
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

#if 0
        LOG_I("send_buf len: %d\r\n", send_buf.len);
        for (uint8_t i = 0; i < send_buf.len; i++)
        {
            LOG_I("%02x ", send_buf.buf[i]);
        }
        LOG_I("\r\n");
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
    .stack_size = 1024 * 4,
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


