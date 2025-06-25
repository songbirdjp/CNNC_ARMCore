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

#include "drv_tca9535.h"
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
        /**************tca9535 0x01[0:15] Size: 16 bits, Offset: 0x0***********/
#define DI_DATA_TCA9535_0X01_OFFSET (0x00)
        __IN union
        {
            struct
            {
                uint16_t DI_STAND_BREAKER1 : 1;
                uint16_t DI_STAND_BREAKER2 : 1;
                uint16_t DI_STAND_BREAKER7 : 1;
                uint16_t DI_TREATMENT_ROOM_DOOR_READY : 1;
                uint16_t DI_CONTROL_ROOM_EMERGENCY : 1;
                uint16_t DI_CITB_TREATMENT_ROOM_DOOR2 : 1;
                uint16_t DI_CITB_EMERGENCY4 : 1;
                uint16_t DI_CONTROL_ROOM_BREAKER1 : 1;

                uint16_t DI_UserTreatmentEnable : 1;
                uint16_t DI_UserHvEnable : 1;
                uint16_t DI_UserMoveEnable : 1;
                uint16_t DI_STAND_RESERVE : 1;
                uint16_t DI_STAND_EMERGENCY : 1;
                uint16_t DI_STAND_BREAKER4 : 1;
                uint16_t DI_TouchGuard : 1;
                uint16_t reserve : 1;
            } tca9535_0x01_bit;
            uint16_t tca9535_0x01;
        } tca9535_0x01_u;
        /**************tca9535 0x02[0:15] Size: 16 bits, Offset: 0x2***********/
#define DI_DATA_TCA9535_0X02_OFFSET (0x02)
        __IN union
        {
            struct
            {
                uint16_t DI_STAND_BREAKER3 : 1;
                uint16_t DI_COVER_EMERGENCY4 : 1;
                uint16_t DI_STAND_CONTACTOR1 : 1;
                uint16_t DI_COVER_EMERGENCY1 : 1;
                uint16_t DI_CONTROL_ROOM_BREAKER3 : 1;
                uint16_t DI_UPS_LOAD_PORT : 1;
                uint16_t DI_CITB_TREATMENT_ROOM_DOOR1 : 1;
                uint16_t DI_CITB_EMERGENCY2 : 1;

                uint16_t DI_COVER_DOOR : 1;
                uint16_t DI_HvKey : 1;
                uint16_t DI_CONTROL_ROOM_CONTACTOR1 : 1;
                uint16_t DI_CITB_EMERGENCY3 : 1;
                uint16_t DI_STAND_CONTACTOR2 : 1;
                uint16_t DI_COVER_EMERGENCY3 : 1;
                uint16_t DI_CONTROL_ROOM_BREAKER5 : 1;
                uint16_t DI_UPS_ON_BYPASS : 1;
            } tca9535_0x02_bit;
            uint16_t tca9535_0x02;
        } tca9535_0x02_u;
        /**************tca9535 0x03[0:15] Size: 16 bits, Offset: 0x4***********/
#define DI_DATA_TCA9535_0X03_OFFSET (0x04)
        __IN union
        {
            struct
            {
                uint16_t DI_STAND_BREAKER5 : 1;
                uint16_t DI_COVER_RESERVE : 1;
                uint16_t DI_CONTROL_ROOM_CONTACTOR2 : 1;
                uint16_t DI_CITB_EMERGENCY5 : 1;
                uint16_t DI_CITB_SEARCH_TREATMENT_ROOM : 1;
                uint16_t DI_CITB_EXTERNAL_TERMINATE : 1;
                uint16_t DI_UPS_LOW_BATT : 1;
                uint16_t DI_CONTROL_ROOM_BREAKER2 : 1;

                uint16_t DI_CONTROL_ROOM_BREAKER4 : 1;
                uint16_t DI_UPS_ON_BATT : 1;
                uint16_t DI_CITB_EMERGENCY1 : 1;
                uint16_t DI_COVER_EMERGENCY2 : 1;
                uint16_t DI_STAND_BREAKER6 : 1;
                uint16_t DI_HvEn : 1;
                uint16_t DI_MV_TreatmentEN : 1;
                uint16_t DI_KV_TreatmentEN : 1;
            } tca9535_0x03_bit;
            uint16_t tca9535_0x03;
        } tca9535_0x03_u;
        /**************tca9535 di reserve Size: 16 bits, Offset: 0x6***********/
        __IN uint16_t tca9535_di_reserve;
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
        /*tca9535 0x04[0:7] reserve[8:15] Size: 16 bits, Offset: 0xA***********/
#define DO_DATA_TCA9535_0X04_OFFSET (0x0A)
        __OUT union
        {
            struct
            {
                uint16_t DO_TreatmentRoomLight : 1;
                uint16_t DO_STAND_RESERVE : 1;
                uint16_t DO_Laser : 1;
                uint16_t DO_RadiationIndicator : 1;
                uint16_t DO_ReadyIndicator : 1;
                uint16_t DO_SearchTreatmentRoomRelay : 1;
                uint16_t DO_softwareTouchGuard : 1;

                uint16_t reserve : 9;
            } tca9535_0x04_bit;
            uint16_t tca9535_0x04;
        } tca9535_0x04_u;
        /**************tca9535 do reserve Size: 16 bits, Offset: 0xC***********/
        __OUT uint16_t tca9535_do_reserve;
        /******gpio do[0:6] reserve[7:15] Size: 16 bits, Offset: 0xE***********/
#define DO_DATA_GPIO_OFFSET (0x0E)
        __OUT union
        {
            struct
            {
                /****************************U14:B1-B7[0:7]********************/
                uint16_t DO_SoftwareMVTreatmentEn : 1;
                uint16_t DO_SoftwareKVTreatmentEn : 1;
                uint16_t DO_softwareMoveEN : 1;
                uint16_t DO_SoftwareHvEn : 1;
                uint16_t DO_TreatmentMotionEnable : 1;
                uint16_t DO_ThreePhasePowerOn : 1;
                uint16_t DO_AsuMotionEnable : 1;
                uint16_t reserve0 : 1;
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
        dido_structure_t dido_enable_mask;
        
        device_t *di_tca9535_0x01;
        device_t *di_tca9535_INT1;

        device_t *di_tca9535_0x02;
        device_t *di_tca9535_INT2;

        device_t *di_tca9535_0x03;
        device_t *di_tca9535_INT3;

        device_t *di_gpio_gating;

        device_t *do_tca9535_0x04;

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
