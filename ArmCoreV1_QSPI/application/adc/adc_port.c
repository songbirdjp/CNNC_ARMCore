#include "adc_port.h"
#include "adc.h"
#include <stdint.h>
#include "shell.h"
#include "cmsis_os2.h"
#include "lptim.h"

#ifdef ADC_TEST

uint16_t adc_result[3] __attribute__((section(".ram_d3"))) = {0};

osEventFlagsId_t adc_event = NULL;


void ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    osEventFlagsSet(adc_event, 1);
}

static int8_t adc_test(uint8_t argc, char **argv)
{
    uint16_t ref_vol = 0;

    adc_event = osEventFlagsNew(NULL);
    if (adc_event == NULL)
    {
        printf("osEventFlagsNew failed\r\n");
        return -1;
    }

    HAL_StatusTypeDef status = HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID, ConvCpltCallback);
    if (status != HAL_OK)
    {
        printf("HAL_ADC_RegisterCallback err: %d\r\n", status);
        return -2;
    }

    status = HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    if (status != HAL_OK)
    {
        printf("HAL_ADCEx_Calibration_Start err: %d\r\n", status);
        return -3;
    }
    
    status = HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_result, sizeof(adc_result) / sizeof(adc_result[0]));
    if (status!= HAL_OK)
    {
        printf("HAL_ADC_Start_DMA err: %d\r\n", status);
        return -4;
    }

    status = HAL_LPTIM_Counter_Start(&hlptim1, 7500 - 1);    /* 96M / 128分频 / 7500 = 100Hz */
    if (status != HAL_OK)
    {
        printf("HAL_LPTIM_Counter_Start err: %d\r\n", status);
        return -5;
    }

    for (;;)
    {
        osEventFlagsWait(adc_event, 1, osFlagsWaitAny, osWaitForever);

        printf("%d %d %d\r\n", adc_result[0], adc_result[1], adc_result[2]);

        ref_vol = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(adc_result[0], ADC_RESOLUTION_16B);
        // printf("Vref: %d mv\r\n", (uint32_t)(*VREFINT_CAL_ADDR) * VREFINT_CAL_VREF / adc_result[0]);
        printf("Vref: %d mv\r\n", ref_vol);

        printf("temperature: %d\r\n", __HAL_ADC_CALC_TEMPERATURE(ref_vol, adc_result[1], ADC_RESOLUTION_16B));

        // printf("vbat1:%d mv\r\n", 4 * ref_vol * adc_result[0] / 4095);
        printf("vbat: %d mv\r\n", __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_vol, adc_result[2], ADC_RESOLUTION_12B) * 4);

    }


    return 0;
}
MSH_CMD_EXPORT_ALIAS(adc_test, adc_test, test adc);

#endif