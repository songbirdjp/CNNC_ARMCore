#include "bgm_error.h"
#include "ulog.h"
#include "cmsis_os2.h"
#include "init_call.h"

struct bgm_error_info
{
    union
    {
        uint32_t bytes;
        struct
        {
            uint32_t afc_link_stat : 1;
            uint32_t dose1_link_stat : 1;
            uint32_t dose2_link_stat : 1;
            uint32_t plan_data : 1;
            uint32_t reserved : 28;
        }bits;
    }error_code;

    osMutexId_t mutex;
};

static struct bgm_error_info bgm_error_info_obj = {0};
static struct bgm_error_info *bgm_error_info_object_get(void)
{
    return &bgm_error_info_obj;
}
static int8_t bgm_error_init(void)
{
    osMutexAttr_t attr = {
    .name = "bgm_error_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    bgm_error_info_object_get()->mutex = osMutexNew(&attr);
    if (bgm_error_info_object_get()->mutex == NULL)
    {
        LOG_E("bgm error mutex create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_ENV_EXPORT(bgm_error_init);

int8_t bgm_error_info_clear(void)
{
    struct bgm_error_info *obj = bgm_error_info_object_get();
    osMutexAcquire(obj->mutex, osWaitForever);
    obj->error_code.bytes = 0;
    osMutexRelease(obj->mutex);
    return 0;
}

uint32_t bgm_error_info_get(enum bgm_error_type type)
{
    uint32_t value = 0;
    struct bgm_error_info *obj = bgm_error_info_object_get();
    osMutexAcquire(obj->mutex, osWaitForever);

    switch (type)
    {
    case BGM_ERROR_AFC_LINK:
        value = obj->error_code.bits.afc_link_stat;
        break;
    case BGM_ERROR_DOSE1_LINK:
        value = obj->error_code.bits.dose1_link_stat;
        break;
    case BGM_ERROR_DOSE2_LINK:
        value = obj->error_code.bits.dose2_link_stat;
        break;
    case BGM_ERROR_PLAN_DATA:
        value = obj->error_code.bits.plan_data;
        break;
    case BGM_ERROR_ALL:
        value = obj->error_code.bytes;
        break;
    default:
        LOG_E("invalid bgm error type: %d\r\n", type);
        value = 0xFFFFFFFF;
        break;
    }

    osMutexRelease(obj->mutex);

    return value;
}

int8_t bgm_error_info_set(enum bgm_error_type type, uint8_t value)
{
    int8_t ret = 0;
    struct bgm_error_info *obj = bgm_error_info_object_get();
    osMutexAcquire(obj->mutex, osWaitForever);

    switch (type)
    {
    case BGM_ERROR_AFC_LINK:
        obj->error_code.bits.afc_link_stat = value == 0 ? 0 : 1;
        break;
    case BGM_ERROR_DOSE1_LINK:
        obj->error_code.bits.dose1_link_stat = value == 0 ? 0 : 1;
        break;
    case BGM_ERROR_DOSE2_LINK:
        obj->error_code.bits.dose2_link_stat = value == 0 ? 0 : 1;
        break;
    case BGM_ERROR_PLAN_DATA:
        obj->error_code.bits.plan_data = value == 0 ? 0 : 1;
        break;
    default:
        LOG_E("invalid bgm error type: %d\r\n", type);
        ret =  -1;
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
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