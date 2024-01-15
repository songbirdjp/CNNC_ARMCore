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
	1.3			Re-arranged the function. Moved the other functions to PIC32SPIDriver.C
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

#include "9252_HW.h"
#include "SPIDriver.h"
#include "lan9252_port.h"

/*******************************************************************************
  Function:
	UINT32 SPIReadDWord (UINT16 Address)
  Summary:
    This function reads the LAN9252 CSR registers.        
  
*****************************************************************************/
UINT32 SPIReadDWord (UINT16 Address)
{
    UINT32 value;

#ifndef USING_SQI_CMD
    device_lan9252_data_read(Address, &value, sizeof(value));
#else
    device_lan9252_sqi_data_read(Address, &value, sizeof(value));
#endif
    return value;
}

/*******************************************************************************
  Function:
	void SPISendAddr (UINT16 Address)
  Summary:
    This function write address to SPI data bus.        
  
*****************************************************************************/
void SPISendAddr (UINT16 Address)
{
    UINT16_VAL wAddr;

    wAddr.Val  = Address;
    //Write Address
    SPIWriteByte(wAddr.byte.HB);
    SPIWriteByte(wAddr.byte.LB);
}

/*******************************************************************************
  Function:
	UINT32 SPIReadBurstMode ()
  Summary:
    This function read 4 bytes continuosly.        
  
*****************************************************************************/
UINT32 SPIReadBurstMode ()
{
    UINT32_VAL dwResult;
    //Read Bytes
    dwResult.byte.LB = SPIReadByte();
    dwResult.byte.HB = SPIReadByte();
    dwResult.byte.UB = SPIReadByte();
    dwResult.byte.MB = SPIReadByte();
    
    return dwResult.Val;
}

/*******************************************************************************
  Function:
	void SPIWriteBurstMode (UINT32 Val)
  Summary:
    This function writes 4 bytes continuosly.        
  
*****************************************************************************/
void SPIWriteBurstMode (UINT32 Val)
{
    UINT32_VAL dwData;
    dwData.Val = Val;
    
    //Write Bytes
    SPIWriteByte(dwData.byte.LB);
    SPIWriteByte(dwData.byte.HB);
    SPIWriteByte(dwData.byte.UB);
    SPIWriteByte(dwData.byte.MB);
}

#define ADDRESS_AUTO_INCREMENT 0x40
/*******************************************************************************
  Function:
	void SPIWriteBytes(UINT16 Address, UINT8 *Val, UINT8 nLenght)
  Summary:
    This function writes the LAN9252 CSR registers.        
  
*****************************************************************************/
void SPIWriteBytes(UINT16 Address, UINT8 *Val, UINT8 nLenght)
{
    UINT8 *dwData;
    UINT16_VAL wAddr;

    wAddr.Val  = Address;
    dwData = Val;
    //Assert CS line
    CSLOW();
    //Write Command
    SPIWriteByte(CMD_SERIAL_WRITE);
    //Write Address
    SPIWriteByte(wAddr.byte.HB|ADDRESS_AUTO_INCREMENT);
    SPIWriteByte(wAddr.byte.LB);
    //Write Bytes
    while(nLenght--)
    {
     SPIWriteByte(*(dwData++));
    }
        
    //De-Assert CS line
    CSHIGH();
}

/*******************************************************************************
  Function:
	void SPIWriteDWord (UINT16 Address, UINT32 Val)
  Summary:
    This function writes the LAN9252 CSR registers.        
  
*****************************************************************************/
void SPIWriteDWord (UINT16 Address, UINT32 Val)
{
#ifndef USING_SQI_CMD
    device_lan9252_data_write(Address, &Val, sizeof(Val));
#else
    device_lan9252_sqi_data_write(Address, &Val, sizeof(Val));
#endif
}

/*******************************************************************************
  Function:
   void SPIReadRegUsingCSR(UINT8 *ReadBuffer, UINT16 Address, UINT8 Count)
  Summary:
    This function reads the EtherCAT core registers using LAN9252 CSR registers.        
  
*****************************************************************************/
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

    SPIWriteDWord (ESC_CSR_CMD_REG, param32_1.Val);

    do
    {
        param32_1.Val = SPIReadDWord (ESC_CSR_CMD_REG);
		
    }while(param32_1.v[3] & ESC_CSR_BUSY);

    param32_1.Val = SPIReadDWord (ESC_CSR_DATA_REG);

    
    for(i=0;i<Count;i++)
         ReadBuffer[i] = param32_1.v[i];

    return;
}

/*******************************************************************************
  Function:
   void SPIWriteRegUsingCSR( UINT8 *WriteBuffer, UINT16 Address, UINT8 Count)
  Summary:
    This function writes the EtherCAT core registers using LAN9252 CSR registers.        
  
*****************************************************************************/
void SPIWriteRegUsingCSR( UINT8 *WriteBuffer, UINT16 Address, UINT8 Count)
{
    UINT32_VAL param32_1 = {0};
    UINT8 i = 0;
    UINT16_VAL wAddr;

    for(i=0;i<Count;i++)
         param32_1.v[i] = WriteBuffer[i];

    SPIWriteDWord (ESC_CSR_DATA_REG, param32_1.Val);


    wAddr.Val = Address;

    param32_1.v[0] = wAddr.byte.LB;
    param32_1.v[1] = wAddr.byte.HB;
    param32_1.v[2] = Count;
    param32_1.v[3] = ESC_WRITE_BYTE;

    SPIWriteDWord (ESC_CSR_CMD_REG, param32_1.Val);
    do
    {
        param32_1.Val = SPIReadDWord (ESC_CSR_CMD_REG);

    }while(param32_1.v[3] & ESC_CSR_BUSY);

    return;
}

/*******************************************************************************
  Function:
   void SPIReadPDRamRegister(UINT8 *ReadBuffer, UINT16 Address, UINT16 Count)
  Summary:
    This function reads the PDRAM using LAN9252 FIFO.        
  
*****************************************************************************/
void SPIReadPDRamRegister(UINT8 *ReadBuffer, UINT16 Address, UINT16 Count)
{
    UINT32_VAL param32_1 = {0};
    UINT8 i = 0,nlength, nBytePosition;
    UINT8 nReadSpaceAvblCount;
    UINT16 RefAddr = Address;

	/*Reset/Abort any previous commands.*/
    param32_1.Val = PRAM_RW_ABORT_MASK;                                                 

    SPIWriteDWord (PRAM_READ_CMD_REG, param32_1.Val);

    /*The host should not modify this field unless the PRAM Read Busy
    (PRAM_READ_BUSY) bit is a 0.*/
	do
    {
        param32_1.Val = SPIReadDWord (PRAM_READ_CMD_REG);

    }while((param32_1.v[3] & PRAM_RW_BUSY_8B));
    
    /*Write Address and Length Register (PRAM_READ_ADDR_LEN) with the
    starting UINT8 address and length) and Set PRAM Read Busy (PRAM_READ_BUSY) bit(-EtherCAT Process RAM Read Command Register)
    to start read operatrion*/
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
        param32_1.Val = SPIReadDWord (PRAM_READ_CMD_REG);

    }while(!(param32_1.v[0] & IS_PRAM_SPACE_AVBL_MASK));

    nReadSpaceAvblCount = param32_1.v[1] & PRAM_SPACE_AVBL_COUNT_MASK;

    /*Fifo registers are aliased address. In indexed it will read indexed data reg 0x04, but it will point to reg 0
     In other modes read 0x04 FIFO register since all registers are aliased*/

    /*get the UINT8 lenth for first read*/
    //Auto increment is supported in SPIO
//    param32_1.Val = SPIReadDWord(PRAM_READ_FIFO_REG);
//    nReadSpaceAvblCount--;
//    nBytePosition = (Address & 0x03);
//    nlength = (4 - nBytePosition) > Count ? Count : (4 - nBytePosition);
//    memcpy(ReadBuffer + i, &param32_1.v[nBytePosition], nlength);
//    Count -= nlength;
//    i += nlength;
    if (Count > 0)
    {
        // uint8_t tempBuff[Count];
        // CSLOW();
        // qspi_readBurstMode(PRAM_READ_FIFO_REG,tempBuff,Count);
#ifndef USING_SQI_CMD
        device_lan9252_data_read(PRAM_READ_FIFO_REG, ReadBuffer, Count);
#else
        device_lan9252_sqi_data_read(PRAM_READ_FIFO_REG, ReadBuffer, Count);
#endif
        // uint8_t buf[10] = {0};
        // device_lan9252_read_write(PRAM_READ_FIFO_REG, ReadBuffer, Count);
        // memcpy(ReadBuffer ,tempBuff,Count);
        nReadSpaceAvblCount = nReadSpaceAvblCount - Count / 4;//不一定用�?
        // CSHIGH();
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
        
/*******************************************************************************
  Function:
   void SPIWritePDRamRegister(UINT8 *WriteBuffer, UINT16 Address, UINT16 Count)
  Summary:
    This function writes the PDRAM using LAN9252 FIFO.        
  
*****************************************************************************/
void SPIWritePDRamRegister(UINT8 *WriteBuffer, UINT16 Address, UINT16 Count)
{
    UINT32_VAL param32_1 = {0};
    UINT8 i = 0,nlength, nBytePosition,nWrtSpcAvlCount;

    /*Reset or Abort any previous commands.*/
    param32_1.Val = PRAM_RW_ABORT_MASK;                                                

    SPIWriteDWord (PRAM_WRITE_CMD_REG, param32_1.Val);

    /*Make sure there is no previous write is pending
    (PRAM Write Busy) bit is a 0 */
    do
    {
        param32_1.Val = SPIReadDWord (PRAM_WRITE_CMD_REG);

    }while((param32_1.v[3] & PRAM_RW_BUSY_8B));

    /*Write Address and Length Register (ECAT_PRAM_WR_ADDR_LEN) with the
    starting UINT8 address and length) and write to the EtherCAT Process RAM Write Command Register (ECAT_PRAM_WR_CMD) with the  PRAM Write Busy
    (PRAM_WRITE_BUSY) bit set*/
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
       param32_1.Val = SPIReadDWord (PRAM_WRITE_CMD_REG);

    }while(!(param32_1.v[0] & IS_PRAM_SPACE_AVBL_MASK));

    /*Check write data available count*/
    nWrtSpcAvlCount = param32_1.v[1] & PRAM_SPACE_AVBL_COUNT_MASK;

    /*Write data to Write FIFO) */ 
    /*get the byte lenth for first read*/
//    nBytePosition = (Address & 0x03);
//
//    nlength = (4 - nBytePosition) > Count ? Count : (4 - nBytePosition);
//
//    param32_1.Val = 0;
//    memcpy(&param32_1.v[nBytePosition], WriteBuffer + i, nlength);
//
//    SPIWriteDWord(PRAM_WRITE_FIFO_REG, param32_1.Val);

//    nWrtSpcAvlCount--;
//    Count -= nlength;
//    i += nlength;
    // memcpy(testBuff, WriteBuffer , Count);
    //Auto increment mode
    // CSLOW();

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
    // memcpy(tempBuff, WriteBuffer, Count);//change count from nlength
//    nWrtSpcAvlCount = nWrtSpcAvlCount - Count;
    // qspi_writeBurstMode(PRAM_WRITE_FIFO_REG,tempBuff,Count);
    // CSHIGH();
    
#ifndef USING_SQI_CMD
    device_lan9252_data_write(PRAM_WRITE_FIFO_REG, WriteBuffer, Count);
#else
    device_lan9252_sqi_data_write(PRAM_WRITE_FIFO_REG, WriteBuffer, Count);
#endif
    return;
}

/*******************************************************************************
  Function:
   void PDIReadReg(UINT8 *ReadBuffer, UINT16 Address, UINT16 Count)
  Summary:
    This function reads the ESC registers using LAN9252 CSR or FIFO.         
  
*****************************************************************************/
void PDIReadReg(UINT8 *ReadBuffer, UINT16 Address, UINT16 Count)
{
    if (Address >= MIN_PD_READ_ADDRESS)
    {
#ifdef USING_OSPI_DMA_MODE
        uint8_t num = 0;
        uint8_t i = 0;

        while (Count > 0)
        {
            if (Count > 64)
            {
                num = 64;
            }
            else
            {
                num = Count;
            }

            SPIReadPDRamRegister(ReadBuffer + 64 * i, Address + 64 * i, num);

            Count -= num;
            i++;
        }
#else
        SPIReadPDRamRegister(ReadBuffer, Address,Count);
#endif
    }
    else
    {
         SPIReadRegUsingCSR(ReadBuffer, Address,Count);
    }
}

/*******************************************************************************
  Function:
   void PDIWriteReg( UINT8 *WriteBuffer, UINT16 Address, UINT16 Count)
  Summary:
    This function writes the ESC registers using LAN9252 CSR or FIFO.        
  
*****************************************************************************/
void PDIWriteReg( UINT8 *WriteBuffer, UINT16 Address, UINT16 Count)
{
   
   if (Address >= MIN_PD_WRITE_ADDRESS)
   {
#ifdef USING_OSPI_DMA_MODE
        uint8_t num = 0;
        uint8_t i = 0;

        while (Count > 0)
        {
            if (Count > 64) 
            {
                num = 64;
            }
            else
            {
                num = Count;
            }

            SPIWritePDRamRegister(WriteBuffer + 64 * i, Address + 64 * i, num);

            Count -= num;
            i++;
        }
#else
        SPIWritePDRamRegister(WriteBuffer, Address,Count);
#endif
   }
   else
   {
		SPIWriteRegUsingCSR(WriteBuffer, Address,Count);
   }
    
}

/*******************************************************************************
  Function:
	UINT32 PDIReadLAN9252DirectReg( UINT16 Address)
  Summary:
    This function reads the LAN9252 CSR registers(Not ESC registers).        
  
*****************************************************************************/
UINT32 PDIReadLAN9252DirectReg( UINT16 Address)
{   
    UINT32 data;
    data = SPIReadDWord (Address);
    return data;
}

/*******************************************************************************
  Function:
	void PDIWriteLAN9252DirectReg( UINT32 Val, UINT16 Address)
  Summary:
    This function writes the LAN9252 CSR registers(Not ESC registers).        
  
*****************************************************************************/
void PDIWriteLAN9252DirectReg( UINT32 Val, UINT16 Address)
{
    SPIWriteDWord (Address, Val);
}

/*******************************************************************************
  Function:
	void PDI_Init()
  Summary:
    This function initialize the PDI(SPI).        
  
*****************************************************************************/
void PDI_Init()
{
    SPIOpen();  
}

#ifdef USING_SQI_CMD
void sqi_enable(void)
{
    device_lan9252_sqi_mode_set(0x38);
}

void sqi_disable(void)
{
    device_lan9252_sqi_mode_set(0xFF);
}
#endif