/*******************************************************************************
 PIC32 SPI Interface Driver

  Company:
    Microchip Technology Inc.

  File Name:
    SPIDriver.c

  Summary:
    Contains the functional implementation of PIC32 SPI Interface Driver

  Description:
    This file contains the functional implementation of PIC32 SPI Interface Driver
	
  Change History:
    Version		Changes
	0.1			Initial version.
	0.2			-
	0.3			-	
	0.4 		-
*******************************************************************************/

/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#include "SPIDriver.h"
#include "retarget.h"
//#include "../Common/UserDataTypes.h"
// "9252_HW.h"

void Delay(UINT16 Count)
{
    do
    {
//        __NOP;

    } while (Count--);
}

//void SPIPut(UINT8 data)
//{
////    // Wait for free buffer
////    while(!SPI4STATbits.SPITBE);
////    SPI4BUF = data;
//
////    // Wait for data UINT8
////    while(!SPI4STATbits.SPIRBF);
//
//}
//void SPIWrite(UINT8 data)
//{
////    SPIPut(data);
////    SPIGet();
//		HAL_SPI_Transmit(&hspi1 , &data, 1,1000);
//}
//UINT8 SPIRead()
//{
//    UINT8 data;
////    SPIPut(0);
////    data = (UINT8)SPIGet();
//		HAL_SPI_Receive(&hspi5,&data,1,1000);
//    return (data);
//}

//void SPIOpen()
//{
//
//}

void SPIWriteByte(uint8_t _data)
{
    printf("SPIWriteByte");
    OSPIWrite(_data);
}

uint8_t SPIReadByte()
{
    printf("SPIReadByte");
    return OSPIRead();
}

UINT32 SPIReadDWord(UINT16 Address)
{
    UINT32_VAL dwResult;
    UINT16_VAL wAddr;
    uint32_t value;

    wAddr.Val = Address;
//    //Assert CS line
    CSLOW();
//    //Write Command
////    SPIWriteByte(CMD_FAST_READ);//changed by yh
//    SPIWriteByte(CMD_SERIAL_READ);//changed by yh
//    //Write Address
//    SPIWriteByte(wAddr.byte.HB);
//    SPIWriteByte(wAddr.byte.LB);
//
//    //Dummy Byte
////    SPIWriteByte(CMD_FAST_READ_DUMMY);
//    //Read Bytes
//    dwResult.byte.LB = SPIReadByte();
//    dwResult.byte.HB = SPIReadByte();
//    dwResult.byte.UB = SPIReadByte();
//    dwResult.byte.MB = SPIReadByte();
//    //De-Assert CS line

    value = lan9252_read_32(Address);
    CSHIGH();
    return value;
}

void SPISendAddr(UINT16 Address)
{
    UINT16_VAL wAddr;

    wAddr.Val = Address;
    //Write Address
    SPIWriteByte(wAddr.byte.HB);
    SPIWriteByte(wAddr.byte.LB);
}

UINT32 SPIReadBurstMode()
{
    UINT32_VAL dwResult;
    //Read Bytes
    dwResult.byte.LB = SPIReadByte();
    dwResult.byte.HB = SPIReadByte();
    dwResult.byte.UB = SPIReadByte();
    dwResult.byte.MB = SPIReadByte();

    return dwResult.Val;
}

void SPIWriteBurstMode(UINT32 Val)
{
    UINT32_VAL dwData;
    dwData.Val = Val;

    //Write Bytes
    SPIWriteByte(dwData.byte.LB);
    SPIWriteByte(dwData.byte.HB);
    SPIWriteByte(dwData.byte.UB);
    SPIWriteByte(dwData.byte.MB);
}

void SPIWriteDWord(UINT16 Address, UINT32 Val)
{
    UINT32_VAL dwData;
    UINT16_VAL wAddr;

    wAddr.Val = Address;
    dwData.Val = Val;
//    //Assert CS line
    CSLOW();
//    //Write Command
//    SPIWriteByte(CMD_SERIAL_WRITE);
//    //Write Address
//    SPIWriteByte(wAddr.byte.HB);
//    SPIWriteByte(wAddr.byte.LB);
//    //Write Bytes
//    SPIWriteByte(dwData.byte.LB);
//    SPIWriteByte(dwData.byte.HB);
//    SPIWriteByte(dwData.byte.UB);
//    SPIWriteByte(dwData.byte.MB);
//
//    //De-Assert CS line


    lan9252_write_32(Address, Val);
    CSHIGH();
}

void SPIReadRegUsingCSR(UINT8 *ReadBuffer, UINT16 Address, UINT8 Count)
{
    UINT32_VAL param32_1 = {0};
    UINT8 i = 0;
    UINT16_VAL wAddr;
    wAddr.Val = Address;

    param32_1.v[0] = wAddr.byte.LB;
    param32_1.v[1] = wAddr.byte.HB;
    param32_1.v[2] = Count;
    param32_1.v[3] = ESC_READ_BYTE;

    SPIWriteDWord(ESC_CSR_CMD_REG, param32_1.Val);
//    HAL_Delay(1);//���������С�ȴ�����
    do
    {
        param32_1.Val = SPIReadDWord(ESC_CSR_CMD_REG);

    } while (param32_1.v[3] & ESC_CSR_BUSY);

    param32_1.Val = SPIReadDWord(ESC_CSR_DATA_REG);


    for (i = 0; i < Count; i++)
        ReadBuffer[i] = param32_1.v[i];

    return;
}

void SPIWriteRegUsingCSR(UINT8 *WriteBuffer, UINT16 Address, UINT8 Count)
{
    UINT32_VAL param32_1 = {0};
    UINT8 i = 0;
    UINT16_VAL wAddr;

    for (i = 0; i < Count; i++)
        param32_1.v[i] = WriteBuffer[i];

    SPIWriteDWord(ESC_CSR_DATA_REG, param32_1.Val);


    wAddr.Val = Address;

    param32_1.v[0] = wAddr.byte.LB;
    param32_1.v[1] = wAddr.byte.HB;
    param32_1.v[2] = Count;
    param32_1.v[3] = ESC_WRITE_BYTE;

    SPIWriteDWord(0x304, param32_1.Val);
//        HAL_Delay(1);//���������С�ȴ�����
    do
    {
        param32_1.Val = SPIReadDWord(0x304);

    } while (param32_1.v[3] & ESC_CSR_BUSY);

    return;
}

void SPIReadPDRamRegister(UINT8 *ReadBuffer, UINT16 Address, UINT16 Count)
{
    UINT32_VAL param32_1 = {0};
    if (Count > 0)
    {
        uint8_t tempBuff[Count];
    }
    else
    {
        return;
    }
    UINT8 i = 0, nlength, nBytePosition;
    UINT8 nReadSpaceAvblCount;
//    UINT16 RefAddr = Address;


    /*Reset/Abort any previous commands.*/
    param32_1.Val = PRAM_RW_ABORT_MASK;

    SPIWriteDWord(PRAM_READ_CMD_REG, param32_1.Val);

    /*The host should not modify this field unless the PRAM Read Busy
    (PRAM_READ_BUSY) bit is a 0.*/
    do
    {
        param32_1.Val = SPIReadDWord(PRAM_READ_CMD_REG);

    } while ((param32_1.v[3] & PRAM_RW_BUSY_8B));

    /*Write address and length in the EtherCAT Process RAM Read Address and
     * Length Register (ECAT_PRAM_RD_ADDR_LEN)*/
    param32_1.w[0] = Address;
    param32_1.w[1] = Count;

    SPIWriteDWord(PRAM_READ_ADDR_LEN_REG, param32_1.Val);


    /*Set PRAM Read Busy (PRAM_READ_BUSY) bit(-EtherCAT Process RAM Read Command Register)
     *  to start read operatrion*/

    param32_1.Val = PRAM_RW_BUSY_32B; /*TODO:replace with #defines*/

    SPIWriteDWord(PRAM_READ_CMD_REG, param32_1.Val);

    /*Read PRAM Read Data Available (PRAM_READ_AVAIL) bit is set*/
    do
    {
        param32_1.Val = SPIReadDWord(PRAM_READ_CMD_REG);

    } while (!(param32_1.v[0] & IS_PRAM_SPACE_AVBL_MASK));

    nReadSpaceAvblCount = param32_1.v[1] & PRAM_SPACE_AVBL_COUNT_MASK;

    /*Fifo registers are aliased address. In indexed it will read indexed data reg 0x04, but it will point to reg 0
     In other modes read 0x04 FIFO register since all registers are aliased*/

    /*get the UINT8 lenth for first read*/
    //Auto increment is supported in SPIO
    param32_1.Val = SPIReadDWord(PRAM_READ_FIFO_REG);
    nReadSpaceAvblCount--;
    nBytePosition = (Address & 0x03);
    nlength = (4 - nBytePosition) > Count ? Count : (4 - nBytePosition);
    memcpy(ReadBuffer + i, &param32_1.v[nBytePosition], nlength);
//    Count -= nlength;
//    i += nlength;
    if (Count > 0)
    {
        uint8_t tempBuff[Count];
        CSLOW();
        qspi_readBurstMode(PRAM_READ_FIFO_REG,tempBuff,Count);
        memcpy((ReadBuffer+i) ,tempBuff,Count);
        nReadSpaceAvblCount = nReadSpaceAvblCount - Count / 4;//不一定用到
        CSHIGH();
        return;
    }
    else
    {
        return;
    }
    //Lets do it in auto increment mode
//    CSLOW();

    //Write Command
//    SPIWriteByte(CMD_FAST_READ);
//
//    SPISendAddr(PRAM_READ_FIFO_REG);
//
//    //Dummy Byte
//    SPIWriteByte(CMD_FAST_READ_DUMMY);
//
//    while (Count)
//    {
//        param32_1.Val = SPIReadBurstMode();
//
//        nlength = Count > 4 ? 4 : Count;
//        memcpy((ReadBuffer + i), &param32_1, nlength);
//
//        i += nlength;
//        Count -= nlength;
//        nReadSpaceAvblCount--;
//    }

//    CSHIGH();

    return;
}

void SPIWritePDRamRegister(UINT8 *WriteBuffer, UINT16 Address, UINT16 Count)
{
    uint8_t tempBuff[Count];
    UINT32_VAL param32_1 = {0};
    UINT8 i = 0, nlength, nBytePosition, nWrtSpcAvlCount;

    /*Reset or Abort any previous commands.*/
    param32_1.Val = PRAM_RW_ABORT_MASK;

    SPIWriteDWord(PRAM_WRITE_CMD_REG, param32_1.Val);

    /*Make sure there is no previous write is pending
    (PRAM Write Busy) bit is a 0 */
    do
    {
        param32_1.Val = SPIReadDWord(PRAM_WRITE_CMD_REG);

    } while ((param32_1.v[3] & PRAM_RW_BUSY_8B));

    /*Write Address and Length Register (ECAT_PRAM_WR_ADDR_LEN) with the
    starting UINT8 address and length)*/
    param32_1.w[0] = Address;
    param32_1.w[1] = Count;

    SPIWriteDWord(PRAM_WRITE_ADDR_LEN_REG, param32_1.Val);

    /*write to the EtherCAT Process RAM Write Command Register (ECAT_PRAM_WR_CMD) with the  PRAM Write Busy
    (PRAM_WRITE_BUSY) bit set*/

    param32_1.Val = PRAM_RW_BUSY_32B; /*TODO:replace with #defines*/

    SPIWriteDWord(PRAM_WRITE_CMD_REG, param32_1.Val);

    /*Read PRAM write Data Available (PRAM_READ_AVAIL) bit is set*/
    do
    {
        param32_1.Val = SPIReadDWord(PRAM_WRITE_CMD_REG);

    } while (!(param32_1.v[0] & IS_PRAM_SPACE_AVBL_MASK));

    /*Check write data available count*/
    nWrtSpcAvlCount = param32_1.v[1] & PRAM_SPACE_AVBL_COUNT_MASK;

    /*Write data to Write FIFO) */
    /*get the byte lenth for first read*/
    nBytePosition = (Address & 0x03);

    nlength = (4 - nBytePosition) > Count ? Count : (4 - nBytePosition);

    param32_1.Val = 0;
    memcpy(&param32_1.v[nBytePosition], WriteBuffer + i, nlength);

    SPIWriteDWord(PRAM_WRITE_FIFO_REG, param32_1.Val);

    nWrtSpcAvlCount--;
//    Count -= nlength;
//    i += nlength;

    //Auto increment mode
    CSLOW();

    //Write Command
//    SPIWriteByte(CMD_SERIAL_WRITE);//todo :replace with single command functions
//
//    SPISendAddr(PRAM_WRITE_FIFO_REG);//todo :replace with single command functions
//
//    while (Count)
//    {
//        nlength = Count > 4 ? 4 : Count;
//        param32_1.Val = 0;
//        memcpy(&param32_1, (WriteBuffer + i), nlength);
//
//        SPIWriteBurstMode(param32_1.Val);//todo :replace with single command functions
//        i += nlength;
//        Count -= nlength;
//        nWrtSpcAvlCount--;
//    }
    memcpy(tempBuff, (WriteBuffer+i), nlength);
    nWrtSpcAvlCount = nWrtSpcAvlCount - Count;
    qspi_writeBurstMode(PRAM_WRITE_FIFO_REG,tempBuff,Count);
    CSHIGH();
    return;
}

void SPIReadDRegister(UINT8 *ReadBuffer, UINT16 Address, UINT16 Count)
{
    if (Address >= 0x1000)
    {
//        printf(">>>>R1000\r\n");
//        HAL_Delay(1);
        SPIReadPDRamRegister(ReadBuffer, Address, Count);
       // printf("ReadBuffer = %x\r\n",ReadBuffer);
    }
    else
    {
        SPIReadRegUsingCSR(ReadBuffer, Address, Count);
    }
}

void SPIWriteRegister(UINT8 *WriteBuffer, UINT16 Address, UINT16 Count)
{

    if (Address >= 0x1000)
    {
//        printf(">>>>W1000\r\n");
//        HAL_Delay(1);
       SPIWritePDRamRegister(WriteBuffer, Address, Count);
       // printf("WriteBuffer = %x\r\n",WriteBuffer);
    }
    else
    {
        SPIWriteRegUsingCSR(WriteBuffer, Address, Count);
    }

}
uint32_t qspi_readBurstMode(uint16_t address,uint8_t *buff,uint32_t num)
{
    uint8_t data = 0xff;
    OSPI_RegularCmdTypeDef sCommand = {0};

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
    sCommand.NbData = num;

    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles = 8;
//    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;

    if(HAL_OSPI_Command(&hospi1,&sCommand,5000)==HAL_OK)
    {
//        printf("qspi_readByte_ReceiveCMD done \n"); //test by yh

        if(HAL_OSPI_Receive(&hospi1, buff, 5000)==HAL_OK)
        {
//            printf("qspi_Receive done \n"); //test by yh
        }
        else
        {
//            printf("qspi_readByte_Receive error \n"); //test by yh
            Error_Handler();
        }


    }
    else
    {
//        printf("qspi_readByte_ReceiveCMD err \n"); //test by yh
        Error_Handler();
    }
    return 0;

}
void qspi_writeBurstMode(uint16_t address,uint8_t *buff,uint32_t num)
{
    OSPI_RegularCmdTypeDef sCommand = {0};

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
    sCommand.NbData = num;
    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;

    if(HAL_OSPI_Command(&hospi1, &sCommand, 5000) == HAL_OK)
    {
//        printf("qspi_writeByete_transmitCMD \n"); //test by yh

        if(HAL_OSPI_Transmit(&hospi1, buff, 5000) == HAL_OK)
        {
//            printf("qspi_writeByete_transmitCMD \n"); //test by yh
        }
        else
        {
//            printf("qspi_writeByete_Transmit err \n"); //test by yh
            Error_Handler();
        }

    }
    else
    {
//        printf("qspi_writeByete_transmitCMD errCode = %0x  \n",hospi1.ErrorCode); //test by yh
        Error_Handler();
    }
}
