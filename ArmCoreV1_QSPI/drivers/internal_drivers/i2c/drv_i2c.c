#include "drv_i2c.h"
#include "i2c.h"

#define I2C_SEND_SUCCEED_EVENT  (1 << 0)
#define I2C_RECV_SUCCEED_EVENT  (1 << 1)

static void MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    DEVICE_I2C *i2c = (DEVICE_I2C *)hi2c;
    osEventFlagsSet(i2c->event, I2C_SEND_SUCCEED_EVENT);
}
static void MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    DEVICE_I2C *i2c = (DEVICE_I2C *)hi2c;
    osEventFlagsSet(i2c->event, I2C_RECV_SUCCEED_EVENT);
}
static void MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    DEVICE_I2C *i2c = (DEVICE_I2C *)hi2c;
    osEventFlagsSet(i2c->event, I2C_SEND_SUCCEED_EVENT);
}
static void MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    DEVICE_I2C *i2c = (DEVICE_I2C *)hi2c;
    osEventFlagsSet(i2c->event, I2C_RECV_SUCCEED_EVENT);
}
static void ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    DEVICE_I2C *i2c = (DEVICE_I2C *)hi2c;
    printf("device %s err:%d\r\n", i2c->name, hi2c->ErrorCode);
}
static void AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
    DEVICE_I2C *i2c = (DEVICE_I2C *)hi2c;
    printf("device %s abort\r\n", i2c->name);
}

static int8_t i2c_open(DEVICE_I2C *i2c)
{
    if (i2c->open_state)
    {
        printf("device %s already opened\r\n", i2c->name);
        return -1;
    }
    else
    {
        i2c->open_state = 1;
    }

    return 0;
}

static int8_t i2c_close(DEVICE_I2C *i2c)
{
    HAL_StatusTypeDef ret = HAL_OK;
    osStatus_t stat = osOK;

    if (i2c->open_state)
    {
        ret = HAL_I2C_DeInit(&i2c->hi2c);
        if (ret != HAL_OK)
        {
            printf("device %s deinit err:%d\r\n", i2c->name, ret);
            return -1;
        }

        stat = osEventFlagsDelete(i2c->event);
        if (stat != osOK)
        {
            printf("device %s delete event err:%d\r\n", i2c->name, stat);
            return -2;
        }

        stat = osMutexDelete(i2c->mutex);
        if (stat != osOK)
        {
            printf("device %s delete mutex err:%d\r\n", i2c->name, stat);
            return -3;
        }

        i2c->open_state = 0;
    }
    else
    {
        /* device already closed */
    }

    return 0;
}

static int8_t i2c_write(DEVICE_I2C *i2c, uint16_t addr, uint8_t *buf, uint16_t size, uint32_t timeout)
{
    osStatus_t ret = osOK;
    HAL_StatusTypeDef status = HAL_OK;

    if (!i2c->open_state)
    {
        printf("device %s is closed\r\n", i2c->name);
        return -1;
    }

    ret = osMutexAcquire(i2c->mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", i2c->name, ret);
        return -2;
    }

    status = HAL_I2C_Master_Transmit_DMA(&i2c->hi2c, addr, buf, size);
    if (status != HAL_OK)
    {
        printf("device %s write data err:%d\r\n", i2c->name, status);
        ret = -3;
        goto err;
    }

    uint32_t ret_val = osEventFlagsWait(i2c->event, I2C_SEND_SUCCEED_EVENT, osFlagsWaitAny, timeout);
    if (ret_val != I2C_SEND_SUCCEED_EVENT)
    {
        printf("device %s wait event flag err: %#.8x\r\n", i2c->name, ret_val);
        ret = -4;
        goto err;
    }

err:
    osMutexRelease(i2c->mutex);

    return ret;
}

static int8_t i2c_read(DEVICE_I2C *i2c, uint16_t addr, uint8_t *buf, uint16_t size, uint32_t timeout)
{
    osStatus_t ret = osOK;
    HAL_StatusTypeDef status = HAL_OK;

    if (!i2c->open_state)
    {
        printf("device %s is closed\r\n", i2c->name);
        return -1;
    }

    ret = osMutexAcquire(i2c->mutex, timeout);
    if (ret != osOK)
    {
        printf("device %s acquire mutex err:%d\r\n", i2c->name, ret);
        return -2;
    }

    status = HAL_I2C_Master_Receive_DMA(&i2c->hi2c, addr, buf, size);
    if (status != HAL_OK)
    {
        printf("device %s read data err:%d\r\n", i2c->name, status);
        ret = -3;
        goto err;
    }

    uint32_t ret_val = osEventFlagsWait(i2c->event, I2C_RECV_SUCCEED_EVENT, osFlagsWaitAny, timeout);
    if (ret_val != I2C_RECV_SUCCEED_EVENT)
    {
        printf("device %s wait event flag err: %#.8x\r\n", i2c->name, ret_val);
        ret = -4;
        goto err;
    }

err:
    osMutexRelease(i2c->mutex);

    return ret;
}

static int8_t i2c_ioctl(DEVICE_I2C *i2c, uint8_t cmd, void *arg)
{
    int8_t ret = 0;

    if (i2c == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    switch (cmd)
    {
    default:
        printf("i2c ioctl cmd %d is not supported\r\n", cmd);
        return -2;
    }

    return ret;
}

int8_t i2c_init(DEVICE_I2C *i2c, uint8_t *device_name)
{
    if (i2c == NULL || device_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    if (i2c->open_state)
    {
        printf("device %s is opened\r\n", i2c->name);
        return -2;
    }

    /* 1. init hardware */
    if (!memcmp(device_name, DEVICE_NAME_I2C1, sizeof(DEVICE_NAME_I2C1)))
    {

    }
    else if (!memcmp(device_name, DEVICE_NAME_I2C2, sizeof(DEVICE_NAME_I2C2)))
    {

    }
    else if (!memcmp(device_name, DEVICE_NAME_I2C3, sizeof(DEVICE_NAME_I2C3)))
    {

    }
    else if (!memcmp(device_name, DEVICE_NAME_I2C4, sizeof(DEVICE_NAME_I2C4)))
    {
        MX_I2C4_Init();
        memcpy(i2c, &hi2c4, sizeof(I2C_HandleTypeDef));
        extern DMA_HandleTypeDef hdma_i2c4_rx;
        extern DMA_HandleTypeDef hdma_i2c4_tx;
        hdma_i2c4_tx.Parent = (void *)i2c;
        hdma_i2c4_rx.Parent = (void *)i2c;
    }
    else if (!memcmp(device_name, DEVICE_NAME_I2C5, sizeof(DEVICE_NAME_I2C5)))
    {
    }
    else
    {
        /* add other i2c here */
    }

    /* 2. create event and mutex for device */
    osMutexAttr_t i2c_mutex_attributes = {
    .name = "i2c_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    i2c->mutex = osMutexNew(&i2c_mutex_attributes);

    const osEventFlagsAttr_t i2c_event_attributes = {
    .name = "i2c_event"
    };
    i2c->event = osEventFlagsNew(&i2c_event_attributes);

    /* 3. register callback function */
    HAL_I2C_RegisterCallback((I2C_HandleTypeDef *)i2c, HAL_I2C_MASTER_TX_COMPLETE_CB_ID, MasterTxCpltCallback);
    HAL_I2C_RegisterCallback((I2C_HandleTypeDef *)i2c, HAL_I2C_MASTER_RX_COMPLETE_CB_ID, MasterRxCpltCallback);
    HAL_I2C_RegisterCallback((I2C_HandleTypeDef *)i2c, HAL_I2C_MEM_TX_COMPLETE_CB_ID, MemTxCpltCallback);
    HAL_I2C_RegisterCallback((I2C_HandleTypeDef *)i2c, HAL_I2C_MEM_RX_COMPLETE_CB_ID, MemRxCpltCallback);
    HAL_I2C_RegisterCallback((I2C_HandleTypeDef *)i2c, HAL_I2C_ERROR_CB_ID, ErrorCallback);
    HAL_I2C_RegisterCallback((I2C_HandleTypeDef *)i2c, HAL_I2C_ABORT_CB_ID, AbortCpltCallback);

    /* 4. device rename */
    memcpy(i2c->name, device_name, DEVICE_NAME_LENGTH);

    /* 5. register operation function */
    i2c->open = i2c_open;
    i2c->close = i2c_close;
    i2c->write = i2c_write;
    i2c->read = i2c_read;
    i2c->ioctl = i2c_ioctl;

    /* 6. set open state */
    i2c->open_state = 1;

    return 0;
}