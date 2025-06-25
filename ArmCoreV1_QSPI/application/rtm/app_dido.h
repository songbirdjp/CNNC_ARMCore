/**
 * @file dido_app.h
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2024-08-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef _APP_DIDO_H_
#define _APP_DIDO_H_

#include "drv_mcp23017.h"
#include "app_manage.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define DIDO_DIDO_INIT_BIT (0)
#define DIDO_STATE_BIT (1)

#define __IN     /*!< Defines 'read only' permissions               */
#define __OUT    /*!< Defines 'write only' permissions              */
#define __IN_OUT /*!< Defines 'read / write' permissions            */

    typedef struct dido_structure
    {
#define DI_DATA_OFFSET_START (0x00)
        /*MCP23017  Size: 8 bits, Offset: 0x0***********/
#define DI_DATA_MCP23017_0X00_OFFSET (0x00)
        __IN union
        {
            struct
            {
                uint8_t RTC_WD_OK_IN : 1;
                uint8_t reserve : 1;
                uint8_t DI_BSM_NOT_READY : 1;
                uint8_t DI_MV_TreatmentEN : 1;
                uint8_t DI_HVEN : 1;
                uint8_t DI_Pulse_Inhibit : 1;
                uint8_t DI_KV_TreatmentEN : 1;
                uint8_t DI_Power_cut_FB : 1;
            } mcp23017_0x00_bit;
            uint8_t mcp23017_0x00;
        } mcp23017_0x00_u;
        /********gpio di Size: 8 bits, Offset: 0x1***********/
#define DI_DATA_GPIO_OFFSET (0x01)
        __IN union
        {
            struct
            {
                uint8_t DI_GATING_IN : 1;
                uint8_t DI_Slipring_HVEN_IN : 1;
                uint8_t DI_Slipring_KV_TreatmentEN_IN : 1;
                uint8_t DI_Slipring_MV_TreatmentEN_IN : 1;
                uint8_t reserve : 4;
            } gpio_di_bit;
            uint8_t gpio_di;
        } gpio_di_u;
#define DI_DATA_OFFSET_END (0x02)
#define DO_DATA_OFFSET_START (0x02)
        /******gpio do Size: 16 bits, Offset: 0x2***********/
#define DO_DATA_GPIO_OFFSET (0x02)
        __OUT union
        {
            struct
            {
                uint16_t reserve0 : 1;
                uint16_t DO_MV_TreatmentEN : 1;
                uint16_t DO_KV_TreatmentEN : 1;
                uint16_t DO_Emergency : 1;
                uint16_t DO_Pulse_Inhibit : 1;
                uint16_t DO_HVEN : 1;
                uint16_t DO_PowerCut : 1;

                uint16_t reserve1 : 1;
                uint16_t RUN_LED1 : 1;
                uint16_t RUN_LED2 : 1;
                uint16_t RUN_LED3 : 1;
                uint16_t RUN_LED4 : 1;
                uint16_t RUN_LED5 : 1;
                uint16_t RUN_LED6 : 1;
                uint16_t reserve2 : 1;
            } gpio_do_bit;
            uint16_t gpio_do;
        } gpio_do_u;
#define DO_DATA_OFFSET_END (0x4)
    } dido_structure_t __attribute__((aligned(1)));

    typedef int32_t (*app_dido_callback_t)(dido_structure_t);

    typedef struct app_dido
    {
        manage_info_t manage_info;

        osThreadId_t do_thread_id;
        osMutexId_t mutex;
        dido_structure_t dido_structure_temp;

        dido_structure_t dido_structure;
        dido_structure_t dido_enable_mask;

        device_t *di_mcp23017_0x00;
        device_t *di_mcp23017_INT0;

        device_t *di_gpio_gating;
        device_t *di_gpio_slipring_HVEN;
        device_t *di_gpio_slipring_KV_TreatmentEN;
        device_t *di_gpio_slipring_MV_TreatmentEN;

        device_t *do_gpio_MV_TreatmentEN;
        device_t *do_gpio_KV_TreatmentEN;
        device_t *do_gpio_Emergency;
        device_t *do_gpio_Pulse_Inhibit;
        device_t *do_gpio_HVEN;
        device_t *do_gpio_power_cut;

        device_t *do_gpio_RUN_LED1;
        device_t *do_gpio_RUN_LED2;
        device_t *do_gpio_RUN_LED3;
        device_t *do_gpio_RUN_LED4;
        device_t *do_gpio_RUN_LED5;
        device_t *do_gpio_RUN_LED6;

    } app_dido_t;

    int32_t app_dido_create(app_dido_t *self);
    void app_di_get(app_dido_t *self, dido_structure_t *dido_value);
    void app_do_set(app_dido_t *self, dido_structure_t *dido_value);
    void app_do_get(app_dido_t *self, dido_structure_t *dido_value);
#ifdef __cplusplus
}
#endif

#endif /* _APP_DIDO_H_ */
