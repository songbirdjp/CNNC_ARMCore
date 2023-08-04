//#include "main.h"
//#include "usart.h"
//#include "RS485.h"
//void RS485_SendEnable()
//{
//    HAL_GPIO_WritePin( GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
//}
//void RS485_SendDisable()
//{
//    HAL_GPIO_WritePin( GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
//}
//HAL_StatusTypeDef RS485_SendOneByte(uint8_t _byte2Send)
//{
//    RS485_SendEnable();
//    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1,&_byte2Send,1,5000);
//    RS485_SendDisable();
//    return status;
//}
//HAL_StatusTypeDef RS485_ReceiveOneByte(void)
//{
//    uint8_t _byte2Receive;
//    HAL_StatusTypeDef status = HAL_UART_Receive(&huart1,&_byte2Receive,1,5000);
//    return status;
//}
//HAL_StatusTypeDef RS485_SendBytes(uint8_t* pBuffer,uint16_t _byteCount)
//{
//    RS485_SendEnable();
//    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, pBuffer, _byteCount, 5000);
//    RS485_SendDisable();
//    return status;
//}
//HAL_StatusTypeDef RS485_ReceiveBytes(uint8_t* pBuffer,uint16_t _byteCount)
//{
//    HAL_StatusTypeDef  status = HAL_UART_Receive(&huart1,pBuffer,_byteCount,5000);
//    return status;
//}