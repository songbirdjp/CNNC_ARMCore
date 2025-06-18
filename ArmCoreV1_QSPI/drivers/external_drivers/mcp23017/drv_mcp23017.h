/**
 * @file drv_mcp23017.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-06-05
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __DRV_MCP23017_H__
#define __DRV_MCP23017_H__

#include "dev_i2c.h"
#include "drv_i2c.h"
#include "drv_gpio.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define DEVICE_NAME_MCP23017_0 "MCP23017.0"

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

    typedef struct mcp23017_msg
    {
        driver_mcp23017_reg_t driver_mcp23017_reg;
        uint8_t *data;
        uint16_t dataLen;
    } mcp23017_msg_t;

    typedef struct mcp23017_cmd_int_enable
    {
        uint8_t *gpio_pin;
        void (*callback)(void);
    } mcp23017_cmd_int_enable_t;

    typedef enum driver_mcp23017_cmd
    {
        DRIVER_MCP23017_CMD_MIN = (I2C_CMD_MAX + 1),

        DRIVER_MCP23017_CMD_INT_ENABLE,

        DRIVER_MCP23017_CMD_MAX
    } driver_mcp23017_cmd_t;

    void driver_mcp23017_init(void);
#ifdef __cplusplus
}
#endif

#endif