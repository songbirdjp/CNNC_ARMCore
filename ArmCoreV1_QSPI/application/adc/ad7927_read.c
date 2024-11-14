#include "ad7927_read.h"

uint16_t spiSixteenBits =(
        (AD7927_CFGREG_11 << (15 - BUFFER))|
        (AD7927_CFGREG_10 << (14 - BUFFER))|
        (AD7927_CFGREG_9 << (13 - BUFFER))|
        (AD7927_CFGREG_8 << (12 - BUFFER))|
        (AD7927_CFGREG_7 << (11 - BUFFER))|
        (AD7927_CFGREG_6 << (10 - BUFFER))|
        (AD7927_CFGREG_5 << (9 - BUFFER))|
        (AD7927_CFGREG_4 << (8 - BUFFER))|
        (AD7927_CFGREG_3 << (7 - BUFFER))|
        (AD7927_CFGREG_2 << (6 - BUFFER))|
        (AD7927_CFGREG_1 << (5 - BUFFER))|
        (AD7927_CFGREG_0 << (4 - BUFFER))
);

HAL_StatusTypeDef AD7927_Init(SPI_HandleTypeDef *hspi)
{
    uint8_t spiData[2] = {0};
    uint8_t spiFFFF[2] = {0xff,0xff};
    HAL_StatusTypeDef status;
    spiData[0] = spiSixteenBits >> 8;
    spiData[1] = spiSixteenBits ;
    HAL_SPI_Transmit(hspi, spiFFFF, 2, HAL_MAX_DELAY);
    HAL_Delay(1);
    HAL_SPI_Transmit(hspi, spiFFFF, 2, HAL_MAX_DELAY);
    HAL_Delay(1);

    return status;
}

uint16_t ADCgetValue(SPI_HandleTypeDef* hspi, uint8_t channel)
{
    uint8_t TXData[2] = {0};
    uint8_t RXData[2] = {0};
    uint16_t adcResult = 0;
    HAL_StatusTypeDef _spiADCStatus;

    spiSixteenBits &= 0xE3FF;//clear all channel bits， bit12 - 10
    spiSixteenBits |= (channel << 10);//set channel
    TXData[0] = spiSixteenBits >> 8;
    TXData[1] = spiSixteenBits ;

    _spiADCStatus = HAL_SPI_Transmit(hspi, TXData, 2, 1000);//send config
    _spiADCStatus = HAL_SPI_Receive(hspi, RXData, 2, 1000);//receive data
  //  printf("_spiADCStatus = %d\r\n",_spiADCStatus);

//    printf("RXData[0] = 0x%x\r\n",RXData[0]);
//    printf("RXData[1] = 0x%x\r\n",RXData[1]);
//    printf("TXData[0] = 0x%x\r\n",TXData[0]);
//    printf("TXData[1] = 0x%x\r\n",TXData[1]);

    adcResult = ((uint16_t)RXData[0] << 8 | RXData[1]) & 0x0FFF;
  //  printf("adcResult = %d\r\n",adcResult);
    return adcResult;
}