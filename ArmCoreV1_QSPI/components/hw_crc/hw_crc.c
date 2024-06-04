#include "hw_crc.h"
#include "crc.h"

/* note: user must ^0xFFFFFFFF with below functions to get final result */
uint32_t hardware_crc_calculate(uint8_t pBuffer[], uint32_t size)
{
    return HAL_CRC_Calculate(&hcrc, (uint32_t *)pBuffer, size);
}

uint32_t hardware_crc_calculate_continue(uint8_t pBuffer[], uint32_t size)
{
    return HAL_CRC_Accumulate(&hcrc, (uint32_t *)pBuffer, size);
}

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

HAL_StatusTypeDef hardware_crc_config(enum hardware_crc_default index)
{
    HAL_StatusTypeDef status = HAL_OK;

    __HAL_CRC_INITIALCRCVALUE_CONFIG(&hcrc, crc_config_default[index].init_val);
    status = HAL_CRCEx_Polynomial_Set(&hcrc, crc_config_default[index].poly, crc_config_default[index].bit_len);
    status |= HAL_CRCEx_Input_Data_Reverse(&hcrc, crc_config_default[index].input_inversion);
    status |= HAL_CRCEx_Output_Data_Reverse(&hcrc, crc_config_default[index].output_inversion);    
}