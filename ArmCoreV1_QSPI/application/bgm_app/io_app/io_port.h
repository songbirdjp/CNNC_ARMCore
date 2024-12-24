#ifndef __IO_PORT_H__
#define __IO_PORT_H__

#include <stdint.h>

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

typedef struct {
     ExpandGPIOStatus_t exGPIODetect;
     uint8_t LvOKDetect;
     uint8_t HvEnDetect;
     uint8_t ModTrigFB;
     uint8_t ModArcDetect;
     uint8_t ModHvONDetect;
     uint8_t ModSumDetect;
     uint8_t ModTrigONDetect;
     uint8_t EmergencyDetect;
     uint8_t PulseInhibitDetect;
     uint8_t ModTriggerInhibitDetect;
     uint8_t Dose1Detect;
     uint8_t Dose2Detect;
} InterlocksDetect_t;

InterlocksDetect_t interlock_status_get(void);

#endif  /* __IO_PORT_H__ */
