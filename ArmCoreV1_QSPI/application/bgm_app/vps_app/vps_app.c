#include "vps_app.h"
#include <stddef.h>
#include "bgm_uart.h"
#include "ulog.h"
#include "init_call.h"

#define DEVICE_ADDRESS_VPS  0x02

static struct vps_status vps_status_obj = {0};
static struct vps_status *vps_status_get(void)
{
    return &vps_status_obj;
}

static int8_t vps_cmd_parse(enum uart_id id, struct cmd_object *cmd)
{
    if (cmd == NULL)
    {
        LOG_E("[%d][vps] cmd is NULL\r\n", id);
        return -1;
    }

#if 0
    LOG_I("[%d][vps] cmd id: %d, type: %d, len: %d, data: \r\n", id, cmd->id.byte, cmd->type, *cmd->len);
    for (uint8_t i = 0; i < *cmd->len + 2; i++)
    {
        LOG_I("%.2x ", cmd->data[i]);
    }
    LOG_I("\r\n");
#endif

    /* 1. check cmd id */
    if (cmd->id.byte != DEVICE_ADDRESS_VPS)
    {
        LOG_E("[%d][vps] invalid device id: %d\r\n", id, cmd->id.byte);
        return -2;
    }

    /* 2. check crc */
    uint8_t buf[128] = {0};
    buf[0] = cmd->id.byte;
    buf[1] = cmd->type;
    enum vps_read_write_id cmd_id = cmd->data[0] | cmd->data[1] << 8;
    memcpy(&buf[2], &cmd->data[2], *cmd->len);

    uint16_t crc = buf[*cmd->len] << 8 | buf[*cmd->len + 1];
    uint16_t crc_cal = modbus_crc16_cal(buf, *cmd->len);
    if (crc_cal != crc)
    {
        LOG_E("[%d][vps] crc err: %x, %x \r\n", id, crc_cal, crc);
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
        LOG_E("[%d][vps] cmd frame err: %d\r\n", id, buf[1]);
        return -4;
    }

    struct vps_status *obj = vps_status_get();

    switch (buf[1])
    {
    case READ_HOLDING_REGISTERS:
#if 0
        LOG_I("[%d][vps] ", id);
        for (uint8_t i = 0; i < buf[2] / 2; i++)
        {
            LOG_I("%.4x ", buf[3 + i * 2] << 8 | buf[4 + i * 2]);
        }
        LOG_I("\r\n");
#endif
        switch (cmd_id)
        {
        case VPS_SOFTWARE_VERSION:
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->software_version = buf[3] << 8 | buf[4];
            osMutexRelease(obj->mutex);
            break;
        case VPS_RUN_STATUS:
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->run_status = buf[3] << 8 | buf[4];
            obj->voltage_output = buf[5] << 8 | buf[6];
            obj->current_output = (float)(buf[7] << 8 | buf[8]) / 1000.0f + (float)(buf[9] << 8 | buf[10]) / 1000000.0f;
            obj->power_voltage = (float)(buf[11] << 8 | buf[12]) / 100.0f;
            obj->button_lock = buf[19] << 8 | buf[20];
            osMutexRelease(obj->mutex);
            break;
        case VPS_FAULT_STOP:
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->fault_stop = buf[3] << 8 | buf[4];
            obj->fault_cur = buf[7] << 8 | buf[8];
            obj->fault_code = buf[9] << 24 | buf[10] << 16 | buf[11] << 8 | buf[12];
            obj->fault_record[0] = buf[13] << 8 | buf[14];
            obj->fault_record[1] = buf[15] << 8 | buf[16];
            obj->fault_record[2] = buf[17] << 8 | buf[18];
            obj->fault_record[3] = buf[19] << 8 | buf[20];
            osMutexRelease(obj->mutex);
            break;
        case VPS_FIRE_COUNT:
            osMutexAcquire(obj->mutex, osWaitForever);
            obj->fire_count = buf[3] << 8 | buf[4];
            obj->fire_count_uplimit = buf[5] << 8 | buf[6];
            obj->fire_stop_time = buf[7] << 8 | buf[8];
            osMutexRelease(obj->mutex);
            break;
        default:
            break;
        }
        break;
    case READ_INPUT_REGISTERS:
        LOG_I("[%d][vps] ", id);
        for (uint8_t i = 0; i < buf[2] / 2; i++)
        {
            LOG_I("%.4x ", buf[3 + i * 2] << 8 | buf[4 + i * 2]);
        }
        LOG_I("\r\n");
        break;
    case WRITE_SINGLE_REGISTER:
        LOG_I("[%d][vps] reg addr: %#.4x, value: %#.4x\r\n", id, buf[2] << 8 | buf[3], buf[4] << 8 | buf[5]);
        break;
    case WRITE_MULTIPLE_REGISTERS:
        LOG_I("[%d][vps] reg addr: %#.4x, len: %#.4x\r\n", id, buf[2] << 8 | buf[3], buf[4] << 8 | buf[5]);
        break;
    default:
        LOG_E("[%d][vps] invalid cmd type: %d\r\n", id, buf[1]);
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

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
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

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
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

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
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

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -1;
    }
#else
#if 0
   /* 1. read software version */
   cmd.addr = DEVICE_ADDRESS_VPS;
   cmd.type = READ_HOLDING_REGISTERS;
   cmd.len = 4;
   cmd.data = data;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -1;
   }

   /* 2. read run status */
   data[0] = 0x00;
   data[1] = 0x64;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -2;
   }

   /* 3. read power voltage */
   data[0] = 0x00;
   data[1] = 0x68;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -3;
   }

   /* 4. read output voltage */
   data[0] = 0x00;
   data[1] = 0x65;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -4;
   }

   /* 5. read output current H */
   data[0] = 0x00;
   data[1] = 0x66;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -5;
   }

   /* 6. read output current L */
   data[0] = 0x00;
   data[1] = 0x67;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -6;
   }

   /* 7. read halt status */
   data[0] = 0x01;
   data[1] = 0x90;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -7;
   }

   /* 8. read reset status */
   data[0] = 0x01;
   data[1] = 0x91;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -8;
   }

   /* 9. read halt code H */
   data[0] = 0x01;
   data[1] = 0x93;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -9;
   }

   /* 10. read halt code L */
   data[0] = 0x01;
   data[1] = 0x94;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -10;
   }

   /* 11. read halt record 1 */
   data[0] = 0x01;
   data[1] = 0x95;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -11;
   }

   /* 12. read halt record 2 */
   data[0] = 0x01;
   data[1] = 0x96;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -12;
   }

   /* 13. read halt record 3 */
   data[0] = 0x01;
   data[1] = 0x97;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -13;
   }

   /* 14. read halt record 4 */
   data[0] = 0x01;
   data[1] = 0x98;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -14;
   }

   /* 15. read fire count */
   data[0] = 0x01;
   data[1] = 0xA9;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -15;
   }

   /* 16. read fire halt time */
   data[0] = 0x01;
   data[1] = 0xAB;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -16;
   }
#endif
   /* 1. read software version */
   cmd.addr = DEVICE_ADDRESS_VPS;
   cmd.type = READ_HOLDING_REGISTERS;
   cmd.cmd_id = VPS_SOFTWARE_VERSION;
   cmd.len = 4;
   cmd.data = data;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -1;
   }

   /* 2. read run status */
   cmd.cmd_id = VPS_RUN_STATUS;
   data[0] = 0x00;
   data[1] = 0x64;
   data[2] = 0x00;
   data[3] = 0x09;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -2;
   }

   /* 3. read halt status */
   cmd.cmd_id = VPS_FAULT_STOP;
   data[0] = 0x01;
   data[1] = 0x90;
   data[2] = 0x00;
   data[3] = 0x09;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -3;
   }

   /* 4. read fire count */
   cmd.cmd_id = VPS_FIRE_COUNT;
   data[0] = 0x01;
   data[1] = 0xA9;
   data[2] = 0x00;
   data[3] = 0x03;

   ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
   if (ret != 0)
   {
       LOG_E("uart modbus cmd write err: %d\r\n", ret);
       return -4;
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
    cmd.cmd_id = VPS_SOFTWARE_VERSION;
    cmd.len = 4;
    cmd.data = data;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -1;
    }

    /* 2. set 205 is remote */
    cmd.type = WRITE_SINGLE_REGISTER;
    cmd.cmd_id = VPS_REMOTE_MODE;
    cmd.len = 4;
    cmd.data = data;
    data[0] = 0x00;
    data[1] = 0xCC;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -2;
    }

    /* 3. set 203 is remote */
    cmd.cmd_id = VPS_START_MODE;
    data[0] = 0x00;
    data[1] = 0xCA;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -3;
    }

    /* 4. set 207 is remote voltage */
    // data[0] = 0x00;
    // data[1] = 0xCE;
    // data[2] = 0x0F;
    // data[3] = 0xA0;

    // ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    // if (ret != 0)
    // {
    //     LOG_E("uart modbus cmd write err: %d\r\n", ret);
    //     return -4;
    // }

    /* 5. set 202 is remote start */
    cmd.cmd_id = VPS_REMOTE_START_ENABLE;
    data[0] = 0x00;
    data[1] = 0xC9;
    data[2] = 0x00;
    data[3] = 0x01;

    ret = uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
    if (ret != 0)
    {
        LOG_E("uart modbus cmd write err: %d\r\n", ret);
        return -5;
    }

    /* 6. set link menu */
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

    ret = uart_cmd_parse_callback_register(BGM_UART_EPS_VPS + 1, vps_cmd_parse);
    if (ret != 0)
    {
        LOG_E("vps cmd parse callback register err: %d\r\n", ret);
        return -1;
    }

    ret = uart_init_callback_register(BGM_UART_EPS_VPS + 1, vps_init);
    if (ret != 0)
    {
        LOG_E("vps init callback register err: %d\r\n", ret);
        return -2;
    }

    return 0;
}
INIT_ENV_EXPORT(vps_functions_init);

static int8_t vps_status_get_entry(void *argument)
{
    int8_t ret = 0;

    osDelay(10000);

    for (;;)
    {
        ret = vps_link_menu_value_read();
        if (ret != 0)
        {
            LOG_E("vps status read err: %d\r\n", ret);
        }

        osDelay(500);
    }

    return 0;
}

static int8_t vps_thread_init(void)
{
    osThreadAttr_t attr = {
    .name = "vps_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t)osPriorityNormal,
    };

    osThreadId_t vps_threadHandle = osThreadNew(vps_status_get_entry, NULL, &attr);
    if (vps_threadHandle == NULL)
    {
        LOG_E("thread vps status get create err\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(vps_thread_init);

struct vps_status *vps_state_get(struct vps_status *buf)
{
    if (buf == NULL)
    {
        return NULL;
    }

    struct vps_status *obj = vps_status_get();
    osMutexAcquire(obj->mutex, osWaitForever);
    memcpy(buf, obj, sizeof(struct vps_status));
    osMutexRelease(obj->mutex);

    return buf;
}

#ifndef VPS_TEST
#include "shell.h"
static int8_t vps_read_test(uint8_t argc, char **argv)
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

    cmd.addr = DEVICE_ADDRESS_VPS;
    cmd.type = READ_HOLDING_REGISTERS;
    cmd.len = 4;
    cmd.data = buf;

    return uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
}
MSH_CMD_EXPORT_ALIAS(vps_read_test, vps_read_test, read vps value);
static int8_t vps_write_test(uint8_t argc, char **argv)
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

    cmd.addr = DEVICE_ADDRESS_VPS;
    cmd.type = WRITE_SINGLE_REGISTER;
    cmd.len = 4;
    cmd.data = buf;

    return uart_modbus_cmd_write(BGM_UART_EPS_VPS, &cmd);
}
MSH_CMD_EXPORT_ALIAS(vps_write_test, vps_write_test, write vps value);

static int8_t vps_cmd_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct modbus_cmd_object cmd = {0};
    uint8_t buf[16] = {0};

    switch (atoi(argv[1]))
    {
    case 0:
        ret = vps_read_test(argc - 1, argv + 1);
        break;
    case 1:
        ret = vps_write_test(argc - 1, argv + 1);
        break;
    case 2:
        ret = vps_link_menu_value_read();
        break;
    default:
        break;
    }

    return ret;
}
MSH_CMD_EXPORT_ALIAS(vps_cmd_test, vps_cmd_test, vps cmd test);
static int8_t vps_status_output(uint8_t argc, char **argv)
{
    struct vps_status *obj = vps_status_get();
    osMutexAcquire(obj->mutex, osWaitForever);
    LOG_I("software_version: %.3f\r\n", obj->software_version / 1000.0f);
    LOG_I("button_lock: %d\r\n", obj->button_lock);
    LOG_I("run_status: %d\r\n", obj->run_status);
    LOG_I("voltage_output: %f\r\n", obj->voltage_output);
    LOG_I("current_output: %f\r\n", obj->current_output);
    LOG_I("power_voltage: %f\r\n", obj->power_voltage);
    LOG_I("fault_stop: %d\r\n", obj->fault_stop);
    LOG_I("fault_cur: %d\r\n", obj->fault_cur);
    LOG_I("fault_record: %d %d %d %d\r\n", obj->fault_record[0], obj->fault_record[1], obj->fault_record[2], obj->fault_record[3]);
    LOG_I("fault_code: %d\r\n", obj->fault_code);
    LOG_I("fire_count: %d\r\n", obj->fire_count);
    LOG_I("fire_count_uplimit: %d\r\n", obj->fire_count_uplimit);
    LOG_I("fire_stop_time: %d\r\n", obj->fire_stop_time);
    osMutexRelease(obj->mutex);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(vps_status_output, vps_status_output, vps status output);
#endif