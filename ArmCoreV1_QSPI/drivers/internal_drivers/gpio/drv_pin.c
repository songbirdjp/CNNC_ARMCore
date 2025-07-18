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


// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
// {
//     uint8_t pin_index = POSITION_VAL(GPIO_Pin);

// }
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

static driver_pin_t driver_pin_A15 = {0};
static driver_pin_t driver_pin_C6 = {0};
static driver_pin_t driver_pin_A8 = {0};
static driver_pin_t driver_pin_C7 = {0};
static driver_pin_t driver_pin_C8 = {0};
static driver_pin_t driver_pin_C9 = {0};

static driver_pin_t driver_pin_B1 = {0};
static driver_pin_t driver_pin_B0 = {0};
static driver_pin_t driver_pin_B13 = {0};
static driver_pin_t driver_pin_B14 = {0};
static driver_pin_t driver_pin_B15 = {0};

static driver_pin_t driver_pin_E6 = {0};
static driver_pin_t driver_pin_E5 = {0};
static driver_pin_t driver_pin_E4 = {0};
static driver_pin_t driver_pin_E2 = {0};
static driver_pin_t driver_pin_G6 = {0};
static driver_pin_t driver_pin_G7 = {0};

int driver_pin_init(void)
{
    driver_pin_register(&driver_pin_A15, GPIOA, GPIO_PIN_15, DEVICE_NAME_PIN_DO_POWER_CUT);
    driver_pin_register(&driver_pin_C6, GPIOC, GPIO_PIN_6, DEVICE_NAME_PIN_DO_HVEN);
    driver_pin_register(&driver_pin_A8, GPIOA, GPIO_PIN_8, DEVICE_NAME_PIN_DO_PULSE_INHIBIT);
    driver_pin_register(&driver_pin_C7, GPIOC, GPIO_PIN_7, DEVICE_NAME_PIN_DO_MV_TREATMENT_EN);
    driver_pin_register(&driver_pin_C8, GPIOC, GPIO_PIN_8, DEVICE_NAME_PIN_DO_KV_TREATMENT_EN);
    driver_pin_register(&driver_pin_C9, GPIOC, GPIO_PIN_9, DEVICE_NAME_PIN_DO_EMERGENCY);

    driver_pin_register(&driver_pin_B1, GPIOB, GPIO_PIN_1, DEVICE_NAME_PIN_DI_INT);
    driver_pin_register(&driver_pin_B0, GPIOB, GPIO_PIN_0, DEVICE_NAME_PIN_DI_GATING);
    driver_pin_register(&driver_pin_B13, GPIOB, GPIO_PIN_13, DEVICE_NAME_PIN_DI_SLIPRING_HVEN);
    driver_pin_register(&driver_pin_B14, GPIOB, GPIO_PIN_14, DEVICE_NAME_PIN_DI_SLIPRING_KV_TREATMENT_EN);
    driver_pin_register(&driver_pin_B15, GPIOB, GPIO_PIN_15, DEVICE_NAME_PIN_DI_SLIPRING_MV_TREATMENT_EN);

    driver_pin_register(&driver_pin_E6, GPIOE, GPIO_PIN_6, DEVICE_NAME_PIN_RUN_LED_3);
    driver_pin_register(&driver_pin_E5, GPIOE, GPIO_PIN_5, DEVICE_NAME_PIN_RUN_LED_4);
    driver_pin_register(&driver_pin_E4, GPIOE, GPIO_PIN_4, DEVICE_NAME_PIN_RUN_LED_5);
    driver_pin_register(&driver_pin_E2, GPIOE, GPIO_PIN_2, DEVICE_NAME_PIN_RUN_LED_6);
    driver_pin_register(&driver_pin_G6, GPIOG, GPIO_PIN_6, DEVICE_NAME_PIN_RUN_LED_1);
    driver_pin_register(&driver_pin_G7, GPIOG, GPIO_PIN_7, DEVICE_NAME_PIN_RUN_LED_2);
}
INIT_BOARD_EXPORT(driver_pin_init);
