/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "cmsis_os.h"
#include "adc.h"
#include "bdma.h"
#include "crc.h"
#include "dma.h"
#include "iwdg.h"
#include "lptim.h"
#include "mdma.h"
#include "memorymap.h"
#include "rtc.h"
#include "tim.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* note: component and configuration header file */
#include "shell.h"
#include "console.h"
#include "sys_cfg.h"
#include "init_call.h"
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
static void vector_table_init(void)
{
    extern uint32_t __isr_vector_start;

    SCB->VTOR = (uint32_t)&__isr_vector_start;
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

static void rdp_test(uint8_t argc, uint8_t **argv)
{
    if (argc != 2)
    {
        printf("param error\r\n");
        return;
    }

    FLASH_OBProgramInitTypeDef OBInit = {0};
    HAL_FLASHEx_OBGetConfig(&OBInit);

    printf("RDPLevel:%#x\r\n", OBInit.RDPLevel);

    OBInit.OptionType = OPTIONBYTE_RDP;

    switch (atoi(argv[1]))
    {
    case 0:
        OBInit.RDPLevel = OB_RDP_LEVEL_0;
        break;
    case 1:
        OBInit.RDPLevel = OB_RDP_LEVEL_1;
        break;
    case 2:

        return;
    default:
        break;
    }

    HAL_StatusTypeDef status;

    status = HAL_FLASH_OB_Unlock();
    if (status != HAL_OK)
    {
        printf("OBUnlock error:%#x\r\n", status);
    }

    status = HAL_FLASHEx_OBProgram(&OBInit);
    if (status != HAL_OK)
    {
        printf("OBProgram error:%#x\r\n", status);
    }

    status = HAL_FLASH_OB_Launch();
    if (status != HAL_OK)
    {
        printf("OBLaunch error:%#x\r\n", status);
    }

    status = HAL_FLASH_OB_Lock();
    if (status != HAL_OK)
    {
        printf("OBLock error:%#x\r\n", status);
    }
}
MSH_CMD_EXPORT_ALIAS(rdp_test, rdp_test, rtc rdp);

int8_t fpu_test(uint8_t argc, uint8_t **argv)
{

    if (argc != 2)
    {
        printf("param error\r\n");
    }

    uint32_t loop = atoi(argv[1]);

    uint32_t start, end;

    __disable_irq();

    start = __HAL_TIM_GET_COUNTER(&htim2);

    float f = 1;
    for (int i = 0; i < loop; i++)
    {
        f = f * 1.1;
    }

    end = __HAL_TIM_GET_COUNTER(&htim2);

    __enable_irq();

    if (end < start)
    {
        end += __HAL_TIM_GET_AUTORELOAD(&htim2);
    }
    printf("time:%u\r\n", end - start);
    
    printf("%f\r\n", f);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(fpu_test, fpu_test, test fpu);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  vector_table_init();
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
  MX_TIM2_Init();
  MX_CRC_Init();
  MX_IWDG1_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_ADC3_Init();
  MX_LPTIM1_Init();
  /* USER CODE BEGIN 2 */
#ifdef configGENERATE_RUN_TIME_STATS
  HAL_TIM_Base_Start_IT(&htim6);
#endif

  bank1_sdram_init();

  /* Console initialize */
  device_console_init(CONSOLE_NAME_DEFAULT);
  system_info_print();
  system_fun_init(); 

//   printf("----this is bootloader----\r\n");

  printf("Init ok\r\n");

  if (system_encrypt_init() != 0)
  {
    goto err;
  }

  printf("portCPUID:%#.8x\r\n", * ( ( volatile uint32_t * ) 0xE000ed00 ));

#if 1
  portENABLE_INTERRUPTS();
  extern int8_t app_valid_check_and_jump(void);
  app_valid_check_and_jump();
#endif

#if 0
    uint8_t buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint32_t res = hardware_crc_calculate(CRC32, buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc32 res = %#x\r\n", res^0xFFFFFFFF);

    res = hardware_crc_calculate(CRC8, buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc8 res = %#x\r\n", res);

    res = hardware_crc_calculate(CRC16, buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc16 res = %#x\r\n", res);

    res = hardware_crc_calculate(CRC32, buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc32 res = %#x\r\n", res^0xFFFFFFFF);

    res = hardware_crc_calculate(CRC8, buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc8 res = %#x\r\n", res);

    res = hardware_crc_calculate(CRC16, buf, sizeof(buf)/sizeof(buf[0]));
    printf("crc16 res = %#x\r\n", res);
    
#endif

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
err:
  portENABLE_INTERRUPTS();

  while (1)
  {
    HAL_IWDG_Refresh(&hiwdg1);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_7);

    HAL_Delay(100);
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
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_MEDIUMHIGH);

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
                              |RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_LPTIM1
                              |RCC_PERIPHCLK_USART1;
  PeriphClkInitStruct.PLL2.PLL2M = 5;
  PeriphClkInitStruct.PLL2.PLL2N = 96;
  PeriphClkInitStruct.PLL2.PLL2P = 5;
  PeriphClkInitStruct.PLL2.PLL2Q = 4;
  PeriphClkInitStruct.PLL2.PLL2R = 6;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16910CLKSOURCE_PLL2;
  PeriphClkInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_PLL2;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
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
