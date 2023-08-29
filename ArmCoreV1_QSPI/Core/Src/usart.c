/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include <string.h>
#include "queue.h"
#include "main.h"
uint8_t RxTemp[1] = {'\0'};
uint8_t RxBuffer[2048];
uint16_t RxCounter = 0;

TCP_DATA_t  RecvByUART;

extern osMessageQId CmdQueueHandle;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16910CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
// cjh add above
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, 0xffff);
    return ch;
}

void start_uart_receive(void)
{
    HAL_UART_Receive_IT(&huart1, (uint8_t *) RxTemp, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        //printf("%x\r\n",RxTemp[0]);
        if(0x0d != RxTemp[0])
        {
            RxBuffer[RxCounter] = RxTemp[0];
            RxCounter++;
        }
        else
        {
            CmdDecode();
            memset(RxBuffer, 0x00, sizeof(RxBuffer));
            RxCounter = 0;
        }

        HAL_UART_Receive_IT(&huart1, (uint8_t *) RxTemp, 1);
    }
}

void CmdDecode(void)
{
    CmdMsg.Cmd = 0;
    CmdMsg.Parameter = 0;
    /**********************************Help mode*******************************************/
    if((4 == RxCounter)
       && ('h' == (RxBuffer[0]) || 'H' == (RxBuffer[0]))
       && ('e' == (RxBuffer[1]) || 'E' == (RxBuffer[1]))
       && ('l' == (RxBuffer[2]) || 'L' == (RxBuffer[2]))
       && ('p' == (RxBuffer[3]) || 'P' == (RxBuffer[3])))//help mode
    {
        CmdMsg.Cmd = 1;
        CmdMsg.Parameter = 1;
        printf("\r\n>>> cmd run\r\n");
        //xQueueSendFromISR(CmdQueueHandle, &CmdMsg, 0);
    }
/****************Clear all the data from SDRAM & Flash****************************************/
    if((3 == RxCounter)
       && ('c' == (RxBuffer[0]) || 'C' == (RxBuffer[0]))
       && ('l' == (RxBuffer[1]) || 'L' == (RxBuffer[1]))
       && ('s' == (RxBuffer[2]) || 'S' == (RxBuffer[2])))//Clear all the data from SDRAM & Flash
    {
        CmdMsg.Cmd = 2;
        CmdMsg.Parameter = 1;
        printf("\f<<<Motor control>>>\r\n");
    }
/************************Send data for EtherNet or EtherCAT*******************************/
    if((8 == RxCounter)
       && ('s' == (RxBuffer[0]) || 'S' == (RxBuffer[0]))
       && ('e' == (RxBuffer[1]) || 'E' == (RxBuffer[1]))
       && ('n' == (RxBuffer[2]) || 'N' == (RxBuffer[2]))
       && ('d' == (RxBuffer[3]) || 'D' == (RxBuffer[3])))//Send data
    {
        CmdMsg.Cmd = 3;
        if((' ' == (RxBuffer[4]))
           && ('n' == (RxBuffer[5]) || 'N' == (RxBuffer[5]))
           && ('e' == (RxBuffer[6]) || 'E' == (RxBuffer[6]))
           && ('t' == (RxBuffer[7]) || 'T' == (RxBuffer[7])))
        {
            CmdMsg.Parameter = 1;
            printf("net\r\n");
        }
        else if((' ' == (RxBuffer[4]))
                && ('c' == (RxBuffer[5]) || 'C' == (RxBuffer[5]))
                && ('a' == (RxBuffer[6]) || 'A' == (RxBuffer[6]))
                && ('t' == (RxBuffer[7]) || 'T' == (RxBuffer[7])))
        {
            CmdMsg.Parameter = 2;
            printf("cat\r\n");
        }
    }
/*********************************State Machine Control ***************************************/
    if(('s' == (RxBuffer[0]) || 'S' == (RxBuffer[0]))
       && ('t' == (RxBuffer[1]) || 'T' == (RxBuffer[1]))
       && ('a' == (RxBuffer[2]) || 'A' == (RxBuffer[2]))
       && ('t' == (RxBuffer[3]) || 'T' == (RxBuffer[3]))
       && ('e' == (RxBuffer[4]) || 'E' == (RxBuffer[4])))
    {
        CmdMsg.Cmd = 4;
        if((' ' == (RxBuffer[5]))
           && ('i' == (RxBuffer[6]) || 'I' == (RxBuffer[6]))
           && ('n' == (RxBuffer[7]) || 'N' == (RxBuffer[7]))
           && ('i' == (RxBuffer[8]) || 'I' == (RxBuffer[8]))
           && ('t' == (RxBuffer[9]) || 'T' == (RxBuffer[9])))
        {
            CmdMsg.Parameter = 1;
            printf("state  init\r\n");
        }
        else if((' ' == (RxBuffer[5]))
                && ('i' == (RxBuffer[6]) || 'I' == (RxBuffer[6]))
                && ('d' == (RxBuffer[7]) || 'D' == (RxBuffer[7]))
                && ('l' == (RxBuffer[8]) || 'L' == (RxBuffer[8]))
                && ('e' == (RxBuffer[9]) || 'E' == (RxBuffer[9])))
        {
            CmdMsg.Parameter = 2;
            printf("state  idle\r\n");
        }
        else if((' ' == (RxBuffer[5]))
                && ('p' == (RxBuffer[6]) || 'P' == (RxBuffer[6]))
                && ('r' == (RxBuffer[7]) || 'R' == (RxBuffer[7]))
                && ('e' == (RxBuffer[8]) || 'E' == (RxBuffer[8]))
                )
        {
            CmdMsg.Parameter = 3;
            printf("state  prepare\r\n");
        }
        else if((' ' == (RxBuffer[5]))
                && ('r' == (RxBuffer[6]) || 'R' == (RxBuffer[6]))
                && ('e' == (RxBuffer[7]) || 'E' == (RxBuffer[7]))
                && ('a' == (RxBuffer[8]) || 'A' == (RxBuffer[8]))
                && ('d' == (RxBuffer[9]) || 'D' == (RxBuffer[9]))
                && ('y' == (RxBuffer[10]) || 'Y' == (RxBuffer[10])))
        {
            CmdMsg.Parameter = 4;
            printf("state  ready\r\n");
        }
        else if((' ' == (RxBuffer[5]))
                && ('s' == (RxBuffer[6]) || 'S' == (RxBuffer[6]))
                && ('e' == (RxBuffer[7]) || 'E' == (RxBuffer[7]))
                && ('r' == (RxBuffer[8]) || 'R' == (RxBuffer[8]))
                && ('v' == (RxBuffer[9]) || 'V' == (RxBuffer[9]))
                && ('o' == (RxBuffer[10]) || 'O' == (RxBuffer[10])))
        {
            CmdMsg.Parameter = 5;
            printf("state  servo\r\n");
        }
        else if((' ' == (RxBuffer[5]))
                && ('m' == (RxBuffer[6]) || 'M' == (RxBuffer[6]))
                && ('a' == (RxBuffer[7]) || 'A' == (RxBuffer[7]))
                && ('n' == (RxBuffer[8]) || 'N' == (RxBuffer[8]))
                && ('u' == (RxBuffer[9]) || 'U' == (RxBuffer[9]))
                && ('a' == (RxBuffer[10]) || 'A' == (RxBuffer[10]))
                && ('l' == (RxBuffer[11]) || 'L' == (RxBuffer[11])))
        {
            CmdMsg.Parameter = 6;
            printf("state manual\r\n");
        }
        else if((' ' == (RxBuffer[5]))
                && ('f' == (RxBuffer[6]) || 'F' == (RxBuffer[6]))
                && ('a' == (RxBuffer[7]) || 'A' == (RxBuffer[7]))
                && ('u' == (RxBuffer[8]) || 'U' == (RxBuffer[8]))
                && ('l' == (RxBuffer[9]) || 'L' == (RxBuffer[9]))
                && ('t' == (RxBuffer[10]) || 'T' == (RxBuffer[10])))
        {
            CmdMsg.Parameter = 7;
            printf("state fault\r\n");
        }
        else if((' ' == (RxBuffer[5]))
                && ('d' == (RxBuffer[6]) || 'D' == (RxBuffer[6]))
                && ('o' == (RxBuffer[7]) || 'O' == (RxBuffer[7]))
                && ('w' == (RxBuffer[8]) || 'W' == (RxBuffer[8]))
                && ('n' == (RxBuffer[9]) || 'N' == (RxBuffer[9])))
        {
            CmdMsg.Parameter = 8;
            printf("state shutdown\r\n");
        }
    }
    xQueueSendFromISR(CmdQueueHandle, &CmdMsg, 0);

    if(RxCounter > 13)
    {
        RxCounter = RecvByUART.Len;
        memcpy(RecvByUART.gDATABUF,RxBuffer,RecvByUART.Len);
    }
}
/* USER CODE END 1 */
