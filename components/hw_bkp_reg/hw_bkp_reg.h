#ifndef __HW_BKP_REG_H__
#define __HW_BKP_REG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum bkp_reg
{
/*RTC_BKP_DR0  (0x00u)  -> used for reboot from user cmd */ REG_REBOOT_FLAG = 0,
/*RTC_BKP_DR1  (0x01u)  -> used for reboot time count */    REG_REBOOT_TIMES,
/*RTC_BKP_DR2  (0x02u)  -> used for upgrade flag */         REG_UPGRADE_FLAG,
/*RTC_BKP_DR3  (0x03u)  -> */
/*RTC_BKP_DR4  (0x04u)  -> */
/*RTC_BKP_DR5  (0x05u)  -> */
/*RTC_BKP_DR6  (0x06u)  -> */
/*RTC_BKP_DR7  (0x07u)  -> */
/*RTC_BKP_DR8  (0x08u)  -> */
/*RTC_BKP_DR9  (0x09u)  -> */
/*RTC_BKP_DR10 (0x0Au)  -> */
/*RTC_BKP_DR11 (0x0Bu)  -> */
/*RTC_BKP_DR12 (0x0Cu)  -> */
/*RTC_BKP_DR13 (0x0Du)  -> */
/*RTC_BKP_DR14 (0x0Eu)  -> */
/*RTC_BKP_DR15 (0x0Fu)  -> */
/*RTC_BKP_DR16 (0x10u)  -> used for app here and below */
/*RTC_BKP_DR17 (0x11u)  -> */
/*RTC_BKP_DR18 (0x12u)  -> */
/*RTC_BKP_DR19 (0x13u)  -> */
/*RTC_BKP_DR20 (0x14u)  -> */
/*RTC_BKP_DR21 (0x15u)  -> */
/*RTC_BKP_DR22 (0x16u)  -> */
/*RTC_BKP_DR23 (0x17u)  -> */
/*RTC_BKP_DR24 (0x18u)  -> */
/*RTC_BKP_DR25 (0x19u)  -> */
/*RTC_BKP_DR26 (0x1Au)  -> */
/*RTC_BKP_DR27 (0x1Bu)  -> */
/*RTC_BKP_DR28 (0x1Cu)  -> */
/*RTC_BKP_DR29 (0x1Du)  -> */
/*RTC_BKP_DR30 (0x1Eu)  -> */
/*RTC_BKP_DR31 (0x1Fu)  -> */
/*RTC_BKP_DR32 (0x20u)  -> end */                           REG_VALID_MAX,
};

int8_t bkp_reg_write(enum bkp_reg reg, uint32_t value);
int8_t bkp_reg_read(enum bkp_reg reg, uint32_t *value);


#ifdef __cplusplus
}
#endif
#endif /* __HW_BKP_REG_H__ */