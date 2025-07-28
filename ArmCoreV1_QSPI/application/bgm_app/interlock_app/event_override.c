#include "event_override.h"
#include "cmsis_os2.h"
#include "init_call.h"
#include "ulog.h"
#include "bgm_app.h"

struct unready_event_info
{
    uint32_t unready_event;

    osMutexId_t mutex;
};

static struct unready_event_info unready_event = {0};
static struct unready_event_info *unready_info_get(void)
{
    return &unready_event;
}

static int8_t unready_event_polling_entry(void *argument)
{
    uint32_t event = 0;
    struct unready_event_info *info = (struct unready_event_info *)argument;

    for (;;)
    {
        osDelay(100);

        event = 0;

        /* bit0: plan data */
        event |= checkPlanRecvStatus() == 0 ? 0 : 1;
        /* TODO: add other event check */

        osMutexAcquire(info->mutex, osWaitForever);
        info->unready_event = event;
        osMutexRelease(info->mutex);
    }

    return 0;
}

static int8_t unready_event_thread_init(void)
{
    osMutexAttr_t mutex_attr = 
    {
        .name = "unready_event_mutex",
        .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    unready_info_get()->mutex = osMutexNew(&mutex_attr);
    if (unready_info_get()->mutex == NULL)
    {
        LOG_E("mutex create failed");
        return -1;
    }

    osThreadAttr_t attr = 
    {
        .name = "unready_event_thread",
        .stack_size = 1024 * 4,
        .priority = osPriorityNormal,
    };

    osThreadId_t unready_event_thread_id = osThreadNew(unready_event_polling_entry, unready_info_get(), &attr);
    if (unready_event_thread_id == NULL)
    {
        LOG_E("thread unready event create failed");
        return -2;
    }

    return 0;
}
INIT_APP_EXPORT(unready_event_thread_init);

uint32_t unready_event_get(void)
{
    uint32_t event = 0;
    struct unready_event_info *info = unready_info_get();

    osMutexAcquire(info->mutex, osWaitForever);
    event = info->unready_event;
    osMutexRelease(info->mutex);

    return event;
}

static uint32_t unready_override_get(void)
{
    uint32_t override = 0;
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    override = obj->unready_override;
    osMutexRelease(obj->mutex);

    return override;
}

int8_t unready_event_with_override_get(enum unready_event type)
{
    int8_t ret = 0;
    uint32_t unready_event = unready_event_get();
    uint32_t unready_override = unready_override_get();
    uint32_t value = unready_event & ~unready_override;

    switch (type)
    {
    case UNREADY_EVENT_PLAN_DATA:
        ret = (value >> UNREADY_EVENT_PLAN_DATA) & 0x01;
        break;

    default:
        ret = -1;
        LOG_E("invalid unready event type: %d\r\n", type);
        break;
    }

    return ret;
}

static uint32_t interlock_override_get(void)
{
    uint32_t override = 0;
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    override = obj->interlock_override;
    osMutexRelease(obj->mutex);

    return override;
}

int8_t interlock_with_override_get(enum interlock_event type)
{
    int8_t ret = 0;
    uint32_t interlock = 0;  /* TODO: add interlock get function here */
    uint32_t interlock_override = interlock_override_get();
    uint32_t value = interlock & ~interlock_override;

    switch (type)
    {
    case INTERLOCK_EVENT_PLAN_DATA:
        ret = (value >> INTERLOCK_EVENT_PLAN_DATA) & 0x01;
        break;

    default:
        ret = -1;
        LOG_E("invalid interlock event type: %d\r\n", type);
        break;
    }

    return ret;
}


#ifndef EVENT_OVERRIDE_TEST
#include "shell.h"
static int8_t event_override_test(int argc, char **argv)
{
    switch (atoi(argv[1]))
    {
    case 0:
        LOG_I("unready event: %#.8x\r\n", unready_event_get());
        LOG_I("unready override: %#.8x\r\n", unready_override_get());
        break;
    case 1:
        LOG_I("unready event with override: %d\r\n", unready_event_with_override_get(atoi(argv[2])));
        break;
    case 2:
        LOG_I("interlock overide: %#.8x\r\n", interlock_override_get());
        break;
    case 3:
        LOG_I("interlock with override: %d\r\n", interlock_with_override_get(atoi(argv[2])));
        break;
    default:
        break;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(event_override_test, event_override_test, event override test);
#endif