#ifndef __HW_CRC_H__
#define __HW_CRC_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

#define USING_HARDWARE_CRC_CALCULATE

enum hardware_crc_default
{
    CRC8 = 0,
    CRC16,
    CRC32,
    CRC_MAX
};

uint32_t hardware_crc_calculate(enum hardware_crc_default index, uint8_t pBuffer[], uint32_t size);


#ifdef __cplusplus
}
#endif

#endif /* __HW_CRC_H__ */