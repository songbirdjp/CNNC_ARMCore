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
#include "bdma.h"
#include "crc.h"
#include "dma.h"
#include "iwdg.h"
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
#include "hw_crc.h"
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
#if configAPPLICATION_ALLOCATED_HEAP
uint8_t ucHeap[configTOTAL_HEAP_SIZE] = {0};
#ifdef USE_FreeRTOS_HEAP_5
static HeapRegion_t xHeapRegions[] = 
{
    { ucHeap, configTOTAL_HEAP_SIZE },
    { (uint8_t *)0xC0000000, 0x2000000},
    { NULL,   0                     }
};
#endif
#endif
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

#if 0
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

static int8_t fpu_test(uint8_t argc, uint8_t **argv)
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

static uint32_t itcm[1024] __attribute__((section(".ram_itcm"))) = {0};
static uint32_t dtcm[1024] __attribute__((section(".ram_dtcm"))) = {0};
static uint32_t sdram[1024] __attribute__((section(".sdram_ext"))) = {0};
static uint32_t sram[1024] = {0};
#include "utilities.h"
static int8_t ram_speed_test(uint8_t argc, uint8_t **argv)
{
    uint32_t loop = atoi(argv[1]);
    struct system_time begin = {0};
    struct system_time end = {0};

    /* 1. itcm */
    system_time_get(&begin);
    for (int i = 0; i < loop; i+=4)
    {
        itcm[i % 1024] = i;
        itcm[i % 1024 + 1] = i + 1;
        itcm[i % 1024 + 2] = i + 2;
        itcm[i % 1024 + 3] = i + 3;
    }
    system_time_get(&end);
    printf("itcm time: %u us\r\n", time_diff_us(&begin, &end));

    osDelay(100);

    /* 2. dtcm */
    system_time_get(&begin);
    for (int i = 0; i < loop; i+=4)
    {
        dtcm[i % 1024] = i;
        dtcm[i % 1024 + 1] = i + 1;
        dtcm[i % 1024 + 2] = i + 2;
        dtcm[i % 1024 + 3] = i + 3;
    }
    system_time_get(&end);
    printf("dtcm time: %u us\r\n", time_diff_us(&begin, &end));

    osDelay(100);

    /* 3. sdram */
    system_time_get(&begin);
    for (int i = 0; i < loop; i+=4)
    {
        sdram[i % 1024] = i;
        sdram[i % 1024 + 1] = i + 1;
        sdram[i % 1024 + 2] = i + 2;
        sdram[i % 1024 + 3] = i + 3;
    }
    system_time_get(&end);
    printf("sdram time: %u us\r\n", time_diff_us(&begin, &end));

    osDelay(100);

    /* 4. sram */
    system_time_get(&begin);
    for (int i = 0; i < loop; i+=4)
    {
        sram[i % 1024] = i;
        sram[i % 1024 + 1] = i + 1;
        sram[i % 1024 + 2] = i + 2;
        sram[i % 1024 + 3] = i + 3;
    }
    system_time_get(&end);
    printf("sram time: %u us\r\n", time_diff_us(&begin, &end));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(ram_speed_test, ram_speed_test, test ram);

static int8_t hw_crc_test(uint8_t argc, uint8_t **argv)
{
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
    
    return 0;
}
MSH_CMD_EXPORT_ALIAS(hw_crc_test, hw_crc_test, test crc);

static int8_t ext_sdram_test(uint8_t argc, uint8_t **argv)
{
    uint16_t *ext_sdram_array = (uint16_t *)pvPortMalloc(1024 * 1024 * 32 - 16);
    if (ext_sdram_array == NULL)
    {
        printf("malloc error\r\n");
        return -1;
    }

    printf("ext sdram malloc ok: %p\r\n", ext_sdram_array);

    for (int i = 0; i < 1024; i++)
    {
        if (i % 16 == 0 && i != 0)
        {
            printf("\r\n");
        }
        printf("%.4x ", ext_sdram_array[i]);
    }

    printf("\r\n");

    for (int i = 0; i < 1024; i++)
    {
        ext_sdram_array[i] = i;
    }

    for (int i = 0; i < 1024; i++)
    {
        if (i % 16 == 0 && i != 0)
        {
            printf("\r\n");
        }
        printf("%.4x ", ext_sdram_array[i]);
    }

    printf("\r\n");

    vPortFree(ext_sdram_array);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(ext_sdram_test, ext_sdram_test, test ext_sdram);
#endif

static int8_t system_heap_init(void)
{
#ifdef USE_FreeRTOS_HEAP_5
    vPortDefineHeapRegions (xHeapRegions);
#endif
    return 0;
}

static int8_t system_reset_status_check(void)
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_CPURST) != 0)
    {
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != 0)
        {
            __HAL_RCC_CLEAR_RESET_FLAGS();
        }
        else
        {
            printf("CPU reset checked\r\n");
            printf("RCC_FLAG_D1RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_D1RST));
            printf("RCC_FLAG_D2RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_D2RST));
            printf("RCC_FLAG_BORRST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_BORRST));
            printf("RCC_FLAG_PINRST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_PINRST));
            printf("RCC_FLAG_PORRST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_PORRST));
            printf("RCC_FLAG_SFTRST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST));
            printf("RCC_FLAG_IWDG1RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST));
            printf("RCC_FLAG_WWDG1RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_WWDG1RST));
            printf("RCC_FLAG_LPWR1RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_LPWR1RST));
        }
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(system_reset_status_check, system_reset_status_check, system wdg check);
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
  /* USER CODE BEGIN 2 */
#ifdef configGENERATE_RUN_TIME_STATS
  HAL_TIM_Base_Start_IT(&htim6);
#endif

  bank1_sdram_init();

  system_heap_init();

  /* Console initialize */
  device_console_init(CONSOLE_NAME_DEFAULT);
  system_info_print();
  system_fun_init(); 

//   printf("----this is bootloader----\r\n");

  system_reset_status_check();

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
                              |RCC_PERIPHCLK_SPI2|RCC_PERIPHCLK_SPI1
                              |RCC_PERIPHCLK_USART1;
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
