/**
 * @file drv_mcp23017.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-06-05
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "drv_mcp23017.h"
#include "dev_i2c.h"
#include "init_call.h"

TAG("drv_mcp23017.c");

typedef enum driver_mcp23017_reg
{
    DRIVER_MCP23017_REG_IODIR_A = 0x00, /**< IO direction A register */
    DRIVER_MCP23017_REG_IODIR_B,        /**< IO direction B register */
    DRIVER_MCP23017_REG_IPOL_A,         /**< Input polarity A register */
    DRIVER_MCP23017_REG_IPOL_B,         /**< Input polarity B register */
    DRIVER_MCP23017_REG_GPINTEN_A,      /**< Interrupt-on-change A register */
    DRIVER_MCP23017_REG_GPINTEN_B,      /**< Interrupt-on-change B register */
    DRIVER_MCP23017_REG_DEFVAL_A,       /**< Default value A register */
    DRIVER_MCP23017_REG_DEFVAL_B,       /**< Default value B register */
    DRIVER_MCP23017_REG_INTCON_A,       /**< Interrupt control A register */
    DRIVER_MCP23017_REG_INTCON_B,       /**< Interrupt control B register */
    DRIVER_MCP23017_REG_IOCON,          /**< Configuration register */
    DRIVER_MCP23017_REG_GPPU_A = 0x0C,  /**< Pull-up resistors A register */
    DRIVER_MCP23017_REG_GPPU_B,         /**< Pull-up resistors B register */
    DRIVER_MCP23017_REG_INTF_A,         /**< Interrupt flag A register */
    DRIVER_MCP23017_REG_INTF_B,         /**< Interrupt flag B register */
    DRIVER_MCP23017_REG_INTCAP_A,       /**< Interrupt capture A register */
    DRIVER_MCP23017_REG_INTCAP_B,       /**< Interrupt capture B register */
    DRIVER_MCP23017_REG_GPIO_A,         /**< Port A register */
    DRIVER_MCP23017_REG_GPIO_B,         /**< Port B register */
    DRIVER_MCP23017_REG_OLAT_A,         /**< Output latch A register */
    DRIVER_MCP23017_REG_OLAT_B,         /**< Output latch B register */
} driver_mcp23017_reg_t;

typedef struct driver_mcp23017
{
    device_t super;

    device_i2c_t *device_i2c;

    i2c_addr_len_t mcp23017_i2c_addr_len;
    i2c_mode_t mcp23017_i2c_mode_t;
    i2c_type_t mcp23017_i2c_type;

    uint8_t *gpio_pin;
    uint8_t device_addr;

} driver_mcp23017_t;

static driver_mcp23017_t driver_mcp23017_0 = {0};
static driver_mcp23017_t driver_mcp23017_1 = {0};
static driver_mcp23017_t driver_mcp23017_2 = {0};
static driver_mcp23017_t driver_mcp23017_3 = {0};
static device_err_t driver_mcp23017_open(device_t *const self)
{
    dev_assert(self != NULL);

    driver_mcp23017_t *device = (driver_mcp23017_t *)self;

    dev_assert(device->device_i2c != NULL);

    return device_open((device_t *)(device->device_i2c));
}
static device_err_t driver_mcp23017_close(device_t *const self)
{
    dev_assert(self != NULL);

    driver_mcp23017_t *device = (driver_mcp23017_t *)self;

    dev_assert(device->device_i2c != NULL);

    return device_close((device_t *)(device->device_i2c));
}

static device_err_t driver_mcp23017_read(device_t *const self,
                                         void *const buffer,
                                         uint32_t size,
                                         uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buffer != NULL);

    driver_mcp23017_t *device = (driver_mcp23017_t *)self;

    dev_assert(device->device_i2c != NULL);

    device_err_t device_err;

    mcp23017_msg_t *mcp23017_msg = (mcp23017_msg_t *)buffer;
    i2c_msg_t i2c_msg = {
        .dev_addr = device->device_addr,
        .data = mcp23017_msg->data,
        .dataLen = mcp23017_msg->dataLen};

    if (mcp23017_msg->gpio_port == DRIVER_MCP23017_GPA)
    {
        i2c_msg.reg_addr = DRIVER_MCP23017_REG_GPIO_A;
    }
    else if (mcp23017_msg->gpio_port == DRIVER_MCP23017_GPB)
    {
        if (mcp23017_msg->dataLen != 1)
        {
            return DEV_EINVAL;
        }
        i2c_msg.reg_addr = DRIVER_MCP23017_REG_GPIO_B;
    }
    else
    {
        return DEV_EINVAL;
    }
    if (device->mcp23017_i2c_addr_len != device->device_i2c->i2c_addr_len)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_ADDR_LEN,
                                  &(device->mcp23017_i2c_addr_len));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    if (device->mcp23017_i2c_mode_t != device->device_i2c->i2c_mode)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_MODE,
                                  &(device->mcp23017_i2c_mode_t));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    if (device->mcp23017_i2c_type != device->device_i2c->i2c_type)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_TYPE,
                                  &(device->mcp23017_i2c_type));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    return device_read((device_t *)(device->device_i2c), (void *)&i2c_msg, size, timeout);
}
static device_err_t driver_mcp23017_write(device_t *const self,
                                          void const *const buffer,
                                          uint32_t size,
                                          uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buffer != NULL);

    driver_mcp23017_t *device = (driver_mcp23017_t *)self;

    dev_assert(device->device_i2c != NULL);

    device_err_t device_err;

    mcp23017_msg_t *mcp23017_msg = (mcp23017_msg_t *)buffer;
    i2c_msg_t i2c_msg = {
        .dev_addr = device->device_addr,
        .data = mcp23017_msg->data,
        .dataLen = mcp23017_msg->dataLen};

    if (mcp23017_msg->gpio_port == DRIVER_MCP23017_GPA)
    {
        i2c_msg.reg_addr = DRIVER_MCP23017_REG_GPIO_A;
    }
    else if (mcp23017_msg->gpio_port == DRIVER_MCP23017_GPB)
    {
        if (mcp23017_msg->dataLen != 1)
        {
            return DEV_EINVAL;
        }
        i2c_msg.reg_addr = DRIVER_MCP23017_REG_GPIO_B;
    }
    else
    {
        return DEV_EINVAL;
    }

    if (device->mcp23017_i2c_addr_len != device->device_i2c->i2c_addr_len)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_ADDR_LEN,
                                  &(device->mcp23017_i2c_addr_len));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    if (device->mcp23017_i2c_mode_t != device->device_i2c->i2c_mode)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_MODE,
                                  &(device->mcp23017_i2c_mode_t));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    if (device->mcp23017_i2c_type != device->device_i2c->i2c_type)
    {
        device_err = device_ioctl((device_t *)(device->device_i2c),
                                  I2C_CMD_SET_TYPE,
                                  &(device->mcp23017_i2c_type));
        if (device_err != DEV_EOK)
        {
            return DEV_EIO;
        }
    }
    return device_write((device_t *)(device->device_i2c), (void *)&i2c_msg, size, timeout);
}
static device_err_t driver_mcp23017_ioctl(device_t *const self, uint8_t cmd, void *const arg)
{
    dev_assert(self != NULL);

    driver_mcp23017_t *device = (driver_mcp23017_t *)self;

    device_err_t device_err = DEV_EINVAL;

    if (cmd > DRIVER_MCP23017_CMD_MIN)
    {
        switch (cmd)
        {
        case DRIVER_MCP23017_CMD_INT_ENABLE:
        {
            mcp23017_cmd_int_enable_t *int_enable = (mcp23017_cmd_int_enable_t *)arg;
            int8_t ret = gpio_pin_irq_callback_register(int_enable->gpio_pin, int_enable->callback);
            if (ret != 0)
            {
                device_err = DEV_EIO;
            }
            break;
        }
        case DRIVER_MCP23017_CMD_CONFIG:
        {
            mcp23017_cmd_config_t *config = (mcp23017_cmd_config_t *)arg;
            i2c_msg_t i2c_msg = {
                .dev_addr = device->device_addr,
                .dataLen = 2};

            i2c_msg.reg_addr = DRIVER_MCP23017_REG_IODIR_A;
            i2c_msg.data = &config->io_dir;
            device_err = device_write((device_t *)(device->device_i2c), (void *)&i2c_msg, NULL, 100);
            if (device_err != DEV_EOK)
            {
                return DEV_EIO;
            }

            i2c_msg.reg_addr = DRIVER_MCP23017_REG_IPOL_A;
            i2c_msg.data = &config->input_polarity;
            device_err = device_write((device_t *)(device->device_i2c), (void *)&i2c_msg, NULL, 100);
            if (device_err != DEV_EOK)
            {
                return DEV_EIO;
            }

            i2c_msg.reg_addr = DRIVER_MCP23017_REG_GPINTEN_A;
            i2c_msg.data = &config->int_enable;
            device_err = device_write((device_t *)(device->device_i2c), (void *)&i2c_msg, NULL, 100);
            if (device_err != DEV_EOK)
            {
                return DEV_EIO;
            }

            i2c_msg.reg_addr = DRIVER_MCP23017_REG_DEFVAL_A;
            i2c_msg.data = &config->default_value;
            device_err = device_write((device_t *)(device->device_i2c), (void *)&i2c_msg, NULL, 100);
            if (device_err != DEV_EOK)
            {
                return DEV_EIO;
            }

            i2c_msg.reg_addr = DRIVER_MCP23017_REG_INTCON_A;
            i2c_msg.data = &config->interrupt_control;
            device_err = device_write((device_t *)(device->device_i2c), (void *)&i2c_msg, NULL, 100);
            if (device_err != DEV_EOK)
            {
                return DEV_EIO;
            }

            i2c_msg.reg_addr = DRIVER_MCP23017_REG_IOCON;
            i2c_msg.data = &config->io_config;
            i2c_msg.dataLen = 1;
            device_err = device_write((device_t *)(device->device_i2c), (void *)&i2c_msg, NULL, 100);
            if (device_err != DEV_EOK)
            {
                return DEV_EIO;
            }

            i2c_msg.reg_addr = DRIVER_MCP23017_REG_GPPU_A;
            i2c_msg.data = &config->pull_up_resistors;
            i2c_msg.dataLen = 2;
            device_err = device_write((device_t *)(device->device_i2c), (void *)&i2c_msg, NULL, 100);
            if (device_err != DEV_EOK)
            {
                return DEV_EIO;
            }
            break;
        }
        default:
            device_err = DEV_EINVAL;
            break;
        }
    }
    else
    {
        dev_assert(device->device_i2c != NULL);
        return device_ioctl((device_t *)(device->device_i2c), cmd, arg);
    }

    return device_err;
}
static void driver_mcp23017_register(driver_mcp23017_t *const self,
                                     char const *name,
                                     char const *i2c_name,
                                     uint8_t device_addr)
{
    dev_assert(self != NULL);

    dev_assert((driver_mcp23017_t *)device_find(name) == NULL);
    dev_assert((device_i2c_t *)device_find(i2c_name) != NULL);

    memset(self, 0, sizeof(driver_mcp23017_t));

    self->device_i2c = (device_i2c_t *)device_find(i2c_name);

    device_attr_t attr = {
        .device_sole = DEVICE_NON_UNIQUENESS,
        .device_type = DEVICE_I2C_MEM,
        .device_duplex = DEVICE_HALF_DUPLEX,
        .name = name};

    self->mcp23017_i2c_addr_len = I2C_ADDR_7BIT;
    self->mcp23017_i2c_mode_t = I2C_MODE_HARDWARE_DMA;
    self->mcp23017_i2c_type = I2C_TYPE_MEN;

    self->device_addr = (device_addr << 1);

    static device_ops_t device_mcp23017_ops = {
        .open = driver_mcp23017_open,
        .close = driver_mcp23017_close,
        .read = driver_mcp23017_read,
        .write = driver_mcp23017_write,
        .ioctl = driver_mcp23017_ioctl};

    device_register((device_t *)self, &attr, &device_mcp23017_ops, self);
}
void driver_mcp23017_init(void)
{
#define MCP23017_BOARD_ID_ADDR (0b00100000)
    driver_mcp23017_register(&driver_mcp23017_0,
                             DEVICE_NAME_MCP23017_0,
                             DEVICE_NAME_I2C1,
                             MCP23017_BOARD_ID_ADDR);
#define MCP23017_1_ID_ADDR (0b00100001)
    driver_mcp23017_register(&driver_mcp23017_1,
                             DEVICE_NAME_MCP23017_1,
                             DEVICE_NAME_I2C1,
                             MCP23017_1_ID_ADDR);
#define MCP23017_2_ID_ADDR (0b00100010)
    driver_mcp23017_register(&driver_mcp23017_2,
                             DEVICE_NAME_MCP23017_2,
                             DEVICE_NAME_I2C1,
                             MCP23017_2_ID_ADDR);
#define MCP23017_3_ID_ADDR (0b00100011)
    driver_mcp23017_register(&driver_mcp23017_3,
                             DEVICE_NAME_MCP23017_3,
                             DEVICE_NAME_I2C1,
                             MCP23017_3_ID_ADDR);
}
INIT_DEVICE_EXPORT(driver_mcp23017_init);

// #define MCP23017_TEST
#ifdef MCP23017_TEST
#include "shell.h"
#include "ulog.h"
void mcp23017_interrupt_handler(void)
{
}
static void mcp23017_test_thread(void *argument)
{
    device_err_t err;
    driver_mcp23017_t *mcp23017 = (driver_mcp23017_t *)argument;

    err = device_open((device_t *)mcp23017);
    if (err != DEV_EOK)
    {
        return;
    }
    uint16_t outData = 0xFFFF;
    uint16_t inData = 0;
    mcp23017_msg_t msg = {
        .driver_mcp23017_reg = DRIVER_MCP23017_REG_IODIR_A,
        .data = &outData,
        .dataLen = 2};
    err = device_write((device_t *)mcp23017, (void *)&msg, sizeof(msg), 1000);
    if (err != DEV_EOK)
    {
        return;
    }
    for (;;)
    {
        msg.driver_mcp23017_reg = DRIVER_MCP23017_REG_GPIO_A;
        msg.data = &inData;
        msg.dataLen = 2;
        err = device_read((device_t *)mcp23017, (void *)&msg, sizeof(msg), 1000);
        if (err != DEV_EOK)
        {
            LOG_I("read error\r\n");
        }
        else
        {
            LOG_I("read data:0x%x\r\n", inData);
        }
        osDelay(1000);
    }
}
static int8_t mcp23017_test(uint8_t argc, uint8_t *argv[])
{
    osThreadAttr_t thread_attributes = {
        .name = "mcp23017_test",
        .priority = osPriorityNormal,
        .stack_size = 1024 * 4,
        .cb_size = 0};

    driver_mcp23017_t *mcp23017 = (driver_mcp23017_t *)device_find(DEVICE_NAME_MCP23017_0);
    if (mcp23017 == NULL)
    {
        return -1;
    }

    osThreadId_t thread_id;
    thread_id = osThreadNew(mcp23017_test_thread, mcp23017, &thread_attributes);
    if (thread_id == NULL)
    {
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(mcp23017_test, mcp23017_test, mcp23017 test);

#endif