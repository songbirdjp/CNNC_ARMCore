/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    wwdg.c
  * @brief   This file provides code for the configuration
  *          of the WWDG instances.
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
#include "wwdg.h"

/* USER CODE BEGIN 0 */
#if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
void wwdg_refresh(void);
#endif
/* USER CODE END 0 */

WWDG_HandleTypeDef hwwdg1;

/* WWDG1 init function */
void MX_WWDG1_Init(void)
{

  /* USER CODE BEGIN WWDG1_Init 0 */

  /* USER CODE END WWDG1_Init 0 */

  /* USER CODE BEGIN WWDG1_Init 1 */

  /* USER CODE END WWDG1_Init 1 */
  hwwdg1.Instance = WWDG1;
  hwwdg1.Init.Prescaler = WWDG_PRESCALER_128;
  hwwdg1.Init.Window = 0x7F;
  hwwdg1.Init.Counter = 0x7F;
  hwwdg1.Init.EWIMode = WWDG_EWI_ENABLE;
  if (HAL_WWDG_Init(&hwwdg1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN WWDG1_Init 2 */
#if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
  HAL_WWDG_RegisterCallback(&hwwdg1, HAL_WWDG_EWI_CB_ID, wwdg_refresh);
#endif
  /* USER CODE END WWDG1_Init 2 */

}

void HAL_WWDG_MspInit(WWDG_HandleTypeDef* wwdgHandle)
{

  if(wwdgHandle->Instance==WWDG1)
  {
  /* USER CODE BEGIN WWDG1_MspInit 0 */

  /* USER CODE END WWDG1_MspInit 0 */
    /* WWDG1 clock enable */
    HAL_RCCEx_WWDGxSysResetConfig(RCC_WWDG1);
    __HAL_RCC_WWDG1_CLK_ENABLE();

    /* WWDG1 interrupt Init */
    HAL_NVIC_SetPriority(WWDG_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(WWDG_IRQn);
  /* USER CODE BEGIN WWDG1_MspInit 1 */

  /* USER CODE END WWDG1_MspInit 1 */
  }
}

/* USER CODE BEGIN 1 */
#if (USE_HAL_WWDG_REGISTER_CALLBACKS == 1)
void wwdg_refresh(void)
{
    // HAL_WWDG_RegisterCallback

    WWDG_HandleTypeDef hwwdg1;

    hwwdg1.Instance = WWDG1;
    hwwdg1.Init.Prescaler = WWDG_PRESCALER_128;
    hwwdg1.Init.Window = 0x7F;
    hwwdg1.Init.Counter = 0x7F;
    hwwdg1.Init.EWIMode = WWDG_EWI_ENABLE;

    HAL_StatusTypeDef state = HAL_WWDG_Refresh(&hwwdg1);
    if (state != HAL_OK)
    {
        Error_Handler();
    }

}
#endif
/* USER CODE END 1 */
