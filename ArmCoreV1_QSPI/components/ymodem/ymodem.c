/**
  ******************************************************************************
  * @file    STM32F4xx_IAP/src/ymodem.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    10-October-2011
  * @brief   This file provides all the software functions related to the ymodem 
  *          protocol.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/** @addtogroup STM32F4xx_IAP
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
#include "ymodem.h"
#include "string.h"
#include "console.h"
#include "flash_port.h"
#include "cmsis_os2.h"
#include "shell.h"
#include "init_call.h"
#include "stm32h7xx_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (256)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

#define NAK_TIMEOUT             (0x300)
#define MAX_ERRORS              (5)

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString        printf

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t tab_1024[PACKET_1K_SIZE] = {0};
uint8_t FileName[FILE_NAME_LENGTH] = {0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Int2Str(uint8_t* str, int32_t intnum)
{
  uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}

uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}

/**
  * @brief  Receive byte from sender
  * @param  c: Character
  * @param  timeout: Timeout
  * @retval 0: Byte received
  *        -1: Timeout
  */
static  int32_t Receive_Byte (uint8_t *c, uint32_t timeout)
{
  int8_t ret = 0;
  uint16_t len = 0;
  ret = ymodem_data_read(c, &len, timeout);
  if (ret != 0)
  {
    return -1;
  }
  return 0;
}

/**
  * @brief  Send a byte
  * @param  c: Character
  * @retval 0: Byte sent
  */
static uint32_t Send_Byte (uint8_t c)
{
    uint8_t data = c;

    int8_t ret = ymodem_data_write((uint8_t *)&data, sizeof(data), 100);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

/**
  * @brief  Receive a packet from sender
  * @param  data
  * @param  length
  * @param  timeout
  *     0: end of transmission
  *    -1: abort by sender
  *    >0: packet length
  * @retval 0: normally return
  *        -1: timeout or packet error
  *         1: abort by user
  */
static int32_t Receive_Packet (uint8_t *data, int32_t *length, uint32_t timeout)
{
  uint16_t packet_size;
  uint16_t len = 0;
  int8_t ret = 0;

  ret = ymodem_data_read(data, &len, timeout);
  if (ret != 0)
  {
      return -1;
  }

  *length = 0;

  switch (data[0])
  {
    case SOH:
      packet_size = PACKET_SIZE;
      break;
    case STX:
      packet_size = PACKET_1K_SIZE;
      break;
    case EOT:
      return 0;
    case CA:
      if (data[1] == CA)
      {
        *length = -1;
        return 0;
      }
      else
      {
        return -1;
      }
    case ABORT1:
    case ABORT2:
      return 1;
    default:
      return -1;
  }

  if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
  {
    return -1;
  }
  *length = packet_size;
  return 0;
}

/**
  * @brief  Receive a file using the ymodem protocol.
  * @param  buf: Address of the first byte.
  * @retval The size of the file.
  */
int32_t Ymodem_Receive (uint32_t offset)
{
  uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH], *file_ptr, *buf_ptr;
  int32_t i, packet_length, session_done, file_done, packets_received, errors, session_begin, size = 0;
  uint32_t flashdestination, ramsource;

  /* Initialize flashdestination variable */
  flashdestination = offset;

  for (session_done = 0, errors = 0, session_begin = 0; ;)
  {
    for (packets_received = 0, file_done = 0, buf_ptr = tab_1024; ;)
    {
      switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT))
      {
        case 0:
          errors = 0;
          switch (packet_length)
          {
            /* Abort by sender */
            case - 1:
              Send_Byte(ACK);
              return 0;
            /* End of transmission */
            case 0:
              Send_Byte(ACK);
              file_done = 1;
              break;
            /* Normal packet */
            default:
              if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))
              {
                Send_Byte(NAK);
              }
              else
              {
                if (packets_received == 0)
                {
                  /* Filename packet */
                  if (packet_data[PACKET_HEADER] != 0)
                  {
                    /* Filename packet has valid data */
                    for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
                    {
                      FileName[i++] = *file_ptr++;
                    }
                    FileName[i++] = '\0';
                    for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
                    {
                      file_size[i++] = *file_ptr++;
                    }
                    file_size[i++] = '\0';
                    Str2Int(file_size, &size);

                    /* Test the size of the image to be sent */
                    /* Image size is greater than Flash size */
                    if (offset + size > USER_FLASH_SIZE)
                    {
                      /* End session */
                      Send_Byte(CA);
                      Send_Byte(CA);
                      return -1;
                    }
                    /* erase user application area */
                    if (device_flash_erase(DEVICE_NAME_FLASH_BANK1, offset, size) != 0)
                    {
                        printf("flash erase failed\r\n");
                    }
                    Send_Byte(ACK);
                    Send_Byte(CRC16);
                  }
                  /* Filename packet is empty, end session */
                  else
                  {
                    Send_Byte(ACK);
                    file_done = 1;
                    session_done = 1;
                    break;
                  }
                }
                /* Data packet */
                else
                {
                  memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
                  ramsource = (uint32_t)buf_ptr;

                  /* Write received data in Flash */
                //   if (FLASH_If_Write(&flashdestination, (uint32_t*) ramsource, (uint16_t) packet_length/4)  == 0)
                  if (device_flash_write(DEVICE_NAME_FLASH_BANK1, offset, buf_ptr, packet_length, 1000) == 0)
                  {
#if 0
                    for (i = 0; i < packet_length; i++)
                    {
                      if (i%16==0)
                      {
                        printf("\r\n");
                      }
                      printf("%.2x ", buf_ptr[i]);

                    }

                    printf("\r\n----------------------------------\r\n");
#endif
                    offset += packet_length;
                    Send_Byte(ACK);
                  }
                  else /* An error occurred while writing to Flash memory */
                  {
                    /* End session */
                    Send_Byte(CA);
                    Send_Byte(CA);
                    return -2;
                  }
                }
                packets_received ++;
                session_begin = 1;
              }
          }
          break;
        case 1:
          Send_Byte(CA);
          Send_Byte(CA);
          return -3;
        default:
          if (session_begin > 0)
          {
            errors ++;
          }
          if (errors > MAX_ERRORS)
          {
            Send_Byte(CA);
            Send_Byte(CA);
            return 0;
          }
          Send_Byte(CRC16);
          break;
      }
      if (file_done != 0)
      {
        break;
      }
    }
    if (session_done != 0)
    {
        osDelay(50);
        Send_Byte(CRC16);   /* used for secureCRT */
      break;
    }
  }
  return (int32_t)size;
}

/**
  * @brief  check response using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  */
int32_t Ymodem_CheckResponse(uint8_t c)
{
  return 0;
}

/**
  * @brief  Prepare the first block
  * @param  timeout
  *     0: end of transmission
  * @retval None
  */
void Ymodem_PrepareIntialPacket(uint8_t *data, const uint8_t* fileName, uint32_t *length)
{
  uint16_t i, j;
  uint8_t file_ptr[10];
  
  /* Make first three packet */
  data[0] = SOH;
  data[1] = 0x00;
  data[2] = 0xff;
  
  /* Filename packet has valid data */
  for (i = 0; (fileName[i] != '\0') && (i < FILE_NAME_LENGTH);i++)
  {
     data[i + PACKET_HEADER] = fileName[i];
  }

  data[i + PACKET_HEADER] = 0x00;
  
  Int2Str (file_ptr, *length);
  for (j =0, i = i + PACKET_HEADER + 1; file_ptr[j] != '\0' ; )
  {
     data[i++] = file_ptr[j++];
  }
  
  for (j = i; j < PACKET_SIZE + PACKET_HEADER; j++)
  {
    data[j] = 0;
  }
}

/**
  * @brief  Prepare the data packet
  * @param  timeout
  *     0: end of transmission
  * @retval None
  */
void Ymodem_PreparePacket(uint8_t *SourceBuf, uint8_t *data, uint8_t pktNo, uint32_t sizeBlk, uint32_t offset)
{
  uint16_t i, size, packetSize;
  uint8_t* file_ptr;
  
  /* Make first three packet */
  packetSize = sizeBlk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;
  size = sizeBlk < packetSize ? sizeBlk :packetSize;
  if (packetSize == PACKET_1K_SIZE)
  {
     data[0] = STX;
  }
  else
  {
     data[0] = SOH;
  }
  data[1] = pktNo;
  data[2] = (~pktNo);
  file_ptr = SourceBuf;
  
  /* Filename packet has valid data */
//   for (i = PACKET_HEADER; i < size + PACKET_HEADER;i++)
//   {
//      data[i] = *file_ptr++;
//   }

  if (device_flash_read(DEVICE_NAME_FLASH_BANK1, offset, &data[PACKET_HEADER], size, 1000) != 0)
  {
    printf("flash read failed\r\n");
  }

  if ( size  <= packetSize)
  {
    for (i = size + PACKET_HEADER; i < packetSize + PACKET_HEADER; i++)
    {
      data[i] = 0x1A; /* EOF (0x1A) or 0x00 */
    }
  }
}

/**
  * @brief  Update CRC16 for input byte
  * @param  CRC input value 
  * @param  input byte
  * @retval None
  */
uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
  uint32_t crc = crcIn;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  
  while(!(in & 0x10000));

  return crc & 0xffffu;
}


/**
  * @brief  Cal CRC16 for YModem Packet
  * @param  data
  * @param  length
  * @retval None
  */
uint16_t Cal_CRC16(const uint8_t* data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = data+size;

  while(data < dataEnd)
    crc = UpdateCRC16(crc, *data++);
 
  crc = UpdateCRC16(crc, 0);
  crc = UpdateCRC16(crc, 0);

  return crc&0xffffu;
}

/**
  * @brief  Cal Check sum for YModem Packet
  * @param  data
  * @param  length
  * @retval None
  */
uint8_t CalChecksum(const uint8_t* data, uint32_t size)
{
  uint32_t sum = 0;
  const uint8_t* dataEnd = data+size;

  while(data < dataEnd )
    sum += *data++;

  return (sum & 0xffu);
}

/**
  * @brief  Transmit a data packet using the ymodem protocol
  * @param  data
  * @param  length
  * @retval None
  */
void Ymodem_SendPacket(uint8_t *data, uint16_t length)
{
  ymodem_data_write(data, length, 1000);
}

/**
  * @brief  Transmit a file using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  */
uint8_t Ymodem_Transmit (uint8_t *buf, const uint8_t* sendFileName, uint32_t sizeFile, uint32_t offset)
{
  
  uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
  uint8_t filename[FILE_NAME_LENGTH];
  uint8_t *buf_ptr, tempCheckSum;
  uint16_t tempCRC;
  uint16_t blkNumber;
  uint8_t receivedC[2], CRC16_F = 0, i;
  uint32_t errors, ackReceived, size = 0, pktSize;

  errors = 0;
  ackReceived = 0;
  for (i = 0; i < (FILE_NAME_LENGTH - 1); i++)
  {
    filename[i] = sendFileName[i];
  }
  CRC16_F = 1;
    
  /* Prepare first block */
  Ymodem_PrepareIntialPacket(&packet_data[0], filename, &sizeFile);
  
  do 
  {
    /* Send Packet */
    Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER);

    /* Send CRC or Check Sum based on CRC16_F */
    if (CRC16_F)
    {
       tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
       Send_Byte(tempCRC >> 8);
       Send_Byte(tempCRC & 0xFF);
    }
    else
    {
       tempCheckSum = CalChecksum (&packet_data[3], PACKET_SIZE);
       Send_Byte(tempCheckSum);
    }
  
    /* Wait for Ack and 'C' */
    if (Receive_Byte(&receivedC[0], 10000) == 0)  
    {
      if (receivedC[0] == ACK)
      { 
        /* Packet transferred correctly */
        ackReceived = 1;
      }
    }
    else
    {
        errors++;
    }
  }while (!ackReceived && (errors < 0x0A));
  
  if (errors >=  0x0A)
  {
    return errors;
  }
  buf_ptr = buf;
  size = sizeFile;
  blkNumber = 0x01;
  /* Here 1024 bytes package is used to send the packets */
  
  
  /* Resend packet if NAK  for a count of 10 else end of communication */
  while (size)
  {
    /* Prepare next packet */
    Ymodem_PreparePacket(buf_ptr, &packet_data[0], blkNumber, size, offset);
    ackReceived = 0;
    receivedC[0]= 0;
    errors = 0;
    do
    {
      /* Send next packet */
      if (size >= PACKET_1K_SIZE)
      {
        pktSize = PACKET_1K_SIZE;
       
      }
      else
      {
        pktSize = PACKET_SIZE;
      }
      Ymodem_SendPacket(packet_data, pktSize + PACKET_HEADER);
      /* Send CRC or Check Sum based on CRC16_F */
      /* Send CRC or Check Sum based on CRC16_F */
      if (CRC16_F)
      {
         tempCRC = Cal_CRC16(&packet_data[3], pktSize);
         Send_Byte(tempCRC >> 8);
         Send_Byte(tempCRC & 0xFF);
      }
      else
      {
        tempCheckSum = CalChecksum (&packet_data[3], pktSize);
        Send_Byte(tempCheckSum);
      }
      
      /* Wait for Ack */
      if ((Receive_Byte(&receivedC[0], 100000) == 0)  && (receivedC[0] == ACK))
      {
        ackReceived = 1;  
        if (size > pktSize)
        {
           buf_ptr += pktSize;
           offset += pktSize;
           size -= pktSize;
           if (blkNumber == (USER_FLASH_SIZE/1024))
           {
             return 0xFF; /*  error */
           }
           else
           {
              blkNumber++;
           }
        }
        else
        {
          buf_ptr += pktSize;
          offset += pktSize;
          size = 0;
        }
      }
      else
      {
        errors++;
      }
    }while(!ackReceived && (errors < 0x0A));
    /* Resend packet if NAK  for a count of 10 else end of communication */
    
    if (errors >=  0x0A)
    {
      return errors;
    }
    
  }
  ackReceived = 0;
  receivedC[0] = 0x00;
  errors = 0;
  do 
  {
    Send_Byte(EOT);
    /* Send (EOT); */
    /* Wait for Ack */
    if ((Receive_Byte(&receivedC[0], 10000) == 0)  && receivedC[0] == ACK)
    {
      ackReceived = 1;  
    }
    else
    {
      errors++;
    }
  }while (!ackReceived && (errors < 0x0A));
    
  if (errors >=  0x0A)
  {
    return errors;
  }
  
  /* Last packet preparation */
  ackReceived = 0;
  receivedC[0] = 0x00;
  errors = 0;

  packet_data[0] = SOH;
  packet_data[1] = 0;
  packet_data [2] = 0xFF;

  for (i = PACKET_HEADER; i < (PACKET_SIZE + PACKET_HEADER); i++)
  {
     packet_data [i] = 0x00;
  }
  
  do 
  {
    /* Send Packet */
    Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER);

    /* Send CRC or Check Sum based on CRC16_F */
    tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
    Send_Byte(tempCRC >> 8);
    Send_Byte(tempCRC & 0xFF);
  
    /* Wait for Ack and 'C' */
    if (Receive_Byte(&receivedC[0], 10000) == 0)  
    {
      if (receivedC[0] == ACK)
      { 
        /* Packet transferred correctly */
        ackReceived = 1;
      }
    }
    else
    {
        errors++;
    }
  }while (!ackReceived && (errors < 0x0A));

  /* Resend packet if NAK  for a count of 10  else end of communication */
  if (errors >=  0x0A)
  {
    return errors;
  }  
  
//   do 
//   {
//     Send_Byte(EOT);
//     /* Send (EOT); */
//     /* Wait for Ack */
//     if ((Receive_Byte(&receivedC[0], 10000) == 0)  && receivedC[0] == ACK)
//     {
//       ackReceived = 1;  
//     }
//     else
//     {
//       errors++;
//     }
//   }while (!ackReceived && (errors < 0x0A));

//   if (errors >=  0x0A)
//   {
//     return errors;
//   }
  return 0; /* file transmitted successfully */
}

/**
  * @}
  */

void SerialDownload(uint32_t offset, uint32_t size)
{
    int32_t Size = Ymodem_Receive(offset);
    if (Size > 0)
    {
      SerialPutString("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
      SerialPutString(FileName);
      SerialPutString("\n\r Size: %d", Size);
      SerialPutString(" Bytes\r\n");
      SerialPutString("-------------------\n");
    }
    else if (Size == -1)
    {
      SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r");
    }
    else if (Size == -2)
    {
      SerialPutString("\n\n\rVerification failed!\n\r");
    }
    else if (Size == -3)
    {
      SerialPutString("\r\n\nAborted by user.\n\r");
    }
    else
    {
      SerialPutString("\n\rFailed to receive the file!\n\r");
    }    
}

void SerialUpload(uint32_t offset, uint32_t size)
{
  uint8_t status = 0 ; 
  uint8_t buf = 0;
  uint16_t len = 0;

  if (ymodem_data_read(&buf, &len, 0xFFFFFFFF) == 0 && buf == CRC16)
  {
    /* Transmit the flash image through ymodem protocol */
    status = Ymodem_Transmit((uint8_t*)offset, (const uint8_t*)"ethercat_cnncpm.bin", size, offset);

    if (status != 0) 
    {
      printf("\n\rError Occurred while Transmitting File\n\r");
    }
    else
    {
      printf("\n\rFile uploaded successfully \n\r");
    }
  }
}

/***************************************************************************************************************/
/* private code */
struct ymodem_para
{
    uint32_t mode;
    uint32_t offset;
    uint32_t size;
};

osMessageQueueId_t ymodem_queue = NULL;

static void ymodem_thread_entry(void *argument)
{
    struct ymodem_para para = {0};
    osStatus_t stat = osOK;

    for (;;)
    {
        stat = osMessageQueueGet(ymodem_queue, &para, 0, osWaitForever);
        if (stat!= osOK)
        {
            continue;
        }

        switch (para.mode)
        {
        case 1:
            SerialDownload(para.offset, para.size);
            break;

        case 2:
            SerialUpload(para.offset, para.size);
            break;
        
        default:
            printf("Invalid argument\r\n");
            break;
        }

        *com_mode_get() = CONSOLE_MODE;
    }
}

static int8_t ymodem_thread_init(uint8_t argc, uint8_t **argv)
{
    ymodem_queue = osMessageQueueNew(3, sizeof(struct ymodem_para), NULL);
    if (ymodem_queue == NULL)
    {
        return -1;
    }

    osThreadAttr_t ymodem_attributes = {
    .name = "ymodem_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityLow,
    };

    osThreadId_t ymodem_Handle = osThreadNew(ymodem_thread_entry, NULL, &ymodem_attributes);
    if (ymodem_Handle == NULL)
    {
        return -2;
    }

    return 0;
}
// INIT_APP_EXPORT(ymodem_thread_init);

static int8_t ymodem_start(uint8_t argc, uint8_t **argv)
{
    if (argc != 4)
    {
        printf("usage: ymodem_start <download/upload:1/2> <offset eg:0x00> <size: in bytes>\r\n");
        return -1;
    }

    struct ymodem_para para = {0};

    para.mode = strtoul(argv[1], NULL, 16);
    para.offset = strtoul(argv[2], NULL, 16);
    para.size = strtoul(argv[3], NULL, 16);

    printf("mode: %u, offset: %#.8x, size: %u\r\n", para.mode, para.offset, para.size);

    if (para.mode > 2)
    {
        printf("Invalid argument\r\n");
        return -2;
    }

    if (para.offset + para.size > FLASH_SIZE)
    {
        printf("Invalid address\r\n");
        return -3;
    }

    *com_mode_get() = YMODEM_MODE;

    osStatus_t stat = osMessageQueuePut(ymodem_queue, &para, 0, 1000);
    if (stat != osOK)
    {
        printf("Failed to send message to ymodem thread\r\n");
        return -4;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(ymodem_start, ymodem_start, start ymodem);

/*******************(C)COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
