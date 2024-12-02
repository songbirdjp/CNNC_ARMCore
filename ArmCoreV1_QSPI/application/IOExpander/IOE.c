#include "IOE.h"
#include "spi.h"
#include "shell.h"
#include "init_call.h"

uint8_t isIOEInitOK = 0;
int IOE_Init()
{
    // __disable_irq();
    static uint8_t IOECommandToWrite[3];
    static uint8_t IOECommandToRead[3];
    static uint8_t IOEDataToRecv[3];
    //config IOCON register with 0b01110000
    IOECommandToWrite[0] = IOE_OPCode_Write;
    IOECommandToWrite[1] = IOE_RegAddr_IOCON;
    IOECommandToWrite[2] = 0x70;
    HAL_SPI_Transmit(&hspi2,IOECommandToWrite,3,5000);
    //read back to check
    IOECommandToRead[0] = IOE_OPCode_Read;
    IOECommandToRead[1] = IOE_RegAddr_IOCON;
    IOECommandToRead[2] = 0x00;
    HAL_SPI_TransmitReceive(&hspi2,IOECommandToRead,IOEDataToRecv,3,5000);
    if(0x70 != IOEDataToRecv[2])
    {
        printf("IOE_RegAddr_IOCON = %x\r\n",IOEDataToRecv[2]);
        return -1;
    }
    //config GPIO direction
    IOECommandToWrite[0] = IOE_OPCode_Write;
    IOECommandToWrite[1] = IOE_RegAddr_IODIR_A;
    IOECommandToWrite[2] = 0xFF;  // All pins as input for Port A
    HAL_SPI_Transmit(&hspi2, IOECommandToWrite, 3, 1000);

    IOECommandToRead[0] = IOE_OPCode_Read;
    IOECommandToRead[1] = IOE_RegAddr_IODIR_A;
    IOECommandToRead[2] = 0x00;
    HAL_SPI_TransmitReceive(&hspi2,IOECommandToRead,IOEDataToRecv,3,1000);
     if(0xFF != IOEDataToRecv[2])
    {
        printf("IOE_RegAddr_IODIR_A = %x\r\n",IOEDataToRecv[2]);
        return -2;
    }

    IOECommandToWrite[1] = IOE_RegAddr_IODIR_B;
    IOECommandToWrite[2] = 0xFF;  // All pins as input for Port B
    HAL_SPI_Transmit(&hspi2, IOECommandToWrite, 3, 1000);
    //congfig GPIO Interrupt Enable
    IOECommandToRead[0] = IOE_OPCode_Read;
    IOECommandToRead[1] = IOE_RegAddr_IODIR_B;
    IOECommandToRead[2] = 0x00;
    HAL_SPI_TransmitReceive(&hspi2,IOECommandToRead,IOEDataToRecv,3,1000);
    if(0xFF != IOEDataToRecv[2])
    {
        printf("IOE_RegAddr_IODIR_B = %x\r\n",IOEDataToRecv[2]);
        return -3;
    }
    IOECommandToWrite[1] = IOE_RegAddr_GPINTEN_A;
    IOECommandToWrite[2] = 0xFF;  // Enable interrupt for all pins on Port A
    HAL_SPI_Transmit(&hspi2, IOECommandToWrite, 3, 1000);

    IOECommandToRead[0] = IOE_OPCode_Read;
    IOECommandToRead[1] = IOE_RegAddr_GPINTEN_A;
    IOECommandToRead[2] = 0x00;
    HAL_SPI_TransmitReceive(&hspi2,IOECommandToRead,IOEDataToRecv,3,1000);

    if(0xFF != IOEDataToRecv[2])
    {
        printf("IOE_RegAddr_GPINTEN_A = %x\r\n",IOEDataToRecv[2]);
        return -4;
    }
    IOECommandToWrite[1] = IOE_RegAddr_GPINTEN_B;
    IOECommandToWrite[2] = 0xFF;  // Enable interrupt for all pins on Port B
    HAL_SPI_Transmit(&hspi2, IOECommandToWrite, 3, 1000);
    // todo: add some check
    IOECommandToRead[0] = IOE_OPCode_Read;
    IOECommandToRead[1] = IOE_RegAddr_GPINTEN_B;
    IOECommandToRead[2] = 0x00;
    HAL_SPI_TransmitReceive(&hspi2,IOECommandToRead,IOEDataToRecv,3,5000);
    if(0xFF != IOEDataToRecv[2])
    {
        printf("IOE_RegAddr_GPINTEN_B = %x\r\n",IOEDataToRecv[2]);
        return -5;
    }
    isIOEInitOK = 1;
    // __enable_irq();
    return isIOEInitOK;
}
uint16_t IOE_GPIORead(void)
{
    static uint8_t IOECommandToRead[3];
    static uint8_t IOERecvGPIOA[3];
    static uint8_t IOERecvGPIOB[3]; 
    static uint16_t GPIOData;
    IOECommandToRead[0] = IOE_OPCode_Read;
    IOECommandToRead[1] = IOE_RegAddr_GPIO_A;
    IOECommandToRead[2] = 0x00;
    HAL_SPI_TransmitReceive(&hspi2,IOECommandToRead,IOERecvGPIOA,3,5000);

    IOECommandToRead[0] = IOE_OPCode_Read;
    IOECommandToRead[1] = IOE_RegAddr_GPIO_B;
    IOECommandToRead[2] = 0x00;
    HAL_SPI_TransmitReceive(&hspi2,IOECommandToRead,IOERecvGPIOB,3,5000);
    GPIOData = ((IOERecvGPIOB[2]<<8)|(IOERecvGPIOA[2]));
    printf("ExpandGPIOValue = %x\r\n",GPIOData);
    return GPIOData;
}
MSH_CMD_EXPORT_ALIAS(IOE_GPIORead,IOE,"IOE_Read");

ExpandGPIOStatus_t IOE_ExpandGPIODataParse(uint16_t gpioDataToParse)
{
    ExpandGPIOStatus_t gpioStatus;
    gpioStatus.expandGpioData = gpioDataToParse;
    return gpioStatus;  
}

GPIO_PinState ReadIO_ModTrigFB()
{
    return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11);
}

GPIO_PinState ReadIO_LvOKDetect()
{
    return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10);
}
GPIO_PinState ReadIO_HvENFB()
{
    return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3);
}

GPIO_PinState ReadIO_ModArcDetect()
{
    return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
}
GPIO_PinState ReadIO_ModTrigONDetect()
{
    return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
}

GPIO_PinState ReadIO_ModHvONDetect()
{
    return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_5);
}

GPIO_PinState ReadIO_ModSumDetect()
{
    return HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_11);
}
GPIO_PinState ReadIO_Dose1Detect()
{
    return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8);
}

GPIO_PinState ReadIO_Dose2Detect()
{
    return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9);
}
GPIO_PinState ReadIO_EmergencyDetect()
{
    return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8);
}

GPIO_PinState ReadIO_PulseInhibitDetect()
{
    return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9);
}


void EPSEnable(uint8_t _epsStatus)
{
    if(1 == _epsStatus)
    {
        HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_SET);
    }
    else 
    {
        HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_RESET);
    }
}


void VPSEnable(uint8_t _vpsStatus)
{
    if(1 == _vpsStatus)
    {
        HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET);
    }
    else 
    {
        HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET);
    }
}

static int8_t ioe_hw_init(void)
{
    MX_SPI2_Init();

    return 0;
}
INIT_DEVICE_EXPORT(ioe_hw_init);