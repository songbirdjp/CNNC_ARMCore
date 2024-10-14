#include "bgm_uart.h"
#include "init_call.h"
#include "cmsis_os2.h"
#include "ulog.h"
#include "BGM_def.h"
struct dose_info_t
{
    uint8_t hw_version;
    uint8_t sw_version[6];
};

struct AFC_info_t
{
    uint8_t hw_version;
    uint8_t sw_version[6];
};

struct dose_info_t dose_info = {0};
struct AFC_info_t AFC_info = {0};
static int8_t dose_handshake_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;


    // if(5 != cmd->len)
    // {
    //     printf("Dose handshake data read back length wrong\r\n");
    //     ret = -1;
    // }
    // memcpy(DoseVersion,cmd->data,5);
    // printf("Dose ID = %x\r\n",DoseVersion[0]);
    // //todo : need dose id check
    // printf("Dose Handshake ok\r\n");
    // ret = 0;

    LOG_I("Dose ID = %x,Handshake success", cmd->data[0]);
    dose_info.hw_version = cmd->data[1];
    dose_info.sw_version[0] = cmd->data[2];
    dose_info.sw_version[1] = '.';
    dose_info.sw_version[2] = cmd->data[3];
    dose_info.sw_version[3] = '.';
    dose_info.sw_version[4] = cmd->data[4];
    LOG_I("dose hw version: %d\r\n", dose_info.hw_version);
    LOG_I("dose sw version: %s\r\n", dose_info.sw_version);
    return ret;
}

static int8_t dose_calibration_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0x01:
        switch (cmd->data[1])
        {
        case 0x00:
            cmd->data[2] == 0 ? LOG_I("dose calibration data lock opened\r\n") : LOG_I("dose calibration data lock closed\r\n");
            break;
        case 0x01:
            cmd->data[2] == 0 ? LOG_I("dose calibration data invalid\r\n") : LOG_I("dose calibration data valid\r\n");
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x02:
        switch (cmd->data[1])
        {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
            LOG_I("dose adc factor set (1MU == %u code)\r\n", cmd->data[4] << 16 | cmd->data[3] << 8 | cmd->data[2]);
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x03:
        LOG_I("dose dac factor set : %u\r\n", cmd->data[3] << 8 | cmd->data[2]);
        break;
    case 0x04:
        LOG_I("dose trigger interval set: %u\r\n", cmd->data[3] << 8 | cmd->data[2]);
        break;
    default:
        LOG_I("invalid calibration cmd type: %x\r\n", cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}
static int8_t dose_treatment_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0x40:
        cmd->data[2] == 0 ? LOG_I("dose dummy mode set\r\n") : LOG_I("dose normal mode set\r\n");
        break;
    case 0x41:
        switch (cmd->data[1])
        {
        case 0x00:
            LOG_I("pulse generation mode set %d\r\n", cmd->data[2]);
            break;
        case 0x01:
            LOG_I("dose prf set %u ok\r\n", cmd->data[2]);
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x42:
        switch (cmd->data[1])
        {
        case 0x00:
            break;
        case 0x01:
            LOG_I("dose meter set %u ok\r\n", cmd->data[3] << 8 | cmd->data[2]);
            break;
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0x43:
        switch (cmd->data[1])
        {
        case 0x00:
            cmd->data[2] == 0 ? LOG_I("beam data lock opened\r\n") : LOG_I("beam data lock closed\r\n");
            break;
        case 0x01:
            cmd->data[2] == 0 ? LOG_I("beam data valid\r\n") : LOG_I("beam data invalid\r\n");
            break;
        default:
            ret = -1;
            break;
        }
        break;
    default:
        LOG_I("invalid treatment cmd type: %x\r\n", cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}
static int8_t dose_interlock_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0x80:
        break;
    case 0x81:
        break;
    case 0x82:
        switch (cmd->data[1])
        {
        case 0x00:
            LOG_I("ionization chamber voltage get: %u\r\n", (cmd->data[3] << 8 | cmd->data[2]) / 100);
            break;
        case 0x01:
            LOG_I("P5V voltage get: %u\r\n", (cmd->data[3] << 8 | cmd->data[2]) / 100);
            break;
        case 0x02:
            LOG_I("N5V voltage get: %u\r\n", (cmd->data[3] << 8 | cmd->data[2]) / 100);
            break;
        case 0x03:
            LOG_I("dac1 channelA offset code get: %u\r\n", cmd->data[3] << 8 | cmd->data[2]);
            break;
        case 0x04:
            LOG_I("dac1 channelB offset code get: %u\r\n", cmd->data[3] << 8 | cmd->data[2]);
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0xB0:
        LOG_I("dose interlock get %#.4x\r\n", cmd->data[3] << 8 | cmd->data[2]);
        break;
    case 0xB1:
        break;
    default:
        LOG_I("invalid interlock cmd type: %x\r\n", cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}
extern BGMStateMachine_t ARMcurrentState;
static int8_t dose_state_control_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0xC0:
        switch (cmd->data[1])
        {
        case 0x00:  /* dose state switch result */
            cmd->data[2] == 0 ? LOG_I("dose state switch success\r\n") : LOG_I("dose state switch fail\r\n");
            break;
        case 0x01:  /* dose current state */
            LOG_I("Dose Board current state: %d\r\n", cmd->data[2]);
            if(cmd->data[2] == 1)
            {
                ARMcurrentState = BGM_STATE_IDLE;
            }
            if(cmd->data[2] == 3)
            {
                ARMcurrentState = BGM_STATE_PREPARE;
            }
            if(cmd->data[2] == 6)//complete
            {
                ARMcurrentState = BGM_STATE_COMPLETE;
            }
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0xC1:
        switch (cmd->data[1])
        {
        case 0x00:
            LOG_I("pulse abnormal cleanup ok\r\n");
            break;
        case 0x01:
            LOG_I("beam data cleanup ok\r\n");
            break;
        case 0x02:
            LOG_I("dose cumulative data cleanup ok\r\n");
            break;
        case 0x03:
            LOG_I("interlock cleanup ok\r\n");
            break;
        case 0x04:
            LOG_I("one pulse valid flag cleanup ok\r\n");
            break;
        default:
            ret = -1;
            break;
        }
        break;
    case 0xC2:
        switch (cmd->data[1])
        {
        case 0x00:
            LOG_I("dose reset wdt ok\r\n");
            break;
        case 0x01:
            LOG_I("dose reset ok\r\n");
            break;
        default:
            ret = -1;
            break;
        }
        break;
    default:
        LOG_I("invalid state control cmd type: %x\r\n", cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}

static int8_t dose_command_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])   /* first cmd */
    {
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
        ret = dose_calibration_parse(cmd);
        break;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
        ret = dose_treatment_parse(cmd);
        break;
    case 0x80:
    case 0x81:
    case 0x82:
    case 0xB0:
    case 0xB1:
        ret = dose_interlock_parse(cmd);
        break;
    case 0xC0:
    case 0xC1:
    case 0xC2:
        ret = dose_state_control_parse(cmd);
        break;    
    default:
        LOG_I("invalid cmd type: %x\r\n", cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}

static int8_t dose_realtime_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {
    case 0x00:
        if (cmd->data[3] == 0x01)
        {
            printf("set emergency stop\r\n");
        }
        else
        {
            printf("set radiation index: %d\r\n", cmd->data[2] << 8 | cmd->data[1]);
        }
        break;
    case 0x01:
        printf("dose state: %#.2x\r\n", cmd->data[1]);
        printf("dose interlock: %#.4x\r\n", cmd->data[3] << 8 | cmd->data[2]);
        printf("dose current cp: %d\r\n", cmd->data[4]);
        printf("dose current radiation index: %d\r\n", cmd->data[6] << 8 | cmd->data[5]);
        printf("dose current cumulative: %d (0.1MU)\r\n", cmd->data[8] << 8 | cmd->data[7]);
        printf("dose current prf: %d\r\n", cmd->data[9]);
        printf("dose abnormal pulse count: %d\r\n", cmd->data[11] << 8 | cmd->data[10]);
        printf("dose one pulse valid flag: %d\r\n", cmd->data[12]);
        printf("dose one pulse code: %d\r\n", cmd->data[14] << 8 | cmd->data[13]);
        break;
    default:
        printf("invalid realtime cmd type: %x\r\n", cmd->data[0]);
        ret = -1;
        break;
    }

    return ret;
}

static int8_t uart_dose_cmd_parse(struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        printf("cmd is NULL\r\n");
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.bits.cmd_id != BGM_UART_ID)
    {
        printf("BGM_UART_ID Wrong!\r\n");
        return 0;
    }

    int8_t ret = 0;

    /* 2. parse cmd type */
    switch (cmd->type)
    {
    case 0x81:  /* handshake frame */
        ret = dose_handshake_frame_parse(cmd);
        if (ret != 0)
        {
            printf("bgm_uart_handshake_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x82:  /* command frame */
        ret = dose_command_frame_parse(cmd);
        if (ret != 0)
        {
            printf("dose_command_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x83:  /* realtime frame */
        ret = dose_realtime_frame_parse(cmd);
        if (ret != 0)
        {
            printf("bgm_uart_realtime_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    default:
        printf("invalid cmd type: %x\r\n", cmd->type);
        return -2;
        break;
    }

    return ret;
}

static int8_t afc_handshake_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;
   LOG_I("AFC Handshake success");
    AFC_info.hw_version = cmd->data[1];
    AFC_info.sw_version[0] = cmd->data[2];
    AFC_info.sw_version[1] = '.';
    AFC_info.sw_version[2] = cmd->data[3];
    AFC_info.sw_version[3] = '.';
    AFC_info.sw_version[4] = cmd->data[4];
    LOG_I("AFC hw version: %d\r\n", AFC_info.hw_version);
    LOG_I("AFC sw version: %s\r\n", AFC_info.sw_version);
    return ret;
}

static int8_t afc_command_frame_parse(struct cmd_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])   /* first cmd */
    {
    case 0x40:
        break;
    case 0x41:
        break;
    case 0x42:
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

static int8_t uart_afc_cmd_parse(struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        printf("cmd is NULL\r\n");
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
    case 0x01:  /* handshake frame */
        ret = afc_handshake_frame_parse(cmd);
        if (ret != 0)
        {
            printf("afc_handshake_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x02:  /* command frame */
        ret = afc_command_frame_parse(cmd);
        if (ret != 0)
        {
            printf("afc_command_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x03:  /* realtime frame */
        ret = afc_realtime_frame_parse(cmd);
        if (ret != 0)
        {
            printf("afc_realtime_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    default:
        printf("invalid cmd type: %d\r\n", cmd->type);
        return -2;
        break;
    }

    return ret;
}

static uint16_t modbus_crc16_cal(const uint8_t *data, uint16_t length)
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

static int8_t uart_eps_cmd_parse(struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        printf("cmd is NULL\r\n");
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.byte != DEVICE_ADDRESS_EPS)
    {
        printf("invalid cmd id: %d\r\n", cmd->id.byte);
        return -2;
    }

    /* 2. check crc */
    uint8_t buf[128] = {0};
    buf[0] = cmd->id.byte;
    buf[1] = cmd->type;
    memcpy(&buf[2], cmd->data, cmd->len);

    uint16_t crc = buf[cmd->len] << 8 | buf[cmd->len + 1];
    uint16_t crc_cal = modbus_crc16_cal(buf, cmd->len);
    if (crc_cal != crc)
    {
        printf("crc err: %x, %x \r\n", crc_cal, crc);
        return -3;
    }

    if (cmd->type & 0x80)
    {
        /* 
         * 1：功能不支持
         * 2：参数地址不正确
         * 3：寄存器数量超限
         * 4：内部处理出错
         */
        printf("cmd frame err: %d\r\n", cmd->data[0]);
        return -4;
    }

    switch (cmd->type)
    {
    case READ_HOLDING_REGISTERS:
        for (uint8_t i = 0; i < cmd->data[0] / 2; i++)
        {
            printf("%.4x ", cmd->data[1 + i * 2] << 8 | cmd->data[2 + i * 2]);
        }
        printf("\r\n");
        break;
    case READ_INPUT_REGISTERS:
        for (uint8_t i = 0; i < cmd->data[0] / 2; i++)
        {
            printf("%.4x ", cmd->data[1 + i * 2] << 8 | cmd->data[2 + i * 2]);
        }
        printf("\r\n");
        break;
    case WRITE_SINGLE_REGISTER:
        printf("reg addr: %#.4x, value: %#.4x\r\n", cmd->data[0] << 8 | cmd->data[1], cmd->data[2] << 8 | cmd->data[3]);
        break;
    case WRITE_MULTIPLE_REGISTERS:
        printf("reg addr: %#.4x, len: %#.4x\r\n", cmd->data[0] << 8 | cmd->data[1], cmd->data[2] << 8 | cmd->data[3]);
        break;
    default:
        printf("invalid cmd type: %d\r\n", cmd->type);
        return -5;
        break;
    }

    return 0;
}

static int8_t uart_vps_cmd_parse(struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        printf("cmd is NULL\r\n");
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.byte != DEVICE_ADDRESS_VPS)
    {
        printf("invalid cmd id: %d\r\n", cmd->id.byte);
        return -2;
    }

    /* 2. check crc */
    uint8_t buf[128] = {0};
    buf[0] = cmd->id.byte;
    buf[1] = cmd->type;
    memcpy(&buf[2], cmd->data, cmd->len);

    uint16_t crc = buf[cmd->len] << 8 | buf[cmd->len + 1];
    uint16_t crc_cal = modbus_crc16_cal(buf, cmd->len);
    if (crc_cal != crc)
    {
        printf("crc err: %x, %x \r\n", crc_cal, crc);
        return -3;
    }

    if (cmd->type & 0x80)
    {
        /* 
         * 1：功能不支持
         * 2：参数地址不正确
         * 3：寄存器数量超限
         * 4：内部处理出错
         */
        printf("cmd frame err: %d\r\n", cmd->data[0]);
        return -4;
    }

    switch (cmd->type)
    {
    case READ_HOLDING_REGISTERS:
        for (uint8_t i = 0; i < cmd->data[0] / 2; i++)
        {
            printf("%.4x ", cmd->data[1 + i * 2] << 8 | cmd->data[2 + i * 2]);
        }
        printf("\r\n");
        break;
    case READ_INPUT_REGISTERS:
        for (uint8_t i = 0; i < cmd->data[0] / 2; i++)
        {
            printf("%.4x ", cmd->data[1 + i * 2] << 8 | cmd->data[2 + i * 2]);
        }
        printf("\r\n");
        break;
    case WRITE_SINGLE_REGISTER:
        printf("reg addr: %#.4x, value: %#.4x\r\n", cmd->data[0] << 8 | cmd->data[1], cmd->data[2] << 8 | cmd->data[3]);
        break;
    case WRITE_MULTIPLE_REGISTERS:
        printf("reg addr: %#.4x, len: %#.4x\r\n", cmd->data[0] << 8 | cmd->data[1], cmd->data[2] << 8 | cmd->data[3]);
        break;
    default:
        printf("invalid cmd type: %d\r\n", cmd->type);
        return -5;
        break;
    }

    return 0;
}

osMessageQueueId_t uart_send_queue[BGM_UART_MAX] = {NULL};
int8_t uart_cmd_write(enum uart_id id, struct cmd_object *cmd)
{
    if (id >= BGM_UART_MAX)
    {
        printf("invalid uart id: %d\r\n", id);
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

    stat = osMessageQueuePut(uart_send_queue[id], &send_buf, 0, 0);
    if (stat != osOK)
    {
        printf("dose uart send queue put err: %d\r\n", stat);
        return -2;
    }

    return 0;
}

int8_t (*uart_cmd_parse[BGM_UART_MAX])(struct cmd_object *cmd) = 
{
    [BGM_UART_AFC] = uart_afc_cmd_parse,
    [BGM_UART_DOSE1] = uart_dose_cmd_parse,
    [BGM_UART_DOSE2] = uart_dose_cmd_parse,
    [BGM_UART_EPS] = uart_eps_cmd_parse,
    [BGM_UART_VPS] = uart_vps_cmd_parse
};

static int8_t uart_cmd_process(enum uart_id id, struct bgm_uart *buf)
{
    if (buf == NULL)
    {
        return -1;
    }

#if 1
    LOG_I("recv_buf len: %d\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        LOG_I("%02x ", buf->buf[i]);
    }
#endif

    struct cmd_object cmd = {0};

    if (id == BGM_UART_EPS || id == BGM_UART_VPS)
    {
        cmd.id.byte = buf->buf[0];
        cmd.type = buf->buf[1];
        cmd.len = buf->len - 2;
        cmd.data = &buf->buf[2];
    }
    else
    {
        memcpy(&cmd, buf->buf, sizeof(struct cmd_object));
        cmd.data = &buf->buf[sizeof(struct cmd_object) - sizeof(uint8_t *)];
    }

    return uart_cmd_parse[id](&cmd);
}

static int8_t uart_init(enum uart_id id)
{
    int8_t ret = device_uart_init(id);
    if (ret != 0)
    {
        printf("device uart init err: %d\r\n", ret);
        return -1;
    }

    ret = device_uart_open(id);
    if (ret != 0)
    {
        printf("device uart open err: %d\r\n", ret);
        return -2;
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
        printf("uart init err: %d\r\n", ret);
        return -1;
    }

    for (;;)
    {
        ret = device_uart_data_read(uart_id, &recv_buf, osWaitForever);
        if (ret != 0)
        {
            printf("device uart data read err: %d\r\n", ret);
            continue;
        }

        ret = uart_cmd_process(uart_id, &recv_buf);
        if (ret!= 0)
        {
            printf("uart cmd process err: %d\r\n", ret);
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
        printf("uart[%d] init err: %d\r\n", uart_id, ret);
        return -1;
    }

    for (;;)
    {
        osMessageQueueGet(uart_send_queue[uart_id], &send_buf, NULL, osWaitForever);

#if 1
        ("send_buf len: %d\r\n", send_buf.len);
        for (uint8_t i = 0; i < send_buf.len; i++)
        {
            LOG_I("%02x ", send_buf.buf[i]);
        }
#endif

send_data:
        ret = device_uart_data_write(uart_id, &send_buf, send_buf.len, 1000);
        if (ret != 0)
        {
            printf("device uart[%d] data write err: %d\r\n", uart_id, ret);
        }

        #define RECV_TIMEOUT 1000
        ret = device_uart_data_read(uart_id, &recv_buf, RECV_TIMEOUT); /* TODO: timeout can defined by user */
        if (ret != 0)
        {
            printf("device uart[%d] data read err: %d\r\n", uart_id, ret);
            //goto send_data;
        }

        ret = uart_cmd_process(uart_id, &recv_buf);
        if (ret!= 0)
        {
            printf("uart[%d] cmd process err: %d\r\n", uart_id, ret);
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
    //     printf("thread dose uart create failed\r\n");
    //     return -1;
    // }

    // thread_id = osThreadNew(bgm_uart_recv_entry, NULL, &thread_attr);
    // if (thread_id == NULL)
    // {
    //     printf("thread dose uart create failed\r\n");
    //     return -1;
    // }

    // thread_id = osThreadNew(bgm_uart_recv_entry, NULL, &thread_attr);
    // if (thread_id == NULL)
    // {
    //     printf("thread dose uart create failed\r\n");
    //     return -1;
    // }

    uart_send_queue[BGM_UART_AFC] = osMessageQueueNew(5, sizeof(struct bgm_uart), NULL);
    if (uart_send_queue[BGM_UART_AFC] == NULL)
    {
        printf("message queue create failed\r\n");
        return -2;
    }

    uart_send_queue[BGM_UART_DOSE1] = osMessageQueueNew(5, sizeof(struct bgm_uart), NULL);
    if (uart_send_queue[BGM_UART_DOSE1] == NULL)
    {
        printf("message queue create failed\r\n");
        return -2;
    }

    uart_send_queue[BGM_UART_DOSE2] = osMessageQueueNew(5, sizeof(struct bgm_uart), NULL);
    if (uart_send_queue[BGM_UART_DOSE2] == NULL)
    {
        printf("message queue create failed\r\n");
        return -2;
    }

    uart_send_queue[BGM_UART_EPS] = osMessageQueueNew(5, sizeof(struct bgm_uart), NULL);
    if (uart_send_queue[BGM_UART_EPS] == NULL)
    {
        printf("message queue create failed\r\n");
        return -2;
    }

    uart_send_queue[BGM_UART_VPS] = osMessageQueueNew(5, sizeof(struct bgm_uart), NULL);
    if (uart_send_queue[BGM_UART_VPS] == NULL)
    {
        printf("message queue create failed\r\n");
        return -2;
    }

    osThreadId_t thread_id = osThreadNew(uart_send_entry, &uart_id[BGM_UART_AFC], &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread dose uart create failed\r\n");
        return -3;
    }

    thread_id = osThreadNew(uart_send_entry, &uart_id[BGM_UART_DOSE1], &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread dose uart create failed\r\n");
        return -3;
    }

    thread_id = osThreadNew(uart_send_entry, &uart_id[BGM_UART_DOSE2], &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread dose uart create failed\r\n");
        return -3;
    }

    thread_id = osThreadNew(uart_send_entry, &uart_id[BGM_UART_EPS], &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread dose uart create failed\r\n");
        return -3;
    }

    thread_id = osThreadNew(uart_send_entry, &uart_id[BGM_UART_VPS], &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread dose uart create failed\r\n");
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(bgm_uart_thread_init);


#ifndef BGM_UART_TEST
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
        printf("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_uart_cmd_send, bgm_uart_cmd_send, send bgm uart cmd);

static int8_t bgm_uart_modbus_cmd_send(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct cmd_object cmd = {0};
    uint8_t data[10] = {0x00, 0x64, 0x00, 0x01};

    cmd.id.byte = DEVICE_ADDRESS_EPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;

    ret = uart_cmd_write(BGM_UART_EPS, &cmd);
    if (ret != 0)
    {
        printf("uart cmd write err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_uart_modbus_cmd_send, bgm_uart_modbus_cmd_send, send bgm uart modbus cmd);
#endif