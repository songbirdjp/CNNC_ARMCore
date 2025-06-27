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

    typedef struct mcp23017_cmd_int_enable
    {
        uint8_t *gpio_pin;
        void (*callback)(void);
    } mcp23017_cmd_int_enable_t;

    typedef struct mcp23017_cmd_config
    {
        uint16_t io_dir;            // 按位配置IO方向，0为输入，1为输出
        uint16_t input_polarity;    // 按位配置输入极性，0反映输入引脚的相同逻辑状态，1反映输入引脚的相反逻辑状态
        uint16_t int_enable;        // 按位配置中断使能，0为禁止中断，1为使能中断
        uint16_t default_value;     // 按位配置默认值，0为低电平，1为高电平
        uint16_t interrupt_control; // 按位配置中断控制，0引脚值与先前引脚值进行比较，1引脚值与默认值进行比较
        uint8_t io_config;          // 配置IO配置寄存器，默认值为0x00
        uint16_t pull_up_resistors; // 按位配置上拉电阻，0为禁止上拉，1为使能上拉
    } mcp23017_cmd_config_t;

    typedef enum driver_mcp23017_cmd
    {
        DRIVER_MCP23017_CMD_MIN = (I2C_CMD_MAX + 1),

        DRIVER_MCP23017_CMD_INT_ENABLE,      /**< Enable interrupt */
        DRIVER_MCP23017_CMD_CONFIG,          /**< Configure MCP23017 */
        DRIVER_MCP23017_CMD_GET_INT_FLAG,    /**< Get interrupt flag */
        DRIVER_MCP23017_CMD_GET_INT_CAPTURE, /**< Get interrupt capture */
        DRIVER_MCP23017_CMD_MAX
    } driver_mcp23017_cmd_t;

    void driver_mcp23017_init(void);
#ifdef __cplusplus
}
#endif

#endif