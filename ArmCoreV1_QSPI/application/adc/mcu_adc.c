#include "mcu_adc.h"
#include "adc_port.h"
#include "init_call.h"

int8_t board_power_limit_fault_get(void)
{
    /*
    * channel 4(PC4): MCUADC5V
    * channel 7(PA7): MCUADC12
    * channel 8(PC5): MCUADCREF
    */
    int8_t ret = 0;
    uint16_t result[ADC_CHANNEL_NUM_MAX] = {0};

    struct adc_object *adc1 = adc_object_get(DEVICE_NAME_ADC1_DEFAULT);

    osMutexAcquire(adc1->mutex, osWaitForever);
    memcpy(result, adc1->data, (adc1->channel_num + 1) * sizeof(uint16_t));
    osMutexRelease(adc1->mutex);

    ret = adc_sample_data_amend(result, ADC_CHANNEL_NUM_MAX);
    if (ret != 0)
    {
        return ret;
    }

    uint16_t adc_p5v = result[0] * 3 / 2;  /* unit: mV */
    uint32_t adc_n500v = result[1] * 201;
    uint16_t adc_n5v = result[2] * 2;

#if 0
    printf("adc_p5v: %u mv\r\n", adc_p5v);
    printf("adc_n500v: %u mv\r\n", adc_n500v);
    printf("adc_n5v: %u mv\r\n", adc_n5v);
#endif

    if (abs(adc_p5v - 5000) > 5000 * POWER_DIFF_TOLERANCE)
    {
        ret |= (1 << 0);
    }

    if (abs(adc_n500v - 392000) > 392000 * POWER_DIFF_TOLERANCE)
    {
        ret |= (1 << 1);
    }

    if (abs(adc_n5v - 5000) > 5000 * POWER_DIFF_TOLERANCE)
    {
        ret |= (1 << 2);
    }

    return ret;
}

float mcu_adc_value_get(enum mcu_adc_channel channel)
{
    int8_t ret = 0;
    uint16_t result = 0;
    float value = 0.0f;

    struct adc_object *adc1 = adc_object_get(DEVICE_NAME_ADC1_DEFAULT);

    osMutexAcquire(adc1->mutex, osWaitForever);
    memcpy(&result, &adc1->data[channel], sizeof(uint16_t));
    osMutexRelease(adc1->mutex);

    ret = adc_sample_data_amend(&result, 1);
    if (ret != 0)
    {
        printf("adc_sample_data_amend err: %d\r\n", ret);
        return -1;
    }

    switch (channel)
    {
    case MCU_ADC_CHANNEL_P5V:
        value = (float)result * 3 / 2;  /* unit: mV */
        break;
    case MCU_ADC_CHANNEL_N500V:
        value = (float)result * 201;
        break;
    case MCU_ADC_CHANNEL_N5V:
        value = (float)result * 2;
        break;
    default:
        printf("invalid channel: %d\r\n", channel);
        return -2;
        break;
    }

    return value;
}

static int8_t mcu_adc_sample_start(uint16_t sample_interval_10ns)
{
    int8_t ret = adc_sample_interval_set(sample_interval_10ns);
    if (ret != 0)
    {
        printf("adc_sample_interval_set err: %d\r\n", ret);
        return -1;
    }

    ret = adc_sample_start();
    if (ret != 0)
    {
        printf("adc_sample_start err: %d\r\n", ret);
        return -2;
    }

    return 0;
}

osEventFlagsId_t adc_event = NULL;
static int8_t mcu_adc_init(void)
{
    adc_event = osEventFlagsNew(NULL);
    if (adc_event == NULL)
    {
        printf("adc_event create failed\r\n");
        return -1;
    }

    int8_t ret = adc_init(DEVICE_NAME_ADC1_DEFAULT, adc_event);
    if (ret != 0)
    {
        printf("%s init err: %d\r\n", DEVICE_NAME_ADC1_DEFAULT, ret);
        return -2;
    }

    ret = adc_init(DEVICE_NAME_ADC3_DEFAULT, adc_event);
    if (ret != 0)
    {
        printf("%s init err: %d\r\n", DEVICE_NAME_ADC3_DEFAULT, ret);
        return -3;
    }

    return 0;
}

static int8_t mcu_adc_data_convert_process(struct adc_object *obj)
{
    if (obj == NULL)
    {
        return -1;
    }

    osMutexAcquire(obj->mutex, osWaitForever);
    memcpy(obj->data, obj->result, obj->channel_num * sizeof(uint16_t));
    osMutexRelease(obj->mutex);

    if (obj == adc_object_get(DEVICE_NAME_ADC3_DEFAULT))
    {
        struct adc_object *adc1 = adc_object_get(DEVICE_NAME_ADC1_DEFAULT);
        adc1->data[adc1->channel_num] = __HAL_ADC_CONVERT_DATA_RESOLUTION(obj->data[3], ADC_RESOLUTION_12B, ADC_RESOLUTION_16B);
    }


#if 0
    static uint16_t ref_vol = 0;
    if (obj == adc_object_get(DEVICE_NAME_ADC3_DEFAULT))
    {
        ref_vol = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(obj->data[0], ADC_RESOLUTION_16B);
        printf("Vref: %u mv\r\n", ref_vol);
        printf("temperature: %u\r\n", __HAL_ADC_CALC_TEMPERATURE(ref_vol, obj->data[1], ADC_RESOLUTION_16B));
        printf("vbat: %u mv\r\n", __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_vol, obj->data[2], ADC_RESOLUTION_12B) * 4);
        printf("adc_n5v: %u mv\r\n", __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_vol, obj->data[3], ADC_RESOLUTION_12B));
    }

    if (obj == adc_object_get(DEVICE_NAME_ADC1_DEFAULT))
    {
        printf("adc_p5v: %u mv\r\n", __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_vol, obj->data[0], ADC_RESOLUTION_16B));
        printf("adc_n500v: %u mv\r\n", __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_vol, obj->data[1], ADC_RESOLUTION_16B));
        printf("adc_n5v: %u mv\r\n", __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_vol, obj->data[2], ADC_RESOLUTION_16B));
    }
#endif

    return 0;
}

static int8_t mcu_adc_data_convert_entry(void *argument)
{
    int8_t ret = 0;

    ret = mcu_adc_init();
    if (ret != 0)
    {
        printf("mcu_adc_init err: %d\r\n", ret);
        return -1;
    }

    uint32_t event_flag = 0;

    struct adc_object *adc1 = adc_object_get(DEVICE_NAME_ADC1_DEFAULT);
    struct adc_object *adc3 = adc_object_get(DEVICE_NAME_ADC3_DEFAULT);

    for (;;)
    {
        event_flag = osEventFlagsWait(adc_event, adc1->event_flag | adc3->event_flag, osFlagsWaitAll, osWaitForever);
        if (event_flag & adc1->event_flag)
        {
            mcu_adc_data_convert_process(adc1);
        }

        if (event_flag & adc3->event_flag)
        {
            mcu_adc_data_convert_process(adc3);
        }
    }

    return 0;
}

static int8_t mcu_adc_thread_init(void)
{
    osThreadAttr_t attr = {
        .name = "mcu_adc_data_convert_thread",
        .stack_size = 1024 * 4,
        .priority = osPriorityAboveNormal,
    };

    osThreadId_t tid = osThreadNew(mcu_adc_data_convert_entry, NULL, &attr);
    if (tid == NULL)
    {
        printf("thread mcu adc data convert create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(mcu_adc_thread_init);

#ifdef MCU_ADC_TEST
#include "shell.h"

static int8_t mcu_adc_test(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: mcu_adc_test <sample_interval_10ns>\r\n");
    }

    uint16_t sample_interval_10ns = atoi(argv[1]);

    mcu_adc_sample_start(sample_interval_10ns);

}
MSH_CMD_EXPORT_ALIAS(mcu_adc_test, mcu_adc_test, test mcu adc);
#endif