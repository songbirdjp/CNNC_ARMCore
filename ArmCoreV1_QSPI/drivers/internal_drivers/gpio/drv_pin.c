/**
 * @file drv_pin.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "gpio.h"
#include "init_call.h"
#include "drv_pin.h"
#include "stm32h723xx.h"

TAG("drv_pin.c");

typedef struct driver_pin
{
    device_pin_t device_pin;

    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} driver_pin_t;

static driver_pin_t driver_pin_B15 = {0};
static driver_pin_t driver_pin_B14 = {0};
static driver_pin_t driver_pin_C5 = {0};
static driver_pin_t driver_pin_B0 = {0};
static driver_pin_t driver_pin_B1 = {0};
static driver_pin_t driver_pin_A2 = {0};
static driver_pin_t driver_pin_B10 = {0};

static driver_pin_t driver_pin_G3 = {0};

static driver_pin_t driver_pin_C6 = {0};

static driver_pin_t driver_pin_A15 = {0};
static driver_pin_t driver_pin_G14 = {0};
static driver_pin_t driver_pin_B3 = {0};
static driver_pin_t driver_pin_C7 = {0};
static driver_pin_t driver_pin_D12 = {0};
static driver_pin_t driver_pin_D11 = {0};
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
}
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
}
static device_err_t driver_pin_open(device_pin_t *const self)
{
    dev_assert(self != NULL);
    
    return DEV_EOK;
}
static device_err_t driver_pin_close(device_pin_t *const self)
{
    dev_assert(self != NULL);

    return DEV_EOK;
}
static device_err_t driver_pin_read(device_pin_t *const self,
                                    pin_msg_t *const buf,
                                    uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buf != NULL);

    GPIO_PinState bit_status = GPIO_PIN_RESET;

    driver_pin_t *driver = (driver_pin_t *)self->super.user_data;

    bit_status = HAL_GPIO_ReadPin(driver->GPIOx, driver->GPIO_Pin);

    *buf = (pin_state_t)bit_status;

    return DEV_EOK;
}
static device_err_t driver_pin_write(device_pin_t *const self,
                                     pin_msg_t const *const buf,
                                     uint32_t timeout)
{
    dev_assert(self != NULL);
    dev_assert(buf != NULL);

    driver_pin_t *driver = (driver_pin_t *)self->super.user_data;

    HAL_GPIO_WritePin(driver->GPIOx, driver->GPIO_Pin, (GPIO_PinState)(*buf));
    
    return DEV_EOK;
}
static device_err_t driver_pin_ioctl(device_pin_t *const self,
                                     pin_cmd_t cmd,
                                     void *const arg)
{
    return DEV_EOK;
}
static void driver_pin_register(driver_pin_t *const self,
                                GPIO_TypeDef *GPIOx,
                                uint16_t GPIO_Pin,
                                char const *name)
{
    dev_assert(self != NULL);

    memset(self, 0, sizeof(driver_pin_t));

    MX_GPIO_Init();

    self->GPIOx = GPIOx;
    self->GPIO_Pin = GPIO_Pin;

    static device_pin_ops_t device_pin_ops = {
        .open = driver_pin_open,
        .close = driver_pin_close,
        .read = driver_pin_read,
        .write = driver_pin_write,
        .ioctl = driver_pin_ioctl};

    device_pin_register((device_pin_t *)self, name, &device_pin_ops, self);
}
void driver_pin_init(void)
{
    driver_pin_register(&driver_pin_C5, GPIOC, GPIO_PIN_5, DEVICE_NAME_PIN_DO_SOFTWARE_MV_TREATMENT_EN);
    driver_pin_register(&driver_pin_B14, GPIOB, GPIO_PIN_14, DEVICE_NAME_PIN_DO_SOFTWARE_KV_TREATMENT_EN);
    driver_pin_register(&driver_pin_B1, GPIOB, GPIO_PIN_1, DEVICE_NAME_PIN_DO_SOFTWARE_MOVE_EN);
    driver_pin_register(&driver_pin_B15, GPIOB, GPIO_PIN_15, DEVICE_NAME_PIN_DO_SOFTWARE_HV_EN);
    driver_pin_register(&driver_pin_A2, GPIOA, GPIO_PIN_2, DEVICE_NAME_PIN_DO_TREATMENT_MOTION_ENABLE);
    driver_pin_register(&driver_pin_B0, GPIOB, GPIO_PIN_0, DEVICE_NAME_PIN_DO_THREE_PHASE_POWER_ON);
    driver_pin_register(&driver_pin_B10, GPIOB, GPIO_PIN_10, DEVICE_NAME_PIN_DO_ASU_MOTION_ENABLE);

    driver_pin_register(&driver_pin_G3, GPIOG, GPIO_PIN_3, DEVICE_NAME_PIN_DI_GATING);

    driver_pin_register(&driver_pin_A15, GPIOA, GPIO_PIN_15, DEVICE_NAME_PIN_RUN_LED_1);
    driver_pin_register(&driver_pin_G14, GPIOG, GPIO_PIN_14, DEVICE_NAME_PIN_RUN_LED_2);
    driver_pin_register(&driver_pin_B3, GPIOB, GPIO_PIN_3, DEVICE_NAME_PIN_RUN_LED_3);
    driver_pin_register(&driver_pin_C7, GPIOC, GPIO_PIN_7, DEVICE_NAME_PIN_RUN_LED_4);
    driver_pin_register(&driver_pin_D12, GPIOD, GPIO_PIN_12, DEVICE_NAME_PIN_RUN_LED_5);
    driver_pin_register(&driver_pin_D11, GPIOD, GPIO_PIN_11, DEVICE_NAME_PIN_RUN_LED_6);
}
INIT_BOARD_EXPORT(driver_pin_init);
/********************************TEST******************************************/
static void pin_test_rising_callback_high(device_pin_t *self)
{
    printf("rising exti high\r\n");
    printf("pin state:%s\r\n",self->pin_state ? "SET" : "RESET");
}
static void pin_test_rising_callback_middle(device_pin_t *self)
{
    printf("rising exti middle\r\n");
}
static void pin_test_rising_callback_low(device_pin_t *self)
{
    printf("rising exti low\r\n");
}
static void pin_test_falling_callback_high(device_pin_t *self)
{
    printf("falling exti high\r\n");
    printf("pin state:%s\r\n",self->pin_state ? "SET" : "RESET");
}
static void pin_test_falling_callback_middle(device_pin_t *self)
{
    printf("falling exti middle\r\n");
}
static void pin_test_falling_callback_low(device_pin_t *self)
{
    printf("falling exti low\r\n");
}
int8_t pin_test(void)
{
    int8_t ret = 0;
    pin_msg_t pin_msg_r;
    device_t *device_pin15 = NULL;

    driver_pin_init();

    device_pin15 = device_find("PA.15");

    if (device_pin15 == NULL)
    {
        return -1;
    }
    ret = device_open(device_pin15);
    if (ret != 0)
    {
        return -2;
    }
    ret = device_ioctl(device_pin15,
                       PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_HIGH,
                       pin_test_rising_callback_high);
    if (ret != 0)
    {
        return -3;
    }
    ret = device_ioctl(device_pin15,
                       PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_HIGH,
                       pin_test_falling_callback_high);
    if (ret != 0)
    {
        return -3;
    }
    ret = device_ioctl(device_pin15,
                       PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_LOW,
                       pin_test_rising_callback_low);
    if (ret != 0)
    {
        return -3;
    }
    ret = device_ioctl(device_pin15,
                       PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_LOW,
                       pin_test_falling_callback_low);
    if (ret != 0)
    {
        return -3;
    }
    ret = device_ioctl(device_pin15,
                       PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_MIDDLE,
                       pin_test_rising_callback_middle);
    if (ret != 0)
    {
        return -3;
    }
    ret = device_ioctl(device_pin15,
                       PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_MIDDLE,
                       pin_test_falling_callback_middle);
    if (ret != 0)
    {
        return -3;
    }
    while (1)
    {
        ret = device_read(device_pin15, &pin_msg_r, 0, 0);
        if (ret != 0)
        {
            return -3;
        }
    }
}
