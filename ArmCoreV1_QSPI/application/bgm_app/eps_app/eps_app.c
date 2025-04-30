#include "eps_app.h"
#include <stddef.h>
#include "bgm_uart.h"
#include "ulog.h"
#include "init_call.h"

#define DEVICE_ADDRESS_EPS  0x01

static int8_t eps_cmd_parse(enum uart_id id, struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        LOG_E("[%d] cmd is NULL\r\n", id);
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.byte != DEVICE_ADDRESS_EPS)
    {
        LOG_E("[%d] invalid cmd id: %d\r\n", id, cmd->id.byte);
        return -2;
    }

    /* 2. check crc */
    uint8_t buf[128] = {0};
    buf[0] = cmd->id.byte;
    buf[1] = cmd->type;
    memcpy(&buf[2], cmd->data, *cmd->len);

    uint16_t crc = buf[*cmd->len] << 8 | buf[*cmd->len + 1];
    uint16_t crc_cal = modbus_crc16_cal(buf, *cmd->len);
    if (crc_cal != crc)
    {
        LOG_E("[%d] crc err: %x, %x \r\n", id, crc_cal, crc);
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
        LOG_E("[%d] cmd frame err: %d\r\n", id, cmd->data[0]);
        return -4;
    }

    switch (cmd->type)
    {
    case READ_HOLDING_REGISTERS:
        for (uint8_t i = 0; i < cmd->data[0] / 2; i++)
        {
            LOG_I("[%d] %.4x ", id, cmd->data[1 + i * 2] << 8 | cmd->data[2 + i * 2]);
        }
        LOG_I("\r\n");
        break;
    case READ_INPUT_REGISTERS:
        for (uint8_t i = 0; i < cmd->data[0] / 2; i++)
        {
            LOG_I("[%d] %.4x ", id, cmd->data[1 + i * 2] << 8 | cmd->data[2 + i * 2]);
        }
        LOG_I("\r\n");
        break;
    case WRITE_SINGLE_REGISTER:
        LOG_I("[%d] reg addr: %#.4x, value: %#.4x\r\n", id, cmd->data[0] << 8 | cmd->data[1], cmd->data[2] << 8 | cmd->data[3]);
        break;
    case WRITE_MULTIPLE_REGISTERS:
        LOG_I("[%d] reg addr: %#.4x, len: %#.4x\r\n", id, cmd->data[0] << 8 | cmd->data[1], cmd->data[2] << 8 | cmd->data[3]);
        break;
    default:
        LOG_E("[%d] invalid cmd type: %d\r\n", id, cmd->type);
        return -5;
        break;
    }

    return 0;
}

static int8_t eps_link_menu_init(void)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};

    uint8_t data[96] = {0};

    uint8_t offset = 5;
    /* 0. 软件版本 */
    data[offset++] = 0x01;
    data[offset++] = 0x2D;
    /* 1. 运行状态 */
    data[offset++] = 0x00;
    data[offset++] = 0x01;
    /* 2. 输出电压 */
    data[offset++] = 0x00;
    data[offset++] = 0x02;
    /* 3. 输出电流 */
    data[offset++] = 0x00;
    data[offset++] = 0x03;
    /* 4. 输出功率 */
    data[offset++] = 0x00;
    data[offset++] = 0x04;
    /* 5. 故障停机 */
    data[offset++] = 0x00;
    data[offset++] = 0xC9;
    /* 6. 故障复位 */
    data[offset++] = 0x00;
    data[offset++] = 0xCA;
    /* 7. 当前故障 */
    data[offset++] = 0x00;
    data[offset++] = 0xCB;
    /* 8. 故障编码H */
    data[offset++] = 0x00;
    data[offset++] = 0xD0;
    /* 9. 故障编码L */
    data[offset++] = 0x00;
    data[offset++] = 0xD1;
    /* 10. 故障记录1 */
    data[offset++] = 0x00;
    data[offset++] = 0xCC;
    /* 11. 故障记录2 */
    data[offset++] = 0x00;
    data[offset++] = 0xCD;
    /* 12. 故障记录3 */
    data[offset++] = 0x00;
    data[offset++] = 0xCE;
    /* 13. 故障记录4 */
    data[offset++] = 0x00;
    data[offset++] = 0xCF;
    /* 14. 风机故障停机使能 */
    data[offset++] = 0x00;
    data[offset++] = 0xD6;
    /* 15. 通讯存储 */
    data[offset++] = 0x01;
    data[offset++] = 0x4C;

    data[0] = 0x00;
    data[1] = 0x09;
    data[2] = 0x00;
    data[3] = (offset - 5) / 2;
    data[4] = offset - 5;

    cmd.type = WRITE_MULTIPLE_REGISTERS;
    cmd.len = offset;
    cmd.data = data;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -2;
    }

    return 0;
}

static int8_t eps_link_menu_value_read(void)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};
    uint8_t data[10] = {0x01, 0x2C, 0x00, 0x01};

#ifdef EPS_LINK_MODE
    cmd.addr = DEVICE_ADDRESS_EPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -1;
    }
#else
    /* 0. read software version */
    cmd.addr = DEVICE_ADDRESS_EPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 1. read run status */
    data[0] = 0x00;
    data[1] = 0x00;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -2;
    }

    /* 2. read output voltage */
    data[0] = 0x00;
    data[1] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -3;
    }

    /* 3. read output current */
    data[0] = 0x00;
    data[1] = 0x02;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -4;
    }

    /* 4. read output power */
    data[0] = 0x00;
    data[1] = 0x03;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -5;
    }

    /* 5. read fault stop */
    data[0] = 0x00;
    data[1] = 0xC8;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -6;
    }

    /* 6. read fault reset */
    data[0] = 0x00;
    data[1] = 0xC9;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -7;
    }

    /* 7. read current fault */
    data[0] = 0x00;
    data[1] = 0xCA;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -8;
    }

    /* 8. read fault code H */
    data[0] = 0x00;
    data[1] = 0xCF;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -9;
    }

    /* 9. read fault code L */
    data[0] = 0x00;
    data[1] = 0xD0;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -10;
    }

    /* 10. read fault record 1 */
    data[0] = 0x00;
    data[1] = 0xCB;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -11;
    }

    /* 11. read fault record 2 */
    data[0] = 0x00;
    data[1] = 0xCC;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -12;
    }

    /* 12. read fault record 3 */
    data[0] = 0x00;
    data[1] = 0xCD;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -13;
    }

    /* 13. read fault record 4 */
    data[0] = 0x00;
    data[1] = 0xCE;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -14;
    }

    /* 14. read fan fault stop enable */
    data[0] = 0x00;
    data[1] = 0xD5;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -15;
    }

    /* 15. read communication storage */
    data[0] = 0x01;
    data[1] = 0x4B;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -16;
    }
    
#endif
    return 0;
}

static int8_t eps_init(void)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};
    uint8_t data[10] = {0x01, 0x2C, 0x00, 0x01};

    /* 0. read software version */
    cmd.addr = DEVICE_ADDRESS_EPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = data;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 1. read 001: status */
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -2;
    }

    /* 2. read 002: voltage */
    data[0] = 0x00;
    data[1] = 0x01;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -3;
    }

    /* 3. read 003: current */
    data[0] = 0x00;
    data[1] = 0x02;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -4;
    }

    // /* 4. link menu init */
    // ret = eps_link_menu_init();
    // if (ret != 0)
    // {
    //     LOG_E("eps link menu init err: %d\r\n", ret);
    //     return -5;
    // }

    return 0;
}

static int8_t eps_functions_init(void)
{
    int8_t ret = 0;

    ret = uart_cmd_parse_callback_register(BGM_UART_EPS_VPS, eps_cmd_parse);
    if (ret != 0)
    {
        LOG_E("eps cmd parse callback register err: %d\r\n", ret);
        return -1;
    }

    ret = uart_init_callback_register(BGM_UART_EPS_VPS, eps_init);
    if (ret != 0)
    {
        LOG_E("eps init callback register err: %d\r\n", ret);
        return -2;
    }

    return 0;
}
INIT_ENV_EXPORT(eps_functions_init);

#ifndef EPS_TEST
#include "shell.h"
static int8_t eps_read_test(void)
{
    int8_t ret = 0;

    ret = eps_link_menu_value_read();
    if (ret != 0)
    {
        LOG_E("eps link menu value read err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(eps_read_test, eps_read_test, read eps value);
#endif