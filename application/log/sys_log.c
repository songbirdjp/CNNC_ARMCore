#include "sys_log.h"
#include "init_call.h"
#include "ulog.h"

struct log_info
{
    uint32_t addr_begin;
    uint32_t addr_end;
    uint32_t size;
    uint32_t per_log_length;
    uint32_t addr_for_write;
    uint32_t num_of_logs;
};
static struct log_info log_info_init = 
{
    .addr_begin = SYS_LOG_ADDR_BEGIN + sizeof(struct log_info),
    .addr_end = SYS_LOG_ADDR_END,
    .size = SYS_LOG_ADDR_END - SYS_LOG_ADDR_BEGIN - sizeof(struct log_info),
    .per_log_length = SYS_LOG_PER_LENGTH,
    .addr_for_write = SYS_LOG_ADDR_BEGIN  + sizeof(struct log_info),
    .num_of_logs = 0,
};

static struct log_info info __attribute__((section(".sdram_ext_remain"))) = {0};
static struct log_info *log_info_get(void)
{
    return &info;
}

static int8_t device_sdram_write(uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;
    struct log_info *obj = log_info_get();

    if (len > obj->per_log_length)
    {
        LOG_E("len is greater than per_log_length: %d(%d)\r\n", len, obj->per_log_length);
        return -1;
    }

#ifndef SYS_LOG_OVERWRITE_MODE
    if (obj->addr_for_write + obj->per_log_length >= obj->addr_end)
    {
        LOG_E("end of address is exceed the end of sdram\r\n");
        return -2;
    }
#endif

    uint8_t buf_tmp[SYS_LOG_PER_LENGTH] = {0};
    memcpy(buf_tmp, buf, sizeof(buf_tmp));
    ret = sdram_write(obj->addr_for_write, buf_tmp, sizeof(buf_tmp), SYS_LOG_READ_WRITE_TIMEOUT);
    if (ret != 0)
    {
        printf("sdram write err: %d\r\n", ret);
        return -3;
    }

    obj->num_of_logs++;
    obj->addr_for_write += obj->per_log_length;

#ifdef SYS_LOG_OVERWRITE_MODE
    obj->addr_for_write = obj->addr_for_write + obj->per_log_length >= obj->addr_end ? obj->addr_begin : obj->addr_for_write;
#endif

    return 0;
}

static int8_t sdram_log_init(void)
{
#ifdef USING_ULOG_SDRAM
    struct ulog_write_func_info info = 
    {
        .func_init = NULL,
        .func_callback = device_sdram_write,
        .index = 3,
        .level = ULOG_INFO_LEVEL,
    };

    int8_t ret = ulog_write_func_register(&info);
    if (ret != 0)
    {
        printf("sdram log register err: %d\r\n", ret);
        return ret;
    }
#endif

    return 0;
}
INIT_COMPONENT_EXPORT(sdram_log_init);

int8_t system_power_on_post_process(void)
{
    memcpy(log_info_get(), &log_info_init, sizeof(struct log_info));

    return 0;
}

#ifndef SYS_LOG_TEST
#include "shell.h"
static int8_t sys_log_test(uint8_t argc, uint8_t **argv)
{
    int8_t ret = 0;
    uint8_t buf[SYS_LOG_PER_LENGTH] = {0};
    struct log_info *obj = log_info_get();

    switch (atoi(argv[1]))
    {
    case 0:
        printf("addr_begin: %#.8x\r\n", obj->addr_begin);
        printf("addr_end: %#.8x\r\n", obj->addr_end);
        printf("size: %#.8x\r\n", obj->size);
        printf("per_log_length: %#.8x\r\n", obj->per_log_length);
        printf("addr_for_write: %#.8x\r\n", obj->addr_for_write);
        printf("num_of_logs: %u\r\n", obj->num_of_logs);
        break;
    case 1:
        for (uint32_t idx = 0; idx < atoi(argv[2]); idx++)
        {
            snprintf(buf, sizeof(buf), "test sdram log: %d\r\n", idx);
            ret = device_sdram_write(buf, sizeof(buf));
        }
        break;
    case 2:
        uint32_t addr_output = obj->num_of_logs * obj->per_log_length;
        addr_output = addr_output >= obj->size ? obj->addr_end - obj->per_log_length : obj->addr_for_write;
        for (uint32_t idx = obj->addr_begin; idx < addr_output; idx += obj->per_log_length)
        {
            ret = sdram_read(idx, buf, obj->per_log_length, SYS_LOG_READ_WRITE_TIMEOUT);
            if (ret != 0)
            {
                LOG_E("sdram read err: %d\r\n", ret);
                continue;
            }
            printf("%s", buf);
            osDelay(1);
        }
        break;
    default:
        break;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(sys_log_test, sys_log_test, test sdram log);
#endif