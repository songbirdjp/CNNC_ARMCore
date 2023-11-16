/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    octospi.c
  * @brief   This file provides code for the configuration
  *          of the OCTOSPI instances.
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
#include "octospi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

OSPI_HandleTypeDef hospi1;

/* OCTOSPI1 init function */
void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  OSPIM_CfgTypeDef sOspiManagerCfg = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 10;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
  hospi1.Init.DeviceSize = 19;
  hospi1.Init.ChipSelectHighTime = 5;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.ClockPrescaler = 1;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  hospi1.Init.MaxTran = 0;
  hospi1.Init.Refresh = 0;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  sOspiManagerCfg.ClkPort = 1;
  sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  /* USER CODE END OCTOSPI1_Init 2 */

}

void HAL_OSPI_MspInit(OSPI_HandleTypeDef* ospiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(ospiHandle->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspInit 0 */

  /* USER CODE END OCTOSPI1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInitStruct.PLL2.PLL2M = 5;
    PeriphClkInitStruct.PLL2.PLL2N = 64;
    PeriphClkInitStruct.PLL2.PLL2P = 5;
    PeriphClkInitStruct.PLL2.PLL2Q = 5;
    PeriphClkInitStruct.PLL2.PLL2R = 4;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* OCTOSPI1 clock enable */
    __HAL_RCC_OCTOSPIM_CLK_ENABLE();
    __HAL_RCC_OSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOF_CLK_ENABLE();
    /**OCTOSPI1 GPIO Configuration
    PF6     ------> OCTOSPIM_P1_IO3
    PF7     ------> OCTOSPIM_P1_IO2
    PF8     ------> OCTOSPIM_P1_IO0
    PF9     ------> OCTOSPIM_P1_IO1
    PF10     ------> OCTOSPIM_P1_CLK
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* USER CODE BEGIN OCTOSPI1_MspInit 1 */

  /* USER CODE END OCTOSPI1_MspInit 1 */
  }
}

void HAL_OSPI_MspDeInit(OSPI_HandleTypeDef* ospiHandle)
{

  if(ospiHandle->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspDeInit 0 */

  /* USER CODE END OCTOSPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_OCTOSPIM_CLK_DISABLE();
    __HAL_RCC_OSPI1_CLK_DISABLE();

    /**OCTOSPI1 GPIO Configuration
    PF6     ------> OCTOSPIM_P1_IO3
    PF7     ------> OCTOSPIM_P1_IO2
    PF8     ------> OCTOSPIM_P1_IO0
    PF9     ------> OCTOSPIM_P1_IO1
    PF10     ------> OCTOSPIM_P1_CLK
    */
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10);

  /* USER CODE BEGIN OCTOSPI1_MspDeInit 1 */

  /* USER CODE END OCTOSPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
uint32_t lan9252_read_32(uint32_t address)
{
    OSPI_RegularCmdTypeDef sCommand = {0};
    HAL_StatusTypeDef status;
//   uint8_t data[4];
    uint8_t result[4];
    uint32_t readData = 0;
//    printf("read start .............. \n");
//    HAL_Delay(2);
//
//   data[0] = ESC_CMD_FAST_READ;
//   data[1] = ((address >> 8) & 0xFF);
//   data[2] = (address & 0xFF);
//   data[3] = ESC_CMD_FAST_READ_DUMMY;
//
//    HAL_OSPI_Transmit(hospi_lan9252, data, 1);
//
//    HAL_OSPI_Receive(hospi_lan9252, result, 1)
//
//   return ((result[3] << 24) |
//           (result[2] << 16) |
//           (result[1] << 8) |
//            result[0]);



    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
//    sCommand.FlashId = HAL_OSPI_FLASH_ID_1;
    sCommand.Instruction = 0X6B;
//    sCommand.Instruction = 0X03;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;


    sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = HAL_OSPI_ADDRESS_16_BITS;
    sCommand.Address = address;

    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DataMode = HAL_OSPI_DATA_4_LINES;
//    sCommand.DataMode = HAL_OSPI_DATA_1_LINE;
    sCommand.NbData = 4;

    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles = 8;
//    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;


    if (HAL_OSPI_Command(&hospi1, &sCommand, 5000) == HAL_OK)
    {

        if (HAL_OSPI_Receive(&hospi1, result, 5000) == HAL_OK)
        {
//            printf("HAL_OSPI_Receive done \n"); //test by yh
        }
        else
        {

//            printf("HAL_OSPI_Receive error \n"); //test by yh
            Error_Handler();
        }


    }
    else
    {

//        printf("HAL_OSPI_ReceiveCMD err \n"); //test by yh
        Error_Handler();
    }

    return ((result[3] << 24) |
            (result[2] << 16) |
            (result[1] << 8) |
            result[0]);


//        uint8_t _idSingle[4] = {0};
//        uint32_t _id = 0x11111111;
//        OSPI_RegularCmdTypeDef _cmdLan9252ReadID = {0};
//        _cmdLan9252ReadID.OperationType =HAL_OSPI_OPTYPE_COMMON_CFG;
//        _cmdLan9252ReadID.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;    /* 1�߷�ʽ����ָ???????????????????? */
//        _cmdLan9252ReadID.Instruction = 0x6B;
//        _cmdLan9252ReadID.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
//        _cmdLan9252ReadID.AddressSize = HAL_OSPI_ADDRESS_16_BITS;
//        _cmdLan9252ReadID.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
//        _cmdLan9252ReadID.AddressDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
//        _cmdLan9252ReadID.Address = 0x64;
//        _cmdLan9252ReadID.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;  /* �޽�����???????????????????? */
//        _cmdLan9252ReadID.InstructionDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
//        _cmdLan9252ReadID.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;     /* ÿ�δ��䶼��ָ�� */
//        _cmdLan9252ReadID.DataMode = HAL_OSPI_DATA_4_LINES;
//        _cmdLan9252ReadID.DummyCycles = 8;                 /* ����????????????????????  */
//        _cmdLan9252ReadID.NbData = 0x04;
//        _cmdLan9252ReadID.DQSMode = HAL_OSPI_DQS_DISABLE;
//        _cmdLan9252ReadID.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
//        //printf("1 SDRAM Test By Songbird\r\n");
//        HAL_StatusTypeDef _temp1 = HAL_OSPI_Command(&hospi1, &_cmdLan9252ReadID, 5000);
//        //printf("2 SDRAM Test By Songbird\r\n");
//        HAL_StatusTypeDef _temp2 = HAL_OSPI_Receive(&hospi1, _idSingle, 5000);
//        //printf("3 SDRAM Test By Songbird\r\n");
//        if (_temp1 != HAL_OK)
//        {
//            printf("send cmd fail1\r\n");
//            //return HAL_ERROR;
//        }
//        if (_temp2 != HAL_OK)
//        {
//            printf("send cmd fail2\r\n");
//            // return HAL_ERROR;
//        }
//
//        _id = (_idSingle[0] << 24) | (_idSingle[1] << 16) | (_idSingle[2] << 8) | (_idSingle[3]);
//        printf("send cmd successcjh id = %x\r\n", _id);
//        return _id;



}

void lan9252_write_32(uint16_t address, uint32_t val)
{
    OSPI_RegularCmdTypeDef sCommand = {0};//��������Ϊʲô��仰��������͵��·���ʧ�ܣ�
    uint8_t data[4];

//    printf("v  : %0x \n",val);
//    printf("write start .............. \n");
//    HAL_Delay(2);
//
//    data[0] = ESC_CMD_SERIAL_WRITE;
//    data[1] = ((address >> 8) & 0xFF);
//    data[2] = (address & 0xFF);
//    data[3] = (val & 0xFF);
//    data[4] = ((val >> 8) & 0xFF);
//    data[5] = ((val >> 16) & 0xFF);
//    data[6] = ((val >> 24) & 0xFF);

    data[0] = (val & 0xFF);
    data[1] = ((val >> 8) & 0xFF);
    data[2] = ((val >> 16) & 0xFF);
    data[3] = ((val >> 24) & 0xFF);
//
//    HAL_OSPI_Transmit(hospi_lan9252, data, 1);
//    HAL_GPIO_WritePin(OCTOSPIM_P1_NSS1_GPIO_Port, OCTOSPIM_P1_NSS1_Pin, GPIO_PIN_RESET);

    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction = 0x62;
//    sCommand.Instruction = 0x02;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;

    sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = HAL_OSPI_ADDRESS_16_BITS;
    sCommand.Address = address;

    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = HAL_OSPI_DATA_4_LINES;
//    sCommand.DataMode = HAL_OSPI_DATA_2_LINES;
    sCommand.NbData = 4;
    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;

    if (HAL_OSPI_Command(&hospi1, &sCommand, 5000) == HAL_OK)
    {
//        printf("HAL_OSPI_cmd : value is  : %0x,%0x,%0x,%0x \n",data[0],data[1],data[2],data[3]); //test by yh

        if (HAL_OSPI_Transmit(&hospi1, data, 5000) == HAL_OK)
        {
//            printf("HAL_OSPI_Transmit : value is  : %0x,%0x,%0x,%0x \n",data[0],data[1],data[2],data[3]); //test by yh
        }
        else
        {
//            printf("HAL_OSPI_Transmit err \n"); //test by yh
            Error_Handler();
        }

    }
    else
    {
        printf("HAL_OSPI_transmitCMD err \n"); //test by yh
        Error_Handler();
    }


}

void OSPIWrite(uint8_t _data)
{
    OSPI_RegularCmdTypeDef sCommand = {0};
    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_NONE;
    sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = HAL_OSPI_DATA_1_LINE;
    sCommand.NbData = 1;
    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    if (HAL_OSPI_Command(&hospi1, &sCommand, 5000) == HAL_OK)
    {
        printf("HAL_OSPI_Transmit ok\r\n");
        if (HAL_OSPI_Transmit(&hospi1, &_data, 5000) == HAL_OK)
        {
            printf("HAL_OSPI_Transmit ok\n");
        }
        else
        {
            printf("HAL_OSPI_Transmit err \r\n");
//            Error_Handler();
        }
    }
    else
    {
        printf("HAL_OSPI_Transmitcmd err \r\n");
    }
    //HAL_OSPI_Transmit(&hospi1, &_data, 5000);
}

uint8_t OSPIRead(void)
{
    uint8_t _data;
    OSPI_RegularCmdTypeDef sCommand = {0};
    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_NONE;
    sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = HAL_OSPI_DATA_1_LINE;
    sCommand.NbData = 1;
    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    if (HAL_OSPI_Command(&hospi1, &sCommand, 5000))
    {
        printf("HAL_OSPI_Transmit ok\r\n");
        if (HAL_OSPI_Receive(&hospi1, &_data, 5000) == HAL_OK)
        {
            printf("HAL_OSPI_Receive ok\n");
            return _data;
        }
        else
        {
            printf("HAL_OSPI_Receive err\r\n");
            return 0;
        }
    }
    else
    {
        printf("HAL_OSPI_Receivecmd err \r\n");
    }
}

//uint32_t qspi_readBurstMode(uint16_t address,uint8_t *buff,uint32_t num)
//{
//    uint8_t data = 0xff;
//    OSPI_RegularCmdTypeDef sCommand = {0};
//
//    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
////    sCommand.FlashId = HAL_OSPI_FLASH_ID_1;
//    sCommand.Instruction = 0X6B;
////    sCommand.Instruction = 0X03;
//    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
//    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
//
//
//    sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
//    sCommand.AddressSize = HAL_OSPI_ADDRESS_16_BITS;
//    sCommand.Address = address;
//
//    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
//    sCommand.DataMode = HAL_OSPI_DATA_4_LINES;
////    sCommand.DataMode = HAL_OSPI_DATA_1_LINE;
//    sCommand.NbData = num;
//
//    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
//
//    sCommand.DummyCycles = 8;
////    sCommand.DummyCycles = 0;
//    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
//    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
//    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
//
//    if(HAL_OSPI_Command(&hospi1,&sCommand,5000)==HAL_OK)
//    {
//        printf("qspi_readByte_ReceiveCMD done \n"); //test by yh
//
//        if(HAL_OSPI_Receive(&hospi1, buff, 5000)==HAL_OK)
//        {
//            printf("qspi_Receive done \n"); //test by yh
//        }
//        else
//        {
//
//            printf("qspi_readByte_Receive error \n"); //test by yh
//            Error_Handler();
//        }
//
//
//    }
//    else
//    {
//
//        printf("qspi_readByte_ReceiveCMD err \n"); //test by yh
//        Error_Handler();
//    }
//
//    return 0;
//
//}

/* USER CODE END 1 */
