/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "cmsis_os.h"
#include "bdma.h"
#include "crc.h"
#include "dma.h"
#include "iwdg.h"
#include "mdma.h"
#include "rtc.h"
#include "tim.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* note: component and configuration header file */
#include "ulog.h"
#include "shell.h"
#include "console.h"
#include "sys_cfg.h"
#include "init_call.h"
#include "config.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void system_info_print(void)
{
    struct sys_info *sys_info = system_info_get();
    
    printf("\r\n************************************\r\n");

    printf("fw version: %s\r\n", sys_info->fw_version);
    printf("compile time: %s\r\n", sys_info->compile_time);
    printf("git branch: %s\r\n", GIT_BRANCH);
    printf("git hash: %s\r\n", GIT_HASH);
    printf("mcu clock:%.2f M\r\n", HAL_RCC_GetSysClockFreq()/1000000.0);

    printf("stm32 uid:%#.8x%.8x%.8x\r\n", HAL_GetUIDw2(), HAL_GetUIDw1(), HAL_GetUIDw0());
    printf("flash size:%uKB\r\n", FLASH_SIZE / 1024);

    uint32_t hal_version = HAL_GetHalVersion();

    printf("hal driver version:%.2u.%.2u.%.2u.%.2u\r\n", (hal_version >> 24) & 0xFF, 
        (hal_version >> 16) & 0xFF, (hal_version >> 8) & 0xFF, hal_version & 0xFF);

    printf("************************************\r\n");

}
MSH_CMD_EXPORT_ALIAS(system_info_print, system_info, system info);

extern uint32_t  __init_call_start;
extern uint32_t  __init_call_end;

static void system_fun_init(void)
{
    const struct init_desc *desc;
    int result = 0;
    uint32_t fun_num = 1;

    printf("\r\n########## function initialize begin ##########\r\n");

    for (desc = &__init_call_start; desc < &__init_call_end; desc++, fun_num++)
    {
        result = desc->init_fn();
        printf("done_%-2u [%-32s %-2d]\r\n", fun_num, desc->fn_name, result);
    }

    printf("########## function initialize end   ##########\r\n\r\n");
}

static void cmd_rtc_test(uint8_t argc, uint8_t **argv)
{
    uint32_t bkp_data = 0;

    bkp_data = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
    printf("bkp_data:%x\r\n", bkp_data);

    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0xA5A5A5A5);

    bkp_data = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
    printf("bkp_data:%x\r\n", bkp_data);

    /* backup sram: 4KB */
    printf("bkp sram:%x\r\n", *(__IO uint32_t *)D3_BKPSRAM_BASE);
    *(__IO uint32_t *)D3_BKPSRAM_BASE = 0x88234567;
    printf("bkp sram:%x\r\n", *(__IO uint32_t *)D3_BKPSRAM_BASE);

}
MSH_CMD_EXPORT_ALIAS(cmd_rtc_test, rtc_test, rtc tset);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
/* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_MDMA_Init();
  MX_BDMA_Init();
  MX_FMC_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_CRC_Init();
  MX_IWDG1_Init();
  MX_RTC_Init();
  MX_TIM6_Init();

  /* USER CODE BEGIN 2 */

  SDRAM_Init();
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);//leaf servo drive signal
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_SET);//carrier servo drive signal

  /* Console initialize */
  device_console_init(CONSOLE_NAME_DEFAULT);
  system_info_print();
  system_fun_init(); 

  LOG_I("Init ok\r\n");

#if 0
    uint8_t buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint32_t res = hardware_crc_calculate(buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc32 res = %#x\r\n", res^0xFFFFFFFF);

    hardware_crc_config(CRC8);
    res = hardware_crc_calculate(buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc8 res = %#x\r\n", res);

    hardware_crc_config(CRC16);
    res = hardware_crc_calculate(buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc16 res = %#x\r\n", res);

    hardware_crc_config(CRC32);
    res = hardware_crc_calculate(buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc32 res = %#x\r\n", res^0xFFFFFFFF);

    hardware_crc_config(CRC8);
    res = hardware_crc_calculate(buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc8 res = %#x\r\n", res);

    hardware_crc_config(CRC16);
    res = hardware_crc_calculate(buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc16 res = %#x\r\n", res);
    
#endif

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 110;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI|RCC_PERIPHCLK_SPI6
                              |RCC_PERIPHCLK_SPI3|RCC_PERIPHCLK_SPI2
                              |RCC_PERIPHCLK_SPI1|RCC_PERIPHCLK_USART1;
  PeriphClkInitStruct.PLL2.PLL2M = 5;
  PeriphClkInitStruct.PLL2.PLL2N = 96;
  PeriphClkInitStruct.PLL2.PLL2P = 5;
  PeriphClkInitStruct.PLL2.PLL2Q = 4;
  PeriphClkInitStruct.PLL2.PLL2R = 6;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3M = 5;
  PeriphClkInitStruct.PLL3.PLL3N = 50;
  PeriphClkInitStruct.PLL3.PLL3P = 5;
  PeriphClkInitStruct.PLL3.PLL3Q = 10;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL3;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16910CLKSOURCE_PLL2;
  PeriphClkInitStruct.Spi6ClockSelection = RCC_SPI6CLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
