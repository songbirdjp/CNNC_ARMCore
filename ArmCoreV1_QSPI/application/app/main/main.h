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
#define LAN9252_IRQ_Pin GPIO_PIN_3
#define LAN9252_IRQ_GPIO_Port GPIOE
#define LAN9252_IRQ_EXTI_IRQn EXTI3_IRQn
#define LAN9252_SYNC0_Pin GPIO_PIN_13
#define LAN9252_SYNC0_GPIO_Port GPIOC
#define LAN9252_SYNC0_EXTI_IRQn EXTI15_10_IRQn
#define CHIP_RUN_LED_Pin GPIO_PIN_1
#define CHIP_RUN_LED_GPIO_Port GPIOC
#define SYSTEM_STATE_Pin GPIO_PIN_2
#define SYSTEM_STATE_GPIO_Port GPIOC
#define MotorYA_Pin GPIO_PIN_2
#define MotorYA_GPIO_Port GPIOA
#define MotorYB_Pin GPIO_PIN_3
#define MotorYB_GPIO_Port GPIOA
#define YBrake_Pin GPIO_PIN_5
#define YBrake_GPIO_Port GPIOC
#define MotorXB_Pin GPIO_PIN_0
#define MotorXB_GPIO_Port GPIOB
#define MotorXA_Pin GPIO_PIN_1
#define MotorXA_GPIO_Port GPIOB
#define LAN9252_SYNC1_Pin GPIO_PIN_2
#define LAN9252_SYNC1_GPIO_Port GPIOB
#define LAN9252_SYNC1_EXTI_IRQn EXTI2_IRQn
#define LAN9252_CSn_Pin GPIO_PIN_11
#define LAN9252_CSn_GPIO_Port GPIOE
#define EncYZ_Pin GPIO_PIN_10
#define EncYZ_GPIO_Port GPIOB
#define EncYZ_EXTI_IRQn EXTI15_10_IRQn
#define YJAWLMT_Pin GPIO_PIN_11
#define YJAWLMT_GPIO_Port GPIOB
#define YJAWLMT_EXTI_IRQn EXTI15_10_IRQn
#define SPI3_NCS_Pin GPIO_PIN_12
#define SPI3_NCS_GPIO_Port GPIOD
#define DriveBoardCtrl_Pin GPIO_PIN_13
#define DriveBoardCtrl_GPIO_Port GPIOD
#define CarrierServoSignal_Pin GPIO_PIN_3
#define CarrierServoSignal_GPIO_Port GPIOG
#define XPower_Pin GPIO_PIN_6
#define XPower_GPIO_Port GPIOG
#define YPower_Pin GPIO_PIN_7
#define YPower_GPIO_Port GPIOG
#define EncXZ_Pin GPIO_PIN_8
#define EncXZ_GPIO_Port GPIOA
#define EncXZ_EXTI_IRQn EXTI9_5_IRQn
#define nFaultX_Pin GPIO_PIN_15
#define nFaultX_GPIO_Port GPIOA
#define nFaultX_EXTI_IRQn EXTI15_10_IRQn
#define W5500_RSTn_Pin GPIO_PIN_3
#define W5500_RSTn_GPIO_Port GPIOD
#define W5500_INTn_Pin GPIO_PIN_4
#define W5500_INTn_GPIO_Port GPIOD
#define nFaultY_Pin GPIO_PIN_6
#define nFaultY_GPIO_Port GPIOD
#define nFaultY_EXTI_IRQn EXTI9_5_IRQn
#define WATCHDOG2_Pin GPIO_PIN_7
#define WATCHDOG2_GPIO_Port GPIOD
#define WATCHDOG1_Pin GPIO_PIN_9
#define WATCHDOG1_GPIO_Port GPIOG
#define W5500_CSn_Pin GPIO_PIN_10
#define W5500_CSn_GPIO_Port GPIOG
#define FM25V02_CSn_Pin GPIO_PIN_11
#define FM25V02_CSn_GPIO_Port GPIOG
#define XJAWLMT_Pin GPIO_PIN_14
#define XJAWLMT_GPIO_Port GPIOG
#define XJAWLMT_EXTI_IRQn EXTI15_10_IRQn
#define Xbrake_Pin GPIO_PIN_4
#define Xbrake_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
