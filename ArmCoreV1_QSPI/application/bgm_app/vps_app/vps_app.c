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
        LOG_E("[%d] cmd is NULL\r\n", id);
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.byte != DEVICE_ADDRESS_VPS)
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

static int8_t vps_link_menu_init(void)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};

    uint8_t data[64] = {0};

    /* 1. set link mode 512 is 1 */
    cmd.addr = DEVICE_ADDRESS_VPS;
    cmd.type = WRITE_SINGLE_REGISTER;
    cmd.len = 4;
    cmd.data = data;
    data[0] = 0x01;
    data[1] = 0xFF;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 2. set link menu num */
    uint8_t offset = 5;
    /* 0. 软件版本 */
    data[offset++] = 0x01;
    data[offset++] = 0xF5;
    /* 1. 运行状态 */
    data[offset++] = 0x00;
    data[offset++] = 0x65;
    /* 2. 电源电压 */
    data[offset++] = 0x00;
    data[offset++] = 0x69;
    /* 3. 输出电压 */
    data[offset++] = 0x00;
    data[offset++] = 0x66;
    /* 4. 输出电流H */
    data[offset++] = 0x00;
    data[offset++] = 0x67;
    /* 5. 输出电流L */
    data[offset++] = 0x00;
    data[offset++] = 0x68;
    /* 6. 故障停机 */
    data[offset++] = 0x01;
    data[offset++] = 0x91;
    /* 7. 故障复位 */
    data[offset++] = 0x01;
    data[offset++] = 0x92;
    /* 8. 故障编码H */
    data[offset++] = 0x01;
    data[offset++] = 0x94;
    /* 9. 故障编码L */
    data[offset++] = 0x01;
    data[offset++] = 0x95;
    /* 10. 故障记录1 */
    data[offset++] = 0x01;
    data[offset++] = 0x96;
    /* 11. 故障记录2 */
    data[offset++] = 0x01;
    data[offset++] = 0x97;
    /* 12. 故障记录3 */
    data[offset++] = 0x01;
    data[offset++] = 0x98;
    /* 13. 故障记录4 */
    data[offset++] = 0x01;
    data[offset++] = 0x99;
    /* 14. 打火次数 */
    data[offset++] = 0x01;
    data[offset++] = 0xAA;
    /* 15. 打火停机时间 */
    data[offset++] = 0x01;
    data[offset++] = 0xAC;

    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = (offset - 5) / 2;
    data[4] = offset - 5;

    cmd.type = WRITE_MULTIPLE_REGISTERS;
    cmd.len = offset;
    cmd.data = data;

    ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -2;
    }

    /* 3. set link mode 512 is 0 */
    cmd.type = WRITE_SINGLE_REGISTER;
    cmd.len = 4;
    cmd.data = data;
    data[0] = 0x01;
    data[1] = 0xFF;
    data[2] = 0x00;
    data[3] = 0x00;

    ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -3;
    }

    return 0;
}

static int8_t vps_link_menu_value_read(void)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};
    uint8_t data[10] = {0x01, 0xF4, 0x00, 0x01};

#ifdef VPS_LINK_MODE
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
#else

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

   /* 2. read run status */
   data[0] = 0x00;
   data[1] = 0x64;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -2;
   }

   /* 3. read power voltage */
   data[0] = 0x00;
   data[1] = 0x68;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -3;
   }

   /* 4. read output voltage */
   data[0] = 0x00;
   data[1] = 0x65;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -4;
   }

   /* 5. read output current H */
   data[0] = 0x00;
   data[1] = 0x66;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -5;
   }

   /* 6. read output current L */
   data[0] = 0x00;
   data[1] = 0x67;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -6;
   }

   /* 7. read halt status */
   data[0] = 0x01;
   data[1] = 0x90;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -7;
   }

   /* 8. read reset status */
   data[0] = 0x01;
   data[1] = 0x91;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -8;
   }

   /* 9. read halt code H */
   data[0] = 0x01;
   data[1] = 0x93;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -9;
   }

   /* 10. read halt code L */
   data[0] = 0x01;
   data[1] = 0x94;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -10;
   }

   /* 11. read halt record 1 */
   data[0] = 0x01;
   data[1] = 0x95;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -11;
   }

   /* 12. read halt record 2 */
   data[0] = 0x01;
   data[1] = 0x96;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -12;
   }

   /* 13. read halt record 3 */
   data[0] = 0x01;
   data[1] = 0x97;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -13;
   }

   /* 14. read halt record 4 */
   data[0] = 0x01;
   data[1] = 0x98;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -14;
   }

   /* 15. read fire times */
   data[0] = 0x01;
   data[1] = 0xA9;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -15;
   }

   /* 16. read fire halt time */
   data[0] = 0x01;
   data[1] = 0xAB;

   ret = uart_modbus_cmd_write(BGM_UART_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -16;
   }

#endif

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

    // /* 5. set link menu */
    // ret = vps_link_menu_init();
    // if (ret != 0)
    // {
    //     LOG_E("vps link menu init err: %d\r\n", ret);
    //     return -6;
    // }

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

#ifndef VPS_TEST
#include "shell.h"
static int8_t vps_read_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;


    switch (atoi(argv[1]))
    {
    case 0:
        ret = vps_link_menu_value_read();
        if (ret != 0)
        {
            LOG_E("vps link menu value read err: %d\r\n", ret);
        }
        break;
    case 1:
        vps_link_menu_init();
        break;
    default:
        break;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(vps_read_test, vps_read_test, read vps value);
#endif