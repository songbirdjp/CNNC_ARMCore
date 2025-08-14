#include "hw_crc.h"
#include "crc.h"

struct hardware_crc_para
{
    uint8_t bit_len;    /* crc bit length */
    uint32_t poly;      /* polynomial */
    uint32_t init_val;  /* init value */
    uint8_t input_inversion;    /* inversion length */
    uint8_t output_inversion    /* enable or not */
};

static struct hardware_crc_para crc_config_default[] = 
{
    {CRC_POLYLENGTH_8B,  0x07,   0x00, CRC_INPUTDATA_INVERSION_NONE, CRC_OUTPUTDATA_INVERSION_DISABLE},
    {CRC_POLYLENGTH_16B, 0x8005, 0x00, CRC_INPUTDATA_INVERSION_BYTE, CRC_OUTPUTDATA_INVERSION_ENABLE},
    {CRC_POLYLENGTH_32B, DEFAULT_CRC32_POLY, DEFAULT_CRC_INITVALUE, CRC_INPUTDATA_INVERSION_BYTE, CRC_OUTPUTDATA_INVERSION_ENABLE}
};

static HAL_StatusTypeDef hardware_crc_config(enum hardware_crc_default index)
{
    HAL_StatusTypeDef status = HAL_OK;

    __HAL_CRC_INITIALCRCVALUE_CONFIG(&hcrc, crc_config_default[index].init_val);
    status = HAL_CRCEx_Polynomial_Set(&hcrc, crc_config_default[index].poly, crc_config_default[index].bit_len);
    status |= HAL_CRCEx_Input_Data_Reverse(&hcrc, crc_config_default[index].input_inversion);
    status |= HAL_CRCEx_Output_Data_Reverse(&hcrc, crc_config_default[index].output_inversion);

    return status;
}

static uint32_t hardware_crc_calculate_continue(uint8_t pBuffer[], uint32_t size)
{
    return HAL_CRC_Accumulate(&hcrc, (uint32_t *)pBuffer, size);
}

#include "init_call.h"
#include "cmsis_os2.h"
static osMutexId_t crc_mutex = NULL;
static int8_t hw_crc_init(void)
{
    osMutexAttr_t crc_mutex_attributes = {
    .name = "crc_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    crc_mutex = osMutexNew(&crc_mutex_attributes);
    if (crc_mutex == NULL)
    {
        printf("crc_mutex create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_ENV_EXPORT(hw_crc_init);

/* note: user must ^0xFFFFFFFF with below functions to get final result */
int8_t hw_crc_mutex_take(void)
{
    return osMutexAcquire(crc_mutex, osWaitForever);
}

int8_t hw_crc_mutex_give(void)
{
    return osMutexRelease(crc_mutex);
}

uint32_t hardware_crc_calculate(enum hardware_crc_default index, uint8_t pBuffer[], uint32_t size)
{
    static enum hardware_crc_default index_last = CRC_MAX;
    uint32_t crc_result = 0;
    HAL_StatusTypeDef status = HAL_OK;

    osMutexAcquire(crc_mutex, osWaitForever);

    if (index_last != index)
    {
        status = hardware_crc_config(index);
        if (status != HAL_OK)
        {
            osMutexRelease(crc_mutex);
            printf("hardware_crc_config err: %d\r\n", status);
            return -1;
        }
        
        index_last = index;
    }

    if (pBuffer != NULL && size > 0)
    {
        crc_result = HAL_CRC_Calculate(&hcrc, (uint32_t *)pBuffer, size);
    }

    osMutexRelease(crc_mutex);

    return crc_result;
}


