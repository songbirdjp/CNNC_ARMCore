#include "bgm_error.h"
#include "ulog.h"

struct bgm_error_info
{
    union
    {
        uint32_t bytes;
        struct
        {
            uint32_t plan_data : 1;
            uint32_t reserved : 31;
        }bits;
    }error_code;
};

static struct bgm_error_info bgm_error_info_obj = {0};
static struct bgm_error_info *bgm_error_info_object_get(void)
{
    return &bgm_error_info_obj;
}

int8_t bgm_error_info_clear(void)
{
    struct bgm_error_info *obj = bgm_error_info_object_get();
    obj->error_code.bytes = 0;
    return 0;
}

int32_t bgm_error_info_get(enum bgm_error_type type)
{
    int32_t ret = 0;
    struct bgm_error_info *obj = bgm_error_info_object_get();

    switch (type)
    {
    case BGM_ERROR_PLAN_DATA:
        ret = obj->error_code.bits.plan_data;
        break;
    case BGM_ERROR_ALL:
        ret = obj->error_code.bytes;
        break;
    default:
        LOG_E("invalid bgm error type: %d\r\n", type);
        return -1;
        break;
    }

    return ret;
}

int8_t bgm_error_info_set(enum bgm_error_type type, uint8_t value)
{
    struct bgm_error_info *obj = bgm_error_info_object_get();

    switch (type)
    {
    case BGM_ERROR_PLAN_DATA:
        obj->error_code.bits.plan_data = value == 0 ? 0 : 1;
        break;
    default:
        LOG_E("invalid bgm error type: %d\r\n", type);
        return -1;
        break;
    }

    return 0;
}


#ifndef BGM_ERROR_TEST
#include "shell.h"
static int8_t bgm_error_test(uint8_t argc, uint8_t **argv)
{
    switch (atoi(argv[1]))
    {
    case 0:
        LOG_I("bgm error all: %#.8x\r\n", bgm_error_info_get(BGM_ERROR_ALL));
        break;
    case 1:
        bgm_error_info_set(atoi(argv[2]), atoi(argv[3]));
        break;
    default:
        break;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_error_test, bgm_error_test, bgm error test command);
#endif