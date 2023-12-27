/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
//#include "retarget.h"
#include "global.h"
#include "octospi.h"
#include "w5500.h"
#include "spi.h"
// #include "httpserver.h"
#include "nonRealtimeDataProcess.h"
#include "fpga_rw.h"
#include "el9800appl.h"
#include "sdram_fmc_drv.h"

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
#define CHIP_RUN_LED_Pin GPIO_PIN_1
#define CHIP_RUN_LED_GPIO_Port GPIOC
#define OSPI5_ncs_Pin GPIO_PIN_11
#define OSPI5_ncs_GPIO_Port GPIOE
#define SPI3_NCS_Pin GPIO_PIN_12
#define SPI3_NCS_GPIO_Port GPIOD
#define DriveBoardCtrl_Pin GPIO_PIN_13
#define DriveBoardCtrl_GPIO_Port GPIOD
#define CarrierServoSignal_Pin GPIO_PIN_3
#define CarrierServoSignal_GPIO_Port GPIOG
#define LAN9252SYNC1_Pin GPIO_PIN_2
#define LAN9252SYNC1_GPIO_Port GPIOD
#define LAN9252SYNC1_EXTI_IRQn EXTI2_IRQn
#define W5500_RST_Pin GPIO_PIN_3
#define W5500_RST_GPIO_Port GPIOD
#define W5500_INTn_Pin GPIO_PIN_4
#define W5500_INT_GPIO_Port GPIOD
#define WATCHDOG2_Pin GPIO_PIN_7
#define WATCHDOG2_GPIO_Port GPIOD
#define WATCHDOG1_Pin GPIO_PIN_9
#define WATCHDOG1_GPIO_Port GPIOG
#define SPI1_NCS_Pin GPIO_PIN_10
#define SPI1_NCS_GPIO_Port GPIOG
#define KEY1_Pin GPIO_PIN_12
#define KEY1_GPIO_Port GPIOG
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */
void SystemClock_Config(void);
#define CMD_HELP_MODE                   1
#define CMD_CLEAR_MODE                  2
#define CMD_STATE_MACHINE_MODE          3
#define CMD_SEND_DATA_MODE              4
#define CMD_RECV_DATA_MODE              5
#define CMD_POSITION_MODE               6
#define CMD_HOME_MODE                   20
#define CMD_AUTO_MODE                   21
#define CMD_META_DATA_MODE                 99

#define StateMachineIdle                1
#define StateMachineInit                2
#define StateMachinePrepare             3
#define StateMachineReady               4
#define StateMachineServo               5
#define StateMachineFault               6
#define StateMachineShutDown            7

#define SendEtherNetData                1
#define SendEtherCATData                2


#define BANKA
//#define TEST


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
