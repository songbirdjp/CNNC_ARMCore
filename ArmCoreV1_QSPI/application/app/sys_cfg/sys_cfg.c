#include "sys_cfg.h"
#include "stm32h7xx_hal.h"
#include "crypto_sha.h"
#include "cmsis_os.h"
#include "shell.h"
#include "config.h"

extern uint32_t __system_info_start;

static struct sys_info system_info __attribute__((section(".system_info"))) = 
{
    .fw_version = FW_VERSION,
    .compile_time = __DATE__"  "__TIME__,
    .uid_cryptogram = {[0 ... 63] = 0xFF},
    .uid_cryptogram_valid = 0xFFFFFFFF,
    .reserved = {[0 ... 6] = 0xFFFFFFFF}
};

struct sys_info *system_info_get(void)
{
    return &system_info;
}

void system_info_print(void)
{
    struct sys_info *sys_info = system_info_get();
    
    printf("\r\n************************************\r\n");

    printf("fw version: %s\r\n", sys_info->fw_version);
    printf("compile time: %s\r\n", sys_info->compile_time);
    printf("git branch: %s\r\n", GIT_BRANCH);
    printf("git hash: %s\r\n", GIT_HASH);
    printf("mcu clock:%.2f M\r\n", HAL_RCC_GetSysClockFreq()/1000000.0);

    printf("stm32 uid:%#.8x%.8x%.8x\r\n", HAL_GetUIDw2(), HAL_GetUIDw1(), HAL_GetUIDw0());
    printf("flash size:%uKB\r\n", FLASH_SIZE / 1024);

    uint32_t hal_version = HAL_GetHalVersion();

    printf("hal driver version:%.2u.%.2u.%.2u.%.2u\r\n", (hal_version >> 24) & 0xFF, 
        (hal_version >> 16) & 0xFF, (hal_version >> 8) & 0xFF, hal_version & 0xFF);

    printf("free rtos version:%s\r\n", tskKERNEL_VERSION_NUMBER);

    printf("cmsis device version:%.2u.%.2u.%.2u.%.2u\r\n", __STM32H7xx_CMSIS_DEVICE_VERSION_MAIN, __STM32H7xx_CMSIS_DEVICE_VERSION_SUB1, __STM32H7xx_CMSIS_DEVICE_VERSION_SUB2, __STM32H7xx_CMSIS_DEVICE_VERSION_RC);

    printf("************************************\r\n");

}
MSH_CMD_EXPORT_ALIAS(system_info_print, system_info, system info);

static int8_t system_encrypt_check(void)
{
    struct sys_info *info = system_info_get();

    /* 1. 计算加密信息 */
    uint8_t msg_uid[12] = {0};  /* 96bit uid */
    for (uint8_t i = 0; i < sizeof(msg_uid) / sizeof(uint32_t); i++)
    {
        memcpy(msg_uid + i * 4, (uint32_t *)UID_BASE + i, 4);
    }

    uint8_t computed_hash[sizeof(info->uid_cryptogram)] = {0};
    uint32_t computed_size = 0;
    int8_t ret = crypto_sha256_cal(msg_uid, 12, computed_hash, &computed_size);
    if (ret != 0)
    {
        printf("crypto_sha256_cal error %d\r\n", ret);
        return -1;
    }

    /* 2. 校验结果 */
    if (memcmp(info->uid_cryptogram, computed_hash, computed_size) != 0)
    {
        printf("cryptogram not match\r\n");
        return -2;
    }

    return 0;
}

int8_t system_encrypt_init(void)
{
    struct sys_info *info = system_info_get();

    /* 1. 判断加密信息有效性 */
    if (info->uid_cryptogram_valid != 0xFFFFFFFF)
    {
        // printf("no need to update cryptogram\r\n");
        goto check;
    }

    /* 2. 计算加密信息 */
    uint8_t msg_uid[12] = {0};  /* 96bit uid */
    for (uint8_t i = 0; i < sizeof(msg_uid) / sizeof(uint32_t); i++)
    {
        memcpy(msg_uid + i * 4, (uint32_t *)UID_BASE + i, 4);
    }

    struct sys_info info_buf = {0};
    uint32_t computed_size = 0;
    int8_t ret = crypto_sha256_cal(msg_uid, 12, info_buf.uid_cryptogram, &computed_size);
    if (ret != 0)
    {
        printf("crypto_sha256_cal error %d\r\n", ret);
        return -1;
    }

    memcpy(&info_buf.reserved, &info->reserved, sizeof(info_buf.reserved));
    info_buf.uid_cryptogram_valid = 0x01;

#if 0
    printf("cal uid sha256: %u bytes\r\n", computed_size);
    for (uint32_t i = 0; i < sizeof(info_buf.uid_cryptogram); i++)
    {
        printf("%.2x ", info_buf.uid_cryptogram[i]);
    }
    printf("\r\n");
#endif

    /* 3. 写入加密信息 */
    HAL_StatusTypeDef status = HAL_OK;
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
        printf("flash unlock err:%d\r\n", status);
        return -2;
    }

    uint32_t offset = sizeof(info->fw_version) + sizeof(info->compile_time);

    for (uint32_t i = 0; i < sizeof(info->uid_cryptogram) + sizeof(info->uid_cryptogram_valid); i += 32)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, (uint32_t)info + offset + i, info_buf.uid_cryptogram + i); /* flash word == 256bit == 32bytes */
        if (status != HAL_OK) 
        {
            printf("flash write err:%d\r\n", status);
            return -3;
        }
    }

    status = HAL_FLASH_Lock();
    if (status != HAL_OK) 
    {
        printf("flash lock err:%d\r\n", status);
        return -4;
    }

check:
    /* 4. 校验结果 */
    ret = system_encrypt_check();
    if (ret != 0)
    {
        printf("encrypt check error %d\r\n", ret);
    }

    return ret;
}