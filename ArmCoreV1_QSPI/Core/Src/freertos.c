/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ecat_def.h"
#include "applInterface.h"
#include "el9800hw.h"
#include "el9800appl.h"
#include "queue.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for EthercatSlave */
osThreadId_t EthercatSlaveHandle;
const osThreadAttr_t EthercatSlave_attributes = {
  .name = "EthercatSlave",
  .stack_size = 4096 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TCPClient */
osThreadId_t TCPClientHandle;
const osThreadAttr_t TCPClient_attributes = {
  .name = "TCPClient",
  .stack_size = 8192 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DataProcess */
osThreadId_t DataProcessHandle;
const osThreadAttr_t DataProcess_attributes = {
  .name = "DataProcess",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Console */
osThreadId_t ConsoleHandle;
const osThreadAttr_t Console_attributes = {
  .name = "Console",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CmdQueue */
osMessageQueueId_t CmdQueueHandle;
const osMessageQueueAttr_t CmdQueue_attributes = {
  .name = "CmdQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Ethercatfunc(void *argument);
void TCPClientTask(void *argument);
void DataProccessTask(void *argument);
void StartConsoleTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of CmdQueue */
  CmdQueueHandle = osMessageQueueNew (16, sizeof(struct CmdMessage), &CmdQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of EthercatSlave */
  EthercatSlaveHandle = osThreadNew(Ethercatfunc, NULL, &EthercatSlave_attributes);

  /* creation of TCPClient */
  TCPClientHandle = osThreadNew(TCPClientTask, NULL, &TCPClient_attributes);

  /* creation of DataProcess */
  DataProcessHandle = osThreadNew(DataProccessTask, NULL, &DataProcess_attributes);

  /* creation of Console */
  ConsoleHandle = osThreadNew(StartConsoleTask, NULL, &Console_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    osDelay(1000);//delay 1s
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET);//watchdog signal 2
    /* Infinite loop */
    for(;;)
    {
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_7);//watchdog signal 1
//        printf("InfoOut contents: \r\n");
//        for (int i = 0; i < 8; i++)
//        {
//            printf("InfoOut[%d] = %d\r\n",i,sDOOutputs.InfoOut[i]);
//        }
//        printf("\r\n");
//        for (int i = 0; i < 8; i++)
//        {
//            printf("%d\r\n", sDOOutputs.DataOut1[i]);
//        }
//        printf("\r\n");
//        for (int i = 0; i < 8; i++)
//        {
//            printf("%d\r\n", sDOOutputs.DataOut2[i]);
//        }
//        printf("\r\n");
//        for (int i = 0; i < 8; i++)
//        {
//            printf("%d\r\n", sDOOutputs.DataOut3[i]);
//        }
//        printf("\r\n");
//        for (int i = 0; i < 8; i++)
//        {
//            printf("%d\r\n", sDOOutputs.DataOut4[i]);
//        }
//        printf("\r\n");
//        for (int i = 0; i < 8; i++)
//        {
//            printf("%d\r\n", sDOOutputs.DataOut8[i]);
//        }
//        for (int i = 0; i < 8; i++)
//        {
//            printf("%d\r\n", sDOOutputs.DataOut12[i]);
//        }
//        printf("\r\n");

        osDelay(100);
    }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Ethercatfunc */
/**
* @brief Function implementing the EthercatSlave thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Ethercatfunc */
void Ethercatfunc(void *argument)
{
  /* USER CODE BEGIN Ethercatfunc */

    MainInit();
    /* Infinite loop */
    for(;;)
    {
//        printf("Ethercat Mainloop running1\r\n");
        MainLoop();
//        printf("Ethercat Mainloop running2\r\n");
        osDelay(1);//todo : if sth happened,check this delay

    }
  /* USER CODE END Ethercatfunc */
}

/* USER CODE BEGIN Header_TCPClientTask */
/**
* @brief Function implementing the TCPClient thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TCPClientTask */
void TCPClientTask(void *argument)
{
  /* USER CODE BEGIN TCPClientTask */

    W5500_ChipInit();
    TCPFeedbackInit();
    /* Infinite loop */
    for(;;)
    {
        do_tcpc();
        osDelay(1);
    }
  /* USER CODE END TCPClientTask */
}

/* USER CODE BEGIN Header_DataProccessTask */
/**
* @brief Function implementing the DataProcess thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DataProccessTask */
void DataProccessTask(void *argument)
{
  /* USER CODE BEGIN DataProccessTask */
    nrtInit();
    /* Infinite loop */
  //  osDelay(500);
    for(;;)
    {
        nrtDataMainLoop();
      //  osDelay(1);
    }
  /* USER CODE END DataProccessTask */
}

/* USER CODE BEGIN Header_StartConsoleTask */
/**
* @brief Function implementing the Console thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartConsoleTask */
void StartConsoleTask(void *argument)
{
  /* USER CODE BEGIN StartConsoleTask */
    /* Infinite loop */
    for(;;)
    {
#if UART_Control
        if(uxQueueMessagesWaitingFromISR(CmdQueueHandle))
        {
            xQueueReceiveFromISR(CmdQueueHandle, &CmdMsg, 0);
            //printf("CmdMsg.Cmd = %x,CmdMsg.Parameter = %x\r\n", CmdMsg.Cmd, CmdMsg.Parameter);
            ExecuteConsoleCmd(CmdMsg.Cmd, CmdMsg.Parameter);
        }
#else
        vTaskSuspend(ConsoleHandle);
#endif
        osDelay(1);
    }
  /* USER CODE END StartConsoleTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

