/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SYSTEM_RUN_LED_Pin GPIO_PIN_2
#define SYSTEM_RUN_LED_GPIO_Port GPIOE
#define ADC_READING_LED_Pin GPIO_PIN_3
#define ADC_READING_LED_GPIO_Port GPIOE
#define WDT_IN_LED_Pin GPIO_PIN_4
#define WDT_IN_LED_GPIO_Port GPIOE
#define WDT_OK_LED_Pin GPIO_PIN_5
#define WDT_OK_LED_GPIO_Port GPIOE
#define WDT_RESET_Pin GPIO_PIN_1
#define WDT_RESET_GPIO_Port GPIOD
#define WDT_IN_Pin GPIO_PIN_4
#define WDT_IN_GPIO_Port GPIOD
#define WDT_OK_Pin GPIO_PIN_5
#define WDT_OK_GPIO_Port GPIOD
#define WDT_OK_EXTI_IRQn EXTI9_5_IRQn
#define DOSE_EN_Pin GPIO_PIN_8
#define DOSE_EN_GPIO_Port GPIOB
#define TRIGGER_OUT_Pin GPIO_PIN_9
#define TRIGGER_OUT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
