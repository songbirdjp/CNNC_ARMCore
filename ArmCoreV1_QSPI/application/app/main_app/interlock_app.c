#include "interlock_app.h"
#include "cmsis_os2.h"
#include "init_call.h"
#include "fsm_app.h"
#include "ulog.h"
#include "gpio_port.h"
#include "os_tool.h"
#include "dose_uart.h"

struct fault_info
{
    uint16_t com_timeout_cnt;
    uint16_t adcs7476_1_offset_limit_low_cnt;
    uint16_t adcs7476_1_offset_limit_high_cnt;
    uint16_t adcs7476_2_offset_limit_low_cnt;
    uint16_t adcs7476_2_offset_limit_high_cnt;
    uint16_t dose_rate_low_cnt;
    uint16_t dose_rate_high_cnt;
    uint16_t dose_cp_low_cnt;
    uint16_t dose_cp_high_cnt;
    uint16_t dose_symmetry_fault_cnt;
    uint32_t dose_dummy_end_time;

#define COM_TIMEOUT_THRESHOLD_CNT   10
#define ADCS7476_1_OFFSET_LIMIT_THRESHOLD_CNT   1000
#define ADCS7476_2_OFFSET_LIMIT_THRESHOLD_CNT   1000
#define DOSE_RATE_THRESHOLD_CNT 1000
#define DOSE_CP_THRESHOLD_CNT   1
#define DOSE_SYMMETRY_FAULT_THRESHOLD_CNT   1000
#define DOSE_DUMMY_TIMEOUT_THRESHOLD_MS 1000
};

struct interlock_status
{
    union
    {
        uint32_t bytes;
        struct
        {
            uint32_t board_power_fault : 1;
            uint32_t hv_limit : 1;
            uint32_t comm_timeout : 1;
            uint32_t wdt_fault : 1;
            uint32_t adcs7476_1_offset_limit_high : 1;
            uint32_t adcs7476_1_offset_limit_low : 1;
            uint32_t adcs7476_2_offset_limit_high :1;
            uint32_t adcs7476_2_offset_limit_low : 1;
            uint32_t illegal_write : 1;
            uint32_t dose_rate_low : 1;
            uint32_t dose_rate_high : 1;
            uint32_t dose_cp_low : 1;
            uint32_t dose_cp_high : 1;
            uint32_t dose_symmetry_fault : 1;
            uint32_t dose_dummy_timeout : 1;
            uint32_t reserved : 17;
        } bits;
    }value;

    struct fault_info fault_info;

    uint32_t value_locked;  /* record interlock status when fsm entry terminate */

    osMutexId_t mutex;
};

static struct control_para *control_data = NULL;

static struct interlock_status interlock_stat = {0};

static struct interlock_status *interlock_stat_get(void)
{
    return &interlock_stat;
}

int8_t interlock_status_value_locked_set(uint32_t value)
{
    osStatus_t stat = osOK;
    struct interlock_status *obj = interlock_stat_get();
    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }

    obj->value_locked = value;

    osMutexRelease(obj->mutex);

    return 0;
}

uint32_t interlock_status_get(void)
{
    osStatus_t stat = osOK;
    struct interlock_status *obj = interlock_stat_get();

    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }
    uint32_t value = obj->value_locked == 0 ? obj->value.bytes : obj->value_locked;
    osMutexRelease(obj->mutex);

    return value;
}

int8_t interlock_status_cleanup(void)
{
    int8_t ret = interlock_fault_info_clear();
    if (ret != 0)
    {
        LOG_E("interlock fault info clear err: %d\r\n", ret);
        return ret;
    }

    osStatus_t stat = osOK;
    struct interlock_status *obj = interlock_stat_get();

    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }
    uint8_t wdt_fault = obj->value.bits.wdt_fault;
    obj->value.bytes = 0;
    obj->value_locked = 0;
    osMutexRelease(obj->mutex);

    if (wdt_fault)
    {
        gpio_common_get()->write("GPIOD_1", 0);
        gpio_common_get()->write("GPIOD_1", 1);
    }

    return 0;
}

int8_t interlock_fault_info_set(enum interlock_fault_info type, uint32_t value)
{
    int8_t ret = 0;
    osStatus_t stat = osOK;
    struct interlock_status *obj = interlock_stat_get();
    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }

    struct fault_info *info = &obj->fault_info;

    switch (type)
    {
    case INTERLOCK_FAULT_COM_TIMEOUT:
        info->com_timeout_cnt += value;
        break;
    case INTERLOCK_FAULT_ADCS7476_1_OFFSET_LIMIT_LOW:
        info->adcs7476_1_offset_limit_low_cnt += value;
        break;
    case INTERLOCK_FAULT_ADCS7476_1_OFFSET_LIMIT_HIGH:
        info->adcs7476_1_offset_limit_high_cnt += value;
        break;
    case INTERLOCK_FAULT_ADCS7476_2_OFFSET_LIMIT_LOW:
        info->adcs7476_2_offset_limit_low_cnt += value;
        break;
    case INTERLOCK_FAULT_ADCS7476_2_OFFSET_LIMIT_HIGH:
        info->adcs7476_2_offset_limit_high_cnt += value;
        break;
    case INTERLOCK_FAULT_DOSE_RATE_LOW:
        info->dose_rate_low_cnt += value;
        break;
    case INTERLOCK_FAULT_DOSE_RATE_HIGH:
        info->dose_rate_high_cnt += value;
        break;
    case INTERLOCK_FAULT_DOSE_CP_LOW:
        info->dose_cp_low_cnt += value;
        break;
    case INTERLOCK_FAULT_DOSE_CP_HIGH:
        info->dose_cp_high_cnt += value;
        break;
    case INTERLOCK_FAULT_DOSE_SYMMETRY_FAULT:
        info->dose_symmetry_fault_cnt += value;
        break;
    case INTERLOCK_FAULT_DOSE_DUMMY_END_TIME:
         info->dose_dummy_end_time = value;
        break;
    default:
        ret = -1;
        LOG_E("invalid interlock fault type: %d\r\n", type);
        break;
    }

    osMutexRelease(obj->mutex);

    return ret;
}

int8_t interlock_fault_info_clear(void)
{
    osStatus_t stat = osOK;
    struct interlock_status *obj = interlock_stat_get();
    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }
    memset(&obj->fault_info, 0, sizeof(struct fault_info));
    osMutexRelease(obj->mutex);

    return 0;
}

static int8_t (*interlock_fault_callback)(uint32_t interlock) = NULL;

int8_t interlock_fault_register_callback(int8_t (*cb)(uint32_t interlock))
{
    interlock_fault_callback = cb;

    return 0;
}

static uint32_t interlock_override_get(void)
{
    osStatus_t stat = osOK;

    stat = osMutexAcquire(control_data->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(control_data->mutex);
    }
    uint32_t override = control_data->interlock.interlock_override;
    osMutexRelease(control_data->mutex);

    return override;
}

static int8_t interlock_status_update(void)
{
    int8_t ret = 0;
    osStatus_t stat = osOK;
    struct interlock_status *obj = interlock_stat_get();

    stat = osMutexAcquire(obj->mutex, MUTEX_TIMEOUT_MS);
    if (stat != osOK)
    {
        os_tool_mutex_holder_get(obj->mutex);
    }

    /* 1. check board power and hv status */
    ret = board_power_limit_fault_get();
    if (ret > 0)
    {
        obj->value.bits.board_power_fault = (ret & ~(1 << 1)) ? 1 : 0;
        obj->value.bits.hv_limit = (ret & (1 << 1)) ? 1 : 0;
    }
    else if (ret == 0)
    {
        obj->value.bits.board_power_fault = 0;
        obj->value.bits.hv_limit = 0;
    }

    /* 2. check communication status */
    obj->value.bits.comm_timeout = obj->fault_info.com_timeout_cnt < COM_TIMEOUT_THRESHOLD_CNT ? 0 : 1;

    /* 3. check wdt status */
    obj->value.bits.wdt_fault = gpio_common_get()->read("GPIOD_5") == GPIO_PIN_SET ? 0 : 1;
    gpio_common_get()->write("GPIOE_5", obj->value.bits.wdt_fault);

    /* 4. check adcs7476 status */
    obj->value.bits.adcs7476_1_offset_limit_low = obj->fault_info.adcs7476_1_offset_limit_low_cnt < ADCS7476_1_OFFSET_LIMIT_THRESHOLD_CNT ? 0 : 1;
    obj->value.bits.adcs7476_1_offset_limit_high = obj->fault_info.adcs7476_1_offset_limit_high_cnt < ADCS7476_1_OFFSET_LIMIT_THRESHOLD_CNT ? 0 : 1;
    obj->value.bits.adcs7476_2_offset_limit_low = obj->fault_info.adcs7476_2_offset_limit_low_cnt < ADCS7476_2_OFFSET_LIMIT_THRESHOLD_CNT ? 0 : 1;
    obj->value.bits.adcs7476_2_offset_limit_high = obj->fault_info.adcs7476_2_offset_limit_high_cnt < ADCS7476_2_OFFSET_LIMIT_THRESHOLD_CNT ? 0 : 1;

    /* 5. check illegal write status */
    // obj->value.bits.illegal_write = 0;

    /* 6. check dose rate status */
    obj->value.bits.dose_rate_low = obj->fault_info.dose_rate_low_cnt < DOSE_RATE_THRESHOLD_CNT ? 0 : 1;
    obj->value.bits.dose_rate_high = obj->fault_info.dose_rate_high_cnt < DOSE_RATE_THRESHOLD_CNT ? 0 : 1;

    /* 7. check dose control point status */
    obj->value.bits.dose_cp_low = obj->fault_info.dose_cp_low_cnt < DOSE_CP_THRESHOLD_CNT ? 0 : 1;
    obj->value.bits.dose_cp_high = obj->fault_info.dose_cp_high_cnt < DOSE_CP_THRESHOLD_CNT ? 0 : 1;

    /* 8. check dose symmetry status */
    obj->value.bits.dose_symmetry_fault = obj->fault_info.dose_symmetry_fault_cnt < DOSE_SYMMETRY_FAULT_THRESHOLD_CNT ? 0 : 1;

    /* 9. check dose dummy timeout status */
    if (fsm_state_get() == FSM_STATE_PRELIMINARY_BEGIN)
    {
        uint32_t time_now = osKernelGetTickCount() * 1000 / osKernelGetTickFreq();
        uint32_t timestamp_dummy_end = interlock_stat_get()->fault_info.dose_dummy_end_time;

        obj->value.bits.dose_dummy_timeout = time_now > timestamp_dummy_end + DOSE_DUMMY_TIMEOUT_THRESHOLD_MS ? 1 : 0;

        if (obj->value.bits.dose_dummy_timeout)
        {
            LOG_E("dummy timeout\r\n");
        }
    }

    osMutexRelease(obj->mutex);

    // LOG_I("interlock status: %#.8x\r\n", interlock_status_get());

    /* deal with interlock override */
    uint32_t interlock_with_override = interlock_status_get() & ~interlock_override_get();

    if (interlock_with_override != 0 && interlock_fault_callback != NULL)
    {
        ret = interlock_fault_callback(interlock_with_override);
        if (ret != 0)
        {
            LOG_E("interlock fault callback err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t interlock_detect_entry(void *argument)
{
    for (;;)
    {
        interlock_status_update();
        osDelay(100);
    }

    return 0;
}

static int8_t interlock_app_init(void)
{
    int8_t ret = control_data_pointer_get((void **)&control_data);
    if (ret != 0 || control_data == NULL)
    {
        LOG_E("control data pointer get err: %d\r\n", ret);
        return -1;
    }

    osMutexAttr_t mutex_attr = {
    .name = "interlock_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    interlock_stat_get()->mutex = osMutexNew(&mutex_attr);
    if (interlock_stat_get()->mutex == NULL)
    {
        LOG_E("mutex create failed\r\n");
        return -2;
    }

    osThreadAttr_t thread_attr = {
        .name = "interlock_detect_thread",
        .stack_size = 1024 * 4,
        .priority = osPriorityNormal,
    };
    osThreadId_t thread_id = osThreadNew(interlock_detect_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        LOG_E("thread create failed\r\n");
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(interlock_app_init);

#ifndef INTERLOCK_TEST
#include "shell.h"
static int8_t interlock_status_test(uint8_t argc, char **argv)
{
    switch (atoi(argv[1]))
    {
    case 0:
        interlock_status_cleanup();
        break;
    case 1:
        LOG_I("interlock status: %#.8x\r\n", interlock_status_get());
        break;
    case 2:
        {
            struct interlock_status *stat = interlock_stat_get();
            struct fault_info *info = &stat->fault_info;
            osMutexAcquire(stat->mutex, MUTEX_TIMEOUT_MS);
            LOG_I("com_timeout_cnt: %d\r\n", info->com_timeout_cnt);
            LOG_I("adcs7476_1_offset_limit_low_cnt: %d\r\n", info->adcs7476_1_offset_limit_low_cnt);
            LOG_I("adcs7476_1_offset_limit_high_cnt: %d\r\n", info->adcs7476_1_offset_limit_high_cnt);
            LOG_I("adcs7476_2_offset_limit_low_cnt: %d\r\n", info->adcs7476_2_offset_limit_low_cnt);
            LOG_I("adcs7476_2_offset_limit_high_cnt: %d\r\n", info->adcs7476_2_offset_limit_high_cnt);
            LOG_I("dose_rate_low_cnt: %d\r\n", info->dose_rate_low_cnt);
            LOG_I("dose_rate_high_cnt: %d\r\n", info->dose_rate_high_cnt);
            LOG_I("dose_cp_low_cnt: %d\r\n", info->dose_cp_low_cnt);
            LOG_I("dose_cp_high_cnt: %d\r\n", info->dose_cp_high_cnt);
            LOG_I("dose_symmetry_fault_cnt: %d\r\n", info->dose_symmetry_fault_cnt);
            LOG_I("dose_dummy_end_time: %d\r\n", info->dose_dummy_end_time);
            osMutexRelease(stat->mutex);
        }
        break;
    default:
        break;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(interlock_status_test, interlock_status_test, test interlock status);
#endif