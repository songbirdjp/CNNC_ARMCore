/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#define RUN_LED6_Pin GPIO_PIN_2
#define RUN_LED6_GPIO_Port GPIOE
#define LAN9252_IRQ_Pin GPIO_PIN_3
#define LAN9252_IRQ_GPIO_Port GPIOE
#define LAN9252_IRQ_EXTI_IRQn EXTI3_IRQn
#define RUN_LED5_Pin GPIO_PIN_4
#define RUN_LED5_GPIO_Port GPIOE
#define RUN_LED4_Pin GPIO_PIN_5
#define RUN_LED4_GPIO_Port GPIOE
#define RUN_LED3_Pin GPIO_PIN_6
#define RUN_LED3_GPIO_Port GPIOE
#define LAN9252_SYNC1_Pin GPIO_PIN_13
#define LAN9252_SYNC1_GPIO_Port GPIOC
#define LAN9252_SYNC1_EXTI_IRQn EXTI15_10_IRQn
#define CHIP_RUN_LED_Pin GPIO_PIN_1
#define CHIP_RUN_LED_GPIO_Port GPIOC
#define SYSTEM_STATE_Pin GPIO_PIN_2
#define SYSTEM_STATE_GPIO_Port GPIOC
#define DI_GATING_Pin GPIO_PIN_0
#define DI_GATING_GPIO_Port GPIOB
#define DI_INT_Pin GPIO_PIN_1
#define DI_INT_GPIO_Port GPIOB
#define LAN9252_SYNC0_Pin GPIO_PIN_2
#define LAN9252_SYNC0_GPIO_Port GPIOB
#define LAN9252_SYNC0_EXTI_IRQn EXTI2_IRQn
#define DI_Slipring_HVEN_Pin GPIO_PIN_13
#define DI_Slipring_HVEN_GPIO_Port GPIOB
#define DI_Slipring_KV_TreatmentEN_Pin GPIO_PIN_14
#define DI_Slipring_KV_TreatmentEN_GPIO_Port GPIOB
#define DI_Slipring_MV_TreatmentEN_Pin GPIO_PIN_15
#define DI_Slipring_MV_TreatmentEN_GPIO_Port GPIOB
#define RUN_LED1_Pin GPIO_PIN_6
#define RUN_LED1_GPIO_Port GPIOG
#define RUN_LED2_Pin GPIO_PIN_7
#define RUN_LED2_GPIO_Port GPIOG
#define DO_HVEN_Pin GPIO_PIN_6
#define DO_HVEN_GPIO_Port GPIOC
#define DO_MV_TREATMENT_EN_Pin GPIO_PIN_7
#define DO_MV_TREATMENT_EN_GPIO_Port GPIOC
#define DO_KV_TREATMENT_EN_Pin GPIO_PIN_8
#define DO_KV_TREATMENT_EN_GPIO_Port GPIOC
#define DO_EMERGENCY_Pin GPIO_PIN_9
#define DO_EMERGENCY_GPIO_Port GPIOC
#define DO_PULSE_INHIBIT_Pin GPIO_PIN_8
#define DO_PULSE_INHIBIT_GPIO_Port GPIOA
#define DO_PowerCut_Pin GPIO_PIN_15
#define DO_PowerCut_GPIO_Port GPIOA
#define W5500_RSTn_Pin GPIO_PIN_3
#define W5500_RSTn_GPIO_Port GPIOD
#define W5500_INTn_Pin GPIO_PIN_4
#define W5500_INTn_GPIO_Port GPIOD
#define W5500_INTn_EXTI_IRQn EXTI4_IRQn
#define WATCHDOG2_Pin GPIO_PIN_7
#define WATCHDOG2_GPIO_Port GPIOD
#define WATCHDOG1_Pin GPIO_PIN_9
#define WATCHDOG1_GPIO_Port GPIOG
#define W5500_CSn_Pin GPIO_PIN_10
#define W5500_CSn_GPIO_Port GPIOG
#define FM25V02_CSn_Pin GPIO_PIN_11
#define FM25V02_CSn_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
