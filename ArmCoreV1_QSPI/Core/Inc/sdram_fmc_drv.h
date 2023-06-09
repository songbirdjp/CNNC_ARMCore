//
// Created by caijiaheng on 2023/5/9.
//
#ifndef ETHERCAT_CNNCPM_SDRAM_FMC_DRV_H
#define ETHERCAT_CNNCPM_SDRAM_FMC_DRV_H

//#include "fmc.h"

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

#define EXT_SDRAM_ADDR  	((uint32_t)0xC0000000)
#define EXT_SDRAM_SIZE		(32 * 1024 * 1024)
#define SDRAM_BANK_SIZE     (8 * 1024 * 1024)

#define SDRAM_BANK1_ADDR (EXT_SDRAM_ADDR)
#define SDRAM_BANK2_ADDR (EXT_SDRAM_ADDR + SDRAM_BANK_SIZE)
#define SDRAM_BANK3_ADDR (EXT_SDRAM_ADDR + SDRAM_BANK_SIZE * 2)
#define SDRAM_BANK4_ADDR (EXT_SDRAM_ADDR + SDRAM_BANK_SIZE * 3)
#define buffer_size 1024
uint32_t bsp_TestExtSDRAM(void);
void SDRAM_Init(void);
static int SDRAM_SendCommand(uint32_t CommandMode, uint32_t Bank, uint32_t RefreshNum, uint32_t RegVal);
void sdram_read_write_example(void);
#endif //ETHERCAT_CNNCPM_SDRAM_FMC_DRV_H
