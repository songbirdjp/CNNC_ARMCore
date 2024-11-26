#include "drv_flash.h"
#include "stm32h7xx_hal.h"
#include "init_call.h"
#include "ulog.h"
#define FLASH_OPERATION_SUCCEED_EVENT    (1 << 0)

#ifdef OS_FREERTOS
static DEVICE_FLASH *device_flash_bank1 = NULL;
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
    switch (pFlash.ProcedureOnGoing)
    {
    case FLASH_PROC_PROGRAM_BANK1:
        // printf("flash bank1 %#.8x program succeed\r\n", ReturnValue);
        osEventFlagsSet(device_flash_bank1->operation_event, FLASH_OPERATION_SUCCEED_EVENT);
        break;

    case FLASH_PROC_SECTERASE_BANK1:
        printf("flash bank1 sector %u erase succeed\r\n", ReturnValue);
        break;

#if defined (DUAL_BANK)
    case FLASH_PROC_PROGRAM_BANK2:
        printf("flash bank2 %#.8x program succeed\r\n", ReturnValue);
        break;
    case FLASH_PROC_SECTERASE_BANK2:
        printf("flash bank2 sector %u erase succeed\r\n", ReturnValue);
        break;
#endif

    case FLASH_PROC_NONE:
        if (ReturnValue == 0xFFFFFFFFU)
        {
            printf("flash operation complete\r\n");
            osEventFlagsSet(device_flash_bank1->operation_event, FLASH_OPERATION_SUCCEED_EVENT);
        }
        break;
    default:
        break;
    }
}

void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
    printf("flash operation error(%u), error code: %u\r\n", ReturnValue, HAL_FLASH_GetError());
}
#endif

uint32_t flash_sector_get(uint32_t address)
{
    return (address - FLASH_BASE) / FLASH_SECTOR_SIZE;
}

int8_t flash_erase_sector(DEVICE_FLASH *flash, uint32_t address_start, uint32_t address_end, uint32_t timeout)   /* 不包含address_end地址所在的扇区 */
{
    if (flash == NULL)
    {
        return -1;
    }

    if (address_start > address_end || address_start < flash->addr_base || address_start + flash->size - 1 > address_end)
    {
        return -2;
    }

#ifdef OS_FREERTOS
    osStatus_t ret = osOK;
    ret = osMutexAcquire(flash->rw_mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", flash->name, ret);
        return -3;
    }
#else
    int8_t ret = 0;
#endif

    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) 
    {
#ifdef OS_FREERTOS
        ret = -4;
        goto out;
#else
        return -4;
#endif
    }

    FLASH_EraseInitTypeDef EraseInitStruct = {0};    
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_4;
    EraseInitStruct.Sector = flash_sector_get(address_start);
    EraseInitStruct.NbSectors = flash_sector_get(address_end) - flash_sector_get(address_start) + 1;

#ifdef OS_FREERTOS
    status = HAL_FLASHEx_Erase_IT(&EraseInitStruct);
    if (status != HAL_OK) 
    {
        printf("Erase sector err:%d\r\n", status);
        ret = -5;
        goto err;
    }

    uint32_t ret_val = osEventFlagsWait(flash->operation_event, FLASH_OPERATION_SUCCEED_EVENT, osFlagsWaitAny, timeout);
    if (ret_val != FLASH_OPERATION_SUCCEED_EVENT)
    {
        printf("device %s  wait event flag err: %#.8x\r\n", flash->name, ret_val);
        ret = -6;
        goto err;
    }
#else
    uint32_t SectorError = 0;
    status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
    if (status != HAL_OK) 
    {
        printf("Erase sector err:%d(%u)\r\n", status, SectorError);
        ret = -5;
        goto err;
    }
#endif

err:
    status = HAL_FLASH_Lock();
    if (status != HAL_OK) 
    {
        ret = -7;
    }

#ifdef OS_FREERTOS
out:
    ret = osMutexRelease(flash->rw_mutex);
    if (ret != osOK)
    {
        printf("device %s release mutex err:%d\r\n", flash->name, ret);
        return -9;
    }
#endif

    return ret;
}

int8_t flash_open(DEVICE_FLASH *flash)
{
    if (flash->open_state)
    {
        printf("device %s already opened\r\n", flash->name);
        return -1;
    }
    else
    {
        flash->open_state = 1;
    }

    return 0;
}

int8_t flash_close(DEVICE_FLASH *flash)
{
    if (flash->open_state)
    {
#ifdef OS_FREERTOS
        osStatus_t stat = osMutexDelete(flash->rw_mutex);
        if (stat != osOK)
        {
            printf("device %s delete mutex err:%d\r\n", flash->name, stat);
            return -1;
        }

        stat = osEventFlagsDelete(flash->operation_event);
        if (stat != osOK)
        {
            printf("device %s delete event err:%d\r\n", flash->name, stat);
            return -2;
        }
#endif
        flash->open_state = 0;
    }
    else
    {
        /* device already closed */
    }

    return 0;
}

int8_t flash_write(DEVICE_FLASH *flash, uint32_t offset, uint8_t *buf, uint32_t size, uint32_t timeout)
{
    if (flash == NULL || buf == NULL || size == 0)
    {
        printf("ptr is null or size is 0\r\n");
        return -1;
    }
    if (offset + size > flash->size || offset % 32 != 0)
    {
        return -2;
    }

    if (!flash->open_state)
    {
        printf("device %s is closed\r\n", flash->name);
        return -3;
    }

#ifdef OS_FREERTOS
    osStatus_t ret = osOK;
    ret = osMutexAcquire(flash->rw_mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", flash->name, ret);
        return -4;
    }
#else
    int8_t ret = 0;
#endif

    HAL_StatusTypeDef status = HAL_OK;
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
#ifdef OS_FREERTOS
        ret = -5;
        goto out;
#else
        return -5;
#endif
    }

    for (uint32_t i = 0; i < size; i += 32)
    {
#ifdef OS_FREERTOS
        status = HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_FLASHWORD, flash->addr_base + offset + i, buf + i); /* flash word == 256bit == 32bytes */
        if (status != HAL_OK) 
        {
            printf("flash write err:%d\r\n", status);
            ret = -6;
            goto err;
        }
        uint32_t ret_val = osEventFlagsWait(flash->operation_event, FLASH_OPERATION_SUCCEED_EVENT, osFlagsWaitAny, timeout);
        if (ret_val != FLASH_OPERATION_SUCCEED_EVENT)
        {
            printf("device %s  wait event flag err: %#.8x\r\n", flash->name, ret_val);
            ret = -7;
            goto err;
        }
#else
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, flash->addr_base + offset + i, buf + i); /* flash word == 256bit == 32bytes */
        if (status != HAL_OK) 
        {
            printf("flash write err:%d\r\n", status);
            ret = -6;
            goto err;
        }
#endif
    }

err:
    status = HAL_FLASH_Lock();
    if (status != HAL_OK) 
    {
        ret = -8;
    }

#ifdef OS_FREERTOS
out:
    ret = osMutexRelease(flash->rw_mutex);
    if (ret != osOK)
    {
        printf("device %s release mutex err:%d\r\n", flash->name, ret);
        return -9;
    }
#endif

    return ret;
}

int8_t flash_read(DEVICE_FLASH *flash, uint32_t offset, uint8_t *buf, uint32_t size, uint32_t timeout)
{
    if (flash == NULL || buf == NULL || size == 0 || offset & 0x3 != 0)
    {
        printf("ptr is null or size is 0\r\n");
        return -1;
    }

    if (offset + size > flash->size)
    {
        return -2;
    }

    if (!flash->open_state)
    {
        printf("device %s is closed\r\n", flash->name);
        return -3;
    }

#ifdef OS_FREERTOS
    osStatus_t ret = osOK;
    ret = osMutexAcquire(flash->rw_mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", flash->name, ret);
        return -4;
    }
#endif

    for (uint32_t i = 0; i < size; i += 4)
    {
        *(uint32_t *)(buf + i) = *(uint32_t *)(flash->addr_base + offset + i);
    }

#ifdef OS_FREERTOS
    ret = osMutexRelease(flash->rw_mutex);
    if (ret != osOK)
    {
        printf("device %s release mutex err:%d\r\n", flash->name, ret);
        return -5;
    }
#endif

    return 0;
}

int8_t flash_ioctl(DEVICE_FLASH *flash, uint8_t cmd, void *arg)
{
    if (flash == NULL || arg == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    if (!flash->open_state)
    {
        printf("device %s is closed\r\n", flash->name);
        return -2;
    }

    switch (cmd)
    {
    case FLASH_CMD_ERASE_SECTOR:
        flash_erase_sector(flash, *(uint32_t *)arg, *(uint32_t *)arg + *(uint32_t *)(arg + 4) - 1, 5000);
        break;

    default:
        printf("unknown cmd:%d\r\n", cmd);
        return -3;
    }

    return 0;
}

int8_t flash_init(DEVICE_FLASH *flash, uint8_t *device_name)
{
    if (flash == NULL || device_name == NULL)
    {
        printf("cjh1ptr is null\r\n");
        return -1;
    }

    if (flash->open_state)
    {
        printf("device %s is opened\r\n", flash->name);
        return -2;
    }

    /* 1. init hardware */
    /* 2. create queue 、event and mutex for device */
#ifdef OS_FREERTOS    
    osMutexAttr_t flash_rw_mutex_attributes = {
    .name = "flash_rw_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    flash->rw_mutex = osMutexNew(&flash_rw_mutex_attributes);
    if (flash->rw_mutex == NULL)
    {
        printf("create mutex err\r\n");
        return -3;
    }

    osEventFlagsAttr_t flash_operation_event_attributes = {
    .name = "flash_operation_event"
    };
    flash->operation_event = osEventFlagsNew(&flash_operation_event_attributes);
    if (flash->operation_event == NULL)
    {
        printf("create event err\r\n");
        return -4;
    }
#endif
    /* 3. rx buf malloc for dma mode */    
    /* 4. register callback function */
    /* 5. device rename */
    memcpy(flash->name, device_name, DEVICE_NAME_LENGTH);

    /* 6. register operation function */
    flash->open = flash_open;
    flash->close = flash_close;
    flash->write = flash_write;
    flash->read = flash_read;
    flash->ioctl = flash_ioctl;

    /* 7. copy flash info to flash struct */
#ifdef OS_FREERTOS
    device_flash_bank1 = flash;
#endif

    /* 8. open device */
    return flash->open(flash);
}

int8_t flash_operation_address_set(DEVICE_FLASH *flash, uint32_t addr_base, uint32_t size)
{
    if (flash == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    flash->addr_base = addr_base;
    flash->size = size;

    return 0;
}


#ifndef FLASH_TEST
#include "shell.h"

#define FLASH_ADDRESS_BASE  (FLASH_BASE + FLASH_SECTOR_SIZE * 6)//0x08000000UL + 0x00020000UL* 6 = 0x080C0000UL
#define FLASH_VALID_SIZE    (FLASH_SECTOR_SIZE * 2) //0x00020000UL * 2 = 0x00040000UL

#define FLASH_AFC_ADC1_BASE                 FLASH_ADDRESS_BASE// 0x080C0000UL
#define FLASH_AFC_ADC1_OFFSET               FLASH_SECTOR_SIZE * 6
#define FLASH_AFC_ADC2_OFFSET                 FLASH_SECTOR_SIZE * 7// 0x080E0000UL 

static DEVICE_FLASH flash_bank1 = {0};
static DEVICE_FLASH *device_flash_get(void)
{
    return &flash_bank1;
}

#define POLY 0x4c11db7
/* SW_crc32_Calcul function refer to AN5507 */
uint32_t SW_crc32_Calcul(const uint32_t *buf, size_t len, uint32_t BurstSize)
{
    int k;
    /* Define the initial CRC value */
    uint32_t crc = 0;
    /* Define the length of data on which CRC has to be computed depending on CRC burst size
    */
    if ( len > BurstSize)
    {
        if ( (len % BurstSize) != 0)
        {
            len = BurstSize * ( ( len / BurstSize) + 1 );
        }
    }
    else
    {
        len = BurstSize;
    }
    /* Calculate CRC value */
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 32; k++)
            crc = crc & 0x80000000 ? (crc << 1) ^ POLY : crc << 1;
        crc ^= 0x55555555;
    }
    /* Return CRC value */
    return crc;
}

int8_t flash_test(uint8_t argc, char *argv[])
{
    int8_t ret = 0;
    DEVICE_FLASH *flash = device_flash_get();
    FLASH_CRCInitTypeDef CRCInitTypeDef = {0};
    uint32_t CRC_Result = 0;
    uint8_t data[1024] = {0};
    uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE};
    switch (atoi(argv[1]))
    {
    case 0:
        ret = flash_init(device_flash_get(), "DEVICE_NAME_FLASH_BANK1");
        if (ret != 0)
        {
            printf("flash init err:%d\r\n", ret);
            return -1;
        }

        ret = flash_operation_address_set(device_flash_get(), flash_cfg[0], flash_cfg[1]);
        if (ret != 0)
        {
            printf("flash operation address set err:%d\r\n", ret);
            return -2;
        }
        break;

    case 1:
        ret = flash->read(flash, 0, data, sizeof(data), 1000);
        if (ret != 0)
        {
            printf("flash read err:%d\r\n", ret);
            return -3;
        }

        for (uint32_t i = 0; i < 256; i += 16) 
        {
            LOG_E("%08x: ", FLASH_ADDRESS_BASE + i);
            for (uint32_t j = 0; j < 256; j++)
            {
                LOG_E("%02x ", data[i + j]);
            }
            LOG_E("\r\n");
        }
        break;

    case 2:
        for (uint32_t i = 0; i < sizeof(data); i++)
        {
            data[i] = i; 
        }

        ret = flash->write(flash, 0, data, sizeof(data), 1000);
        if (ret != 0)
        {
            printf("flash write err:%d\r\n", ret);
            return -4;
        }
        break;

    case 3:
        ret = flash->ioctl(flash, FLASH_CMD_ERASE_SECTOR, (void *)flash_cfg);
        if (ret != 0)
        {
            printf("flash erase err:%d\r\n", ret);
            return -5;
        }
        break;

    case 4:
        HAL_FLASH_Unlock();
        CRCInitTypeDef.TypeCRC = FLASH_CRC_ADDR;
        CRCInitTypeDef.BurstSize = FLASH_CRC_BURST_SIZE_4;
        CRCInitTypeDef.Bank = FLASH_BANK_1;
        // CRCInitTypeDef.Sector = FLASH_SECTOR_0;
        // CRCInitTypeDef.NbSectors = 1;
        CRCInitTypeDef.CRCStartAddr = FLASH_ADDRESS_BASE;
        CRCInitTypeDef.CRCEndAddr = FLASH_ADDRESS_BASE + 1024 - 1;  /* 共1024个字节进行计算，硬件按512字节对齐计算，而非128字节 */
        HAL_StatusTypeDef status = HAL_FLASHEx_ComputeCRC(&CRCInitTypeDef, &CRC_Result);
        if (status != HAL_OK)
        {
            printf("HAL_FLASHEx_ComputeCRC error\r\n");
        }

        HAL_FLASH_Lock();

        printf("CRC_Result:%08x\r\n", CRC_Result);
        break;

    case 5:
         CRC_Result = SW_crc32_Calcul((const uint32_t *)FLASH_ADDRESS_BASE, 256, 4);    /* 读取1024字节进行计算 */
         printf("crc result:%08x\r\n", CRC_Result);

        break;
    
    default:
        break;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(flash_test, flash_test, flash test);
#endif