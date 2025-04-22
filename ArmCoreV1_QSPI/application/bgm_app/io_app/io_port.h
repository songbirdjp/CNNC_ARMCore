#ifndef __IO_PORT_H__
#define __IO_PORT_H__

#include <stdint.h>

struct extend_status
{
    union
    {
        uint16_t bytes;
        struct
        {
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
        }bits;
    }current;

    uint16_t interrupt_flag;
    uint16_t interrupt_capture;
};

struct interlocks
{
    struct extend_status extend_status;

    union
    {
        uint16_t bytes;
        struct
        {
            uint16_t ModTrigFB : 1;
            uint16_t LvOKDetect : 1;
            uint16_t HvEnDetect : 1;
            uint16_t ModArcDetect : 1;
            uint16_t ModTrigOnDetect : 1;
            uint16_t ModHvOnDetect : 1;
            uint16_t ModSumDetect : 1;
            uint16_t EmergencyDetect : 1;
            uint16_t PulseInhibitDetect : 1;    /* TODO: 呼吸门控 */
            uint16_t Dose2Detect : 1;
            uint16_t Dose1Detect : 1;
            uint16_t rtc_wd_ok : 1;           /* TODO: I2C */
            uint16_t CoolingBkDetect : 1;   /* TODO: I2C */
            uint16_t reserved : 3;
        }bits;
    }detect_status;
};

struct interlocks interlock_status_get(void);

#endif  /* __IO_PORT_H__ */
