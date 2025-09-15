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
        /**************mcp23017 0x00[0:15] Size: 16 bits, Offset: 0x0***********/
#define DI_DATA_MCP23017_0X00_OFFSET (0x00)
        __IN union
        {
            struct
            {
                uint16_t reserve : 8;

                uint16_t DI_CITB_EMERGENCY2 : 1;
                uint16_t DI_UPS_LOAD_PORT : 1;
                uint16_t DI_STAND_BREAKER1 : 1;
                uint16_t DI_STAND_EMERGENCY : 1;
                uint16_t DI_TouchGuard : 1;
                uint16_t DI_HvEn : 1;
                uint16_t DI_MV_TreatmentEN : 1;
                uint16_t DI_KV_TreatmentEN : 1;
            } mcp23017_0x00_bit;
            uint16_t mcp23017_0x00;
        } mcp23017_0x00_u;
        /**************mcp23017 0x01[0:15] Size: 16 bits, Offset: 0x2***********/
#define DI_DATA_MCP23017_0X01_OFFSET (0x02)
        __IN union
        {
            struct
            {
                uint16_t DI_STAND_BREAKER7 : 1;
                uint16_t DI_STAND_BREAKER3 : 1;
                uint16_t DI_UPS_ON_BYPASS : 1;
                uint16_t DI_STAND_BREAKER5 : 1;
                uint16_t DI_CITB_EMERGENCY3 : 1;
                uint16_t DI_COVER3 : 1;
                uint16_t DI_POWER_CUT : 1;
                uint16_t DI_STAND_BREAKER4 : 1;

                uint16_t DI_CITB_TREATMENT_ROOM_DOOR1 : 1;
                uint16_t DI_CITB_TREATMENT_ROOM_DOOR2 : 1;
                uint16_t DI_CITB_EMERGENCY1 : 1;
                uint16_t DI_CITB_SEARCH_TREATMENT_ROOM : 1;
                uint16_t DI_CONTROL_ROOM_BREAKER1 : 1;
                uint16_t DI_UPS_ON_BATT : 1;
                uint16_t DI_CONTROL_ROOM_BREAKER5 : 1;
                uint16_t DI_STAND_BREAKER6 : 1;
            } mcp23017_0x01_bit;
            uint16_t mcp23017_0x01;
        } mcp23017_0x01_u;
        /**************mcp23017 0x02[0:15] Size: 16 bits, Offset: 0x4***********/
#define DI_DATA_MCP23017_0X02_OFFSET (0x04)
        __IN union
        {
            struct
            {
                uint16_t DI_CITB_EMERGENCY5 : 1;
                uint16_t DI_COVER1 : 1;
                uint16_t DI_STAND_RESERVE : 1;
                uint16_t DI_TREATMENT_ROOM_DOOR_READY : 1;
                uint16_t DI_COVER2 : 1;
                uint16_t DI_CONTROL_ROOM_BREAKER2 : 1;
                uint16_t DI_CONTROL_ROOM_BREAKER4 : 1;
                uint16_t DI_STAND_CONTACTOR2 : 1;

                uint16_t DI_STAND_CONTACTOR1 : 1;
                uint16_t DI_UPS_LOW_BATT : 1;
                uint16_t DI_HvKey : 1;
                uint16_t DI_COVER4 : 1;
                uint16_t DI_CITB_EMERGENCY4 : 1;
                uint16_t DI_CONTROL_ROOM_BREAKER3 : 1;
                uint16_t DI_STAND_BREAKER2 : 1;
                uint16_t RTC_WD_OK_IN : 1;
            } mcp23017_0x02_bit;
            uint16_t mcp23017_0x02;
        } mcp23017_0x02_u;
        /**************mcp23017 di reserve Size: 16 bits, Offset: 0x6***********/
        __IN uint16_t mcp23017_di_reserve;
        /********gpio di[0] reserve[1:15] Size: 16 bits, Offset: 0x8***********/
#define DI_DATA_GPIO_OFFSET (0x08)
        __IN union
        {
            struct
            {
                /****************************U87:A4[0]*************************/
                uint16_t DI_GATING : 1;
                /****************************reserve[1:15]*********************/
                uint16_t reserve : 15;
            } gpio_di_bit;
            uint16_t gpio_di;
        } gpio_di_u;
#define DI_DATA_OFFSET_END (0x0A)
#define DO_DATA_OFFSET_START (0x0A)
        /*MCP23017 0x03[0:7] reserve[8:15] Size: 16 bits, Offset: 0xA***********/
#define DO_DATA_MCP23017_0X03_OFFSET (0x0A)
        __OUT union
        {
            struct
            {
                uint16_t DO_RTM_AutoPowerUp : 1;
                uint16_t DO_SearchTreatmentRoomRelay : 1;
                uint16_t DO_Laser : 1;
                uint16_t DO_Power_CUT : 1;
                uint16_t DO_STAND_RESERVE : 1;
                uint16_t DO_TreatmentRoomLight : 1;
                uint16_t DO_RadiationIndicator : 1; 
                uint16_t DO_ReadyIndicator : 1;

                uint16_t DO_RTM_SystemShutDown : 1;
                uint16_t reserve : 7;
            } mcp23017_0x03_bit;
            uint16_t mcp23017_0x03;
        } mcp23017_0x03_u;
        /**************mcp23017 do reserve Size: 16 bits, Offset: 0xC***********/
        __OUT uint16_t mcp23017_do_reserve;
        /******gpio do[0:6] reserve[7:15] Size: 16 bits, Offset: 0xE***********/
#define DO_DATA_GPIO_OFFSET (0x0E)
        __OUT union
        {
            struct
            {
                /****************************U14:B1-B7[0:7]********************/
                uint16_t DO_SoftwareHvEn : 1;
                uint16_t DO_SoftwareKVTreatmentEn : 1;
                uint16_t DO_SoftwareMVTreatmentEn : 1;
                uint16_t DO_ThreePhasePowerOn : 1;
                uint16_t DO_softwareMoveEN : 1;
                uint16_t DO_TreatmentMotionEnable : 1;
                uint16_t DO_AsuMotionEnable : 1;
                uint16_t reserve : 1;
                /****************************U84:B0-B7[0:7]********************/
                uint16_t RUN_LED1 : 1;
                uint16_t RUN_LED2 : 1;
                uint16_t RUN_LED3 : 1;
                uint16_t RUN_LED4 : 1;
                uint16_t RUN_LED5 : 1;
                uint16_t RUN_LED6 : 1;
                uint16_t reserve1 : 2;
            } gpio_do_bit;
            uint16_t gpio_do;
        } gpio_do_u;
#define DO_DATA_OFFSET_END (0x10)
    } dido_structure_t __attribute__((aligned(1)));
    typedef int32_t (*app_dido_callback_t)(dido_structure_t);

    typedef struct app_dido
    {
        manage_info_t manage_info;

        osThreadId_t do_thread_id;
        osMutexId_t mutex;
        dido_structure_t dido_structure_temp;

        dido_structure_t dido_structure;

        device_t *di_mcp23017_0x00;
        device_t *di_mcp23017_INT0;

        device_t *di_mcp23017_0x01;
        device_t *di_mcp23017_INT1;

        device_t *di_mcp23017_0x02;
        device_t *di_mcp23017_INT2;

        device_t *di_gpio_gating;

        device_t *do_mcp23017_0x03;

        device_t *do_gpio_SoftwareMVTreatmentEn;
        device_t *do_gpio_SoftwareKVTreatmentEn;
        device_t *do_gpio_softwareMoveEN;
        device_t *do_gpio_SoftwareHvEn;
        device_t *do_gpio_TreatmentMotionEnable;
        device_t *do_gpio_ThreePhasePowerOn;
        device_t *do_gpio_AsuMotionEnable;

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
