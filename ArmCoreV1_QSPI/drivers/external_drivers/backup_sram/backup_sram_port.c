#include "backup_sram_port.h"
#include "stm32h7xx_hal.h"
#include <stdarg.h>
#include "shell.h"
#include "init_call.h"

#ifdef USING_BACKUP_SRAM
static int8_t backup_ram_clk_enable(void)
{
    __HAL_RCC_BKPRAM_CLK_ENABLE();

    return 0;
}
INIT_BOARD_EXPORT(backup_ram_clk_enable);
#endif

struct backup_sram_log
{
    uint16_t log_addr_start;    /* start addr for log record */
    uint16_t log_line_num;      /* valid log num */
    uint16_t log_addr_offset;   /* address to write log, internal use */
};


static struct backup_sram_log bkp_sram_log __attribute__((section(".bkp")));  /* at start address, to record log info */

struct backup_sram_log *backup_sram_log_info_get(void)
{
    return &bkp_sram_log;
}

int8_t backup_sram_log_info_set(uint16_t log_addr_start, uint8_t log_line_num_reset_flag)
{
    struct backup_sram_log *backup_sram_log = backup_sram_log_info_get();

    if (log_addr_start < sizeof(struct backup_sram_log))
    {
        printf("log start addr must equal or bigger than %d\r\n", sizeof(struct backup_sram_log));
        return -1;
    }

    if (log_addr_start > BACKUP_SRAM_SIZE - BYTE_LEN_PER_LINE)
    {
        printf("log start addr must less than %d\r\n", BACKUP_SRAM_SIZE - BYTE_LEN_PER_LINE);
        return -2;
    }

    backup_sram_log->log_addr_start = log_addr_start;

    if (log_line_num_reset_flag == 1)
    {
        backup_sram_log->log_line_num = 0;
        backup_sram_log->log_addr_offset = backup_sram_log->log_addr_start;

        memset((uint8_t *)backup_sram_log->log_addr_start, '\0', BACKUP_SRAM_SIZE - backup_sram_log->log_addr_start + 1);
    }

    return 0;

}

int8_t backup_sram_write(uint32_t addr_offset, uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;

    if ((addr_offset + len) >= BACKUP_SRAM_SIZE)
    {
        printf("addr + len must less than 0x%x\r\n", BACKUP_SRAM_SIZE);
        return -1;
    }

    if (buf == NULL)
    {
        printf("buf is invalid\r\n");
        return -2;
    }

    if (len == 0)
    {
        printf("write data length err\r\n");
        return -3;
    }

    memcpy((uint8_t *)(BACKUP_SRAM_ADDR_START + addr_offset), buf, len);

    return ret;
}

int8_t backup_sram_read(uint32_t addr_offset, uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;

    if ((addr_offset + len) >= BACKUP_SRAM_SIZE)
    {
        printf("addr + len must less than 0x%x\r\n", BACKUP_SRAM_SIZE);
        return -1;
    } 

    if (buf == NULL)
    {
        printf("buf is invalid\r\n");
        return -2;
    }

    if (len == 0)
    {
        printf("write data length err\r\n");
        return -3;
    }

    memcpy(buf, (uint8_t *)(BACKUP_SRAM_ADDR_START + addr_offset), len);

    return ret;
}

static void backup_sram_addr_offset_update(uint8_t *buf, uint16_t len)
{
    struct backup_sram_log *backup_sram_log = backup_sram_log_info_get();

    backup_sram_log->log_addr_offset += BYTE_LEN_PER_LINE;

    if (backup_sram_log->log_line_num < (BACKUP_SRAM_SIZE - backup_sram_log->log_addr_start) / BYTE_LEN_PER_LINE)
    {
        backup_sram_log->log_line_num++;
    }
}

int8_t backup_sram_log_write(uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;
    struct backup_sram_log *backup_sram_log = backup_sram_log_info_get();

    if (buf == NULL)
    {
        printf("buf is invalid\r\n");
        return -2;
    }

    if (len > BYTE_LEN_PER_LINE)
    {
        printf("write len err:%d\r\n", len);
        return -3;
    }

    if ((backup_sram_log->log_addr_offset + BYTE_LEN_PER_LINE) >= BACKUP_SRAM_SIZE)
    {
        backup_sram_log->log_addr_offset = backup_sram_log->log_addr_start;
        backup_sram_log->log_line_num = (BACKUP_SRAM_SIZE - backup_sram_log->log_addr_start) / BYTE_LEN_PER_LINE;
    }

    // memset((uint8_t *)(BACKUP_SRAM_ADDR_START + backup_sram_log->log_addr_offset), '\0', BYTE_LEN_PER_LINE);

    ret = backup_sram_write(backup_sram_log->log_addr_offset, buf, len);
    if (ret == 0)
    {
        backup_sram_addr_offset_update(buf, len);
    }

    return ret;
}

int8_t backup_sram_log_read(uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;
    struct backup_sram_log *backup_sram_log = backup_sram_log_info_get();

    if (buf == NULL)
    {
        printf("buf is invalid\r\n");
        return -2;
    }

    if (len == 0)
    {
        printf("write data length err\r\n");
        return -3;
    }

    memcpy(buf, (uint8_t *)backup_sram_log->log_addr_start, len);

    return ret;
}

int8_t backup_sram_log_console_output(void)
{
    struct backup_sram_log *backup_sram_log = backup_sram_log_info_get();
    uint8_t log_buf[BYTE_LEN_PER_LINE] = {'\0'};

    if (backup_sram_log->log_line_num == 0)
    {
        printf("there is no valid log\r\n");
        return -1;
    }

    for (uint16_t i = 0; i < backup_sram_log->log_line_num; i++)
    {
        backup_sram_read(backup_sram_log->log_addr_start + BYTE_LEN_PER_LINE * i, log_buf, sizeof(log_buf));
        printf("%s", log_buf);
    }

    return 0;
}

void bkp_sram_log_printf(const char *fmt, ...)
{
    static char log_msg[BYTE_LEN_PER_LINE];
    int ret = 0;

    va_list ap;
    va_start(ap, fmt);

    memset(log_msg, '\0', BYTE_LEN_PER_LINE);
    ret = vsnprintf(log_msg, BYTE_LEN_PER_LINE, fmt, ap);
    va_end(ap);

    if (ret < 0)
    {
        printf("vsnprintf err:%d\r\n", ret);
        return;
    }

    ret = backup_sram_log_write(log_msg, ret + 1);  /* write '\0' to the end */
    if (ret != 0)
    {
        printf("backup_sram_log_write err:%d\r\n", ret);
    }
}

void bkp_sram_log_test(void)
{
    int8_t ret = 0;

    struct backup_sram_log *bkp_sram_log = backup_sram_log_info_get();
    printf("addr_start:%x, line_num:%d, addr_offset:%x\r\n", bkp_sram_log->log_addr_start, bkp_sram_log->log_line_num, bkp_sram_log->log_addr_offset);
    
    ret = backup_sram_log_info_set(8, 1);
    if (ret != 0)
    {
        printf("backup_sram_log_info_set err:%d\r\n", ret);
    }

    // ret = backup_sram_log_console_output();
    // if (ret != 0)
    // {
    //     printf("backup_sram_log_console_output err:%d\r\n", ret);
    // }

    for (uint8_t i = 0; i < 10; i++)
    {
        bkp_sram_log_printf("this is a test demo:%s %s %d\r\n", __DATE__, __TIME__, i);
        bkp_sram_log_printf("hello world:%s %s %d\r\n", __DATE__, __TIME__, i);
    }

    ret = backup_sram_log_console_output();
    if (ret != 0)
    {
        printf("backup_sram_log_console_output err:%d\r\n", ret);
    }

}
MSH_CMD_EXPORT_ALIAS(bkp_sram_log_test, bkp_sram_log_test, bkp_sram log record test);

