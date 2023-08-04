#ifndef ETHERCAT_CNNCPM_RS485_H
#define ETHERCAT_CNNCPM_RS485_H
#include "main.h"

void RS485_SendEnable();
void RS485_SendDisable();
HAL_StatusTypeDef RS485_SendOneByte(uint8_t _byte2Send);
HAL_StatusTypeDef RS485_ReceiveOneByte(void);
HAL_StatusTypeDef RS485_SendBytes(uint8_t* pBuffer,uint16_t _byteCount);
HAL_StatusTypeDef RS485_ReceiveBytes(uint8_t* pBuffer,uint16_t _byteCount);

#endif //ETHERCAT_CNNCPM_RS485_H
