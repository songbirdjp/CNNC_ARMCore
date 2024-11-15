/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lptim.c
  * @brief   This file provides code for the configuration
  *          of the LPTIM instances.
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
#include "lptim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

LPTIM_HandleTypeDef hlptim1;
LPTIM_HandleTypeDef hlptim3;

/* LPTIM1 init function */
void MX_LPTIM1_Init(void)
{

  /* USER CODE BEGIN LPTIM1_Init 0 */

  /* USER CODE END LPTIM1_Init 0 */

  /* USER CODE BEGIN LPTIM1_Init 1 */

  /* USER CODE END LPTIM1_Init 1 */
  hlptim1.Instance = LPTIM1;
  hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
  hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
  hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  hlptim1.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  hlptim1.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
  if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM1_Init 2 */

  /* USER CODE END LPTIM1_Init 2 */

}
/* LPTIM3 init function */
void MX_LPTIM3_Init(void)
{

  /* USER CODE BEGIN LPTIM3_Init 0 */

  /* USER CODE END LPTIM3_Init 0 */

  /* USER CODE BEGIN LPTIM3_Init 1 */

  /* USER CODE END LPTIM3_Init 1 */
  hlptim3.Instance = LPTIM3;
  hlptim3.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim3.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
  hlptim3.Init.Trigger.Source = LPTIM_TRIGSOURCE_2;
  hlptim3.Init.Trigger.ActiveEdge = LPTIM_ACTIVEEDGE_FALLING;
  hlptim3.Init.Trigger.SampleTime = LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION;
  hlptim3.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
  hlptim3.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim3.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  hlptim3.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  if (HAL_LPTIM_Init(&hlptim3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM3_Init 2 */

  /* USER CODE END LPTIM3_Init 2 */

}

void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef* lptimHandle)
{

  if(lptimHandle->Instance==LPTIM1)
  {
  /* USER CODE BEGIN LPTIM1_MspInit 0 */

  /* USER CODE END LPTIM1_MspInit 0 */
    /* LPTIM1 clock enable */
    __HAL_RCC_LPTIM1_CLK_ENABLE();
  /* USER CODE BEGIN LPTIM1_MspInit 1 */

  /* USER CODE END LPTIM1_MspInit 1 */
  }
  else if(lptimHandle->Instance==LPTIM3)
  {
  /* USER CODE BEGIN LPTIM3_MspInit 0 */

  /* USER CODE END LPTIM3_MspInit 0 */
    /* LPTIM3 clock enable */
    __HAL_RCC_LPTIM3_CLK_ENABLE();

    /* LPTIM3 interrupt Init */
    HAL_NVIC_SetPriority(LPTIM3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(LPTIM3_IRQn);
  /* USER CODE BEGIN LPTIM3_MspInit 1 */

  /* USER CODE END LPTIM3_MspInit 1 */
  }
}

void HAL_LPTIM_MspDeInit(LPTIM_HandleTypeDef* lptimHandle)
{

  if(lptimHandle->Instance==LPTIM1)
  {
  /* USER CODE BEGIN LPTIM1_MspDeInit 0 */

  /* USER CODE END LPTIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPTIM1_CLK_DISABLE();
  /* USER CODE BEGIN LPTIM1_MspDeInit 1 */

  /* USER CODE END LPTIM1_MspDeInit 1 */
  }
  else if(lptimHandle->Instance==LPTIM3)
  {
  /* USER CODE BEGIN LPTIM3_MspDeInit 0 */

  /* USER CODE END LPTIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPTIM3_CLK_DISABLE();

    /* LPTIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(LPTIM3_IRQn);
  /* USER CODE BEGIN LPTIM3_MspDeInit 1 */

  /* USER CODE END LPTIM3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
