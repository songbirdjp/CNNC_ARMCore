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
#include "tcp_config.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define DATA_PROCESS_FPGA_EVENT   (1<<0)
#define DATA_PROCESS_LAN_EVENT    (1<<1)
#define DATA_PROCESS_TCP_EVENT    (1<<2)
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
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Console */
osThreadId_t ConsoleHandle;
const osThreadAttr_t Console_attributes = {
  .name = "Console",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for tcp_irq_thread */
osThreadId_t tcp_irq_threadHandle;
const osThreadAttr_t tcp_irq_thread_attributes = {
  .name = "tcp_irq_thread",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Definitions for recv_data_process_thread */
osThreadId_t recv_data_process_threadHandle;
const osThreadAttr_t recv_data_process_thread_attributes = {
  .name = "recv_data_process_thread",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Definitions for CmdQueue */
// osMessageQueueId_t CmdQueueHandle;
// const osMessageQueueAttr_t CmdQueue_attributes = {
//   .name = "CmdQueue"
// };

/* Definitions for tcp_rx_queue */
osMessageQueueId_t tcp_rx_queueHandle;
const osMessageQueueAttr_t tcp_rx_queue_attributes = {
  .name = "tcp_rx_queue"
};

/* Definitions for tcp_access_mutex */
osMutexId_t tcp_access_mutexHandle;
const osMutexAttr_t tcp_access_mutex_attributes = {
  .name = "tcp_access_mutex",
  .attr_bits = osMutexRecursive | osMutexPrioInherit
};

/* Definitions for tcp_irq_event */
osEventFlagsId_t tcp_irq_eventHandle;
const osEventFlagsAttr_t tcp_irq_event_attributes = {
  .name = "tcp_irq_event"
};

/* Definitions for data_process_event */
osEventFlagsId_t data_process_eventHandle;
const osEventFlagsAttr_t data_process_event_attributes = {
  .name = "data_process_event"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Ethercatfunc(void *argument);
void TCPClientTask(void *argument);
void DataProccessTask(void *argument);
void StartConsoleTask(void *argument);
void tcp_client_entry(void *argument);
void data_process_entry(void *argument);

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
  /* creation of tcp_access_mutex */
  tcp_access_mutexHandle = osMutexNew(&tcp_access_mutex_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of CmdQueue */
//   CmdQueueHandle = osMessageQueueNew (16, sizeof(struct CmdMessage), &CmdQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* creation of tcp_rx_queue */
  tcp_rx_queueHandle = osMessageQueueNew (3, sizeof(TCP_DATA_t), &tcp_rx_queue_attributes);
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

  /* creation of tcp_irq_thread */
  tcp_irq_threadHandle = osThreadNew(tcp_client_entry, NULL, &tcp_irq_thread_attributes);

  /* creation of recv_data_process_thread */
  recv_data_process_threadHandle = osThreadNew(data_process_entry, NULL, &recv_data_process_thread_attributes);
  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* creation of data_process_event */
  data_process_eventHandle = osEventFlagsNew(&data_process_event_attributes);
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

    int8_t ret = 0;

    ret = tcp_init(tcp_rx_queueHandle);
    if (ret != 0)
    {
        printf("tcp init err\r\n");
        return;
    }
    /* Infinite loop */
    for(;;)
    {
        osMutexAcquire(tcp_access_mutexHandle, osWaitForever);

        while(tcp_link_detect() == false)
        {
            printf("tcp link off\r\n");

            tcp_link_state_recover();

            osDelay(100);
        }

        ret = do_tcp_client(socket_num_get());
        if (ret != 0)
        {
            printf("do_tcp_client err:%d\r\n", ret);
        }

        osMutexRelease(tcp_access_mutexHandle);

        osDelay(100);
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
   // osDelay(500);
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
        // vTaskSuspend(ConsoleHandle);
#endif

        console_cmd_process();

    }
  /* USER CODE END StartConsoleTask */
}

/* USER CODE BEGIN Header_tcp_client_entry */
/**
* @brief Function implementing the tcp_irq_thread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_tcp_client_entry */
void tcp_client_entry(void *argument)
{
  /* USER CODE BEGIN tcp_client_entry */
  /* Infinite loop */

  for(;;)
  {
        while(tcp_link_status() == false)
        {
            osDelay(100);
        }

        tcp_data_recv_with_block();
  }
  /* USER CODE END tcp_client_entry */
}

/* USER CODE BEGIN Header_data_process_entry */
/**
* @brief Function implementing the recv_data_process_entry thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_data_process_entry */
void data_process_entry(void *argument)
{
  /* USER CODE BEGIN data_process_entry */
  /* Infinite loop */
  osStatus_t stat = 0;
  uint32_t event_flag = 0;
  uint8_t pdo_output_data[MAX_PD_OUTPUT_SIZE] = {0};
  uint8_t spi2_buf[RECV_BUF_LEN] = {0};
  TCP_DATA_t tcp_info = {0};

  for(;;)
  {

    // event_flag = osEventFlagsWait(data_process_eventHandle, DATA_PROCESS_SPI2_EVENT | DATA_PROCESS_LAN_EVENT | DATA_PROCESS_TCP_EVENT, osFlagsWaitAny, osWaitForever);
    // if (event_flag & DATA_PROCESS_LAN_EVENT)
    // {
    //     // printf("recv DATA_PROCESS_LAN_EVENT\n");

    //     stat = osMessageQueueGet(lan9252_rx_queueHandle, pdo_output_data, 0, 0);
    //     if (stat == osOK)
    //     {
    //         APPL_OutputMapping((uint16_t *) pdo_output_data); // 对数据大小端进行转换

    //         // printf("%x %x %x %x\n", sDOOutputs.InfoOut[0], sDOOutputs.InfoOut[1], sDOOutputs.InfoOut[2], sDOOutputs.InfoOut[3]);
    //     }
    //     else
    //     {
    //         printf("no msg in lan9252 rx queue:%d\r\n", stat);
    //     }
    // }

    // if (event_flag & DATA_PROCESS_SPI2_EVENT)
    // {
    //     stat = osMessageQueueGet(spi2_rx_queueHandle, spi2_buf, 0, 0);
    //     if (stat == osOK)
    //     {
    //         APPL_Application_New(spi2_buf);
    //     }
    //     else
    //     {
    //         printf("no msg in spi2 rx queue:%d\r\n", stat);
    //     }
    // }

    // if (event_flag & DATA_PROCESS_TCP_EVENT)
    {
        stat = osMessageQueueGet(tcp_rx_queueHandle, &tcp_info, 0, osWaitForever);
        if (stat == osOK)
        {
            // ntrRecvParamAndPlan(&tcp_info);

            osMutexAcquire(tcp_access_mutexHandle, osWaitForever);
            sendFeedback();
            osMutexRelease(tcp_access_mutexHandle);

            printf("tcp_info len:%d\r\n", tcp_info.Len);
            printf("tcp_info %x %x %x %x\r\n", tcp_info.gDATABUF[0], tcp_info.gDATABUF[1], tcp_info.gDATABUF[2], tcp_info.gDATABUF[3]);
        }
        else
        {
            printf("no msg in tcp rx queue:%d\r\n", stat);
        }        
    }
    
  }
  /* USER CODE END data_process_entry */
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

