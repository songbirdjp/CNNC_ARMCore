#include "i2c_port.h"

static DEVICE_I2C i2c4_bus = {0};
static DEVICE_I2C *i2c4_bus_get(void)
{
    return &i2c4_bus;
}

/**
  * @brief This function handles I2C4 event interrupt.
  */
void I2C4_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C4_EV_IRQn 0 */

  /* USER CODE END I2C4_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(i2c4_bus_get());
  /* USER CODE BEGIN I2C4_EV_IRQn 1 */

  /* USER CODE END I2C4_EV_IRQn 1 */
}

/**
  * @brief This function handles I2C4 error interrupt.
  */
void I2C4_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C4_ER_IRQn 0 */

  /* USER CODE END I2C4_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(i2c4_bus_get());
  /* USER CODE BEGIN I2C4_ER_IRQn 1 */

  /* USER CODE END I2C4_ER_IRQn 1 */
}

int8_t i2c_bus_init(DEVICE_I2C *i2c_bus, uint8_t *bus_name)
{
    int8_t ret = 0;

    if (i2c_bus == NULL)
    {
        return -1;
    }

    ret = i2c_init(i2c_bus, bus_name);
    if (ret != 0) 
    {
        printf("i2c bus: %s init failed\r\n", bus_name);
        return -2;
    }

    return 0;
}

int8_t i2c_device_write(DEVICE_I2C *i2c_bus, uint16_t dev_addr, uint8_t *data, uint16_t len)
{
    int8_t ret = 0;

    if (i2c_bus == NULL || data == NULL || len == 0)
    {
        return -1;
    }

    ret = i2c_bus->write(i2c_bus, dev_addr, data, len, 1000);
    if (ret != 0) 
    {
        printf("%s write err: %d\r\n", i2c_bus->name, ret);
        return -2;
    }

    return 0;
}

int8_t i2c_device_read(DEVICE_I2C *i2c_bus, uint16_t dev_addr, uint8_t *data, uint16_t len)
{
    int8_t ret = 0;

    if (i2c_bus == NULL || data == NULL || len == 0)
    {
        return -1;
    }

    ret = i2c_bus->read(i2c_bus, dev_addr, data, len, 1000);
    if (ret != 0) 
    {
        printf("%s read err: %d\r\n", i2c_bus->name, ret);
        return -2;
    }

    return 0;
}

#ifndef I2C_BUS_TEST
#include "shell.h"

#define I2C_DEVICE_ADDR     0x40
uint8_t buf[2] __attribute__((section(".ram_d3"))) = {0};
static int8_t i2c_bus_test(uint8_t argc, char *argv[])
{
    int8_t ret = 0;

    switch (atoi(argv[1]))
    {
    case 0:
        ret = i2c_bus_init(i2c4_bus_get(), "i2c4");
        if (ret != 0) 
        {
            printf("i2c4 init failed\r\n");
            ret = -1;
        }
        break;
    case 1:
        buf[0] = atoi(argv[2]);
        uint8_t len = atoi(argv[3]);
        ret = i2c_device_write(i2c4_bus_get(), I2C_DEVICE_ADDR, buf, 1);
        if (ret != 0) 
        {
            printf("i2c write err: %d\r\n", ret);
            break;
        }
        ret = i2c_device_read(i2c4_bus_get(), I2C_DEVICE_ADDR, buf, len);
        if (ret != 0) 
        {
            printf("i2c read err: %d\r\n", ret);
            break;
        }
        printf("read data: ");
        for (uint8_t i = 0; i < len; i++)
        {
            printf("%#.2x ", buf[i]);
        }
        printf("\r\n");
        break;
    default:
        break;
    }

    return ret;
}
MSH_CMD_EXPORT_ALIAS(i2c_bus_test, i2c_bus_test, i2c bus test);
#endif