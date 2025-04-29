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
#define ModPRF_Pin GPIO_PIN_2
#define ModPRF_GPIO_Port GPIOE
#define LAN9252_IRQ_Pin GPIO_PIN_3
#define LAN9252_IRQ_GPIO_Port GPIOE
#define LAN9252_IRQ_EXTI_IRQn EXTI3_IRQn
#define LvInterlockEn_Pin GPIO_PIN_4
#define LvInterlockEn_GPIO_Port GPIOE
#define HvInterlockEn_Pin GPIO_PIN_5
#define HvInterlockEn_GPIO_Port GPIOE
#define VPSEnable_Pin GPIO_PIN_6
#define VPSEnable_GPIO_Port GPIOE
#define LAN9252_SYNC1_Pin GPIO_PIN_13
#define LAN9252_SYNC1_GPIO_Port GPIOC
#define LAN9252_SYNC1_EXTI_IRQn EXTI15_10_IRQn
#define CHIP_RUN_LED_Pin GPIO_PIN_1
#define CHIP_RUN_LED_GPIO_Port GPIOC
#define SYSTEM_STATE_Pin GPIO_PIN_2
#define SYSTEM_STATE_GPIO_Port GPIOC
#define HvENDetect_Pin GPIO_PIN_3
#define HvENDetect_GPIO_Port GPIOA
#define ModHVONDetect_Pin GPIO_PIN_5
#define ModHVONDetect_GPIO_Port GPIOC
#define ModTrigONDetect_Pin GPIO_PIN_0
#define ModTrigONDetect_GPIO_Port GPIOB
#define ModArcDetect_Pin GPIO_PIN_1
#define ModArcDetect_GPIO_Port GPIOB
#define LAN9252_SYNC0_Pin GPIO_PIN_2
#define LAN9252_SYNC0_GPIO_Port GPIOB
#define LAN9252_SYNC0_EXTI_IRQn EXTI2_IRQn
#define LvOKDetect_Pin GPIO_PIN_10
#define LvOKDetect_GPIO_Port GPIOB
#define TrigFB_Pin GPIO_PIN_11
#define TrigFB_GPIO_Port GPIOB
#define ModSumDetect_Pin GPIO_PIN_11
#define ModSumDetect_GPIO_Port GPIOD
#define UART3_DE_Pin GPIO_PIN_12
#define UART3_DE_GPIO_Port GPIOD
#define IOEIntB_Pin GPIO_PIN_6
#define IOEIntB_GPIO_Port GPIOG
#define IOEIntB_EXTI_IRQn EXTI9_5_IRQn
#define EPSEnable_Pin GPIO_PIN_7
#define EPSEnable_GPIO_Port GPIOG
#define ModTriggerInhibitCtrl_Pin GPIO_PIN_6
#define ModTriggerInhibitCtrl_GPIO_Port GPIOC
#define EmergencyDetect_Pin GPIO_PIN_8
#define EmergencyDetect_GPIO_Port GPIOC
#define PulseInhibitDetect_Pin GPIO_PIN_9
#define PulseInhibitDetect_GPIO_Port GPIOC
#define PulseInhibitDetect_EXTI_IRQn EXTI9_5_IRQn
#define UART4_DE_Pin GPIO_PIN_15
#define UART4_DE_GPIO_Port GPIOA
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
#define Dose2EnDetect_Pin GPIO_PIN_8
#define Dose2EnDetect_GPIO_Port GPIOB
#define Dose1EnDetect_Pin GPIO_PIN_9
#define Dose1EnDetect_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
