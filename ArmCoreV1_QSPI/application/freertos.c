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
#include "queue.h"
#include "tcp_client.h"
#include "ulog.h"
#include "iwdg.h"
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
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
// /* Definitions for EthercatSlave */
// osThreadId_t EthercatSlaveHandle;
// const osThreadAttr_t EthercatSlave_attributes = {
//   .name = "EthercatSlave",
//   .stack_size = 4096 * 4,
//   .priority = (osPriority_t) osPriorityNormal,
// };
/* Definitions for TCPClient */
// osThreadId_t TCPClientHandle;
// const osThreadAttr_t TCPClient_attributes = {
//   .name = "TCPClient",
//   .stack_size = 8192 * 4,
//   .priority = (osPriority_t) osPriorityNormal,
// };
/* Definitions for DataProcess */
// osThreadId_t DataProcessHandle;
// const osThreadAttr_t DataProcess_attributes = {
//   .name = "DataProcess",
//   .stack_size = 1024 * 4,
//   .priority = (osPriority_t) osPriorityNormal,
// };
/* Definitions for Console */
// osThreadId_t ConsoleHandle;
// const osThreadAttr_t Console_attributes = {
//   .name = "Console",
//   .stack_size = 1024 * 4,
//   .priority = (osPriority_t) osPriorityNormal,
// };
/* Definitions for tcp_irq_thread */
// osThreadId_t tcp_irq_threadHandle;
// const osThreadAttr_t tcp_irq_thread_attributes = {
//   .name = "tcp_irq_thread",
//   .stack_size = 512 * 4,
//   .priority = (osPriority_t) osPriorityAboveNormal,
// };

/* Definitions for fpga_communication_thread */
// osThreadId_t fpga_communication_threadHandle;
// const osThreadAttr_t fpga_communication_thread_attributes = {
//   .name = "fpga_communication_thread",
//   .stack_size = 256 * 4,
//   .priority = (osPriority_t) osPriorityNormal7,
// };

/* Definitions for recv_data_process_thread */
// osThreadId_t recv_data_process_threadHandle;
// const osThreadAttr_t recv_data_process_thread_attributes = {
//   .name = "recv_data_process_thread",
//   .stack_size = 1024 * 4,
//   .priority = (osPriority_t) osPriorityAboveNormal,
// };

/* Definitions for lan9252_irq_thread */
// osThreadId_t lan9252_irq_threadHandle;
// const osThreadAttr_t lan9252_irq_thread_attributes = {
//   .name = "lan9252_irq_thread",
//   .stack_size = 512 * 4,
//   .priority = (osPriority_t) osPriorityHigh,
// };

/* Definitions for CmdQueue */
// osMessageQueueId_t CmdQueueHandle;
// const osMessageQueueAttr_t CmdQueue_attributes = {
//   .name = "CmdQueue"
// };

/* Definitions for tcp_rx_queue */
// osMessageQueueId_t tcp_rx_queueHandle;
// const osMessageQueueAttr_t tcp_rx_queue_attributes = {
//   .name = "tcp_rx_queue"
// };

/* Definitions for send_to_fpga_queue */
// osMessageQueueId_t send_to_fpga_queueHandle;
// const osMessageQueueAttr_t send_to_fpga_queue_attributes = {
//   .name = "send_to_fpga_queue"
// };

/* Definitions for recv_from_fpga_queue */
// osMessageQueueId_t recv_from_fpga_queueHandle;
// const osMessageQueueAttr_t recv_from_fpga_queue_attributes = {
// .name = "recv_from_fpga_queue"
// };

/* Definitions for tcp_access_mutex */
// osMutexId_t tcp_access_mutexHandle;
// const osMutexAttr_t tcp_access_mutex_attributes = {
//   .name = "tcp_access_mutex",
//   .attr_bits = osMutexRecursive | osMutexPrioInherit
// };

/* Definitions for data_process_event */
// osEventFlagsId_t data_process_eventHandle;
// const osEventFlagsAttr_t data_process_event_attributes = {
//   .name = "data_process_event"
// };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
#ifdef configGENERATE_RUN_TIME_STATS
static uint32_t run_time_count = 0;
void run_time_count_increase(void)
{
    run_time_count++;
}
uint32_t run_time_count_get(void)
{
    return run_time_count;
}

void run_time_count_set(uint32_t val)
{
    run_time_count = val;
}
#endif
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
// void Ethercatfunc(void *argument);
// void TCPClientTask(void *argument);
// void DataProccessTask(void *argument);
void StartConsoleTask(void *argument);
// void tcp_client_entry(void *argument);
// void fpga_communication_entry(void *argument);
// void data_process_entry(void *argument);
// void ethercat_slave_entry(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
    run_time_count_set(0);
}

__weak unsigned long getRunTimeCounterValue(void)
{
    return run_time_count_get();
}
/* USER CODE END 1 */

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */

    /*****************************
     * add watchdog feed function here
    *****************************/

   HAL_IWDG_Refresh(&hiwdg1);


}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */

   printf("%s stack over flow\r\n", pcTaskName);
}
/* USER CODE END 4 */

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
//   tcp_access_mutexHandle = osMutexNew(&tcp_access_mutex_attributes);
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
//   tcp_rx_queueHandle = osMessageQueueNew (3, sizeof(TCP_DATA_t), &tcp_rx_queue_attributes);

  /* creation of send_to_fpga_queueHandle */
//   send_to_fpga_queueHandle = osMessageQueueNew (10, sizeof(struct send_to_fpga_msg), &send_to_fpga_queue_attributes);

  /* creation of recv_from_fpga_queueHandle */
//   recv_from_fpga_queueHandle = osMessageQueueNew (3, RECV_BUF_LEN, &recv_from_fpga_queue_attributes);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

//   /* creation of EthercatSlave */
//   EthercatSlaveHandle = osThreadNew(Ethercatfunc, NULL, &EthercatSlave_attributes);

  /* creation of TCPClient */
//   TCPClientHandle = osThreadNew(TCPClientTask, NULL, &TCPClient_attributes);

  /* creation of DataProcess */
//   DataProcessHandle = osThreadNew(DataProccessTask, NULL, &DataProcess_attributes);

  /* creation of Console */
//   ConsoleHandle = osThreadNew(StartConsoleTask, NULL, &Console_attributes);

  /* creation of tcp_irq_thread */
//   tcp_irq_threadHandle = osThreadNew(tcp_client_entry, NULL, &tcp_irq_thread_attributes);

  /* creation of fpga_communication_threadHandle */
//   fpga_communication_threadHandle = osThreadNew(fpga_communication_entry, NULL, &fpga_communication_thread_attributes);

  /* creation of recv_data_process_thread */
//   recv_data_process_threadHandle = osThreadNew(data_process_entry, NULL, &recv_data_process_thread_attributes);

//   /* creation of lan9252_irq_thread */
//   lan9252_irq_threadHandle = osThreadNew(ethercat_slave_entry, NULL, &lan9252_irq_thread_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* creation of data_process_event */
//   data_process_eventHandle = osEventFlagsNew(&data_process_event_attributes);
  /* USER CODE END RTOS_EVENTS */

//   ethercat_thread_init();

//   tcp_client_thread_init();

//   fpga_thread_init();

//   non_realtime_process_thread_init();

// #ifdef USING_ULOG_THREAD
//   ulog_thread_init();
// #endif
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
    // struct send_to_fpga_msg recv_buf = {0,1,2,3,4,5,6};
    for(;;)
    {
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_7);//watchdog signal 1

        osDelay(100);

        // recv_buf.len = 6;
        // osMessageQueuePut(send_to_fpga_queueHandle, &recv_buf, 0, 0);
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
// void Ethercatfunc(void *argument)
// {
//   /* USER CODE BEGIN Ethercatfunc */
//     ethercat_slave_init();

//     ethercat_slave_stack_init();

//     /* Infinite loop */
//     for(;;)
//     {

//         ethercat_slave_main_loop();        

//         osDelay(1);
//     }
//   /* USER CODE END Ethercatfunc */
// }

/* USER CODE BEGIN Header_TCPClientTask */
/**
* @brief Function implementing the TCPClient thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TCPClientTask */
// void TCPClientTask(void *argument)
// {
//   /* USER CODE BEGIN TCPClientTask */

//     int8_t ret = 0;

//     ret = tcp_init(tcp_rx_queueHandle);
//     if (ret != 0)
//     {
//         printf("tcp init err\r\n");
//         return;
//     }
//     /* Infinite loop */
//     for(;;)
//     {
//         osMutexAcquire(tcp_access_mutexHandle, osWaitForever);

//         while(tcp_link_detect() == false)
//         {
//             // printf("tcp link off\r\n");

//             tcp_link_state_recover();

//             osDelay(100);
//         }

//         ret = do_tcp_client(socket_num_get());
//         if (ret != 0)
//         {
//             printf("do_tcp_client err:%d\r\n", ret);
//         }

//         osMutexRelease(tcp_access_mutexHandle);

//         osDelay(100);
//     }
//   /* USER CODE END TCPClientTask */
// }

/* USER CODE BEGIN Header_DataProccessTask */
/**
* @brief Function implementing the DataProcess thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DataProccessTask */
// void DataProccessTask(void *argument)
// {
//   /* USER CODE BEGIN DataProccessTask */
//     nrtInit();
//     /* Infinite loop */
//    // osDelay(500);
//     for(;;)
//     {
//         nrtDataMainLoop();
//       //  osDelay(1);
//     }
//   /* USER CODE END DataProccessTask */
// }

/* USER CODE BEGIN Header_StartConsoleTask */
/**
* @brief Function implementing the Console thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartConsoleTask */
// void StartConsoleTask(void *argument)
// {
//   /* USER CODE BEGIN StartConsoleTask */
//     /* Infinite loop */
//     for(;;)
//     {
//         console_cmd_process();
//     }
//   /* USER CODE END StartConsoleTask */
// }

/* USER CODE BEGIN Header_tcp_client_entry */
/**
* @brief Function implementing the tcp_irq_thread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_tcp_client_entry */
// void tcp_client_entry(void *argument)
// {
//   /* USER CODE BEGIN tcp_client_entry */
//   /* Infinite loop */
//   int32_t ret = 0;

//   for(;;)
//   {
//         while(tcp_link_status() == false)
//         {
//             osDelay(100);
//         }

//         ret = tcp_data_recv_with_block();
//         if (ret < 0)
//         {
//             printf("tcp recv data err:%d\r\n", ret);
//         }
//         else if (ret > 0)
//         {
//             osEventFlagsSet(data_process_eventHandle, DATA_PROCESS_TCP_EVENT);
//         }

        
//   }
//   /* USER CODE END tcp_client_entry */
// }

/* USER CODE BEGIN Header_fpga_communication_entry */
/**
* @brief Function implementing the tcp_irq_thread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_fpga_communication_entry */
// void fpga_communication_entry(void *argument)
// {
//   /* USER CODE BEGIN fpga_communication_entry */
//   /* Infinite loop */
//   int8_t ret = 0;
//   osStatus_t stat = osOK;
//   struct send_to_fpga_msg recv_buf = {0};

//   send_to_fpga_init();
//   recv_from_fpga_init(recv_from_fpga_queueHandle);
  
//   for(;;)
//   {
//     stat = osMessageQueueGet(send_to_fpga_queueHandle, &recv_buf, 0, osWaitForever);
//     if (stat != osOK)
//     {
//         printf("get queue err:%d\r\n", stat);
//     }
  
//     ret = send_to_fpga_write(recv_buf.buf, recv_buf.len, 1000);
//     if (ret != 0)
//     {
//         printf("send data to fpga err:%d\r\n", ret);
//     }

//     // printf("recv_buf.len:%d\r\n", recv_buf.len);
//     // printf("%x %x %x %x %x %x\r\n", recv_buf.buf[0], recv_buf.buf[1], recv_buf.buf[2], recv_buf.buf[3], recv_buf.buf[4], recv_buf.buf[5]);
//   }
//   /* USER CODE END fpga_communication_entry */
// }

/* USER CODE BEGIN Header_ethercat_slave_entry */
/**
* @brief Function implementing the lan9252_irq_thread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ethercat_slave_entry */
// void ethercat_slave_entry(void *argument)
// {
//   /* USER CODE BEGIN ethercat_slave_entry */
//   /* Infinite loop */
//   int32_t ret = 0;
//   osDelay(100); /* wait ethercat init complete */

//   for(;;)
//   {
//     ret = ethercat_slave_wait_event();
//     if (ret < 0)
//     {
//         printf("ethercat wait err:%d\r\n", ret);
//     }
//     // osDelay(100);
//   }
//   /* USER CODE END ethercat_slave_entry */
// }

/* USER CODE BEGIN Header_data_process_entry */
/**
* @brief Function implementing the recv_data_process_entry thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_data_process_entry */
// void data_process_entry(void *argument)
// {
//   /* USER CODE BEGIN data_process_entry */
//   /* Infinite loop */
//   osStatus_t stat = 0;
//   uint32_t event_flag = 0;  
//   uint8_t recv_from_fpga_buf[RECV_BUF_LEN];
//   TCP_DATA_t tcp_info = {0};

//   non_realtime_data_process_init();

//   for(;;)
//   {

//     event_flag = osEventFlagsWait(data_process_eventHandle, DATA_PROCESS_FPGA_EVENT | DATA_PROCESS_LAN_EVENT | DATA_PROCESS_TCP_EVENT, osFlagsWaitAny, osWaitForever);
//     if (event_flag & DATA_PROCESS_LAN_EVENT)
//     {
//         // printf("recv DATA_PROCESS_LAN_EVENT\n");

//         ethercat_recv_data_update();
//     }

//     if (event_flag & DATA_PROCESS_FPGA_EVENT)
//     {
//         stat = recv_from_fpga_data_get(recv_from_fpga_buf);
//         if (stat == osOK)
//         {
//             non_realtime_fpga_data_process(recv_from_fpga_buf);

//             printf("buf: %x %x %x %x\r\n", recv_from_fpga_buf[0], recv_from_fpga_buf[1], recv_from_fpga_buf[2], recv_from_fpga_buf[3]);
//         }
//         else
//         {
//             printf("no msg in spi2 rx queue:%d\r\n", stat);
//         }
//     }

//     if (event_flag & DATA_PROCESS_TCP_EVENT)
//     {
//         stat = tcp_client_data_recv_get(&tcp_info);
//         if (stat == osOK)
//         {
//             ntrRecvParamAndPlan(&tcp_info);

//             sendFeedback();

//             // printf("tcp_info len:%d\r\n", tcp_info.Len);
//             // printf("tcp_info %x %x %x %x\r\n", tcp_info.gDATABUF[0], tcp_info.gDATABUF[1], tcp_info.gDATABUF[2], tcp_info.gDATABUF[3]);
//         }
//         else
//         {
//             printf("no msg in tcp rx queue:%d\r\n", stat);
//         }
//     }
    
//   }
//   /* USER CODE END data_process_entry */
// }

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

