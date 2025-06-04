#include "adc_port.h"
#include "lptim.h"

/* 
 * adc1: convert time = (sample time + 0.5 + Nbit / 2) * fclk;  3 channel total time = 49.14us
 *       rank 1: channel 4,    rank 2: channel 7,    rank 3: channel 8
 * 
 * adc3: convert time = (sample time + 0.5 + Nbit) * fclk;      3 channel total time = 39.18us
 *       rank 1: channel Vrefint,    rank 2: channel temperature,    rank 3: channel Vbat
 */

#define ADC1_CONVERT_COMPLETE_EVENT  (1 << 0)
#define ADC3_CONVERT_COMPLETE_EVENT  (1 << 1)

static uint16_t adc1_result[ADC1_CHANNEL_NUM] = {0};
static uint16_t adc3_result[ADC3_CHANNEL_NUM] __attribute__((section(".ram_d3"))) = {0};
static osEventFlagsId_t adc_event = NULL;


static struct adc_object 
adc1 = {.hadc = &hadc1, .event_flag = ADC1_CONVERT_COMPLETE_EVENT, .init = MX_ADC1_Init, .channel_num = ADC1_CHANNEL_NUM, .result = adc1_result}, 
adc3 = {.hadc = &hadc3, .event_flag = ADC3_CONVERT_COMPLETE_EVENT, .init = MX_ADC3_Init, .channel_num = ADC3_CHANNEL_NUM, .result = adc3_result};

struct adc_object *adc_object_get(uint8_t *device_name)
{
    if (!memcmp(device_name, DEVICE_NAME_ADC1_DEFAULT, sizeof(DEVICE_NAME_ADC1_DEFAULT)))
    {
        return &adc1;
    }
    else if (!memcmp(device_name, DEVICE_NAME_ADC3_DEFAULT, sizeof(DEVICE_NAME_ADC3_DEFAULT)))
    {
        return &adc3;
    }

    return NULL;
}

static void ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    struct adc_object *adc = NULL;

    if (hadc == adc_object_get(DEVICE_NAME_ADC1_DEFAULT)->hadc)
    {
        adc = adc_object_get(DEVICE_NAME_ADC1_DEFAULT);
    }
    else if (hadc == adc_object_get(DEVICE_NAME_ADC3_DEFAULT)->hadc)
    {
        adc = adc_object_get(DEVICE_NAME_ADC3_DEFAULT);
    }
    else
    {
        return;
    }

    osEventFlagsSet(adc->event, adc->event_flag);
}

int8_t adc_init(uint8_t *device_name, osEventFlagsId_t event)
{
    if (device_name == NULL || event == NULL)
    {
        printf("device name or event is NULL\r\n");
        return -1;
    }

    struct adc_object *adc = adc_object_get(device_name);
    if (adc == NULL)
    {
        printf("name invalid: %s\r\n", device_name);
        return -2;
    }
    osMutexAttr_t mutex_attributes = {
    .name = "adc_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    adc->mutex = osMutexNew(&mutex_attributes);
    if (adc->mutex == NULL)
    {
        printf("osMutexNew failed\r\n");
        return -3;
    }

    adc->event = event;
    adc->init();

    HAL_StatusTypeDef status = HAL_ADC_RegisterCallback(adc->hadc, HAL_ADC_CONVERSION_COMPLETE_CB_ID, ConvCpltCallback);
    if (status != HAL_OK)
    {
        printf("HAL_ADC_RegisterCallback err: %d\r\n", status);
        return -4;
    }

    status = HAL_ADCEx_Calibration_Start(adc->hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    if (status != HAL_OK)
    {
        printf("HAL_ADCEx_Calibration_Start err: %d\r\n", status);
        return -5;
    }
    
    status = HAL_ADC_Start_DMA(adc->hadc, (uint32_t*)adc->result, adc->channel_num);
    if (status!= HAL_OK)
    {
        printf("HAL_ADC_Start_DMA err: %d\r\n", status);
        return -6;
    }

    return 0;
}

int8_t adc_sample_interval_set(uint16_t sample_interval_10ns)
{
    if (sample_interval_10ns > 0xFFFF)
    {
        printf("sample interval is too large: (sample_interval_10ns <= 0xFFFF)\r\n");
        return -1;
    }

    HAL_StatusTypeDef status = HAL_OK;

    if (HAL_LPTIM_GetState(&hlptim1) == HAL_LPTIM_STATE_RESET)
    {
        MX_LPTIM1_Init();
    }
    else
    {
        hlptim1.Instance->CFGR &= ~LPTIM_CFGR_WAVE;

        status = HAL_LPTIM_UnRegisterCallback(&hlptim1, HAL_LPTIM_AUTORELOAD_MATCH_CB_ID);
        if (status != HAL_OK)
        {
            printf("HAL_LPTIM_UnRegisterCallback err: %d\r\n", status);
            return -2;
        }
    }

    status = HAL_LPTIM_Counter_Stop(&hlptim1);
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_Counter_Stop err: %d\r\n", status);
        return -3;
    }

    __HAL_LPTIM_AUTORELOAD_SET(&hlptim1, sample_interval_10ns);

    return 0;
}

int8_t adc_sample_start(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    uint32_t period = HAL_LPTIM_ReadAutoReload(&hlptim1);

    status = HAL_LPTIM_Counter_Start(&hlptim1, period);   /* 100M / 1分频 */
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_Counter_Start err: %d\r\n", status);
        return -1;
    }

    return 0;
}

int8_t adc_sample_data_amend(uint16_t *data, uint8_t channel_num)
{
    if (data == NULL || channel_num == 0)
    {
        return -1;
    }

    struct adc_object *adc3 = adc_object_get(DEVICE_NAME_ADC3_DEFAULT);
    osMutexAcquire(adc3->mutex, osWaitForever);
    uint16_t ref_int = adc3->data[0];
    osMutexRelease(adc3->mutex);

    uint16_t ref_vol = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(ref_int, ADC_RESOLUTION_16B);

    for (uint8_t i = 0; i < channel_num; i++)
    {
        *data = __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_vol, *data, ADC_RESOLUTION_16B);
        data++;
    }

    return 0;
}


#ifdef ADC_TEST
#include "shell.h"

static int8_t adc_test(uint8_t argc, char **argv)
{
    int8_t ret = 0;

    osEventFlagsId_t adc_event = osEventFlagsNew(NULL);

    ret = adc_init(DEVICE_NAME_ADC3_DEFAULT, adc_event);
    if (ret != 0)
    {
        printf("adc_init err: %d\r\n", ret);
        return -1;
    }

    ret = adc_convert_start(100);
    if (ret != 0)
    {
        printf("adc_convert_start err: %d\r\n", ret);
        return -2;
    }

    struct adc_object *adc3 = adc_object_get(DEVICE_NAME_ADC3_DEFAULT);
    uint16_t ref_vol = 0;

    for (;;)
    {
        osEventFlagsWait(adc_event, ADC1_CONVERT_COMPLETE_EVENT, osFlagsWaitAny, osWaitForever);

        printf("%d %d %d\r\n", adc3->result[0], adc3->result[1], adc3->result[2]);

        ref_vol = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(adc3->result[0], ADC_RESOLUTION_16B);
        // printf("Vref: %d mv\r\n", (uint32_t)(*VREFINT_CAL_ADDR) * VREFINT_CAL_VREF / adc_result[0]);
        printf("Vref: %d mv\r\n", ref_vol);

        printf("temperature: %d\r\n", __HAL_ADC_CALC_TEMPERATURE(ref_vol, adc3->result[1], ADC_RESOLUTION_16B));

        // printf("vbat1:%d mv\r\n", 4 * ref_vol * adc_result[0] / 4095);
        printf("vbat: %d mv\r\n", __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_vol, adc3->result[2], ADC_RESOLUTION_12B) * 4);

    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(adc_test, adc_test, test adc);

#endif