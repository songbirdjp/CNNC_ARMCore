#include "main_app.h"
#include "init_call.h"
#include "cmsis_os2.h"
#include "adcs7476.h"
#include "ltc2632.h"
#include "fsm_app.h"
#include "ulog.h"


#include "gpio_port.h"
int8_t dose_hv_enable_set(uint8_t en)
{
    return gpio_common_get()->write("GPIOB_8", (en == 0)? 0 : 1);
}

int8_t dose_trigger_out_set(uint8_t en)
{
    return gpio_common_get()->write("GPIOB_9", (en == 0)? 0 : 1);
}


#define ONE_PULSE_TIMEOUT_US    300
#define PULSE_INTERVAL_TIME_US  4000
#define ADCS7476_SERVO_VALUE    50
#define ADCS7476_VALUE_ACCUMULATE_LIMIT 100

struct dose_value
{
    uint16_t cnt_one_pulse;
    uint32_t dose_one_pulse;
    uint64_t dose_accumulated;
};

static struct dose_value dose_value_object = {0};
struct dose_value *dose_value_obj_get(void)
{
    return &dose_value_object;
}

#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
#include "lptim.h"
static uint8_t timer_delay_running_flag = 0;
#define TIM_DELAY_ONE_PULSE_TIMEOUT_US      (1 << 0)
#define TIM_DELAY_PULSE_INTERVAL_TIME_US    (1 << 1)
void LPTIM3_Callback(LPTIM_HandleTypeDef *hlptim)
{
    HAL_StatusTypeDef status = HAL_LPTIM_SetOnce_Stop_IT(hlptim);
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_SetOnce_Stop_IT err: %d\r\n", status);
    }

    if (timer_delay_running_flag & TIM_DELAY_ONE_PULSE_TIMEOUT_US)
    {
        /* one pulse timeout */
        dose_value_obj_get()->cnt_one_pulse = ONE_PULSE_TIMEOUT_US;
    }
    else if (timer_delay_running_flag & TIM_DELAY_PULSE_INTERVAL_TIME_US)
    {
        /* repeat generate a new pulse */
        int8_t ret = dose_trigger_out_set(1);
        if (ret != 0)
        {
            printf("dose_trigger_out_set err: %d\r\n", ret);
        }
    }

    timer_delay_running_flag = 0;
}

static int8_t timer_delay_start(uint16_t timeout_us)
{
    if (timeout_us * 1000 / 320 > 0xFFFF)
    {
        printf("parameter err: %u\r\n", timeout_us);
        return -1;
    }

    if (timer_delay_running_flag & 0x80 != 0)
    {
        return 0;
    }

    HAL_StatusTypeDef status = HAL_LPTIM_SetOnce_Start_IT(&hlptim3, timeout_us * 1000 / 320, timeout_us * 1000 / 320);
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_Counter_Start err: %d\r\n", status);
    }

    timer_delay_running_flag |= 0x80;

    return 0;
}
#endif
/*****************************************************************/

static int8_t adcs7476_value_servo(uint32_t value, uint16_t len, uint8_t channel)
{
    int8_t ret = 0;
    uint16_t avg = value / len;
    uint16_t pre_val = 0;
    struct ltc2632_object *ltc2632 = ltc2632_object_data_get();

    switch (channel)
    {
    case LTC2632_CHANNEL_OUTA:
        pre_val = ltc2632->out_a_value;
        avg < ADCS7476_SERVO_VALUE ? ltc2632->out_a_value++ : avg > ADCS7476_SERVO_VALUE ? ltc2632->out_a_value-- : 0;
        ltc2632->value.bits.data = ltc2632->out_a_value;
        ltc2632->value.bits.channel = LTC2632_CHANNEL_OUTA;
        break;
    case LTC2632_CHANNEL_OUTB:
        pre_val = ltc2632->out_b_value;
        avg < ADCS7476_SERVO_VALUE ? ltc2632->out_b_value++ : avg > ADCS7476_SERVO_VALUE ? ltc2632->out_b_value-- : 0;
        ltc2632->value.bits.data = ltc2632->out_b_value;
        ltc2632->value.bits.channel = LTC2632_CHANNEL_OUTB;
        break;
    case LTC2632_CHANNEL_ALL:
        break;
    default:
        printf("channel invalid: %d\r\n", channel);
        return -1;
        break;
    }

#if 1
    LOG_I("channel: %d, value: %d, avg: %d\r\n", channel, value, avg);
    LOG_I("out_a_value: %d, out_b_value: %d\r\n", ltc2632->out_a_value, ltc2632->out_b_value);
#endif

    if (pre_val != ltc2632->value.bits.data)
    {
        ret = ltc2632_data_write(ltc2632);
        if (ret != 0)
        {
            printf("ltc2632_data_write err: %d\r\n", ret);
            return ret;
        }
    }

    return 0;
}

static int8_t adcs7476_value_dose(uint64_t value, uint16_t pulse_cnt)
{
    struct dose_value *obj = dose_value_obj_get();

    obj->cnt_one_pulse += pulse_cnt;
    obj->dose_one_pulse += value;
    obj->dose_accumulated += value;

    return 0;
}

static uint16_t adcs7476_value_check(uint16_t *buf, uint16_t len, uint64_t *pulse_val, uint32_t *servo_val)
{
    uint16_t valid_cnt = 0;
    int8_t ret = 0;

    for (uint16_t i = 0; i < len; i++)
    {
        if (buf[i] >= ADCS7476_VALUE_ACCUMULATE_LIMIT)
        {
            *pulse_val += buf[i] - ADCS7476_SERVO_VALUE;
#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
            timer_delay_running_flag = TIM_DELAY_ONE_PULSE_TIMEOUT_US;
            ret = timer_delay_start(ONE_PULSE_TIMEOUT_US);
            if (ret != 0)
            {
                printf("timer_delay_start err: %d\r\n", ret);
            }
#else
            valid_cnt++;
#endif
        }
        else
        {
            *servo_val += buf[i];
        }
    }

    return valid_cnt;
}

static int8_t dose_accumulated_check(uint16_t pulse_cnt)
{
    struct dose_value *obj = dose_value_obj_get();

    if (obj->cnt_one_pulse < ONE_PULSE_TIMEOUT_US || pulse_cnt >= BUF_LEN)
    {
        return 0;
    }

    /* 1. check total dose */
    #define RADIATION_DOSE_LIMIT 1000000000 /* TODO: need reload uart recv value */
    /* TODO: 两个相邻radiation point中的剂量值是累加的关系，也即后一个点的剂量值等于前一个点的剂量值加上当前点的剂量值，因此需要考虑两点之间的差值，而不是直接累加 */
    if (obj->dose_accumulated >= RADIATION_DOSE_LIMIT)
    {
        return dose_fsm_state_set(FSM_STATE_STOP);
    }

    /* 2. prepare to repeat trigger out one pulse */
    obj->dose_one_pulse = 0;
    obj->cnt_one_pulse = 0;

    int8_t ret = dose_trigger_out_set(0);
    if (ret != 0)
    {
        printf("dose_trigger_out_set err: %d\r\n", ret);
        return ret;
    }

#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
    timer_delay_running_flag = TIM_DELAY_PULSE_INTERVAL_TIME_US;
    ret = timer_delay_start(PULSE_INTERVAL_TIME_US);
    if (ret != 0)
    {
        printf("timer_delay_start err: %d\r\n", ret);
        return ret;
    }
#endif

    return 0;
}

static int8_t adcs7476_value_process(void)
{
    int8_t ret = 0;
    uint16_t buf[BUF_LEN] = {0}, buf_1[BUF_LEN] = {0}, pulse_cnt = 0, pulse_cnt_1 = 0;
    uint32_t servo_value = 0, servo_value_1 = 0;
    uint64_t pulse_value = 0, pulse_value_1 = 0;

    /* 1. read adcs7476 2 channel data */
    ret = adcs7476_object_data_read(ADCS7476_ID1_NAME, buf, BUF_LEN, osWaitForever);
    if (ret != 0)
    {
        printf("adcs7476_object_data_read err: %d\r\n", ret);
        return ret;
    }

    ret = adcs7476_object_data_read(ADCS7476_ID2_NAME, buf_1, BUF_LEN, osWaitForever);
    if (ret != 0)
    {
        printf("adcs7476_object_data_read err: %d\r\n", ret);
        return ret;
    }

    /* 2. check value */
    pulse_cnt = adcs7476_value_check(buf, BUF_LEN, &pulse_value, &servo_value);
    pulse_cnt_1 = adcs7476_value_check(buf_1, BUF_LEN, &pulse_value_1, &servo_value_1);
    if (pulse_cnt != pulse_cnt_1 && pulse_cnt != 0 && pulse_cnt_1 != 0)
    {
        printf("adcs7476 value pulse count not match: %d, %d\r\n", pulse_cnt, pulse_cnt_1);
    }

    struct dose_value *obj = dose_value_obj_get();

    if (obj->cnt_one_pulse == 0)
    {
        if (pulse_cnt == 0 && pulse_cnt_1 == 0)
        {
            /* servo mode */
            ret = adcs7476_value_servo(servo_value, BUF_LEN, LTC2632_CHANNEL_OUTA);
            if (ret != 0)
            {
                printf("adcs7476 value servo err: %d\r\n", ret);
            }
            ret = adcs7476_value_servo(servo_value_1, BUF_LEN, LTC2632_CHANNEL_OUTB);
            if (ret != 0)
            {
                printf("adcs7476 value servo err: %d\r\n", ret);
            }
        }
        else
        {
            /* pulse mode begin */
            ret = adcs7476_value_dose(pulse_value + pulse_value_1, pulse_cnt);
            if (ret != 0)
            {
                printf("adcs7476 value dose err: %d\r\n", ret);
            }
        }
    }
    else
    {
        if (pulse_cnt < BUF_LEN || pulse_cnt_1 < BUF_LEN)
        {
            /* pulse mode end, servo mode begin */
            ret = adcs7476_value_dose(pulse_value + pulse_value_1, pulse_cnt);
            if (ret != 0)
            {
                printf("adcs7476 value dose err: %d\r\n", ret);
            }

            ret = adcs7476_value_servo(servo_value, BUF_LEN - pulse_cnt, LTC2632_CHANNEL_OUTA);
            if (ret != 0)
            {
                printf("adcs7476 value servo err: %d\r\n", ret);
            }
            ret = adcs7476_value_servo(servo_value_1, BUF_LEN - pulse_cnt_1, LTC2632_CHANNEL_OUTB);
            if (ret != 0)
            {
                printf("adcs7476 value servo err: %d\r\n", ret);
            }
        }
        else
        {
            /* pulse mode running */
            ret = adcs7476_value_dose(pulse_value + pulse_value_1, pulse_cnt);
            if (ret != 0)
            {
                printf("adcs7476 value dose err: %d\r\n", ret);
            }
        }

        ret = dose_accumulated_check(pulse_cnt);
        if (ret != 0)
        {
            printf("dose accumulated check err: %d\r\n", ret);
        }
    }

    return ret;
}


/* adcs7476 sampled data process */
static osEventFlagsId_t adcs7476_event = NULL;
#define ADCS7476_CONVERT_COMPLETE_EVENT   (1 << 0)
static int8_t adcs7476_object_data_complete_callback(uint8_t *name, uint16_t *data, uint16_t len)
{
    osEventFlagsSet(adcs7476_event, ADCS7476_CONVERT_COMPLETE_EVENT);

    return 0;
}

static int8_t data_process_entry(void *argument)
{
#ifdef USING_LPTIM3_FOR_RADIATION_TIMEOUT
    MX_LPTIM3_Init();
    HAL_StatusTypeDef status = HAL_LPTIM_RegisterCallback(&hlptim3, HAL_LPTIM_AUTORELOAD_MATCH_CB_ID, LPTIM3_Callback);
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_RegisterCallback err: %d\r\n", status);
        return -1;
    }
#endif

    int8_t ret = adcs7476_object_data_callback_register(adcs7476_object_data_complete_callback);
    if (ret != 0)
    {
        printf("adcs7476 callback register err: %d\r\n", ret);
        return -2;
    }

    for (;;)
    {
        osEventFlagsWait(adcs7476_event, ADCS7476_CONVERT_COMPLETE_EVENT, osFlagsWaitAny, osWaitForever);

        ret = adcs7476_value_process();
        if (ret != 0)
        {
            printf("data process err: %d\r\n", ret);
        }
    }

    return 0;
}
/*****************************************************************/

/* interlock check */
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

struct interlock_status *interlock_status_get(void)
{
    return &interlock_stat;
}

static int8_t interlock_status_update(void)
{
    struct interlock_status *stat = interlock_status_get();

    if (stat->mutex == NULL)
    {
        osMutexAttr_t mutex_attr = {
        .name = "interlock_mutex",
        .attr_bits = osMutexRecursive | osMutexPrioInherit
        };
        stat->mutex = osMutexNew(&mutex_attr);
    }

    osMutexAcquire(stat->mutex, osWaitForever);

    /* 1. check board power and hv status */
    int8_t ret = board_power_limit_fault_get();
    if (ret > 0)
    {
        stat->value.bits.board_power_fault = (ret & ~(1 << 1)) ? 1 : 0;
        stat->value.bits.hv_limit = (ret & (1 << 1)) ? 1 : 0;
    }

    /* 2. check communication status */
    stat->value.bits.comm_timeout = 0;

    /* 3. check wdt status */
    stat->value.bits.wdt_fault = 0;

    /* 4. check adcs7476 status */
    ret = adcs7476_object_data_limit_fault_get(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT);
    if (ret > 0)
    {
        stat->value.bits.adcs7476_1_limit_high = (ret & (1 << 1)) ? 1 : 0;
        stat->value.bits.adcs7476_1_limit_low = (ret & (1 << 0)) ? 1 : 0;
    }
    
    ret = adcs7476_object_data_limit_fault_get(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT);
    if (ret > 0)
    {
        stat->value.bits.adcs7476_2_limit_high = (ret & (1 << 1)) ? 1 : 0;
        stat->value.bits.adcs7476_2_limit_low = (ret & (1 << 0)) ? 1 : 0;
    }

    /* 5. check illegal write status */
    stat->value.bits.illegal_write = 0;

    /* 6. check dose rate status */
    stat->value.bits.dose_rate_low = 0;
    stat->value.bits.dose_rate_high = 0;

    /* 7. check dose total status */
    stat->value.bits.dose_total_low = 0;
    stat->value.bits.dose_total_high = 0;

    /* 8. check dose symmetry status */
    stat->value.bits.dose_symmetry_fault = 0;

    /* 9. check dose dummy status */
    stat->value.bits.dose_dummy_timeout = 0;

    osMutexRelease(stat->mutex);

    return 0;
}

static int8_t timer_callback(void *argument)
{
    return interlock_status_update();
}
/*****************************************************************/

static int8_t main_app_thread_init(void)
{
    adcs7476_event = osEventFlagsNew(NULL);
    if (adcs7476_event == NULL)
    {
        printf("adcs7476 event create failed\r\n");
        return -1;
    }

    osThreadAttr_t data_process_thread_attributes = {
    .name = "data_process_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal7,
    };

    osThreadId_t data_process_threadHandle = osThreadNew(data_process_entry, NULL, &data_process_thread_attributes);
    if (data_process_threadHandle == NULL)
    {
        printf("thread data process create failed\r\n");
        return -2;
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
INIT_APP_EXPORT(main_app_thread_init);