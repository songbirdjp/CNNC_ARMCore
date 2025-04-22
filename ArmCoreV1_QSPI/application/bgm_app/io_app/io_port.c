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
#define IOE_RegAddr_IOCON_A           0x0A   // Configuration Register (shared for both ports)
#define IOE_RegAddr_IOCON_B           0x0B   // Configuration Register (shared for both ports)
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

#define GPIO_EXTEND_EVENT   (1 << 0)
static osEventFlagsId_t gpio_event = NULL;
static osMutexId_t gpio_mutex = NULL;

static struct interlocks gpio_info = {0};
static struct interlocks *gpio_info_get(void)
{
    return &gpio_info;
}

#define SPI_COMMUNICATION_TIMEOUT_MS 5000
static int8_t ioe_config(void)
{
#if 0
    struct reg_cfg
    {
        uint8_t dir[2];
        uint8_t pol[2];
        uint8_t inten[2];
        uint8_t defval[2];
        uint8_t intcon[2];
        uint8_t iocon[2];
        uint8_t gppu[2]
    };
    
    struct reg_cfg reg_cfg = 
    {
        .dir = {0xFF, 0xFF},
        .pol = {0x00, 0x00},
        .inten = {0xFF, 0xFF},
        .defval = {0xFF, 0xFF},
        .intcon = {0x00, 0x00},
        .iocon = {0x50, 0x50},
        .gppu = {0xFF, 0xFF}
    }, recv_cfg = {0};

    HAL_StatusTypeDef status = HAL_OK;
    uint8_t cmd_buf[2 + sizeof(struct reg_cfg)] = {0}, recv_buf[2 + sizeof(struct reg_cfg)] = {0};

    /* 1. configure registers */
    cmd_buf[0] = IOE_OPCode_Write;
    cmd_buf[1] = IOE_RegAddr_IODIR_A;
    memcpy(&cmd_buf[2], &reg_cfg, sizeof(struct reg_cfg));
    status = HAL_SPI_Transmit(&hspi2, cmd_buf, sizeof(cmd_buf), SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_Transmit err: %d\r\n",status);
        return -1;
    }

    /* 2. read back registers */
    cmd_buf[0] = IOE_OPCode_Read;
    cmd_buf[1] = IOE_RegAddr_IODIR_A;
    status = HAL_SPI_TransmitReceive(&hspi2, cmd_buf, recv_buf, sizeof(recv_buf), SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_TransmitReceive err: %d\r\n",status);
        return -2;
    }
    memcpy(&recv_cfg, &recv_buf[2], sizeof(struct reg_cfg));

    /* 3. check if registers are correct */
    if (memcmp(&reg_cfg, &recv_cfg, sizeof(struct reg_cfg)) != 0)
    {
        LOG_E("IOE config error\r\n");
        return -3;
    }

#else
    uint8_t cmd_buf[4] = {0}, recv_buf[4] = {0};
    HAL_StatusTypeDef status = HAL_OK;
    /* 1. set gpio configuration */
    cmd_buf[0] = IOE_OPCode_Write;
    cmd_buf[1] = IOE_RegAddr_IOCON_A;
    cmd_buf[2] = 0x50;
    cmd_buf[3] = 0x50;
    status = HAL_SPI_Transmit(&hspi2, cmd_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_Transmit err: %d\r\n",status);
        return -1;
    }

    cmd_buf[0] = IOE_OPCode_Read;
    cmd_buf[1] = IOE_RegAddr_IOCON_A;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = 0x00;
    status = HAL_SPI_TransmitReceive(&hspi2, cmd_buf, recv_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_TransmitReceive err: %d\r\n",status);
        return -1;
    }
    if(recv_buf[2] != 0x50 || recv_buf[3] != 0x50)
    {

        LOG_E("IOE_RegAddr_IOCON_A: %#.2x  IOE_RegAddr_IOCON_B: %#.2x\r\n", recv_buf[2], recv_buf[3]);
        return -1;
    }

    /* 2. set gpio direction */
    cmd_buf[0] = IOE_OPCode_Write;
    cmd_buf[1] = IOE_RegAddr_IODIR_A;
    cmd_buf[2] = 0xFF;
    cmd_buf[3] = 0xFF;
    status = HAL_SPI_Transmit(&hspi2, cmd_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_Transmit err: %d\r\n",status);
        return -2;
    }

    cmd_buf[0] = IOE_OPCode_Read;
    cmd_buf[1] = IOE_RegAddr_IODIR_A;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = 0x00;
    status = HAL_SPI_TransmitReceive(&hspi2, cmd_buf, recv_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_TransmitReceive err: %d\r\n",status);
        return -2;
    }
    if(recv_buf[2] != 0xFF || recv_buf[3] != 0xFF)
    {
        LOG_E("IOE_RegAddr_IODIR_A: %#.2x  IOE_RegAddr_IODIR_B: %#.2x\r\n", recv_buf[2], recv_buf[3]);
        return -2;
    }

    /* 3. set gpio polarity */
    cmd_buf[0] = IOE_OPCode_Write;
    cmd_buf[1] = IOE_RegAddr_IPOL_A;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = 0x00;
    status = HAL_SPI_Transmit(&hspi2, cmd_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_Transmit err: %d\r\n",status);
        return -3;
    }

    cmd_buf[0] = IOE_OPCode_Read;
    cmd_buf[1] = IOE_RegAddr_IPOL_A;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = 0x00;
    status = HAL_SPI_TransmitReceive(&hspi2, cmd_buf, recv_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_TransmitReceive err: %d\r\n",status);
        return -3;
    }
    if(recv_buf[2] != 0x00 || recv_buf[3] != 0x00)
    {
        LOG_E("IOE_RegAddr_IPOL_A: %#.2x  IOE_RegAddr_IPOL_B: %#.2x\r\n", recv_buf[2], recv_buf[3]);
        return -3;
    }

    /* 4. set gpio pull-up */
    cmd_buf[0] = IOE_OPCode_Write;
    cmd_buf[1] = IOE_RegAddr_GPPU_A;
    cmd_buf[2] = 0xFF;
    cmd_buf[3] = 0xFF;
    status = HAL_SPI_Transmit(&hspi2, cmd_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_Transmit err: %d\r\n",status);
        return -4;
    }

    cmd_buf[0] = IOE_OPCode_Read;
    cmd_buf[1] = IOE_RegAddr_GPPU_A;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = 0x00;
    status = HAL_SPI_TransmitReceive(&hspi2, cmd_buf, recv_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_TransmitReceive err: %d\r\n",status);
        return -4;
    }
    if(recv_buf[2] != 0xFF || recv_buf[3] != 0xFF)
    {
        LOG_E("IOE_RegAddr_GPPU_A: %#.2x  IOE_RegAddr_GPPU_B: %#.2x\r\n", recv_buf[2], recv_buf[3]);
        return -4;
    }

    /* 5. set gpio interrupt */
    /* 5.1 set gpio interrupt control */
    cmd_buf[0] = IOE_OPCode_Write;
    cmd_buf[1] = IOE_RegAddr_INTCON_A;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = 0x00;
    status = HAL_SPI_Transmit(&hspi2, cmd_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_Transmit err: %d\r\n",status);
        return -5;
    }

    cmd_buf[0] = IOE_OPCode_Read;
    cmd_buf[1] = IOE_RegAddr_INTCON_A;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = 0x00;
    status = HAL_SPI_TransmitReceive(&hspi2, cmd_buf, recv_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_TransmitReceive err: %d\r\n",status);
        return -5;
    }
    if(recv_buf[2] != 0x00 || recv_buf[3] != 0x00)
    {
        LOG_E("IOE_RegAddr_INTCON_A: %#.2x  IOE_RegAddr_INTCON_B: %#.2x\r\n", recv_buf[2], recv_buf[3]);
        return -5;
    }

    /* 5.2 set gpio default compare value */
    cmd_buf[0] = IOE_OPCode_Write;
    cmd_buf[1] = IOE_RegAddr_DEFVAL_A;
    cmd_buf[2] = 0xFF;
    cmd_buf[3] = 0xFF;
    status = HAL_SPI_Transmit(&hspi2, cmd_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_Transmit err: %d\r\n",status);
        return -5;
    }

    cmd_buf[0] = IOE_OPCode_Read;
    cmd_buf[1] = IOE_RegAddr_DEFVAL_A;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = 0x00;
    status = HAL_SPI_TransmitReceive(&hspi2, cmd_buf, recv_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_TransmitReceive err: %d\r\n",status);
        return -5;
    }
    if(recv_buf[2] != 0xFF || recv_buf[3] != 0xFF)
    {
        LOG_E("IOE_RegAddr_DEFVAL_A: %#.2x  IOE_RegAddr_DEFVAL_B: %#.2x\r\n", recv_buf[2], recv_buf[3]);
        return -5;
    }

    /* 5.3 set gpio interrupt enable */
    cmd_buf[0] = IOE_OPCode_Write;
    cmd_buf[1] = IOE_RegAddr_GPINTEN_A;
    cmd_buf[2] = 0xFF;
    cmd_buf[3] = 0xFF;
    status = HAL_SPI_Transmit(&hspi2, cmd_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_Transmit err: %d\r\n",status);
        return -5;
    }

    cmd_buf[0] = IOE_OPCode_Read;
    cmd_buf[1] = IOE_RegAddr_GPINTEN_A;
    cmd_buf[2] = 0x00;
    cmd_buf[3] = 0x00;
    status = HAL_SPI_TransmitReceive(&hspi2, cmd_buf, recv_buf, 4, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_TransmitReceive err: %d\r\n",status);
        return -5;
    }
    if(recv_buf[2] != 0xFF || recv_buf[3] != 0xFF)
    {
        LOG_E("IOE_RegAddr_GPINTEN_A: %#.2x  IOE_RegAddr_GPINTEN_B: %#.2x\r\n", recv_buf[2], recv_buf[3]);
        return -5;
    }
#endif

    return 0;
}
static int8_t ioe_read(struct extend_status *stat)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t cmd_buf[8] = {0}, recv_buf[8] = {0};

    cmd_buf[0] = IOE_OPCode_Read;
    cmd_buf[1] = IOE_RegAddr_INTF_A;

    status = HAL_SPI_TransmitReceive(&hspi2, cmd_buf, recv_buf, 8, SPI_COMMUNICATION_TIMEOUT_MS);
    if(status != HAL_OK)
    {
        LOG_E("HAL_SPI_Transmit err: %d\r\n",status);
        return -1;
    }

    uint16_t interrupt_flag = recv_buf[2] | recv_buf[3] << 8;
    uint16_t interrupt_capture = recv_buf[4] | recv_buf[5] << 8;

    /* here maintain the interrupt flag and capture value which latest changed */
    if (interrupt_flag != 0)
    {
        stat->interrupt_flag = interrupt_flag;
        stat->interrupt_capture = interrupt_capture;
    }

    stat->current.bytes = recv_buf[6] | recv_buf[7] << 8;

#if 0
    LOG_I("read extend status: ");
    for (int i = 0; i < 8; i++)
    {
        LOG_I("%#.2x ", recv_buf[i]);
    }
    LOG_I("\r\n");
#endif

    // LOG_I("interrupt_flag: %#.4x\r\n", stat->interrupt_flag);
    // LOG_I("interrupt_capture: %#.4x\r\n", stat->interrupt_capture);
    // LOG_I("current: %#.4x\r\n", stat->current.bytes);

    return 0;
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

    return 0;
}
static void ioe_irq_callback(void)
{
    osEventFlagsSet(gpio_event, GPIO_EXTEND_EVENT);
}

static GPIO_PinState ReadIO_ModTrigFB(void)
{
    return gpio_common_get()->read("GPIOB_11");
}
static GPIO_PinState ReadIO_LvOKDetect(void)
{
    return gpio_common_get()->read("GPIOB_10");
}
static GPIO_PinState ReadIO_HvEnDetect(void)
{
    return gpio_common_get()->read("GPIOA_3");
}
static GPIO_PinState ReadIO_ModArcDetect(void)
{
    return gpio_common_get()->read("GPIOB_1");
}
static GPIO_PinState ReadIO_ModTrigOnDetect(void)
{
    return gpio_common_get()->read("GPIOB_0");
}
static GPIO_PinState ReadIO_ModHvOnDetect(void)
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
    struct interlocks *obj = gpio_info_get();

    osMutexId_t mutex = (osMutexId_t)argument;
    struct interlocks interlocks = {0};

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

    #define GPIO_POLLING_TIME_MS 50
    for (;;)
    {
        osEventFlagsWait(gpio_event, GPIO_EXTEND_EVENT, osFlagsWaitAny, GPIO_POLLING_TIME_MS);

        ret = ioe_read(&interlocks.extend_status);
        if (ret != 0)
        {
            LOG_E("ioe_read err: %d\r\n",ret);
        }

        interlocks.detect_status.bits.ModTrigFB  = ReadIO_ModTrigFB();
        interlocks.detect_status.bits.LvOKDetect  = ReadIO_LvOKDetect();
        interlocks.detect_status.bits.HvEnDetect  = ReadIO_HvEnDetect();
        interlocks.detect_status.bits.ModArcDetect  = ReadIO_ModArcDetect();
        interlocks.detect_status.bits.ModTrigOnDetect  = ReadIO_ModTrigOnDetect();

        interlocks.detect_status.bits.ModHvOnDetect = ReadIO_ModHvOnDetect();
        interlocks.detect_status.bits.ModSumDetect  = ReadIO_ModSumDetect();
        interlocks.detect_status.bits.Dose1Detect  = ReadIO_Dose1Detect();
        interlocks.detect_status.bits.Dose2Detect  = ReadIO_Dose2Detect();
        interlocks.detect_status.bits.EmergencyDetect  = ReadIO_EmergencyDetect();

        osMutexAcquire(mutex, osWaitForever);
        memcpy(obj, &interlocks, sizeof(struct interlocks));
        osMutexRelease(mutex);
    }

    return 0;
}

static int8_t io_thread_init(void)
{
    gpio_event = osEventFlagsNew(NULL);
    if (gpio_event == NULL)
    {
        printf("event create failed\r\n");
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

    osThreadId_t GPIOThreadHandle = osThreadNew(gpio_thread_entry, gpio_mutex, &GPIOThread_attributes);
    if (GPIOThreadHandle == NULL)
    {
        printf("thread GPIOThread create failed\r\n");
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(io_thread_init);

struct interlocks interlock_status_get(void)
{
    struct interlocks interlock = {0};
    struct interlocks *obj = gpio_info_get();

    osMutexAcquire(gpio_mutex, osWaitForever);
    memcpy(&interlock, obj, sizeof(struct interlocks));
    osMutexRelease(gpio_mutex);

    return interlock;
}


#ifndef INTERLOCK_TEST
#include "shell.h"
static int8_t interlock_status_test(uint8_t argc, char *argv[])
{
    struct interlocks interlock = interlock_status_get();

    LOG_I("//////////////////////BGM Interlocks Status//////////////////////// \r\n");
    LOG_I("interrupt_flag          =   %#.4x\r\n", interlock.extend_status.interrupt_flag);
    LOG_I("interrupt_capture       =   %#.4x\r\n", interlock.extend_status.interrupt_capture);
    LOG_I("current                 =   %#.4x\r\n", interlock.extend_status.current.bytes);
    LOG_I("CoolingLv1Detect        =   %d\r\n", interlock.extend_status.current.bits.CoolingLv1Detect);
    LOG_I("CoolingLv2Detect        =   %d\r\n", interlock.extend_status.current.bits.CoolingLv2Detect);
    LOG_I("WaterSW1Detect          =   %d\r\n", interlock.extend_status.current.bits.WaterSW1Detect);
    LOG_I("WaterSW2Detect          =   %d\r\n", interlock.extend_status.current.bits.WaterSW2Detect);
    LOG_I("WaterSW3Detect          =   %d\r\n", interlock.extend_status.current.bits.WaterSW3Detect);
    LOG_I("WaterSW4Detect          =   %d\r\n", interlock.extend_status.current.bits.WaterSW4Detect);
    LOG_I("WaterSW5Detect          =   %d\r\n", interlock.extend_status.current.bits.WaterSW5Detect);
    LOG_I("SF6HighDetect           =   %d\r\n", interlock.extend_status.current.bits.SF6HighDetect);
    LOG_I("SF6LowDetect            =   %d\r\n", interlock.extend_status.current.bits.SF6LowDetect);
    LOG_I("EPSStateOPDetect        =   %d\r\n", interlock.extend_status.current.bits.EPSStateOPDetect);
    LOG_I("nEPSStateFaultDetect    =   %d\r\n", interlock.extend_status.current.bits.nEPSStateFaultDetect);
    LOG_I("VPSStateFaultDetect     =   %d\r\n", interlock.extend_status.current.bits.VPSStateFaultDetect);
    LOG_I("VPSStateOPDetect        =   %d\r\n", interlock.extend_status.current.bits.VPSStateOPDetect);
    LOG_I("GatingDetect            =   %d\r\n", interlock.extend_status.current.bits.GatingDetect);
    LOG_I("HVConFBDetect           =   %d\r\n", interlock.extend_status.current.bits.HVConFBDetect);
    LOG_I("MVTreatmentENDetect     =   %d\r\n", interlock.extend_status.current.bits.MVTreatmentENDetect);
    LOG_I("Dose1Detect             =   %d\r\n", interlock.detect_status.bits.Dose1Detect);
    LOG_I("Dose2Detect             =   %d\r\n", interlock.detect_status.bits.Dose2Detect);
    LOG_I("EmergencyDetect         =   %d\r\n", interlock.detect_status.bits.EmergencyDetect);
    LOG_I("HvEnDetect              =   %d\r\n", interlock.detect_status.bits.HvEnDetect);
    LOG_I("LvOKDetect              =   %d\r\n", interlock.detect_status.bits.LvOKDetect);
    LOG_I("ModTrigFB               =   %d\r\n", interlock.detect_status.bits.ModTrigFB);
    LOG_I("ModArcDetect            =   %d\r\n", interlock.detect_status.bits.ModArcDetect);
    LOG_I("ModHvONDetect           =   %d\r\n", interlock.detect_status.bits.ModHvOnDetect);
    LOG_I("ModSumDetect            =   %d\r\n", interlock.detect_status.bits.ModSumDetect);
    LOG_I("ModTrigONDetect         =   %d\r\n", interlock.detect_status.bits.ModTrigOnDetect);
    LOG_I("PulseInhibitDetect      =   %d\r\n", interlock.detect_status.bits.PulseInhibitDetect);
    LOG_I("///////////////////////////////////////////////////////////////////// \r\n");

    return 0;
}
MSH_CMD_EXPORT_ALIAS(interlock_status_test, interlock_status_test, test interlock status);
#endif