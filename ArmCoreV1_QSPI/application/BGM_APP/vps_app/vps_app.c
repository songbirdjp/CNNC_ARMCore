#include "vps_app.h"
#include <stddef.h>
#include "bgm_uart.h"
#include "ulog.h"
#include "init_call.h"

#define DEVICE_ADDRESS_VPS  0x01

static int8_t vps_cmd_parse(enum uart_id id, struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        LOG_E("cmd is NULL\r\n");
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.byte != DEVICE_ADDRESS_VPS)
    {
        LOG_E("invalid cmd id: %d\r\n", cmd->id.byte);
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
        LOG_E("crc err: %x, %x \r\n", crc_cal, crc);
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
        LOG_E("cmd frame err: %d\r\n", cmd->data[0]);
        return -4;
    }

    switch (cmd->type)
    {
    case READ_HOLDING_REGISTERS:
        for (uint8_t i = 0; i < cmd->data[0] / 2; i++)
        {
            LOG_I("%.4x ", cmd->data[1 + i * 2] << 8 | cmd->data[2 + i * 2]);
        }
        LOG_I("\r\n");
        break;
    case READ_INPUT_REGISTERS:
        for (uint8_t i = 0; i < cmd->data[0] / 2; i++)
        {
            LOG_I("%.4x ", cmd->data[1 + i * 2] << 8 | cmd->data[2 + i * 2]);
        }
        LOG_I("\r\n");
        break;
    case WRITE_SINGLE_REGISTER:
        LOG_I("reg addr: %#.4x, value: %#.4x\r\n", cmd->data[0] << 8 | cmd->data[1], cmd->data[2] << 8 | cmd->data[3]);
        break;
    case WRITE_MULTIPLE_REGISTERS:
        LOG_I("reg addr: %#.4x, len: %#.4x\r\n", cmd->data[0] << 8 | cmd->data[1], cmd->data[2] << 8 | cmd->data[3]);
        break;
    default:
        LOG_E("invalid cmd type: %d\r\n", cmd->type);
        return -5;
        break;
    }

    return 0;
}

static int8_t vps_init(void)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};
    uint8_t data[10] = {0x01, 0xF4, 0x00, 0x01};

    /* 1. read software version */
    cmd.addr = DEVICE_ADDRESS_VPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;    

    ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
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

    ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -2;
    }

    /* 2. set 203 is remote */
    data[0] = 0x00;
    data[1] = 0xCA;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -3;
    }

    /* 3. set 207 is remote voltage */
    // data[0] = 0x00;
    // data[1] = 0xCE;
    // data[2] = 0x0F;
    // data[3] = 0xA0;

    // ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
    // if (ret != 0)
    // {
    //     LOG_E("uart modbus cmd write err: %d\r\n", ret);
    //     return -4;
    // }

    /* 4. set 202 is remote start */
    data[0] = 0x00;
    data[1] = 0xC9;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -5;
    }

    return 0;
}

static int8_t vps_functions_init(void)
{
    int8_t ret = 0;

    ret = uart_cmd_parse_callback_register(BGM_UART_VPS, vps_cmd_parse);
    if (ret != 0)
    {
        LOG_E("vps cmd parse callback register err: %d\r\n", ret);
        return -1;
    }

    ret = uart_init_callback_register(BGM_UART_VPS, vps_init);
    if (ret != 0)
    {
        LOG_E("vps init callback register err: %d\r\n", ret);
        return -2;
    }

    return 0;
}
INIT_ENV_EXPORT(vps_functions_init);