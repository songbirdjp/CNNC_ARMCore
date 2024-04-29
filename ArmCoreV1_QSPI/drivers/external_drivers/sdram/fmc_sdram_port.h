#ifndef __FMC_SDRAM_PORT_H__
#define __FMC_SDRAM_PORT_H__

#include <stdint.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXT_SDRAM_ADDR      ((uint32_t)0xC0000000)
#define SDRAM_BANK1_ADDR    (EXT_SDRAM_ADDR)        /* TODO: need delete later */

#define SDRAM_ID_W9825G6KH
#ifdef SDRAM_ID_W9825G6KH
#define USING_MDMA_FOR_FMC
#define SDRAM_BANK1_DEVICE_NAME    "W9825G6KH"
#define SDRAM_BANK1_ADDR           EXT_SDRAM_ADDR
#define SDRAM_BANK1_SIZE           (32 * 1024 * 1024)
#endif

#define DEVICE_NAME_LENGTH      16
struct dev_sdram
{
    uint8_t name[DEVICE_NAME_LENGTH];
    uint8_t open_state;
    osMutexId_t mutex;
#ifdef USING_MDMA_FOR_FMC
    osEventFlagsId_t event;
#endif
    int8_t (*open)(struct dev_sdram *sdram);
    int8_t (*close)(struct dev_sdram *sdram);
    int8_t (*read)(struct dev_sdram *sdram, uint32_t addr, uint8_t *data, uint16_t len);
    int8_t (*write)(struct dev_sdram *sdram, uint32_t addr, uint8_t *data, uint16_t len);
    int8_t (*ioctl)(struct dev_sdram *sdram, uint8_t cmd, void *arg);
};


#ifdef __cplusplus
}
#endif

#endif /* __FMC_SDRAM_PORT_H__ */