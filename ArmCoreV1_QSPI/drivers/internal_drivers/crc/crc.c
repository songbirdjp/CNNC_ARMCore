/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    crc.c
  * @brief   This file provides code for the configuration
  *          of the CRC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "crc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CRC_HandleTypeDef hcrc;

/* CRC init function */
void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

void HAL_CRC_MspInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* USER CODE BEGIN CRC_MspInit 0 */

  /* USER CODE END CRC_MspInit 0 */
    /* CRC clock enable */
    __HAL_RCC_CRC_CLK_ENABLE();
  /* USER CODE BEGIN CRC_MspInit 1 */

  /* USER CODE END CRC_MspInit 1 */
  }
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* USER CODE BEGIN CRC_MspDeInit 0 */

  /* USER CODE END CRC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CRC_CLK_DISABLE();
  /* USER CODE BEGIN CRC_MspDeInit 1 */

  /* USER CODE END CRC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

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

/* USER CODE END 1 */
