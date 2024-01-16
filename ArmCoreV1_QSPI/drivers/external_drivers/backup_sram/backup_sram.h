#ifndef __BACKUP_SRAM_H__
#define __BACKUP_SRAM_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C"{
#endif

#define USING_BACKUP_SRAM
#ifdef USING_BACKUP_SRAM

#define BACKUP_SRAM_SIZE        0x1000  /* 4KB */
#define BACKUP_SRAM_ADDR_START  D3_BKPSRAM_BASE
#define BACKUP_SRAM_ADDR_END    (BACKUP_SRAM_ADDR_START + BACKUP_SRAM_SIZE)

#define BYTE_LEN_PER_LINE       64  /* char num + '\0' must less than BYTE_LEN_PER_LINE */

void backup_ram_clk_enable(void);
struct backup_sram_log *backup_sram_log_info_get(void);
int8_t backup_sram_log_info_set(uint16_t log_addr_start, uint8_t log_line_num_reset_flag);
int8_t backup_sram_write(uint32_t addr_offset, uint8_t *buf, uint16_t len);
int8_t backup_sram_read(uint32_t addr_offset, uint8_t *buf, uint16_t len);
int8_t backup_sram_log_write(uint8_t *buf, uint16_t len);
int8_t backup_sram_log_read(uint8_t *buf, uint16_t len);
int8_t backup_sram_log_console_output(void);
void bkp_sram_log_test(void);
void bkp_sram_log_printf(const char *fmt, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif
