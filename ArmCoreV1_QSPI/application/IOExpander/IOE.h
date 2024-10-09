#ifndef __IOE_H__
#define __IOE_H__

#include <stdint.h>
#include "stm32h7xx_hal.h"

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

typedef union {
    uint16_t expandGpioData;    // 用于存储整个16位的GPIO数据
    struct {
        uint16_t HVConFBDetect : 1;
        uint16_t GatingDetect : 1;
        uint16_t MVTreatmentENDetect : 1;
        uint16_t VPSStateOPDetect : 1;
        uint16_t VPSStateFaultDetect : 1;
        uint16_t SF6LowDetect : 1;
        uint16_t SF6HighDetect : 1;
        uint16_t CoolingLv1Detect : 1;
        uint16_t nEPSStateFaultDetect : 1;
        uint16_t EPSStateOPDetect : 1;
        uint16_t WaterSW3Detect : 1;
        uint16_t WaterSW4Detect : 1;
        uint16_t WaterSW5Detect : 1;
        uint16_t WaterSW1Detect : 1;
        uint16_t WaterSW2Detect : 1;
        uint16_t CoolingLv2Detect : 1;
    } bits;               // 使用位域表示每个IO引脚的状态
} ExpandGPIOStatus_t;

int IOE_Init();
uint16_t IOE_GPIORead(void);
ExpandGPIOStatus_t IOE_ExpandGPIODataParse(uint16_t gpioDataToParse);
GPIO_PinState ReadIO_ModTrigFB();
GPIO_PinState ReadIO_LvOKDetect();
GPIO_PinState ReadIO_HvOKFB();
GPIO_PinState ReadIO_ModArcDetect();
GPIO_PinState ReadIO_ModTrigONDetect();
GPIO_PinState ReadIO_ModHvONDetect();
GPIO_PinState ReadIO_ModSumDetect();
GPIO_PinState ReadIO_Dose1Detect();
GPIO_PinState ReadIO_Dose2Detect();
GPIO_PinState ReadIO_EmergencyDetect();
GPIO_PinState ReadIO_PulseInhibitDetect();
void EPSEnable(uint8_t _epsStatus);
void VPSEnable(uint8_t _vpsStatus);
#endif
