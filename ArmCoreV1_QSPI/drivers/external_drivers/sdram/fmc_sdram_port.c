#include "fmc_sdram_port.h"
#include "fmc.h"
#include "init_call.h"

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

static int8_t SDRAM_Send_Cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval)
{
    uint32_t target_bank=0;
    FMC_SDRAM_CommandTypeDef Command;

    if(bankx==0) target_bank=FMC_SDRAM_CMD_TARGET_BANK1;
    else if(bankx==1) target_bank=FMC_SDRAM_CMD_TARGET_BANK2;
    Command.CommandMode=cmd;                //命令
    Command.CommandTarget=target_bank;      //目标SDRAM存储区域
    Command.AutoRefreshNumber=refresh;      //自刷新次数
    Command.ModeRegisterDefinition=regval;  //要写入模式寄存器的值
    if(HAL_SDRAM_SendCommand(&hsdram1, &Command, 0XFFFF)==HAL_OK) //向SDRAM发送命令
    {
        return 0;
    }
    else return -1;
}

static int8_t SDRAM_Init(void)
{
    int8_t ret = 0;
    uint32_t temp=0;
    //SDRAM控制器初始化完成以后还需要按照如下顺序初始化SDRAM
    ret |= SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_CLK_ENABLE,1,0);//时钟配置使能
    HAL_Delay(1);                                   //至少延时200us
    ret |= SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);       //对所有存储区预充电
    ret |= SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);//设置自刷新次数
    //配置模式寄存器,SDRAM的bit0~bit2为指定突发访问的长度，
    //bit3为指定突发访问的类型，bit4~bit6为CAS值，bit7和bit8为运行模式
    //bit9为指定的写突发模式，bit10和bit11位保留位

    temp=(uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |	//设置突发长度:1(可以是1/2/4/8)
         SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	//设置突发类型:连续(可以是连续/交错)
         SDRAM_MODEREG_CAS_LATENCY_3           |	//设置CAS值:3(可以是2/3)
         SDRAM_MODEREG_OPERATING_MODE_STANDARD |   //设置操作模式:0,标准模式
         SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     //设置突发写模式:1,单点访问
    ret |= SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,temp);   //设置SDRAM的模式寄存器
    if (ret != 0)
    {
        printf("SDRAM_Send_Cmd err: %d\r\n", ret);
        return -1;
    }

    //刷新频率计数器(以SDCLK频率计数),计算方法:
    //COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
    //我们使用的SDRAM刷新周期为64ms,SDCLK=200/2=100Mhz,行数为8192(2^13).
    //所以,COUNT=64*1000*100/8192-20=761

    HAL_StatusTypeDef status = HAL_OK;
#ifdef SDRAM_ID_W9825G6KH
    status = HAL_SDRAM_ProgramRefreshRate(&hsdram1, 967);
#else
    status = HAL_SDRAM_ProgramRefreshRate(&hsdram1, 1054);
#endif
    if(status != HAL_OK)
    {
        printf("HAL_SDRAM_ProgramRefreshRate err: %d\r\n", status);
        return -2;
    }

    return 0;
}


/* ******************************************************************************************* */
// private code
#ifdef USING_MDMA_FOR_FMC
#include "mdma.h"
#endif
static struct dev_sdram bank1_sdram = {0};

static struct dev_sdram *bank1_sdram_get(voide)
{
    return &bank1_sdram;
}

#ifdef USING_MDMA_FOR_FMC
#define MDMA_SDRAM_READ_WRITE_EVENT (1 << 0)
static void CpltCallback(MDMA_HandleTypeDef *hmdma)
{
    osEventFlagsSet(bank1_sdram_get()->event, MDMA_SDRAM_READ_WRITE_EVENT);
}
#endif

static int8_t dev_open(struct dev_sdram *sdram)
{
    if (sdram == NULL)
    {
        return -1;
    }

    if (sdram->open_state)
    {
        return 0;
    }
    else
    {
        sdram->open_state = 1;
    }

    return 0;
}

static int8_t dev_close(struct dev_sdram *sdram)
{
    if (sdram == NULL)
    {
        return -1;
    }

    sdram->open_state = 0;

    return 0;
}

static int8_t dev_read(struct dev_sdram *sdram, uint32_t offset, uint8_t *data, uint16_t len, uint32_t timeout)
{
    int8_t ret = 0;

    if (sdram == NULL)
    {
        return -1;
    }

    if (sdram->open_state == 0)
    {
        return -2;
    }

    if (offset + len > SDRAM_BANK1_SIZE)
    {
        return -3;
    }

    osStatus_t stat = osMutexAcquire(sdram->mutex, timeout);
    if (stat != osOK)
    {
        printf("osMutexAcquire error: %d\r\n", stat);
        return -4;
    }

#ifdef USING_MDMA_FOR_FMC
    HAL_StatusTypeDef status = HAL_MDMA_Start_IT(&hmdma_mdma_channel3_sw_0, SDRAM_BANK1_ADDR + offset, data, len, 1);
    if (status != HAL_OK)
    {
        printf("HAL_MDMA_Start_IT error: %d\r\n", status);
        ret = -5;
        goto err;
    }

    ret = osEventFlagsWait(sdram->event, MDMA_SDRAM_READ_WRITE_EVENT, osFlagsWaitAny, timeout);
    if (ret != MDMA_SDRAM_READ_WRITE_EVENT)
    {
        printf("osEventFlagsWait error: %d\r\n", ret);
        ret = -6;
        goto err;
    }
#else
    memcpy(data, (uint8_t *)addr, len);
#endif

err:
    osMutexRelease(sdram->mutex);

    return (ret < 0 ? ret : 0);
}

static int8_t dev_write(struct dev_sdram *sdram, uint32_t offset, uint8_t *data, uint16_t len, uint32_t timeout)
{
    int8_t ret = 0;

    if (sdram == NULL)
    {
        return -1;
    }

    if (sdram->open_state == 0)
    {
        return -2;
    }

    if (offset + len > SDRAM_BANK1_SIZE)
    {
        return -3;
    }

    osStatus_t stat = osMutexAcquire(sdram->mutex, timeout);
    if (stat != osOK)
    {
        printf("osMutexAcquire error: %d\r\n", stat);
        return -4;
    }

#ifdef USING_MDMA_FOR_FMC
    HAL_StatusTypeDef status = HAL_MDMA_Start_IT(&hmdma_mdma_channel3_sw_0, data, SDRAM_BANK1_ADDR + offset, len, 1);
    if (status != HAL_OK)
    {
        printf("HAL_MDMA_Start_IT error: %d\r\n", status);
        ret = -5;
        goto err;
    }

    ret = osEventFlagsWait(sdram->event, MDMA_SDRAM_READ_WRITE_EVENT, osFlagsWaitAny, timeout);
    if (ret != MDMA_SDRAM_READ_WRITE_EVENT)
    {
        printf("osEventFlagsWait error: %d %s\r\n", ret, data);
        ret = -6;
        goto err;
    }
#else
    memcpy((uint8_t *)addr, data, len);
#endif

err:
    osMutexRelease(sdram->mutex);

    return (ret < 0 ? ret : 0);
}

static int8_t dev_ioctl(struct dev_sdram *sdram, uint8_t cmd, void *arg)
{
    if (sdram == NULL)
    {
        return -1;
    }

    return 0;
}

static int8_t sdram_init(struct dev_sdram *device, uint8_t *name)
{
    if (device == NULL || name == NULL)
    {
        return -1;
    }

    if (device->open_state)
    {
        printf("sdram device has been opened\r\n");
        return -2;
    }

    osMutexAttr_t sdram_mutex_attributes = {
    .name = "bank1_sdram_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    device->mutex = osMutexNew(&sdram_mutex_attributes);
    if (device->mutex == NULL)
    {
        printf("create sdram mutex failed\r\n");
        return -3;
    }

#ifdef USING_MDMA_FOR_FMC
    osEventFlagsAttr_t sdram_event_attributes = {
    .name = "bank1_sdram_event",
    };
    device->event = osEventFlagsNew(&sdram_event_attributes);
    if (device->event == NULL)
    {
        printf("create sdram event failed\r\n");
        return -4;
    }

    HAL_StatusTypeDef status = HAL_MDMA_RegisterCallback(&hmdma_mdma_channel3_sw_0, HAL_MDMA_XFER_BLOCKCPLT_CB_ID, CpltCallback);
    if (status != HAL_OK)
    {
        printf("HAL_MDMA_RegisterCallback error: %d\r\n", status);
        return -5;
    }
#endif

    memcpy(device->name, name, DEVICE_NAME_LENGTH);

    device->open = dev_open;
    device->close = dev_close;
    device->read = dev_read;
    device->write = dev_write;
    device->ioctl = dev_ioctl;

    return device->open(device);
}

static int8_t device_sdram_init()
{
    return sdram_init(bank1_sdram_get(), SDRAM_BANK1_DEVICE_NAME);
}
INIT_APP_EXPORT(device_sdram_init);

int8_t sdram_write(uint32_t offset, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    return bank1_sdram_get()->write(bank1_sdram_get(), offset, buf, len, timeout);
}
int8_t sdram_read(uint32_t offset, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    return bank1_sdram_get()->read(bank1_sdram_get(), offset, buf, len, timeout);
}

static int8_t sdram_cfg_init(struct dev_sdram *device, uint8_t *name)
{
    int8_t ret = 0;

    if (device == NULL || name == NULL)
    {
        return -1;
    }

    if (!memcmp(name, SDRAM_BANK1_DEVICE_NAME, sizeof(SDRAM_BANK1_DEVICE_NAME)))
    {
        ret = SDRAM_Init();
    }

    return ret;
}

int8_t bank1_sdram_init(void)
{
    return sdram_cfg_init(bank1_sdram_get(), SDRAM_BANK1_DEVICE_NAME);
}

#ifndef SDRAM_TEST
#include "shell.h"
#include "utilities.h"
static int8_t sdram_write_read_test(uint8_t argc, uint8_t *argv[])
{
    struct system_time begin = {0};
    struct system_time end = {0};
    uint32_t time_diff = 0;

    __IO uint8_t *ptr_8 = (__IO uint8_t *) (SDRAM_BANK1_ADDR);
    __IO uint16_t *ptr_16 = (__IO uint16_t *) (SDRAM_BANK1_ADDR);
    __IO uint32_t *ptr_32 = (__IO uint32_t *) (SDRAM_BANK1_ADDR);

    uint8_t num_8 = 0;
    uint16_t num_16 = 0;
    uint32_t num_32 = 0;

    uint64_t test_num = 0x123456789ABCDEF0;

    // printf("test_num:%#llx\r\n", test_num);
    // printf("test_num:%#llu\r\n", test_num);

    // printf("test_num addr:%#x\r\n", (uint32_t) &test_num);

    /* 1. test 8 bit */
    system_time_get(&begin);
    for (uint32_t i = 0; i < 1024 * 1024; i++)
    {
        ptr_8[i] = i % 256;
        if (i%256 != ptr_8[i])
        {
            printf("write 8bit error:%d\r\n", i);
        }
        // printf("ptr_8[%d] addr:%#x\r\n", i, &ptr_8[i]);
    }
    system_time_get(&end);
    printf("write 8bit use time:%lu us\r\n", time_diff_us(&begin, &end));

    /* 2. test 16 bit */
    system_time_get(&begin);
    for (uint32_t i = 0; i < 1024 * 1024 / 2; i++)
    {
        ptr_16[i] = (i + 1)%256;
        if ((i + 1)%256 != ptr_16[i])
        {
            printf("write 16bit error:%d\r\n", i + 1);
        }
        // printf("ptr_16[%d] addr:%#x\r\n", i, &ptr_16[i]);
    }
    system_time_get(&end);
    printf("write 16bit use time:%lu us\r\n", time_diff_us(&begin, &end));

    /* 3. test 32 bit */
    system_time_get(&begin);
    for (uint32_t i = 0; i < 1024 * 1024 / 4; i++)
    {
        ptr_32[i] = (i + 2)%256;
        if ((i + 2)%256 != ptr_32[i])
        {
            printf("write 32bit error:%d\r\n", i + 2);
        }
        // printf("ptr_32[%d] addr:%#x\r\n", i, &ptr_32[i]);
    }
    system_time_get(&end);
    printf("write 32bit use time:%lu us\r\n", time_diff_us(&begin, &end));

    return 0;
}
MSH_CMD_EXPORT_ALIAS(sdram_write_read_test, sdram_write_read_test, test sdram_write_read);

static int8_t sdram_read_test(uint8_t argc, uint8_t *argv[])
{
    struct dev_sdram *dev = bank1_sdram_get();

    uint16_t buf[64] = {0};

    uint16_t *ptr = (__IO uint16_t *) (SDRAM_BANK1_ADDR);

    for (uint32_t i = 0; i < sizeof(buf)/sizeof(buf[0]); i++)
    {
        printf("ptr[%d] = %d\r\n", i, ptr[i]);
    }

    printf("-------------------------------------------\r\n");

    dev->read(dev, 0, buf, 128, osWaitForever);
    for (uint32_t i = 0; i < sizeof(buf)/sizeof(buf[0]); i++)
    {
        printf("buf[%d] = %d\r\n", i, buf[i]);
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(sdram_read_test, sdram_read_test, test sdram_read);

static int8_t sdram_write_test(uint8_t argc, uint8_t *argv[])
{
    struct dev_sdram *dev = bank1_sdram_get();
    uint16_t buf[64] = {0};

    for (uint32_t i = 0; i < sizeof(buf)/sizeof(buf[0]); i++)
    {
        buf[i] = i*2;
    }

    dev->write(dev, 0, buf, 128, osWaitForever);

    sdram_read_test(0, NULL);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(sdram_write_test, sdram_write_test, test sdram_write);

#include <stdlib.h>
#include <stdio.h>
#include "ulog.h"
static int8_t sdram_test_entry(void *argument)
{
    osDelay(1000);

    uint16_t write_buf[1024] = {0}, read_buf[1024] = {0};

    struct dev_sdram *dev = bank1_sdram_get();
    uint32_t offset = 0, succeed = 0, count = 0;

    for (;;)
    {
        srand(osKernelGetTickCount());

        /* 1. update buffer */
        for (uint32_t i = 0; i < sizeof(write_buf) / sizeof(write_buf[0]); i++)
        {
            write_buf[i] = rand() % 0xFFFF;
        }

        offset = ALIGN(rand(), 4) % SDRAM_BANK1_SIZE;
        offset = offset > (SDRAM_BANK1_SIZE - sizeof(write_buf)) ? SDRAM_BANK1_SIZE - sizeof(write_buf) : offset;

        /* 2. write buffer to sdram */
        dev->write(dev, offset, write_buf, sizeof(write_buf), osWaitForever);
        dev->read(dev, offset, read_buf, sizeof(read_buf), osWaitForever);

        /* 3. check buffer */
        if (memcmp(write_buf, read_buf, sizeof(write_buf)) != 0)
        {
            LOG_I("sdram test failed\r\n");
            goto err;
        }

        // offset += sizeof(write_buf);
        // offset %= SDRAM_BANK1_SIZE;

        // if (offset == 0)
        succeed++;
        if (++count % 10000 == 0)
        {
            LOG_I("sdram test succeed: %d\r\n", succeed);
        }

        osDelay(1);
    }

err:
    for (;;)
    {
        LOG_I("sdram test failed, succeed: %d\r\n", succeed);
        osDelay(1000);
    }

    return 0;
}


static int8_t sdram_test_init(void)
{
    osThreadAttr_t attr = {
    .name = "sdram_64m_test",
    .stack_size = 2048 * 4,
    .priority = osPriorityNormal,
    };

    osThreadId_t tid = osThreadNew(sdram_test_entry, NULL, &attr);
    if (tid == NULL)
    {
        printf("thread sdram_64m_test create failed\r\n");
        return -1;
    }

    return 0;
}
// INIT_APP_EXPORT(sdram_test_init);
#endif