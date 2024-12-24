#include "io_port.h"
#include "spi.h"
#include "cmsis_os2.h"
#include "drv_gpio.h"
#include "init_call.h"
#include "gpio_port.h"
#include "ulog.h"

#define IOE_OPCode_Write             0x40
#define IOE_OPCode_Read              0x41

#define IOE_RegAddr_IODIR_A           0x00   // I/O Direction Register for Port A
#define IOE_RegAddr_IODIR_B           0x01   // I/O Direction Register for Port B
#define IOE_RegAddr_IPOL_A            0x02   // Input Polarity Register for Port A
#define IOE_RegAddr_IPOL_B            0x03   // Input Polarity Register for Port B
#define IOE_RegAddr_GPINTEN_A         0x04   // Interrupt-on-Change Control Register for Port A
#define IOE_RegAddr_GPINTEN_B         0x05   // Interrupt-on-Change Control Register for Port B
#define IOE_RegAddr_DEFVAL_A          0x06   // Default Compare Register for Interrupt-on-Change for Port A
#define IOE_RegAddr_DEFVAL_B          0x07   // Default Compare Register for Interrupt-on-Change for Port B
#define IOE_RegAddr_INTCON_A          0x08   // Interrupt Control Register for Port A
#define IOE_RegAddr_INTCON_B          0x09   // Interrupt Control Register for Port B
#define IOE_RegAddr_IOCON             0x0A   // Configuration Register (shared for both ports)
#define IOE_RegAddr_GPPU_A            0x0C   // GPIO Pull-Up Resistor Register for Port A
#define IOE_RegAddr_GPPU_B            0x0D   // GPIO Pull-Up Resistor Register for Port B
#define IOE_RegAddr_INTF_A            0x0E   // Interrupt Flag Register for Port A
#define IOE_RegAddr_INTF_B            0x0F   // Interrupt Flag Register for Port B
#define IOE_RegAddr_INTCAP_A          0x10   // Interrupt Captured Value for Port A
#define IOE_RegAddr_INTCAP_B          0x11   // Interrupt Captured Value for Port B
#define IOE_RegAddr_GPIO_A            0x12   // General Purpose I/O Port Register for Port A
#define IOE_RegAddr_GPIO_B            0x13   // General Purpose I/O Port Register for Port B
#define IOE_RegAddr_OLAT_A            0x14   // Output Latch Register for Port A
#define IOE_RegAddr_OLAT_B            0x15   // Output Latch Register for Port B

static osMessageQueueId_t ExpandGPIOQueueHandle = NULL;
static osMutexId_t gpio_mutex = NULL;
static InterlocksDetect_t gpio_info = {0};
static InterlocksDetect_t *gpio_info_get(void)
{
    return &gpio_info;
}

static int8_t ioe_config(void)
{
    uint8_t IOECommandToWrite[3];
    uint8_t IOECommandToRead[3];
    uint8_t IOEDataToRecv[3];
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
        LOG_E("IOE_RegAddr_IOCON = %x\r\n",IOEDataToRecv[2]);
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
        LOG_E("IOE_RegAddr_IODIR_A = %x\r\n",IOEDataToRecv[2]);
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
        LOG_E("IOE_RegAddr_IODIR_B = %x\r\n",IOEDataToRecv[2]);
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
        LOG_E("IOE_RegAddr_GPINTEN_A = %x\r\n",IOEDataToRecv[2]);
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
        LOG_E("IOE_RegAddr_GPINTEN_B = %x\r\n",IOEDataToRecv[2]);
        return -5;
    }

    return 0;
}
static uint16_t ioe_read(void)
{
    uint8_t IOECommandToRead[3];
    uint8_t IOERecvGPIOA[3];
    uint8_t IOERecvGPIOB[3];

    IOECommandToRead[0] = IOE_OPCode_Read;
    IOECommandToRead[1] = IOE_RegAddr_GPIO_A;
    IOECommandToRead[2] = 0x00;
    HAL_SPI_TransmitReceive(&hspi2, IOECommandToRead, IOERecvGPIOA, 3, 5000);

    IOECommandToRead[0] = IOE_OPCode_Read;
    IOECommandToRead[1] = IOE_RegAddr_GPIO_B;
    IOECommandToRead[2] = 0x00;
    HAL_SPI_TransmitReceive(&hspi2, IOECommandToRead, IOERecvGPIOB, 3, 5000);

    return (IOERecvGPIOB[2] << 8 | IOERecvGPIOA[2]);
}
static int8_t ioe_init(void)
{
    int8_t ret = 0;

    MX_SPI2_Init();

    ret = ioe_config();
    if(ret != 0)
    {
        LOG_E("ioe_config err: %d\r\n",ret);
        return -1;
    }

    ioe_read();

    return 0;
}
static int8_t ioe_irq_callback(void)
{
    osStatus_t stat = osOK;
    uint16_t ExpandGPIOData = ioe_read();
    stat = osMessageQueuePut(ExpandGPIOQueueHandle, &ExpandGPIOData, 0, 0);
    if (stat != osOK)
    {
        printf("osMessageQueuePut err: %d\r\n",stat);
    }

    return stat;
}

static int8_t io_init(void)
{
    int8_t ret = 0;

    ret = gpio_common_get()->write("GPIOG_7", GPIO_PIN_SET);    //EPS Enable
    ret |= gpio_common_get()->write("GPIOE_6", GPIO_PIN_SET);   //VPS Enable

    ret |= gpio_common_get()->write("GPIOE_2", GPIO_PIN_SET);   //PRF Enable
    ret |= gpio_common_get()->write("GPIOE_4", GPIO_PIN_SET);   //Lv Interlock Enable
    ret |= gpio_common_get()->write("GPIOE_5", GPIO_PIN_SET);   //Hv Interlock Enable
    ret |= gpio_common_get()->write("GPIOC_6", GPIO_PIN_SET);   //Trig Inhibit Disable

    return ret;
}

static int8_t gpio_thread_entry(void *argument)
{
    int8_t ret = 0;
    uint16_t ExpandGPIOData = 0;
    InterlocksDetect_t *obj = gpio_info_get();

    ret = ioe_init();
    if(ret != 0)
    {
        LOG_E("ioe_init err: %d\r\n",ret);
        osThreadExit();
    }

    ret = io_init();
    if(ret != 0)
    {
        LOG_E("io_init err: %d\r\n",ret);
        osThreadExit();
    }

    ret = gpio_pin_irq_callback_register("GPIOG_6", ioe_irq_callback);
    if(ret != 0)
    {
        LOG_E("gpio_pin_irq_callback_register err: %d\r\n",ret);
        osThreadExit();
    }

    for (;;)
    {
        osMessageQueueGet(ExpandGPIOQueueHandle, &ExpandGPIOData, NULL, osWaitForever);

        osMutexAcquire(gpio_mutex, osWaitForever);
        obj->exGPIODetect.expandGpioData = ExpandGPIOData;
        osMutexRelease(gpio_mutex);
    }

    return 0;
}

static GPIO_PinState ReadIO_ModTrigFB(void)
{
    return gpio_common_get()->read("GPIOB_11");
}
static GPIO_PinState ReadIO_LvOKDetect(void)
{
    return gpio_common_get()->read("GPIOB_10");
}
static GPIO_PinState ReadIO_HvENFB(void)
{
    return gpio_common_get()->read("GPIOA_3");
}
static GPIO_PinState ReadIO_ModArcDetect(void)
{
    return gpio_common_get()->read("GPIOB_1");
}
static GPIO_PinState ReadIO_ModTrigONDetect(void)
{
    return gpio_common_get()->read("GPIOB_0");
}
static GPIO_PinState ReadIO_ModHvONDetect(void)
{
    return gpio_common_get()->read("GPIOC_5");
}
static GPIO_PinState ReadIO_ModSumDetect(void)
{
    return gpio_common_get()->read("GPIOD_11");
}
static GPIO_PinState ReadIO_Dose1Detect(void)
{
    return gpio_common_get()->read("GPIOB_8");   
}
static GPIO_PinState ReadIO_Dose2Detect(void)
{
    return gpio_common_get()->read("GPIOB_9");  
}
static GPIO_PinState ReadIO_EmergencyDetect(void)
{
    return gpio_common_get()->read("GPIOC_8");  
}
static GPIO_PinState ReadIO_PulseInhibitDetect(void)
{
    return gpio_common_get()->read("GPIOC_9"); 
}
static int8_t gpio_status_polling(void *argument)
{
    osStatus_t stat = osOK;
    osMutexId_t mutex = (osMutexId_t)argument;
    InterlocksDetect_t *obj = gpio_info_get();
    InterlocksDetect_t AllInterlocks= {0};

    AllInterlocks.ModTrigFB  = ReadIO_ModTrigFB();
    AllInterlocks.LvOKDetect  = ReadIO_LvOKDetect();
    AllInterlocks.HvEnDetect  = ReadIO_HvENFB();
    AllInterlocks.ModArcDetect  = ReadIO_ModArcDetect();
    AllInterlocks.ModTrigONDetect  = ReadIO_ModTrigONDetect();

    AllInterlocks.ModHvONDetect = ReadIO_ModHvONDetect();
    AllInterlocks.ModSumDetect  = ReadIO_ModSumDetect();
    AllInterlocks.Dose1Detect  = ReadIO_Dose1Detect();
    AllInterlocks.Dose2Detect  = ReadIO_Dose2Detect();
    AllInterlocks.EmergencyDetect  = ReadIO_EmergencyDetect();

    AllInterlocks.ModTriggerInhibitDetect  = ReadIO_PulseInhibitDetect();

    osMutexAcquire(mutex, osWaitForever);
    memcpy(&obj->LvOKDetect, &AllInterlocks.LvOKDetect, sizeof(InterlocksDetect_t) - sizeof(ExpandGPIOStatus_t));
    osMutexRelease(mutex);

    return 0;
}

static int8_t io_thread_init(void)
{
    osMessageQueueAttr_t ExpandGPIOQueue_attributes = {
    .name = "ExpandGPIOQueueHandle"
    };

    ExpandGPIOQueueHandle = osMessageQueueNew(16, sizeof(uint16_t), &ExpandGPIOQueue_attributes);
    if (ExpandGPIOQueueHandle == NULL)
    {
        printf("queue ExpandGPIO create failed\r\n");
        return -1;
    }

    osMutexAttr_t mutex_attributes = {
    .name = "gpio_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    gpio_mutex = osMutexNew(&mutex_attributes);
    if (gpio_mutex == NULL)
    {
        printf("mutex gpio create failed\r\n");
        return -2;
    }

    osThreadAttr_t GPIOThread_attributes = {
        .name = "GPIOThreadHandle",
        .priority = osPriorityNormal,
        .stack_size = 1024 * 4
    };

    osThreadId_t GPIOThreadHandle = osThreadNew(gpio_thread_entry, NULL, &GPIOThread_attributes);
    if (GPIOThreadHandle == NULL)
    {
        printf("thread GPIOThread create failed\r\n");
        return -3;
    }

    osTimerId_t timerId = osTimerNew(gpio_status_polling, osTimerPeriodic, gpio_mutex, NULL);
    if (timerId == NULL)
    {
        printf("timer create failed\r\n");
        return -4;
    }

    osStatus_t stat = osTimerStart(timerId, 10);
    if (stat != osOK)
    {
        printf("timerId start failed\r\n");
        return -5;
    }

    return 0;
}
INIT_APP_EXPORT(io_thread_init);

InterlocksDetect_t interlock_status_get(void)
{
    InterlocksDetect_t interlock = {0};
    InterlocksDetect_t *obj = gpio_info_get();

    osMutexAcquire(gpio_mutex, osWaitForever);
    memcpy(&interlock, obj, sizeof(InterlocksDetect_t));
    osMutexRelease(gpio_mutex);

    return interlock;
}


// uint16_t BGM_ReadModInterlocks(void)
// {
//     static uint16_t ModInterlockStatus;
//     uint8_t ModTrigONStatus;
//     uint8_t ModHvONDetectStatus;
//     uint8_t ModArcDetectStatus;
//     uint8_t ModSumDetectStatus;

//     ModTrigONStatus = ReadIO_ModTrigONDetect();
//     ModHvONDetectStatus = ReadIO_ModArcDetect();
//     ModArcDetectStatus = ReadIO_ModHvONDetect();
//     ModSumDetectStatus = ReadIO_ModSumDetect();

//     ModInterlockStatus = (uint16_t)((ModTrigONStatus << 7)|(ModTrigONStatus << 6)|(ModArcDetectStatus << 5)|(ModSumDetectStatus << 4));
//     return ModInterlockStatus;
// }



#ifndef INTERLOCK_TEST
#include "shell.h"
static int8_t InterlockTest(uint8_t argc, char *argv[])
{
    InterlocksDetect_t *obj = gpio_info_get();
    InterlocksDetect_t interlock = interlock_status_get();

    LOG_I("//////////////////////BGM Interlocks Status//////////////////////// \r\n");
    LOG_I("CoolingLv1Detect        =   %d\r\n", interlock.exGPIODetect.bits.CoolingLv1Detect);
    LOG_I("CoolingLv2Detect        =   %d\r\n", interlock.exGPIODetect.bits.CoolingLv2Detect);
    LOG_I("WaterSW1Detect          =   %d\r\n", interlock.exGPIODetect.bits.WaterSW1Detect);
    LOG_I("WaterSW2Detect          =   %d\r\n", interlock.exGPIODetect.bits.WaterSW2Detect);
    LOG_I("WaterSW3Detect          =   %d\r\n", interlock.exGPIODetect.bits.WaterSW3Detect);
    LOG_I("WaterSW4Detect          =   %d\r\n", interlock.exGPIODetect.bits.WaterSW4Detect);
    LOG_I("WaterSW5Detect          =   %d\r\n", interlock.exGPIODetect.bits.WaterSW5Detect);
    LOG_I("SF6HighDetect           =   %d\r\n", interlock.exGPIODetect.bits.SF6HighDetect);
    LOG_I("SF6LowDetect            =   %d\r\n", interlock.exGPIODetect.bits.SF6LowDetect);
    LOG_I("EPSStateOPDetect        =   %d\r\n", interlock.exGPIODetect.bits.EPSStateOPDetect);
    LOG_I("nEPSStateFaultDetect    =   %d\r\n", interlock.exGPIODetect.bits.nEPSStateFaultDetect);
    LOG_I("VPSStateFaultDetect     =   %d\r\n", interlock.exGPIODetect.bits.VPSStateFaultDetect);
    LOG_I("VPSStateOPDetect        =   %d\r\n", interlock.exGPIODetect.bits.VPSStateOPDetect);
    LOG_I("GatingDetect            =   %d\r\n", interlock.exGPIODetect.bits.GatingDetect);
    LOG_I("HVConFBDetect           =   %d\r\n", interlock.exGPIODetect.bits.HVConFBDetect);
    LOG_I("MVTreatmentENDetect     =   %d\r\n", interlock.exGPIODetect.bits.MVTreatmentENDetect);
    LOG_I("Dose1Detect             =   %d\r\n", interlock.Dose1Detect);
    LOG_I("Dose2Detect             =   %d\r\n", interlock.Dose2Detect);
    LOG_I("EmergencyDetect         =   %d\r\n", interlock.EmergencyDetect);
    LOG_I("HvEnDetect              =   %d\r\n", interlock.HvEnDetect);
    LOG_I("LvOKDetect              =   %d\r\n", interlock.LvOKDetect);
    LOG_I("ModTrigFB               =   %d\r\n", interlock.ModTrigFB);
    LOG_I("ModArcDetect            =   %d\r\n", interlock.ModArcDetect);
    LOG_I("ModHvONDetect           =   %d\r\n", interlock.ModHvONDetect);
    LOG_I("ModSumDetect            =   %d\r\n", interlock.ModSumDetect);
    LOG_I("ModTrigONDetect         =   %d\r\n", interlock.ModTrigONDetect);
    LOG_I("ModTriggerInhibitDetect =   %d\r\n", interlock.ModTriggerInhibitDetect);
    LOG_I("PulseInhibitDetect      =   %d\r\n", interlock.PulseInhibitDetect);
    LOG_I("///////////////////////////////////////////////////////////////////// \r\n");

    return 0;
}
MSH_CMD_EXPORT_ALIAS(InterlockTest, interlocktest, test interlocks);
#endif