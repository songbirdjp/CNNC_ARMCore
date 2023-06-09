#ifndef STM32F401_GLOBAL_H
#define STM32F401_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern uint32_t KeyVal;



typedef union
{
    uint32_t Val;
    uint8_t v[4];
    uint16_t w[2];
    struct
    {
        uint8_t LB;
        uint8_t HB;
        uint8_t UB;
        uint8_t MB;
    } byte;
} UINT32_VAL1;

typedef union
{
    uint16_t Val;
    struct
    {
        uint8_t LB;
        uint8_t HB;
    } byte;
} UINT16_VAL1;



#define LAN9252_OSPI_ENABLE         0x38
#define LAN9252_OSPI_RESET          0xFF
#define LAN9252_OSPI_ReadID         0x50
#define LAN9252_OSPI_ByteTest       0x64

#define CMD_SERIAL_READ             0x03
#define CMD_FAST_READ               0x0B
#define CMD_DUAL_OP_READ            0x3B
#define CMD_DUAL_IO_READ            0xBB
#define CMD_QUAD_OP_READ            0x6B
#define CMD_QUAD_IO_READ            0xEB
#define CMD_SERIAL_WRITE            0x02
#define CMD_DUAL_DATA_WRITE         0x32
#define CMD_DUAL_ADDR_DATA_WRITE    0xB2
#define CMD_QUAD_DATA_WRITE         0x62
#define CMD_QUAD_ADDR_DARA_WRITE    0xE2

#define CMD_SERIAL_READ_DUMMY 0
#define CMD_FAST_READ_DUMMY 1
#define CMD_DUAL_OP_READ_DUMMY 1
#define CMD_DUAL_IO_READ_DUMMY 2
#define CMD_QUAD_OP_READ_DUMMY 1
#define CMD_QUAD_IO_READ_DUMMY 4
#define CMD_SERIAL_WRITE_DUMMY 0
#define CMD_DUAL_DATA_WRITE_DUMMY 0
#define CMD_DUAL_ADDR_DATA_WRITE_DUMMY 0
#define CMD_QUAD_DATA_WRITE_DUMMY 0
#define CMD_QUAD_ADDR_DARA_WRITE_DUMMY 0

#define ESC_CSR_CMD_REG		0x304
#define ESC_CSR_DATA_REG	0x300
#define ESC_WRITE_BYTE 		0x80
#define ESC_READ_BYTE 		0xC0
#define ESC_CSR_BUSY		0x80
#endif //STM32F401_GLOBAL_H
