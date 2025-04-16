/**
 * @file dev_pin.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DEV_PIN_H_
#define DEV_PIN_H_

#include "dev_base.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /*****************************ioctl cmd:public*****************************/
    typedef void (*irq_handle_callback_t)(void *);

    typedef enum pin_cmd
    {
        PIN_CMD_MIN = 0,

#define CALLBACK_TIRE_LIMIT (PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_LOW - \
                             PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_HIGH + 1)
        PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_HIGH,
        PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_MIDDLE,
        PIN_CMD_SET_IRQ_HANDLE_RISING_CALLBACK_LOW,

        PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_HIGH,
        PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_MIDDLE,
        PIN_CMD_SET_IRQ_HANDLE_FALLING_CALLBACK_LOW,

        PIN_CMD_MAX
    } pin_cmd_t;
    /******************************msg struct:public***************************/
    typedef enum pin_state
    {
        PIN_STATE_LOW = 0,
        PIN_STATE_HIGH = 1,
        PIN_STATE_NONE
    } pin_state_t;
    typedef pin_state_t pin_msg_t;

    /*****************************pin device struct:private********************/
    typedef struct device_pin
    {
        device_t super;

        pin_state_t pin_state;
        irq_handle_callback_t irq_handle_callback[CALLBACK_TIRE_LIMIT];

        struct device_pin_ops *device_pin_ops;
    } device_pin_t;

    typedef struct device_pin_ops
    {
        device_err_t (*open)(device_pin_t *const self);
        device_err_t (*close)(device_pin_t *const self);
        device_err_t (*read)(device_pin_t *const self,
                             pin_msg_t *const buf,
                             uint32_t timeout);
        device_err_t (*write)(device_pin_t *const self,
                              pin_msg_t const *const buf,
                              uint32_t timeout);
        device_err_t (*ioctl)(device_pin_t *const self,
                              pin_cmd_t cmd,
                              void *const arg);
    } device_pin_ops_t;

    /******************************low level:public****************************/
    void device_pin_register(device_pin_t *const self,
                             char const *name,
                             device_pin_ops_t *const device_pin_ops,
                             void *const user_data);
#ifdef __cplusplus
}
#endif

#endif /* DEV_PIN_H_ */
