#ifndef __HW_RAMECC_H__
#define __HW_RAMECC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ram_ecc_type
{
    RAM_ECC_AXI = 0,
    RAM_ECC_ITCM,
    RAM_ECC_D0TCM,
    RAM_ECC_D1TCM,
    RAM_ECC_AXI_SHARED,
    RAM_ECC_SRAM1,
    RAM_ECC_SRAM2,
    RAM_ECC_FDCAN,
    RAM_ECC_SRAM4,
    RAM_ECC_BKP,
    RAM_ECC_MAX,
};

int8_t hw_ram_ecc_init_func_callback_register(enum ram_ecc_type ram_ecc, int8_t (*callback)(void *arg));
int8_t hw_ram_ecc_double_err_callback_register(enum ram_ecc_type ram_ecc, int8_t (*callback)(void *arg));

#ifdef __cplusplus
}
#endif

#endif /* __HW_RAMECC_H__ */