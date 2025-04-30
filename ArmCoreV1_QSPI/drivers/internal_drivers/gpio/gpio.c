/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, ModPRF_Pin|LvInterlockEn_Pin|HvInterlockEn_Pin|VPSEnable_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, CHIP_RUN_LED_Pin|SYSTEM_STATE_Pin|ModTriggerInhibitCtrl_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, EPSEnable_Pin|WATCHDOG1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(W5500_RSTn_GPIO_Port, W5500_RSTn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(WATCHDOG2_GPIO_Port, WATCHDOG2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, W5500_CSn_Pin|FM25V02_CSn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : ModPRF_Pin LvInterlockEn_Pin HvInterlockEn_Pin VPSEnable_Pin */
  GPIO_InitStruct.Pin = ModPRF_Pin|LvInterlockEn_Pin|HvInterlockEn_Pin|VPSEnable_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : LAN9252_IRQ_Pin */
  GPIO_InitStruct.Pin = LAN9252_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(LAN9252_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LAN9252_SYNC1_Pin */
  GPIO_InitStruct.Pin = LAN9252_SYNC1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(LAN9252_SYNC1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CHIP_RUN_LED_Pin SYSTEM_STATE_Pin ModTriggerInhibitCtrl_Pin */
  GPIO_InitStruct.Pin = CHIP_RUN_LED_Pin|SYSTEM_STATE_Pin|ModTriggerInhibitCtrl_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : HvENDetect_Pin */
  GPIO_InitStruct.Pin = HvENDetect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(HvENDetect_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ModHVONDetect_Pin EmergencyDetect_Pin */
  GPIO_InitStruct.Pin = ModHVONDetect_Pin|EmergencyDetect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : ModTrigONDetect_Pin ModArcDetect_Pin LvOKDetect_Pin TrigFB_Pin
                           Dose2EnDetect_Pin Dose1EnDetect_Pin */
  GPIO_InitStruct.Pin = ModTrigONDetect_Pin|ModArcDetect_Pin|LvOKDetect_Pin|TrigFB_Pin
                          |Dose2EnDetect_Pin|Dose1EnDetect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LAN9252_SYNC0_Pin */
  GPIO_InitStruct.Pin = LAN9252_SYNC0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(LAN9252_SYNC0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ModSumDetect_Pin */
  GPIO_InitStruct.Pin = ModSumDetect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ModSumDetect_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IOEIntB_Pin */
  GPIO_InitStruct.Pin = IOEIntB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IOEIntB_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : EPSEnable_Pin WATCHDOG1_Pin W5500_CSn_Pin FM25V02_CSn_Pin */
  GPIO_InitStruct.Pin = EPSEnable_Pin|WATCHDOG1_Pin|W5500_CSn_Pin|FM25V02_CSn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PulseInhibitDetect_Pin */
  GPIO_InitStruct.Pin = PulseInhibitDetect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PulseInhibitDetect_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : W5500_RSTn_Pin WATCHDOG2_Pin */
  GPIO_InitStruct.Pin = W5500_RSTn_Pin|WATCHDOG2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : W5500_INTn_Pin */
  GPIO_InitStruct.Pin = W5500_INTn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(W5500_INTn_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
