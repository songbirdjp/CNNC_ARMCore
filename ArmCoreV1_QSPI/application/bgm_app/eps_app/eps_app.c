#include "eps_app.h"
#include <stddef.h>
#include "bgm_uart.h"
#include "ulog.h"
#include "init_call.h"

#define DEVICE_ADDRESS_EPS  0x01

static struct eps_status eps_status_obj = {0};
static struct eps_status *eps_status_get(void)
{
    return &eps_status_obj;
}

static int8_t eps_cmd_parse(enum uart_id id, struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        LOG_E("[%d][eps] cmd is NULL\r\n", id);
        return -1;
    }

#if 0
    LOG_I("[%d][eps] cmd id: %d, type: %d, len: %d, data: \r\n", id, cmd->id.byte, cmd->type, *cmd->len);
    for (uint8_t i = 0; i < *cmd->len + 2; i++)
    {
        LOG_I("%.2x ", cmd->data[i]);
    }
    LOG_I("\r\n");
#endif

    /* 1. check cmd id */
    if (cmd->id.byte != DEVICE_ADDRESS_EPS)
    {
        LOG_E("[%d][eps] invalid device id: %d\r\n", id, cmd->id.byte);
        return -2;
    }

    /* 2. check crc */
    uint8_t buf[128] = {0};
    buf[0] = cmd->id.byte;
    buf[1] = cmd->type;
    enum eps_read_write_id cmd_id = cmd->data[0] | cmd->data[1] << 8;
    memcpy(&buf[2], &cmd->data[2], *cmd->len);

    uint16_t crc = buf[*cmd->len] << 8 | buf[*cmd->len + 1];
    uint16_t crc_cal = modbus_crc16_cal(buf, *cmd->len);
    if (crc_cal != crc)
    {
        LOG_E("[%d][eps] crc err: %x, %x \r\n", id, crc_cal, crc);
        return -3;
    }

    if (buf[1] & 0x80)
    {
        /* 
         * 1：功能不支持
         * 2：参数地址不正确
         * 3：寄存器数量超限
         * 4：内部处理出错
         */
        LOG_E("[%d][eps] cmd frame err: %d\r\n", id, buf[1]);
        return -4;
    }

    struct eps_status *obj = eps_status_get();

    switch (buf[1])
    {
    case READ_HOLDING_REGISTERS:
        LOG_I("[%d][eps] ", id);
        for (uint8_t i = 0; i < buf[2] / 2; i++)
        {
            LOG_I("%.4x ", buf[3 + i * 2] << 8 | buf[4 + i * 2]);
        }
        LOG_I("\r\n");

        switch (cmd_id)
        {
        case EPS_SOFTWARE_VERSION:
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->software_version = buf[3] << 8 | buf[4];
            osMutexRelease(obj->mutex);
            break;
        case EPS_RUN_STATUS:
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->run_status = buf[3] << 8 | buf[4];
            obj->voltage_output = (float)(buf[5] << 8 | buf[6]) / 100.0f;
            obj->current_output = (float)(buf[7] << 8 | buf[8]) / 100.0f;
            obj->power_output = (float)(buf[9] << 8 | buf[10]) / 100.0f;
            osMutexRelease(obj->mutex);
            break;
        case EPS_FAULT_STOP:
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->fault_stop = buf[3] << 8 | buf[4];
            obj->fault_cur = buf[7] << 8 | buf[8];
            obj->fault_record[0] = buf[9] << 8 | buf[10];
            obj->fault_record[1] = buf[11] << 8 | buf[12];
            obj->fault_record[2] = buf[13] << 8 | buf[14];
            obj->fault_record[3] = buf[15] << 8 | buf[16];
            obj->fault_code = buf[17] << 24 | buf[18] << 16 | buf[19] << 8 | buf[20];
            osMutexRelease(obj->mutex);
            break;
        case EPS_FAN_FAULT_ENABLE:
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->fan_fault_enable = buf[3] << 8 | buf[4];
            osMutexRelease(obj->mutex);
            break;
        case EPS_COMMUNICATION_STORAGE:
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->storage = buf[3] << 8 | buf[4];
            osMutexRelease(obj->mutex);
            break;
        default:
            break;
        }
        break;
    case READ_INPUT_REGISTERS:
        LOG_I("[%d][eps] ", id);
        for (uint8_t i = 0; i < buf[2] / 2; i++)
        {
            LOG_I("%.4x ", buf[3 + i * 2] << 8 | buf[4 + i * 2]);
        }
        LOG_I("\r\n");
        break;
    case WRITE_SINGLE_REGISTER:
        LOG_I("[%d][eps] reg addr: %#.4x, value: %#.4x\r\n", id, buf[2] << 8 | buf[3], buf[4] << 8 | buf[5]);
        break;
    case WRITE_MULTIPLE_REGISTERS:
        LOG_I("[%d][eps] reg addr: %#.4x, len: %#.4x\r\n", id, buf[2] << 8 | buf[3], buf[4] << 8 | buf[5]);
        break;
    default:
        LOG_E("[%d][eps] invalid cmd type: %d\r\n", id, buf[1]);
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
#if 0
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
    data[0] = 0x00; /* TODO: 合并为2个, 连续地址可以合并 */
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
    /* 1. read software version */
    cmd.addr = DEVICE_ADDRESS_EPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.cmd_id = EPS_SOFTWARE_VERSION;
    cmd.len = 4;
    cmd.data = data;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 2. read run status */
    cmd.cmd_id = EPS_RUN_STATUS;
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x04;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -2;
    }

    /* 3. read fault stop */
    cmd.cmd_id = EPS_FAULT_STOP;
    data[0] = 0x00;
    data[1] = 0xC8;
    data[2] = 0x00;
    data[3] = 0x09;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -3;
    }

    /* 4. read fan fault stop enable */
    cmd.cmd_id = EPS_FAN_FAULT_ENABLE;
    data[0] = 0x00;
    data[1] = 0xD5;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -4;
    }

    /* 5. read communication storage */
    cmd.cmd_id = EPS_COMMUNICATION_STORAGE;
    data[0] = 0x01;
    data[1] = 0x4B;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -5;
    }
#endif
    return 0;
}

static int8_t eps_init(void)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};
    uint8_t data[10] = {0x01, 0x2C, 0x00, 0x01};

    /* 1. read software version */
    cmd.addr = DEVICE_ADDRESS_EPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.cmd_id = EPS_SOFTWARE_VERSION;
    cmd.len = 4;
    cmd.data = data;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 2. read 001: status */
    cmd.cmd_id = EPS_RUN_STATUS;
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

    /* 3. read 002: voltage */
    cmd.cmd_id = EPS_VOLTAGE_OUTPUT;
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

    /* 4. read 003: current */
    cmd.cmd_id = EPS_CURRENT_OUTPUT;
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

    /* 5. link menu init */
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
static int8_t eps_read_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};
    uint8_t buf[16] = {0};

    uint16_t reg_addr = strtoul(argv[1], NULL, 16);
    uint16_t len = atoi(argv[2]);
    buf[0] = reg_addr >> 8;
    buf[1] = reg_addr;
    buf[2] = len >> 8;
    buf[3] = len;

    cmd.addr = DEVICE_ADDRESS_EPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = buf;

    return uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
}
MSH_CMD_EXPORT_ALIAS(eps_read_test, eps_read_test, read eps value);
static int8_t eps_write_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};
    uint8_t buf[16] = {0};

    uint16_t reg_addr = strtoul(argv[1], NULL, 16);
    uint16_t data = atoi(argv[2]);
    buf[0] = reg_addr >> 8;
    buf[1] = reg_addr;
    buf[2] = data >> 8;
    buf[3] = data;

    cmd.addr = DEVICE_ADDRESS_EPS;
    cmd.type = WRITE_SINGLE_REGISTER;
    cmd.len = 4;
    cmd.data = buf;

    return uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
}
MSH_CMD_EXPORT_ALIAS(eps_write_test, eps_write_test, write eps value);

static int8_t eps_cmd_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};
    uint8_t buf[16] = {0};

    switch (atoi(argv[1]))
    {
    case 0:
        ret = eps_read_test(argc - 1, argv + 1);
        break;
    case 1:
        ret = eps_write_test(argc - 1, argv + 1);
        break;
    case 2:
        ret = eps_link_menu_value_read();
        break;
    default:
        break;
    }

    return ret;
}
MSH_CMD_EXPORT_ALIAS(eps_cmd_test, eps_cmd_test, eps cmd test);
#endif