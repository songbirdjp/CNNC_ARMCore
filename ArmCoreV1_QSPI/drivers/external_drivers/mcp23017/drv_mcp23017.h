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

    typedef struct mcp23017_cmd_config
    {
        uint16_t io_dir;//按位配置IO方向，0为输入，1为输出
        uint16_t input_polarity;//按位配置输入极性，0反映输入引脚的相同逻辑状态，1反映输入引脚的相反逻辑状态
        uint16_t int_enable;//按位配置中断使能，0为禁止中断，1为使能中断
        uint16_t default_value;//按位配置默认值，0为低电平，1为高电平
        uint16_t interrupt_control;//按位配置中断控制，0引脚值与先前引脚值进行比较，1引脚值与默认值进行比较
        uint8_t io_config;//配置IO配置寄存器，默认值为0x00
        uint16_t pull_up_resistors;//按位配置上拉电阻，0为禁止上拉，1为使能上拉
    } mcp23017_cmd_config_t;

    typedef enum driver_mcp23017_cmd
    {
        DRIVER_MCP23017_CMD_MIN = (I2C_CMD_MAX + 1),

        DRIVER_MCP23017_CMD_INT_ENABLE,
        DRIVER_MCP23017_CMD_CONFIG,
        DRIVER_MCP23017_CMD_MAX
    } driver_mcp23017_cmd_t;

    void driver_mcp23017_init(void);
#ifdef __cplusplus
}
#endif

#endif