#include "interlock_app.h"
#include "cmsis_os2.h"
#include "adcs7476.h"
#include "init_call.h"
#include "fsm_app.h"

struct interlock_status
{
    union
    {
        uint16_t bytes;
        struct
        {
            uint16_t board_power_fault : 1;
            uint16_t hv_limit : 1;
            uint16_t comm_timeout : 1;
            uint16_t wdt_fault : 1;
            uint16_t adcs7476_1_limit_high : 1;
            uint16_t adcs7476_1_limit_low : 1;
            uint16_t adcs7476_2_limit_high :1;
            uint16_t adcs7476_2_limit_low : 1;
            uint16_t illegal_write : 1;
            uint16_t dose_rate_low : 1;
            uint16_t dose_rate_high : 1;
            uint16_t dose_total_low : 1;
            uint16_t dose_total_high : 1;
            uint16_t dose_symmetry_fault : 1;
            uint16_t dose_dummy_timeout : 1;
            uint16_t reserved : 1;
        } bits;
    }value;

    osMutexId_t mutex;
};

static struct interlock_status interlock_stat = {0};

static struct interlock_status *interlock_stat_get(void)
{
    return &interlock_stat;
}

uint16_t interlock_status_get(void)
{
    struct interlock_status *stat = interlock_stat_get();

    osMutexAcquire(stat->mutex, osWaitForever);
    uint16_t value = stat->value.bytes;
    osMutexRelease(stat->mutex);

    return value;
}

int8_t interlock_status_set(enum interlock_status_bits bit, uint8_t value)
{
    int8_t ret = 0;
    struct interlock_status *stat = interlock_stat_get();

    osMutexAcquire(stat->mutex, osWaitForever);

    switch (bit)
    {
    case INTERLOCK_BOARD_POWER_FAULT:
        stat->value.bits.board_power_fault = value;
        break;
    case INTERLOCK_HV_LIMIT:
        stat->value.bits.hv_limit = value;
        break;
    case INTERLOCK_COMM_TIMEOUT:
        stat->value.bits.comm_timeout = value;
        break;
    case INTERLOCK_WDT_FAULT:
        stat->value.bits.wdt_fault = value;
        break;
    case INTERLOCK_ADCS7476_1_LIMIT_HIGH:
        stat->value.bits.adcs7476_1_limit_high = value;
        break;
    case INTERLOCK_ADCS7476_1_LIMIT_LOW:
        stat->value.bits.adcs7476_1_limit_low = value;
        break;
    case INTERLOCK_ADCS7476_2_LIMIT_HIGH:
        stat->value.bits.adcs7476_2_limit_high = value;
        break;
    case INTERLOCK_ADCS7476_2_LIMIT_LOW:
        stat->value.bits.adcs7476_2_limit_low = value;
        break;
    case INTERLOCK_ILLEGAL_WRITE:
        stat->value.bits.illegal_write = value;
        break;
    case INTERLOCK_DOSE_RATE_LOW:
        stat->value.bits.dose_rate_low = value;
        break;
    case INTERLOCK_DOSE_RATE_HIGH:
        stat->value.bits.dose_rate_high = value;
        break;
    case INTERLOCK_DOSE_TOTAL_LOW:
        stat->value.bits.dose_total_low = value;
        break;
    case INTERLOCK_DOSE_TOTAL_HIGH:
        stat->value.bits.dose_total_high = value;
        break;
    case INTERLOCK_DOSE_SYMMETRY_FAULT:
        stat->value.bits.dose_symmetry_fault = value;
        break;
    case INTERLOCK_DOSE_DUMMY_TIMEOUT:
        stat->value.bits.dose_dummy_timeout = value;
        break;
    default:
        printf("invalid interlock bit: %d\r\n", bit);
        ret = -1;
        break;
    }

    osMutexRelease(stat->mutex);

    return ret;
}

int8_t interlock_status_cleanup(void)
{
    struct interlock_status *stat = interlock_stat_get();

    osMutexAcquire(stat->mutex, osWaitForever);
    stat->value.bytes = 0;
    osMutexRelease(stat->mutex);

    return 0;    
}

static int8_t interlock_status_update(void)
{
    struct interlock_status *stat = interlock_stat_get();

    osMutexAcquire(stat->mutex, osWaitForever);

    /* 1. check board power and hv status */
    int8_t ret = board_power_limit_fault_get();
    if (ret > 0)
    {
        stat->value.bits.board_power_fault = (ret & ~(1 << 1)) ? 1 : 0;
        stat->value.bits.hv_limit = (ret & (1 << 1)) ? 1 : 0;
    }

    /* 2. check communication status */
    // stat->value.bits.comm_timeout = 0;

    /* 3. check wdt status */
    // stat->value.bits.wdt_fault = 0;

    /* 4. check adcs7476 status */
    ret = adcs7476_object_data_limit_fault_get(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT);
    if (ret > 0)
    {
        stat->value.bits.adcs7476_1_limit_high = (ret & (1 << 1)) ? 1 : 0;
        stat->value.bits.adcs7476_1_limit_low = (ret & (1 << 0)) ? 1 : 0;
    }
    
    ret = adcs7476_object_data_limit_fault_get(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT);
    if (ret > 0)
    {
        stat->value.bits.adcs7476_2_limit_high = (ret & (1 << 1)) ? 1 : 0;
        stat->value.bits.adcs7476_2_limit_low = (ret & (1 << 0)) ? 1 : 0;
    }

    /* 5. check illegal write status */
    // stat->value.bits.illegal_write = 0;

    /* 6. check dose rate status */
    // stat->value.bits.dose_rate_low = 0;
    // stat->value.bits.dose_rate_high = 0;

    /* 7. check dose total status */
    // stat->value.bits.dose_total_low = 0;
    // stat->value.bits.dose_total_high = 0;

    /* 8. check dose symmetry status */
    // stat->value.bits.dose_symmetry_fault = 0;

    /* 9. check dose dummy status */
    // stat->value.bits.dose_dummy_timeout = 0;

    osMutexRelease(stat->mutex);

    if (interlock_status_get() != 0 && fsm_state_get() != FSM_STATE_INIT)
    {
        ret = fsm_state_switch(FSM_STATE_FAULT);
        if (ret != 0)
        {
            printf("fsm state switch err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t timer_callback(void *argument)
{
    return interlock_status_update();
}

static int8_t interlock_app_init(void)
{
    osMutexAttr_t mutex_attr = {
    .name = "interlock_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    interlock_stat_get()->mutex = osMutexNew(&mutex_attr);
    if (interlock_stat_get()->mutex == NULL)
    {
        printf("mutex create failed\r\n");
        return -1;
    }

    osTimerId_t timer_id = osTimerNew(timer_callback, osTimerPeriodic, NULL, NULL);
    if (timer_id == NULL)
    {
        printf("timer create failed\r\n");
        return -2;
    }

    osStatus_t stat = osTimerStart(timer_id, 100);  /* start timer with 100ms interval */
    if (stat != osOK)
    {
        printf("timer start err: %d\r\n", stat);
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(interlock_app_init);