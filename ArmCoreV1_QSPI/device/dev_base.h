/**
 * @file dev_base.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DEV_BASE_H_
#define DEV_BASE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief
 *
 */
#define TAG(tag) static const char *TAG = tag

#define dev_assert(test_)                                                      \
    do                                                                         \
    {                                                                          \
        if (!(test_))                                                          \
        {                                                                      \
            printf("[%s] %s:%d assert failure!\r\n", TAG, __FILE__, __LINE__); \
            printf("assert info:%s.\r\n", #test_);                             \
            while (1)                                                          \
                ;                                                              \
        }                                                                      \
    } while (0)
// #define dev_assert(test_)       ((void)0U)
#define container_of(ptr, type, member) ({             \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offset_of(type, member)); })

#define offset_of(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
    typedef enum device_err
    {
        DEV_EOK = 0,        /**< No error */
        DEV_ENOMEM = -7,    /**< No enough memory */
        DEV_EIO = -8,       /**< I/O error */
        DEV_ENOTFOUND = -9, /**< Not found */
        DEV_EBUSY = -10,     /**< Resource busy */
        DEV_EEXIST = -11,    /**< Exists */
        DEV_ENOTSUP = -12,   /**< Operation not supported */
        DEV_EINVAL = -13,    /**< Invalid argument */
        DEV_ETIMEOUT = -14,  /**< Timeout */
        DEV_EREPTD = -15,    /**< Repeated*/
        DEV_ENOTOPEN = -16, /**< Not OPEN */
    } device_err_t;

    typedef enum device_type
    {
        DEVICE_NULL = 0,

        DEVICE_PIN,
        DEVICE_PWM,
        DEVICE_ADC,
        DEVICE_DAC,
        DEVICE_USART,
        DEVICE_I2C_BUS,
        DEVICE_I2C_MEM,
        DEVICE_I2C,
        DEVICE_SPI_BUS,
        DEVICE_SPI,
        DEVICE_CAN,
        DEVICE_WATCHDOG,
        DEVICE_RTC,

        DEVICE_DATA_DISTRIBUTE,
        DEVICE_UNKNOWN,

        DEVICE_NORMAL_MAX,
    } device_type_t;

    typedef enum device_sole
    {
        DEVICE_UNIQUENESS = 0,
        DEVICE_NON_UNIQUENESS,
    } device_sole_t;
    typedef enum device_duplex
    {
        DEVICE_FULL_DUPLEX = 0,
        DEVICE_HALF_DUPLEX,
    } device_duplex_t;

    typedef struct device_attr
    {
        const char *name;
        device_sole_t device_sole;
        device_type_t device_type;
        device_duplex_t device_duplex;
    } device_attr_t;

    typedef struct device
    {
        struct device_base *next;

        device_attr_t device_attr;

        osMutexId_t mutex_rx;
        osMutexId_t mutex_tx;
        bool open_state;
        uint16_t open_count;

        struct device_ops *ops;
        void *user_data;
    } device_t;

    typedef struct device_ops
    {
        device_err_t (*open)(device_t *const self);
        device_err_t (*close)(device_t *const self);
        device_err_t (*read)(device_t *const self, void *const buffer, uint32_t size, uint32_t timeout);
        device_err_t (*write)(device_t *const self, void const *const buffer, uint32_t size, uint32_t timeout);
        device_err_t (*ioctl)(device_t *const self, uint8_t cmd, void *const arg);
    } device_ops_t;
    /****************************************************low level********************************************************************* */
    /**
     * @brief
     *
     * @param self
     * @param attr
     */
    void device_register(device_t *const self, device_attr_t *const attr, device_ops_t *const device_ops, void *const user_data);
    /****************************************************high level********************************************************************* */
    /**
     * @brief
     *
     * @param name
     * @return device_t*
     */
    device_t *device_find(char const *name);

    device_err_t device_open(device_t *const self);
    device_err_t device_close(device_t *const self);
    device_err_t device_read(device_t *const self, void *const buffer, uint32_t size, uint32_t timeout);
    device_err_t device_write(device_t *const self, void const *const buffer, uint32_t size, uint32_t timeout);
    device_err_t device_ioctl(device_t *const self, uint8_t cmd, void *const arg);

    void device_test(void);
#ifdef __cplusplus
}
#endif

#endif /* DEV_BASE_H_ */
