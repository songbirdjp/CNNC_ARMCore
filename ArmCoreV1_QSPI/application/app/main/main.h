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
#define SPI3_NSS_Pin GPIO_PIN_4
#define SPI3_NSS_GPIO_Port GPIOA
#define LED1PG4_Pin GPIO_PIN_4
#define LED1PG4_GPIO_Port GPIOG
#define LED2PG5_Pin GPIO_PIN_5
#define LED2PG5_GPIO_Port GPIOG
#define LED3PG6_Pin GPIO_PIN_6
#define LED3PG6_GPIO_Port GPIOG
#define LED4PG7_Pin GPIO_PIN_7
#define LED4PG7_GPIO_Port GPIOG
#define WDT_RESET_Pin GPIO_PIN_1
#define WDT_RESET_GPIO_Port GPIOD
#define W5500_RSTn_Pin GPIO_PIN_3
#define W5500_RSTn_GPIO_Port GPIOD
#define W5500_INTn_Pin GPIO_PIN_4
#define W5500_INTn_GPIO_Port GPIOD
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
