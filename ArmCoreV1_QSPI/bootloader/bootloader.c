#include "bootloader.h"
#include "stm32h7xx_hal.h"

/* 
 *  1. Set the vector table offset to the specified value.
 *  2. modify the xxx_FLASH.ld file -> MEMORY item -> flash origin and length. origin addr must multiple of 0x400
 */

int8_t vector_table_offset_set(uint32_t offset)
{
    if (offset % 0x400 != 0)    /* must be a multiple of 0x400 */
    {
        return -1;
    }

    SCB->VTOR = FLASH_BASE | offset;

    return 0;
}

uint32_t vector_table_offset_get(void)
{
    return SCB->VTOR;
}


/* 
 * note: normal power on reset
 * 1. bootloader must encryption 
 * 2. check code info crc & code crc
 * 3. jump to application
 * 
 * note: upgrade reset
 * 1. first: write data to flash
 * 2. second: calculate & write uid cryptogram to flash
 * 3. third: calculate crc of code
 * 4. fourth: calculate crc of info and write it to flash
 * 5. finally: clear flag and reset
*/

/*
* bank1:
* sector 0: 0x08000000 - 0x0801FFFF (128k)  -> bootloader 1
* sector 1: 0x08020000 - 0x0803FFFF (128k)  -> bootloader 2
* sector 2: 0x08040000 - 0x0805FFFF (128k)  -> application
* sector 3: 0x08060000 - 0x0807FFFF (128k)  -> reserved
* sector 4: 0x08080000 - 0x0809FFFF (128k)  -> reserved
* sector 5: 0x080A0000 - 0x080BFFFF (128k)  -> config data, such as log info, etc.
* sector 6: 0x080C0000 - 0x080DFFFF (128k)  -> log 1
* sector 7: 0x080E0000 - 0x080FFFFF (128k)  -> log 2
*/

#define BOOT_LOADER_MAX_SIZE    (FLASH_SECTOR_SIZE * 2)
#define FW_BIN_INFO_MAX_SIZE    0x400
#define FW_BIN_INFO_OFFSET      BOOT_LOADER_MAX_SIZE
#define FW_BIN_INFO_ADDR_BASE   (FLASH_BASE + FW_BIN_INFO_OFFSET)
//#define FW_BIN_APP_ADDR_BASE    (FW_BIN_INFO_ADDR_BASE + FW_BIN_INFO_LEN)   /* Vector Table base address must be a multiple of 0x400 */
#define FW_MAGIC_NUMBER         0x411fc272

enum code_type
{
    CODE_TYPE_FULL = 0,
    CODE_TYPE_DIFF
};

struct patch_info
{
    uint32_t patch_addr;
    uint32_t patch_len;
    uint32_t patch_crc;
};

struct code_info
{
    uint32_t magic_number;          /* 标识文件类型 */
    uint8_t  info_version[4];       /* info版本 */
    uint32_t info_len;              /* info长度 */
    uint32_t patch_flag;            /* 补丁标志 */
    struct patch_info patch[4];     /* 补丁信息 */
    uint8_t  code_name[16];         /* code名称 */
    uint32_t code_type;             /* code类型：0：全包  1：差分包 */
    uint32_t code_offset;           /* code起始地址，相对于info的偏移 */
    uint32_t boot_offset;           /* 引导程序起始地址，相对于code的偏移 */    
    uint32_t code_size;             /* code大小 */
    uint32_t code_crc;              /* code crc */
    uint32_t info_crc;              /* info crc */
};

#include "crc.h"
static int8_t info_magic_number_check(uint32_t magic_number)
{
    // uint32_t cpuid = *((volatile uint32_t *)0xE000ed00);    /* 0x411fc272 */
    if (magic_number != FW_MAGIC_NUMBER)
    {
        return -1;
    }

    return 0;
}

static int8_t info_len_check(uint32_t size)
{
    if (size > FW_BIN_INFO_MAX_SIZE)
    {
        return -1;
    }

    return 0;
}

static int8_t info_crc_check(struct code_info *info)
{
    if (info == NULL)
    {
        return -1;
    }

    HAL_StatusTypeDef status = hardware_crc_config(CRC32);
    if (status != HAL_OK)
    {
        printf("hardware_crc_config error:%d\r\n", status);
        return -2;
    }

    uint32_t res = hardware_crc_calculate(info, info->info_len - sizeof (info->info_crc));
    res ^= 0xFFFFFFFF;

    if (res != info->info_crc)
    {
        printf("info crc check err, cal_crc = %#.8x, info crc = %#.8x\r\n", res, info->info_crc);
        return -3;
    }

    return 0;
}

static int8_t info_code_type_check(uint32_t type)
{
    int8_t ret = 0;

    switch (type)
    {
    case CODE_TYPE_FULL:
        ret = 0;
        break;
    case CODE_TYPE_DIFF:
        ret = 1;
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}

static int8_t info_code_offset_check(uint32_t offset)
{
    if (offset != FW_BIN_INFO_MAX_SIZE)
    {
        return -1;
    }

    return 0;
}

static int8_t info_boot_offset_check(uint32_t offset, uint32_t size)
{
    if (offset >= size)
    {
        return -1;
    }

    return 0;
}

static int8_t info_code_size_check(uint32_t size)
{
    if (size == 0 || size > (FLASH_SIZE - BOOT_LOADER_MAX_SIZE - FW_BIN_INFO_MAX_SIZE))
    {
        return -1;
    }

    return 0;
}

static int8_t flash_crc_calculate(uint32_t offset, uint32_t size, uint32_t *crc)
{
    if (crc == NULL || (offset + size) > FLASH_SIZE)
    {
        return -1;
    }

    FLASH_CRCInitTypeDef CRCInitTypeDef = {0};
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
        printf("HAL_FLASH_Unlock error\r\n");
        return -2;
    }

    CRCInitTypeDef.TypeCRC = FLASH_CRC_ADDR;
    CRCInitTypeDef.BurstSize = FLASH_CRC_BURST_SIZE_4;
    CRCInitTypeDef.Bank = FLASH_BANK_1;
    // CRCInitTypeDef.Sector = FLASH_SECTOR_0;
    // CRCInitTypeDef.NbSectors = 1;
    CRCInitTypeDef.CRCStartAddr = FLASH_BASE + offset;
    CRCInitTypeDef.CRCEndAddr = FLASH_BASE + offset + size - 1; /* 共1024个字节进行计算，硬件按512字节对齐计算，而非128字节 */
    status = HAL_FLASHEx_ComputeCRC(&CRCInitTypeDef, crc);
    if (status != HAL_OK)
    {
        printf("HAL_FLASHEx_ComputeCRC error\r\n");
        return -3;
    }
    
    status = HAL_FLASH_Lock();
    if (status != HAL_OK)
    {
        printf("HAL_FLASH_Lock error\r\n");
        return -4;
    }

    return 0;
}

static int8_t info_code_crc_check(struct code_info *info)
{
    if (info == NULL)
    {
        return -1;
    }

    uint32_t crc = 0;

    int8_t ret = flash_crc_calculate(FW_BIN_INFO_OFFSET + info->code_offset, info->code_size, &crc);

    if (info->code_crc != crc)
    {
        printf("fw crc check err\r\n");
        return -2;
    }

    return 0;
}

static int8_t fw_info_check(struct code_info *info)
{
    if (info == NULL)
    {
        return -1;
    }

    int8_t ret = 0;

    /* 1. check magic number */
    ret = info_magic_number_check(info->magic_number);
    if (ret != 0)
    {
        printf("magic number check err\r\n");
        return ret;
    }

    /* 2. check info len */
    ret = info_len_check(info->info_len);
    if (ret != 0)
    {
        printf("info len check err\r\n");
        return ret;
    }

    /* 3. check info crc */
    ret = info_crc_check(info);
    if (ret != 0)
    {
        printf("info crc check err\r\n");
        return ret;
    }

    /* 4. check code type */
    ret = info_code_type_check(info->code_type);
    if (ret != 0)
    {
        printf("code type check err\r\n");
        return ret;
    }

    /* 5. check code offset */
    ret = info_code_offset_check(info->code_offset);
    if (ret != 0)
    {
        printf("code offset check err\r\n");
        return ret;
    }

    /* 6. check boot offset */
    ret = info_boot_offset_check(info->boot_offset, info->code_size);
    if (ret != 0)
    {
        printf("boot offset check err\r\n");
        return ret;
    }

    /* 7. check code size */
    ret = info_code_size_check(info->code_size);
    if (ret != 0)
    {
        printf("code size check err\r\n");
        return ret;
    }

    /* 8. check code crc */
    ret = info_code_crc_check(info);
    if (ret != 0)
    {
        printf("code crc check err\r\n");
        return ret;
    }

    return 0;
}

static int8_t jump_to_addr(uint32_t addr)
{
    if (addr < FW_BIN_INFO_ADDR_BASE || addr >= (FLASH_BASE + FLASH_SIZE - 4) || (addr & 0x3) != 0)
    {
        return -1;
    }

    typedef void (*pFunction)(void);
    pFunction JumpToApplication;
    uint32_t JumpAddress;

    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_DeInit();
    if (status != HAL_OK)
    {
        printf("HAL_DeInit error\r\n");
        return -2;
    }

    HAL_SuspendTick();

    SCB_DisableICache();
    // SCB_DisableDCache();

    if (((*(__IO uint32_t*)addr) & 0x2FFF0000) == 0x24050000)   /* here use RAM_D1 */
    {
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (addr + 4);
      JumpToApplication = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) addr);
      __set_CONTROL(0); /* for RTOS, set to privilege mode */
      __ISB();
      JumpToApplication();
    }
    else
    {
        return -3;
    }

    return 0;
}

enum bkp_reg
{
/*RTC_BKP_DR0  (0x00u)  -> used for reboot time count */    REG_REBOOT_TIMES = 0,
/*RTC_BKP_DR1  (0x01u)  -> used for upgrade flag */         REG_UPGRADE_FLAG,
/*RTC_BKP_DR2  (0x02u)  -> */                               REG_VALID_MAX
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
/*RTC_BKP_DR16 (0x10u)  -> */  
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
};

#include "rtc.h"
static int8_t bkp_reg_write(enum bkp_reg reg, uint32_t value)
{
    if (reg >= REG_VALID_MAX)
    {
        return -1;
    }

    HAL_RTCEx_BKUPWrite(&hrtc, reg, value);

    return 0;
}

static int8_t bkp_reg_read(enum bkp_reg reg, uint32_t *value)
{
    if (reg >= REG_VALID_MAX)
    {
        return -1;
    }

    *value = HAL_RTCEx_BKUPRead(&hrtc, reg);

    return 0;
}

static int8_t reboot_times_check(uint32_t *times)
{
    int8_t ret = bkp_reg_read(REG_REBOOT_TIMES, times);
    if (ret != 0)
    {
        return ret;
    }

    if (*times >= 0x10)
    {
        printf("reboot reached max times:%u\r\n", *times);
        return -1;
    }

    return 0;
}

int8_t app_valid_check_and_jump(void)
{
    int8_t ret = 0;
    uint32_t reboot_count = 0;

    ret = reboot_times_check(&reboot_count);
    if (ret != 0)
    {
        return ret;
    }

    ret = bkp_reg_write(REG_REBOOT_TIMES, reboot_count + 1);
    if (ret != 0)
    {
        return ret;
    }

    struct code_info info = {0};

    memcpy(&info, (__IO uint32_t *)FW_BIN_INFO_ADDR_BASE, sizeof(struct code_info));

    if (info.info_len < sizeof(struct code_info))
    {
        memcpy(&info, (__IO uint32_t *)FW_BIN_INFO_ADDR_BASE, info.info_len);
    }
    else if (info.info_len > sizeof(struct code_info))
    {
        printf("info len err:%u\r\n", info.info_len);
        return -1;
    }

#if 1
    printf("fw info:\r\n");
    printf("magic_number:%#.8x\r\n", info.magic_number);
    printf("info_version:%d.%d.%d.%d\r\n", info.info_version[0], info.info_version[1], info.info_version[2], info.info_version[3]);
    printf("info_len:%#.8x\r\n", info.info_len);
    printf("patch_flag:%#.8x\r\n", info.patch_flag);
    for (int i = 0; i < 4; i++)
    {
        printf("patch%d_addr:%#.8x\r\n", i, info.patch[i].patch_addr);
        printf("patch%d_len:%#.8x\r\n", i, info.patch[i].patch_len);
        printf("patch%d_crc:%#.8x\r\n", i, info.patch[i].patch_crc);
    }
    printf("code_name:%s\r\n", info.code_name);
    printf("code_type:%u\r\n", info.code_type);
    printf("code_offset:%#.8x\r\n", info.code_offset);
    printf("boot_offset:%#.8x\r\n", info.boot_offset);
    printf("code_size:%#.8x\r\n", info.code_size);
    printf("code_crc:%#.8x\r\n", info.code_crc);
    printf("info_crc:%#.8x\r\n", info.info_crc);
#endif

    ret = fw_info_check(&info);
    if (ret != 0)
    {
        printf("fw info check err\r\n");
    }
    else
    {
        ret = jump_to_addr(FW_BIN_INFO_ADDR_BASE + info.code_offset + info.boot_offset);
        if (ret != 0)
        {
            printf("jump to app failed\r\n");
        }
    }

    return ret;
}

/********************************************used for upgrade below**********************************************/
static int8_t upgrade_info_check(struct code_info *info)
{
    if (info == NULL)
    {
        return -1;
    }

    int8_t ret = 0;

    /* 1. check magic number */
    ret = info_magic_number_check(info->magic_number);
    if (ret != 0)
    {
        printf("magic number check err\r\n");
        return ret;
    }

    /* 2. check info len */
    ret = info_len_check(info->info_len);
    if (ret != 0)
    {
        printf("info len check err\r\n");
        return ret;
    }

    /* 3. check info crc */
    ret = info_crc_check(info);
    if (ret != 0)
    {
        printf("info crc check err\r\n");
        return ret;
    }

    /* 4. check code type */
    ret = info_code_type_check(info->code_type);
    if (ret == -1)
    {
        printf("code type check err\r\n");
        return ret;
    }
    ret == 0 ? printf("code type: full package\r\n") : printf("code type: diff package\r\n");

    /* 5. check code offset */
    ret = info_code_offset_check(info->code_offset);
    if (ret != 0)
    {
        printf("code offset check err\r\n");
        return ret;
    }

    /* 6. check boot offset */
    ret = info_boot_offset_check(info->boot_offset, info->code_size);
    if (ret != 0)
    {
        printf("boot offset check err\r\n");
        return ret;
    }

    /* 7. check code size */
    ret = info_code_size_check(info->code_size);
    if (ret != 0)
    {
        printf("code size check err\r\n");
        return ret;
    }

    return 0;
}

#include "sys_cfg.h"
static int8_t upgrade_code_cryptogram_update(void)
{
    struct sys_info *info = system_info_get();
    struct sys_info info_buf = {0};

    memcpy(&info_buf.reserved, &info->reserved, sizeof(info_buf.reserved));
    info_buf.uid_cryptogram_valid = 0x01;

    HAL_StatusTypeDef status = HAL_OK;
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
        printf("flash unlock err:%d\r\n", status);
        return -1;
    }

    uint32_t offset = sizeof(info->fw_version) + sizeof(info->compile_time);

    for (uint32_t i = 0; i < sizeof(info->uid_cryptogram) + sizeof(info->uid_cryptogram_valid); i += 32)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, (uint32_t)info + offset + i, info_buf.uid_cryptogram + i); /* flash word == 256bit == 32bytes */
        if (status != HAL_OK) 
        {
            printf("flash write err:%d\r\n", status);
            return -2;
        }
    }

    status = HAL_FLASH_Lock();
    if (status != HAL_OK) 
    {
        printf("flash lock err:%d\r\n", status);
        return -3;
    }

    return 0;
}

#include "drv_flash.h"
static int8_t upgrade_info_crc_update(struct code_info *info)
{
    if (info == NULL)
    {
        return -1;
    }

    /* 1. update code cryptogram */
    int8_t ret = upgrade_code_cryptogram_update();
    if (ret != 0)
    {
        printf("upgrade code cryptogram update err\r\n");
        return ret;
    }

    /* 2. calculate crc of code */
    ret = flash_crc_calculate(FW_BIN_INFO_OFFSET + info->code_offset, info->code_size, &info->code_crc);
    if (ret != 0)
    {
        printf("upgrade code crc calculate err\r\n");
        return ret;
    }

    /* 3. calculate crc of info */
    HAL_StatusTypeDef status = hardware_crc_config(CRC32);
    if (status != HAL_OK)
    {
        printf("hardware_crc_config error:%d\r\n", status);
        return -2;
    }

    uint32_t res = hardware_crc_calculate(info, info->info_len - sizeof (info->info_crc));
    printf("crc32 res = %#x\r\n", res^0xFFFFFFFF);
    info->info_crc = res;

    /* 4. write info to flash */
    DEVICE_FLASH *flash = device_flash_get();
    ret = flash->write(flash, FW_BIN_INFO_OFFSET, info, info->info_len, 1000);
    if (ret != 0)
    {
        printf("info write err:%d\r\n", ret);
        return ret;
    }

    return 0;
}

int8_t upgrade_valid_check_and_reset(struct code_info *info)
{
    int8_t ret = 0;

    /* 1. check upgrade info */
    ret = upgrade_info_check(info);
    if (ret != 0)
    {
        printf("upgrade info check err\r\n");
        return ret;
    }

    /* 2. update info crc */
    ret = upgrade_info_crc_update(info);
    if (ret != 0)
    {
        printf("upgrade info crc update err\r\n");
        return ret;
    }

    /* 3. clear upgrade flag to bkp reg */
    ret = bkp_reg_write(REG_UPGRADE_FLAG, 0);
    if (ret != 0)
    {
        printf("bkp reg write err\r\n");
        return ret;
    }

    /* 4. clear reboot times to bkp reg */
    ret = bkp_reg_write(REG_REBOOT_TIMES, 0);
    if (ret != 0)
    {
        printf("bkp reg write err\r\n");
        return ret;
    }

    /* 5. reset system */
    HAL_NVIC_SystemReset();

    return 0;
}

/*
 * app_valid_check_and_jump: main function use. meanwhile, increase the reboot times if upgrade flag is reset.
 * upgrade_valid_check_and_reset: upgrade function use. when upgrade complete, reset reboot times and upgrade flag.
*/

#ifndef BOOTLOARDER_TEST
#include "shell.h"
int8_t app_jump_test(void)
{
    int8_t ret = jump_to_addr(0x08040400);
    if (ret != 0)
    {
        printf("jump to app err: %d\r\n", ret);
        return ret;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(app_jump_test, app_jump_test, jump to app);
#endif
