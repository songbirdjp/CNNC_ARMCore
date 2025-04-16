/**
 * @file drv_i2c.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-07-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "i2c.h"
#include "init_call.h"
#include "drv_i2c.h"
#include "stm32h723xx.h"
TAG("drv_i2c.c");

extern I2C_HandleTypeDef hi2c2;

typedef struct i2c_software_io
{
    GPIO_TypeDef *i2c_gpio_sda;
    uint16_t i2c_pin_sda;
    GPIO_TypeDef *i2c_gpio_scl;
    uint16_t i2c_pin_scl;
} i2c_software_io_t;

typedef struct driver_i2c
{
    device_i2c_t device_i2c;

    I2C_HandleTypeDef *hi2cx;
    i2c_software_io_t i2c_software_io;
} driver_i2c_t;

static driver_i2c_t driver_i2c1 = {0};
// static driver_i2c_t driver_i2c2 = {0};
// static driver_i2c_t driver_i2c3 = {0};
/*software IO*/
// static i2c_software_io_t i2c_software_io =
// {
//     GPIOx, GPIO_PIN_x
// };
static void I2C_Unlock(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    /*Unlock I2C*/
    int8_t times = 9;
    while (times--)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
        osDelay(1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
        osDelay(1);
    }
}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_DeInit(hi2c);
    I2C_Unlock();
    MX_I2C1_Init();
}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        device_i2c_xfer_end((device_i2c_t *)&driver_i2c1);
    }
    // else if (hi2c->Instance == I2C2)
    // {
    //     device_i2c_xfer_end((device_i2c_t *)&driver_i2c2);
    // }
    // else if (hi2c->Instance == I2C3)
    // {
    //     device_i2c_xfer_end((device_i2c_t *)&driver_i2c3);
    // }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        device_i2c_xfer_end((device_i2c_t *)&driver_i2c1);
    }
    // else if (hi2c->Instance == I2C2)
    // {
    //     device_i2c_xfer_end((device_i2c_t *)&driver_i2c2);
    // }
    // else if (hi2c->Instance == I2C3)
    // {
    //     device_i2c_xfer_end((device_i2c_t *)&driver_i2c3);
    // }
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        device_i2c_xfer_end((device_i2c_t *)&driver_i2c1);
    }
    // else if (hi2c->Instance == I2C2)
    // {
    //     device_i2c_xfer_end((device_i2c_t *)&driver_i2c2);
    // }
    // else if (hi2c->Instance == I2C3)
    // {
    //     device_i2c_xfer_end((device_i2c_t *)&driver_i2c3);
    // }
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        device_i2c_xfer_end((device_i2c_t *)&driver_i2c1);
    }
    // else if (hi2c->Instance == I2C2)
    // {
    //     device_i2c_xfer_end((device_i2c_t *)&driver_i2c2);
    // }
    // else if (hi2c->Instance == I2C3)
    // {
    //     device_i2c_xfer_end((device_i2c_t *)&driver_i2c3);
    // }
}

static device_err_t driver_i2c_open(device_i2c_t *const self)
{
    dev_assert(self != NULL);

    driver_i2c_t *driver = (driver_i2c_t *)self->super.user_data;

    const char *name = driver->device_i2c.super.device_attr.name;

    if (0 == strcmp(name, DEVICE_NAME_I2C1))
    {
        I2C_Unlock();
        MX_I2C1_Init();
    }
    else if (0 == strcmp(name, DEVICE_NAME_I2C2))
    {
        // MX_I2C2_Init();
    }
    else if (0 == strcmp(name, DEVICE_NAME_I2C3))
    {
        // MX_I2C3_Init();
    }
    else
    {
        return DEV_ENOTFOUND;
    }

    return DEV_EOK;
}

static device_err_t driver_i2c_close(device_i2c_t *const self)
{
    dev_assert(self != NULL);

    driver_i2c_t *driver = (driver_i2c_t *)self->super.user_data;

    const char *name = driver->device_i2c.super.device_attr.name;

    if ((0 == strcmp(name, DEVICE_NAME_I2C1)) ||
        (0 == strcmp(name, DEVICE_NAME_I2C2)) ||
        (0 == strcmp(name, DEVICE_NAME_I2C3)))
    {
        HAL_I2C_DeInit(driver->hi2cx);
    }
    else
    {
        return DEV_ENOTFOUND;
    }

    return DEV_EOK;
}
/**
 * @brief
 *
 * @param i2c
 * @param buf
 * @param timeout
 * @return int8_t
 */
static device_err_t driver_i2c_write(device_i2c_t *const self, i2c_msg_t const *const buf, uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buf != NULL);
    dev_assert(buf->dataLen != 0);

    HAL_StatusTypeDef HAL_Status = HAL_TIMEOUT;

    driver_i2c_t *driver = (driver_i2c_t *)self->super.user_data;
    i2c_mode_t i2c_mode = driver->device_i2c.i2c_mode;

    if (HAL_I2C_GetState(driver->hi2cx) != HAL_I2C_STATE_READY)
    {
        return DEV_EBUSY;
    }

    switch (i2c_mode)
    {
    case I2C_MODE_HARDWARE_POLLING:
    {
        if (driver->device_i2c.i2c_type == I2C_TYPE_MEN)
        {
            if (driver->device_i2c.i2c_addr_len == I2C_ADDR_7BIT)
            {
                HAL_Status = HAL_I2C_Mem_Write(driver->hi2cx,
                                               buf->dev_addr,
                                               buf->reg_addr,
                                               I2C_MEMADD_SIZE_8BIT,
                                               buf->data,
                                               buf->dataLen,
                                               timeout);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else if (driver->device_i2c.i2c_addr_len == I2C_ADDR_10BIT)
            {
                HAL_Status = HAL_I2C_Mem_Write(driver->hi2cx,
                                               buf->dev_addr,
                                               buf->reg_addr,
                                               I2C_MEMADD_SIZE_16BIT,
                                               buf->data,
                                               buf->dataLen,
                                               timeout);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else
            {
                return DEV_EINVAL;
            }
        }
        else if (driver->device_i2c.i2c_type == I2C_TYPE_BUS)
        {
            HAL_Status = HAL_I2C_Master_Transmit(driver->hi2cx,
                                                 buf->dev_addr,
                                                 buf->data,
                                                 buf->dataLen,
                                                 timeout);
            if (HAL_Status != HAL_OK)
            {
                return DEV_EIO;
            }
        }
        else
        {
            return DEV_EINVAL;
        }
        break;
    }
    case I2C_MODE_HARDWARE_INTERRUPT:
    {
        if (driver->device_i2c.i2c_type == I2C_TYPE_MEN)
        {
            if (driver->device_i2c.i2c_addr_len == I2C_ADDR_7BIT)
            {
                HAL_Status = HAL_I2C_Mem_Write_IT(driver->hi2cx,
                                                  buf->dev_addr,
                                                  buf->reg_addr,
                                                  I2C_MEMADD_SIZE_8BIT,
                                                  buf->data,
                                                  buf->dataLen);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else if (driver->device_i2c.i2c_addr_len == I2C_ADDR_10BIT)
            {
                HAL_Status = HAL_I2C_Mem_Write_IT(driver->hi2cx,
                                                  buf->dev_addr,
                                                  buf->reg_addr,
                                                  I2C_MEMADD_SIZE_16BIT,
                                                  buf->data,
                                                  buf->dataLen);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else
            {
                return DEV_EINVAL;
            }
        }
        else if (driver->device_i2c.i2c_type == I2C_TYPE_BUS)
        {
            HAL_Status = HAL_I2C_Master_Transmit_IT(driver->hi2cx,
                                                    buf->dev_addr,
                                                    buf->data,
                                                    buf->dataLen);
            if (HAL_Status != HAL_OK)
            {
                return DEV_EIO;
            }
        }
        else
        {
            return DEV_EINVAL;
        }
        break;
    }
    case I2C_MODE_HARDWARE_DMA:
    {
        if (driver->device_i2c.i2c_type == I2C_TYPE_MEN)
        {
            if (driver->device_i2c.i2c_addr_len == I2C_ADDR_7BIT)
            {
                HAL_Status = HAL_I2C_Mem_Write_DMA(driver->hi2cx,
                                                   buf->dev_addr,
                                                   buf->reg_addr,
                                                   I2C_MEMADD_SIZE_8BIT,
                                                   buf->data,
                                                   buf->dataLen);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else if (driver->device_i2c.i2c_addr_len == I2C_ADDR_10BIT)
            {
                HAL_Status = HAL_I2C_Mem_Write_DMA(driver->hi2cx,
                                                   buf->dev_addr,
                                                   buf->reg_addr,
                                                   I2C_MEMADD_SIZE_16BIT,
                                                   buf->data,
                                                   buf->dataLen);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else
            {
                return DEV_EINVAL;
            }
        }
        else if (driver->device_i2c.i2c_type == I2C_TYPE_BUS)
        {
            HAL_Status = HAL_I2C_Master_Transmit_DMA(driver->hi2cx,
                                                     buf->dev_addr,
                                                     buf->data,
                                                     buf->dataLen);
            if (HAL_Status != HAL_OK)
            {
                return DEV_EIO;
            }
        }
        else
        {
            return DEV_EINVAL;
        }
        break;
    }

    case I2C_MODE_SOFTWARE:
        /* code */
        break;
    default:
        break;
    }
    return DEV_EOK;
}
/**
 * @brief
 *
 * @param i2c
 * @param buf
 * @param timeout
 * @return int8_t
 */
static device_err_t driver_i2c_read(device_i2c_t *const self, i2c_msg_t *const buf, uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buf != NULL);
    dev_assert(buf->dataLen != 0);

    HAL_StatusTypeDef HAL_Status = HAL_TIMEOUT;

    driver_i2c_t *driver = (driver_i2c_t *)self->super.user_data;
    i2c_mode_t i2c_mode = driver->device_i2c.i2c_mode;

    if (HAL_I2C_GetState(driver->hi2cx) != HAL_I2C_STATE_READY)
    {
        return DEV_EBUSY;
    }
    switch (i2c_mode)
    {
    case I2C_MODE_HARDWARE_POLLING:
    {
        if (driver->device_i2c.i2c_type == I2C_TYPE_MEN)
        {
            if (driver->device_i2c.i2c_addr_len == I2C_ADDR_7BIT)
            {
                HAL_Status = HAL_I2C_Mem_Read(driver->hi2cx,
                                              buf->dev_addr,
                                              buf->reg_addr,
                                              I2C_MEMADD_SIZE_8BIT,
                                              buf->data,
                                              buf->dataLen,
                                              timeout);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else if (driver->device_i2c.i2c_addr_len == I2C_ADDR_10BIT)
            {
                HAL_Status = HAL_I2C_Mem_Read(driver->hi2cx,
                                              buf->dev_addr,
                                              buf->reg_addr,
                                              I2C_MEMADD_SIZE_16BIT,
                                              buf->data,
                                              buf->dataLen,
                                              timeout);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else
            {
                return DEV_EINVAL;
            }
        }
        else if (driver->device_i2c.i2c_type == I2C_TYPE_BUS)
        {
            HAL_Status = HAL_I2C_Master_Receive(driver->hi2cx,
                                                buf->dev_addr,
                                                buf->data,
                                                buf->dataLen,
                                                timeout);
            if (HAL_Status != HAL_OK)
            {
                return DEV_EIO;
            }
        }
        else
        {
            return DEV_EINVAL;
        }
        break;
    }
    case I2C_MODE_HARDWARE_INTERRUPT:
    {
        if (driver->device_i2c.i2c_type == I2C_TYPE_MEN)
        {
            if (driver->device_i2c.i2c_addr_len == I2C_ADDR_7BIT)
            {
                HAL_Status = HAL_I2C_Mem_Read_IT(driver->hi2cx,
                                                 buf->dev_addr,
                                                 buf->reg_addr,
                                                 I2C_MEMADD_SIZE_8BIT,
                                                 buf->data,
                                                 buf->dataLen);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else if (driver->device_i2c.i2c_addr_len == I2C_ADDR_10BIT)
            {
                HAL_Status = HAL_I2C_Mem_Read_IT(driver->hi2cx,
                                                 buf->dev_addr,
                                                 buf->reg_addr,
                                                 I2C_MEMADD_SIZE_16BIT,
                                                 buf->data,
                                                 buf->dataLen);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else
            {
                return DEV_EINVAL;
            }
        }
        else if (driver->device_i2c.i2c_type == I2C_TYPE_BUS)
        {
            HAL_Status = HAL_I2C_Master_Receive_IT(driver->hi2cx,
                                                   buf->dev_addr,
                                                   buf->data,
                                                   buf->dataLen);
            if (HAL_Status != HAL_OK)
            {
                return DEV_EIO;
            }
        }
        else
        {
            return DEV_EINVAL;
        }
        break;
    }
    case I2C_MODE_HARDWARE_DMA:
    {
        if (driver->device_i2c.i2c_type == I2C_TYPE_MEN)
        {
            if (driver->device_i2c.i2c_addr_len == I2C_ADDR_7BIT)
            {
                HAL_Status = HAL_I2C_Mem_Read_DMA(driver->hi2cx,
                                                  buf->dev_addr,
                                                  buf->reg_addr,
                                                  I2C_MEMADD_SIZE_8BIT,
                                                  buf->data,
                                                  buf->dataLen);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else if (driver->device_i2c.i2c_addr_len == I2C_ADDR_10BIT)
            {
                HAL_Status = HAL_I2C_Mem_Read_DMA(driver->hi2cx,
                                                  buf->dev_addr,
                                                  buf->reg_addr,
                                                  I2C_MEMADD_SIZE_16BIT,
                                                  buf->data,
                                                  buf->dataLen);
                if (HAL_Status != HAL_OK)
                {
                    return DEV_EIO;
                }
            }
            else
            {
                return DEV_EINVAL;
            }
        }
        else if (driver->device_i2c.i2c_type == I2C_TYPE_BUS)
        {
            HAL_Status = HAL_I2C_Master_Transmit_DMA(driver->hi2cx,
                                                     buf->dev_addr,
                                                     buf->data,
                                                     buf->dataLen);
            if (HAL_Status != HAL_OK)
            {
                return DEV_EIO;
            }
        }
        else
        {
            return DEV_EINVAL;
        }
        break;
    }
    case I2C_MODE_SOFTWARE:
    {
        break;
    }
    default:
        break;
    }
    return DEV_EOK;
}

/**
 * @brief
 *
 * @param i2c
 * @param cmd
 * @param arg
 * @return int8_t
 */
static device_err_t driver_i2c_ioctl(device_i2c_t *const self, i2c_cmd_t cmd, void *const arg)
{
    dev_assert(self != NULL);
    dev_assert((cmd > I2C_CMD_MIN) && (cmd < I2C_CMD_MAX));

    HAL_StatusTypeDef HAL_Status = HAL_ERROR;
    device_err_t device_err = DEV_EOK;

    driver_i2c_t *driver = (driver_i2c_t *)self->super.user_data;

    switch (cmd)
    {
    case I2C_CMD_SET_ADDR_LEN:
        dev_assert((*(i2c_addr_len_t *)arg == I2C_ADDR_7BIT) ||
                   (*(i2c_addr_len_t *)arg == I2C_ADDR_10BIT));

        driver->device_i2c.i2c_addr_len = *((i2c_addr_len_t *)arg);

        if (driver->device_i2c.i2c_addr_len == I2C_ADDR_10BIT)
        {
            (driver->hi2cx->Instance->CR2) |= (I2C_CR2_ADD10);
        }
        else
        {
            (driver->hi2cx->Instance->CR2) &= ~(I2C_CR2_ADD10);
        }
        break;
    case I2C_CMD_SET_MODE:
        dev_assert((*(i2c_mode_t *)arg == I2C_MODE_HARDWARE_POLLING) ||
                   (*(i2c_mode_t *)arg == I2C_MODE_HARDWARE_INTERRUPT) ||
                   (*(i2c_mode_t *)arg == I2C_MODE_HARDWARE_DMA) ||
                   (*(i2c_mode_t *)arg == I2C_MODE_SOFTWARE));

        driver->device_i2c.i2c_mode = *((i2c_mode_t *)arg);
        break;
    case I2C_CMD_SET_TYPE:
        dev_assert((*(i2c_type_t *)arg == I2C_TYPE_MEN) ||
                   (*(i2c_type_t *)arg == I2C_TYPE_BUS));

        driver->device_i2c.i2c_type = *((i2c_type_t *)arg);
        break;
    case I2C_CMD_GET_ACK_POLLING:
        HAL_Status = HAL_I2C_IsDeviceReady(driver->hi2cx,
                                           ((i2c_ack_polling_arg_t *)arg)->device_addr,
                                           1,
                                           1);
        if (HAL_Status == HAL_OK)
        {
            ((i2c_ack_polling_arg_t *)arg)->i2c_ack_polling_state = I2C_ACK_POLLING_READY;
        }
        else
        {
            ((i2c_ack_polling_arg_t *)arg)->i2c_ack_polling_state = I2C_ACK_POLLING_BUSY;
        }
        break;
    case I2C_CMD_INIT:
        HAL_I2C_DeInit(driver->hi2cx);
        if (driver->hi2cx == &hi2c1)
        {
            I2C_Unlock();
            MX_I2C1_Init();
        }
        break;
    default:
        device_err = DEV_EINVAL;
        break;
    }
    return device_err;
}
/**
 * @brief
 *
 * @param i2c
 * @param name
 * @param i2c_mode
 * @return uint8_t
 */
static void driver_i2c_register(driver_i2c_t *const self, I2C_HandleTypeDef *const hi2cx, char const *name)
{
    dev_assert(self != NULL);
    dev_assert(!strcmp(name, DEVICE_NAME_I2C1) ||
               !strcmp(name, DEVICE_NAME_I2C2) ||
               !strcmp(name, DEVICE_NAME_I2C3));

    memset(self, 0, sizeof(driver_i2c_t));

    self->hi2cx = hi2cx;

    static device_i2c_ops_t device_i2c_ops = {
        .open = driver_i2c_open,
        .close = driver_i2c_close,
        .read = driver_i2c_read,
        .write = driver_i2c_write,
        .ioctl = driver_i2c_ioctl};

    device_i2c_register((device_i2c_t *)self, name, &device_i2c_ops, self);
}
void driver_i2c_init(void)
{
    driver_i2c_register(&driver_i2c1, &hi2c1, DEVICE_NAME_I2C1);
    //    driver_i2c_register(&driver_i2c2, &hi2c2, DEVICE_NAME_I2C2);
    // driver_i2c_register(&driver_i2c3, &hi2c3, DEVICE_NAME_I2C3);
}
INIT_BOARD_EXPORT(driver_i2c_init);
int8_t i2c_test(void)
{
    int8_t ret = 0;
    device_t *device_i2c1 = NULL;
    uint8_t data_w[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t data_r[8] = {0x00};
    uint8_t deviceID = 0xA0;
    i2c_ack_polling_arg_t i2c_ack_polling_arg = {
        .device_addr = deviceID,
        .i2c_ack_polling_state = I2C_ACK_POLLING_BUSY};
    i2c_msg_t i2c_w_msg = {
        .dev_addr = deviceID,
        .reg_addr = 0x00,
        .data = data_w,
        .dataLen = 8};

    i2c_msg_t i2c_r_msg = {
        .dev_addr = deviceID,
        .reg_addr = 0x00,
        .data = data_r,
        .dataLen = 8};

    i2c_addr_len_t i2c_addr_len = I2C_ADDR_7BIT;
    printf("i2c test\r\n");
    driver_i2c_init();

    device_i2c1 = device_find(DEVICE_NAME_I2C1);

    if (device_i2c1 == NULL)
    {
        return -1;
    }
    ret = device_open(device_i2c1);
    if (ret != 0)
    {
        return -2;
    }
    ret = device_ioctl(device_i2c1, I2C_CMD_SET_ADDR_LEN, &i2c_addr_len);
    if (ret != 0)
    {
        return -3;
    }
    ret = device_write(device_i2c1, &i2c_w_msg, 0, 1000);
    if (ret != 0)
    {
        return -4;
    }
    do
    {
        ret = device_ioctl(device_i2c1, I2C_CMD_GET_ACK_POLLING, &i2c_ack_polling_arg);
        if (ret != 0)
        {
            return -5;
        }
    } while (i2c_ack_polling_arg.i2c_ack_polling_state != I2C_ACK_POLLING_READY);
    ret = device_read(device_i2c1, &i2c_r_msg, 0, 1000);
    if (ret != 0)
    {
        return -6;
    }
    ret = device_close(device_i2c1);
    if (ret != 0)
    {
        return -7;
    }
    printf("i2c test succeed!\r\n");
    return 0;
}
