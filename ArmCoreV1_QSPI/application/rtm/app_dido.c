#include "app_dido.h"
#include "drv_pin.h"
#include "drv_i2c.h"
#include "init_call.h"
#include "shell.h"
#include "ulog.h"
#include "app_manage.h"
enum
{
    DIDO_STATE_STABLE = 0,
    DIDO_STATE_UNSTABLE,
} dido_state;
#define APP_RTM_THREAD_FLAG_DO_UPDATE (1 << 0)
static int32_t do_device_init(app_dido_t *self)
{
    device_err_t device_err = DEV_EIO;
    uint16_t pca9557_cfg;
    uint8_t output_value = 0x00;
    /**********************************DEVICE_NAME_MCP23017_3*******************/
    self->do_mcp23017_0x03 = device_find(DEVICE_NAME_MCP23017_3);
    if (self->do_mcp23017_0x03 == NULL)
    {
        return -1;
    }
    device_err = device_open(self->do_mcp23017_0x03);
    if (device_err != DEV_EOK)
    {
        return -2;
    }
    mcp23017_cmd_config_t mcp23017_3_cmd_config = {
        .io_dir = 0x0000,
        .input_polarity = 0x0000,
        .int_enable = 0x0000,
        .default_value = 0x0000,
        .interrupt_control = 0x0000,
        .io_config = 0x44,
        .pull_up_resistors = 0x0000,
    };
    device_err = device_ioctl(self->do_mcp23017_0x03,
                              DRIVER_MCP23017_CMD_CONFIG,
                              &mcp23017_3_cmd_config);
    if (device_err != DEV_EOK)
    {
        return -3;
    }
    /********************DEVICE_NAME_PIN_DO_SOFTWARE_MV_TREATMENT_EN***********/
    self->do_gpio_SoftwareMVTreatmentEn = device_find(DEVICE_NAME_PIN_DO_SOFTWARE_MV_TREATMENT_EN);
    if (self->do_gpio_SoftwareMVTreatmentEn == NULL)
    {
        return -4;
    }
    device_err = device_open(self->do_gpio_SoftwareMVTreatmentEn);
    if (device_err != DEV_EOK)
    {
        return -5;
    }
    /*******************DEVICE_NAME_PIN_DO_SOFTWARE_KV_TREATMENT_EN************/
    self->do_gpio_SoftwareKVTreatmentEn = device_find(DEVICE_NAME_PIN_DO_SOFTWARE_KV_TREATMENT_EN);
    if (self->do_gpio_SoftwareKVTreatmentEn == NULL)
    {
        return -6;
    }
    device_err = device_open(self->do_gpio_SoftwareKVTreatmentEn);
    if (device_err != DEV_EOK)
    {
        return -7;
    }
    /************************DEVICE_NAME_PIN_DO_SOFTWARE_MOVE_EN***************/
    self->do_gpio_softwareMoveEN = device_find(DEVICE_NAME_PIN_DO_SOFTWARE_MOVE_EN);
    if (self->do_gpio_softwareMoveEN == NULL)
    {
        return -8;
    }
    device_err = device_open(self->do_gpio_softwareMoveEN);
    if (device_err != DEV_EOK)
    {
        return -9;
    }
    /********************DEVICE_NAME_PIN_DO_SOFTWARE_HV_EN*********************/
    self->do_gpio_SoftwareHvEn = device_find(DEVICE_NAME_PIN_DO_SOFTWARE_HV_EN);
    if (self->do_gpio_SoftwareHvEn == NULL)
    {
        return -10;
    }
    device_err = device_open(self->do_gpio_SoftwareHvEn);
    if (device_err != DEV_EOK)
    {
        return -11;
    }
    /********************DEVICE_NAME_PIN_DO_TREATMENT_MOTION_ENABLE************/
    self->do_gpio_TreatmentMotionEnable = device_find(DEVICE_NAME_PIN_DO_TREATMENT_MOTION_ENABLE);
    if (self->do_gpio_TreatmentMotionEnable == NULL)
    {
        return -12;
    }
    device_err = device_open(self->do_gpio_TreatmentMotionEnable);
    if (device_err != DEV_EOK)
    {
        return -13;
    }
    /********************DEVICE_NAME_PIN_DO_THREE_PHASE_POWER_ON***************/
    self->do_gpio_ThreePhasePowerOn = device_find(DEVICE_NAME_PIN_DO_THREE_PHASE_POWER_ON);
    if (self->do_gpio_ThreePhasePowerOn == NULL)
    {
        return -14;
    }
    device_err = device_open(self->do_gpio_ThreePhasePowerOn);
    if (device_err != DEV_EOK)
    {
        return -15;
    }
    /********************DEVICE_NAME_PIN_DO_ASU_MOTION_ENABLE******************/
    self->do_gpio_AsuMotionEnable = device_find(DEVICE_NAME_PIN_DO_ASU_MOTION_ENABLE);
    if (self->do_gpio_AsuMotionEnable == NULL)
    {
        return -16;
    }
    device_err = device_open(self->do_gpio_AsuMotionEnable);
    if (device_err != DEV_EOK)
    {
        return -17;
    }
    /********************DEVICE_NAME_PIN_RUN_LED_1******************/
    self->do_gpio_RUN_LED1 = device_find(DEVICE_NAME_PIN_RUN_LED_1);
    if (self->do_gpio_RUN_LED1 == NULL)
    {
        return -18;
    }
    device_err = device_open(self->do_gpio_RUN_LED1);
    if (device_err != DEV_EOK)
    {
        return -19;
    }

    /********************DEVICE_NAME_PIN_RUN_LED_2******************/
    self->do_gpio_RUN_LED2 = device_find(DEVICE_NAME_PIN_RUN_LED_2);
    if (self->do_gpio_RUN_LED2 == NULL)
    {
        return -20;
    }
    device_err = device_open(self->do_gpio_RUN_LED2);
    if (device_err != DEV_EOK)
    {
        return -21;
    }
    /********************DEVICE_NAME_PIN_RUN_LED_3******************/
    self->do_gpio_RUN_LED3 = device_find(DEVICE_NAME_PIN_RUN_LED_3);
    if (self->do_gpio_RUN_LED3 == NULL)
    {
        return -22;
    }
    device_err = device_open(self->do_gpio_RUN_LED3);
    if (device_err != DEV_EOK)
    {
        return -23;
    }
    /********************DEVICE_NAME_PIN_RUN_LED_4******************/
    self->do_gpio_RUN_LED4 = device_find(DEVICE_NAME_PIN_RUN_LED_4);
    if (self->do_gpio_RUN_LED4 == NULL)
    {
        return -24;
    }
    device_err = device_open(self->do_gpio_RUN_LED4);
    if (device_err != DEV_EOK)
    {
        return -25;
    }
    /********************DEVICE_NAME_PIN_RUN_LED_5******************/
    self->do_gpio_RUN_LED5 = device_find(DEVICE_NAME_PIN_RUN_LED_5);
    if (self->do_gpio_RUN_LED5 == NULL)
    {
        return -26;
    }
    device_err = device_open(self->do_gpio_RUN_LED5);
    if (device_err != DEV_EOK)
    {
        return -27;
    }
    /********************DEVICE_NAME_PIN_RUN_LED_6******************/
    self->do_gpio_RUN_LED6 = device_find(DEVICE_NAME_PIN_RUN_LED_6);
    if (self->do_gpio_RUN_LED6 == NULL)
    {
        return -28;
    }
    device_err = device_open(self->do_gpio_RUN_LED6);
    if (device_err != DEV_EOK)
    {
        return -29;
    }
}
static int32_t di_device_init(app_dido_t *self)
{
    device_err_t device_err = DEV_EIO;

    /**********************************DEVICE_NAME_MCP23017_0*******************/
    self->di_mcp23017_0x00 = device_find(DEVICE_NAME_MCP23017_0);
    if (self->di_mcp23017_0x00 == NULL)
    {
        return -1;
    }
    device_err = device_open(self->di_mcp23017_0x00);
    if (device_err != DEV_EOK)
    {
        return -2;
    }
    mcp23017_cmd_config_t mcp23017_0_cmd_config = {
        .io_dir = 0xFFFF,
        .input_polarity = 0xFF00,
        .int_enable = 0x0000,
        .default_value = 0x0000,
        .interrupt_control = 0x0000,
        .io_config = 0x44,
        .pull_up_resistors = 0x0000,
    };
    device_err = device_ioctl(self->di_mcp23017_0x00,
                              DRIVER_MCP23017_CMD_CONFIG,
                              &mcp23017_0_cmd_config);
    if (device_err != DEV_EOK)
    {
        return -3;
    }
    /**********************************DEVICE_NAME_MCP23017_1*******************/
    self->di_mcp23017_0x01 = device_find(DEVICE_NAME_MCP23017_1);
    if (self->di_mcp23017_0x01 == NULL)
    {
        return -4;
    }
    device_err = device_open(self->di_mcp23017_0x01);
    if (device_err != DEV_EOK)
    {
        return -5;
    }
    mcp23017_cmd_config_t mcp23017_1_cmd_config = {
        .io_dir = 0xFFFF,
        .input_polarity = 0xFFFF,
        .int_enable = 0x0000,
        .default_value = 0x0000,
        .interrupt_control = 0x0000,
        .io_config = 0x44,
        .pull_up_resistors = 0x0000,
    };
    device_err = device_ioctl(self->di_mcp23017_0x01,
                              DRIVER_MCP23017_CMD_CONFIG,
                              &mcp23017_1_cmd_config);
    if (device_err != DEV_EOK)
    {
        return -6;
    }
    /**********************************DEVICE_NAME_MCP23017_2*******************/
    self->di_mcp23017_0x02 = device_find(DEVICE_NAME_MCP23017_2);
    if (self->di_mcp23017_0x02 == NULL)
    {
        return -7;
    }
    device_err = device_open(self->di_mcp23017_0x02);
    if (device_err != DEV_EOK)
    {
        return -8;
    }
    mcp23017_cmd_config_t mcp23017_2_cmd_config = {
        .io_dir = 0xFFFF,
        .input_polarity = 0x7FFF,
        .int_enable = 0x0000,
        .default_value = 0x0000,
        .interrupt_control = 0x0000,
        .io_config = 0x44,
        .pull_up_resistors = 0x0000,
    };
    device_err = device_ioctl(self->di_mcp23017_0x02,
                              DRIVER_MCP23017_CMD_CONFIG,
                              &mcp23017_2_cmd_config);
    if (device_err != DEV_EOK)
    {
        return -9;
    }
    /******************************DEVICE_NAME_PIN_DI_GATING*******************/
    self->di_gpio_gating = device_find(DEVICE_NAME_PIN_DI_GATING);
    if (self->di_gpio_gating == NULL)
    {
        return -10;
    }
    device_err = device_open(self->di_gpio_gating);
    if (device_err != DEV_EOK)
    {
        return -11;
    }
}
/**
 * @brief
 *
 * @param argument
 */
static void app_di_poll_entry(void *argument)
{
    app_dido_t *self = (app_dido_t *)argument;
    device_err_t device_err = DEV_EIO;
    osStatus_t osStatus = osError;

    dido_structure_t dido_value_cur;
    dido_structure_t dido_value_last;
    dido_structure_t dido_state; /**>0 确定状态，1 不确定状态 */
    memset(&dido_value_cur, 0, sizeof(dido_structure_t));
    memset(&dido_value_last, 0, sizeof(dido_structure_t));
    memset(&dido_state, 0, sizeof(dido_structure_t));

    mcp23017_msg_t mcp23017_msg = {
        .gpio_port = DRIVER_MCP23017_GPA,
        .dataLen = 2};
    pin_msg_t pin_msg = PIN_STATE_NONE;

    int32_t retVal = di_device_init(self);
    if (retVal != 0)
    {
        manage_info_status_word_set(&self->manage_info, DIDO_PERIPHERAL_INIT_BIT, 1);
        LOG_I("di device init fail, errorCode:%d.\r\n", retVal);
        goto exit;
    }
    app_rtm_thread_flag_set(APP_RTM_THREAD_FLAG_DI_READY);
    for (;;)
    {
        mcp23017_msg.data = (uint8_t *)(&(dido_value_cur.mcp23017_0x00_u.mcp23017_0x00));
        device_err = device_read(self->di_mcp23017_0x00,
                                 &mcp23017_msg,
                                 0,
                                 1000);
        if (device_err != DEV_EOK)
        {
            manage_info_status_word_set(&self->manage_info, DIDO_LINK_STATE_BIT, 1);
            LOG_I("read mcp23017_0 fail, errorCode:%d\r\n", device_err);
            device_ioctl(self->di_mcp23017_0x00,
                         I2C_CMD_INIT,
                         NULL);
        }
        else
        {
            manage_info_status_word_set(&self->manage_info, DIDO_LINK_STATE_BIT, 0);
        }

        mcp23017_msg.data = (uint8_t *)(&(dido_value_cur.mcp23017_0x01_u.mcp23017_0x01));
        device_err = device_read(self->di_mcp23017_0x01,
                                 &mcp23017_msg,
                                 0,
                                 1000);
        if (device_err != DEV_EOK)
        {
            manage_info_status_word_set(&self->manage_info, DIDO_LINK_STATE_BIT, 1);
            LOG_I("read mcp23017_1 fail, errorCode:%d\r\n", device_err);
            device_ioctl(self->di_mcp23017_0x01,
                         I2C_CMD_INIT,
                         NULL);
        }
        else
        {
            manage_info_status_word_set(&self->manage_info, DIDO_LINK_STATE_BIT, 0);
        }

        mcp23017_msg.data = (uint8_t *)(&(dido_value_cur.mcp23017_0x02_u.mcp23017_0x02));
        device_err = device_read(self->di_mcp23017_0x02,
                                 &mcp23017_msg,
                                 0,
                                 1000);
        if (device_err != DEV_EOK)
        {
            manage_info_status_word_set(&self->manage_info, DIDO_LINK_STATE_BIT, 1);
            LOG_I("read mcp23017_2 fail, errorCode:%d\r\n", device_err);
            device_ioctl(self->di_mcp23017_0x02,
                         I2C_CMD_INIT,
                         NULL);
        }
        else
        {
            manage_info_status_word_set(&self->manage_info, DIDO_LINK_STATE_BIT, 0);
        }
        /*读取gpio di*/
        device_err = device_read(self->di_gpio_gating,
                                 &pin_msg,
                                 0,
                                 1000);
        if (device_err != DEV_EOK)
        {
            LOG_I("read gating fail!\r\n");
        }

        dido_value_cur.gpio_di_u.gpio_di_bit.DI_GATING = pin_msg;

        /*判断当前9535di状态，异或，与上次不同则为不确定状态1*/
        dido_state.mcp23017_0x00_u.mcp23017_0x00 = ((dido_value_cur.mcp23017_0x00_u.mcp23017_0x00) ^
                                                  (dido_value_last.mcp23017_0x00_u.mcp23017_0x00));
        dido_value_last.mcp23017_0x00_u.mcp23017_0x00 =
            dido_value_cur.mcp23017_0x00_u.mcp23017_0x00;

        dido_state.mcp23017_0x01_u.mcp23017_0x01 = ((dido_value_cur.mcp23017_0x01_u.mcp23017_0x01) ^
                                                  (dido_value_last.mcp23017_0x01_u.mcp23017_0x01));
        dido_value_last.mcp23017_0x01_u.mcp23017_0x01 =
            dido_value_cur.mcp23017_0x01_u.mcp23017_0x01;

        dido_state.mcp23017_0x02_u.mcp23017_0x02 = ((dido_value_cur.mcp23017_0x02_u.mcp23017_0x02) ^
                                                  (dido_value_last.mcp23017_0x02_u.mcp23017_0x02));
        dido_value_last.mcp23017_0x02_u.mcp23017_0x02 =
            dido_value_cur.mcp23017_0x02_u.mcp23017_0x02;
        /*判断当前gpio di状态，异或，与上次不同则为不确定状态1*/
        dido_state.gpio_di_u.gpio_di = ((dido_value_cur.gpio_di_u.gpio_di) ^
                                        (dido_value_last.gpio_di_u.gpio_di));
        dido_value_last.gpio_di_u.gpio_di = dido_value_cur.gpio_di_u.gpio_di;

        /*di 改变*/
        if ((0 != memcmp((uint8_t *)(&(self->dido_structure)) + DI_DATA_OFFSET_START,
                         (uint8_t *)(&dido_value_cur) + DI_DATA_OFFSET_START,
                         DI_DATA_OFFSET_END - DI_DATA_OFFSET_START)))
        {
            self->dido_structure.mcp23017_0x00_u.mcp23017_0x00 = (self->dido_structure.mcp23017_0x00_u.mcp23017_0x00 & dido_state.mcp23017_0x00_u.mcp23017_0x00) |
                                                               (dido_value_cur.mcp23017_0x00_u.mcp23017_0x00 & ~dido_state.mcp23017_0x00_u.mcp23017_0x00);
            self->dido_structure.mcp23017_0x01_u.mcp23017_0x01 = (self->dido_structure.mcp23017_0x01_u.mcp23017_0x01 & dido_state.mcp23017_0x01_u.mcp23017_0x01) |
                                                               (dido_value_cur.mcp23017_0x01_u.mcp23017_0x01 & ~dido_state.mcp23017_0x01_u.mcp23017_0x01);
            self->dido_structure.mcp23017_0x02_u.mcp23017_0x02 = (self->dido_structure.mcp23017_0x02_u.mcp23017_0x02 & dido_state.mcp23017_0x02_u.mcp23017_0x02) |
                                                               (dido_value_cur.mcp23017_0x02_u.mcp23017_0x02 & ~dido_state.mcp23017_0x02_u.mcp23017_0x02);
            self->dido_structure.gpio_di_u.gpio_di = (self->dido_structure.gpio_di_u.gpio_di & dido_state.gpio_di_u.gpio_di) |
                                                     (dido_value_cur.gpio_di_u.gpio_di & ~dido_state.gpio_di_u.gpio_di);

            osMutexAcquire(self->mutex, osWaitForever);
            memcpy(&self->dido_structure_temp, &self->dido_structure, DI_DATA_OFFSET_END - DI_DATA_OFFSET_START);
            osMutexRelease(self->mutex);
        }
        osDelay(10);
    }
exit:
    osThreadExit();
}
/**
 * @brief
 *
 * @param argument
 */
static void app_do_entry(void *argument)
{
    app_dido_t *self = (app_dido_t *)argument;
    device_err_t device_err = DEV_EIO;
    dido_structure_t dido_value;
    memset(&dido_value, 0, sizeof(dido_structure_t));

    mcp23017_msg_t mcp23017_msg = {
        .gpio_port = DRIVER_MCP23017_GPA,
        .dataLen = 2};
    pin_msg_t gpio_msg = PIN_STATE_NONE;
    uint32_t ret = 0;

    int32_t retVal = do_device_init(self);
    if (retVal != 0)
    {
        LOG_I("do device init fail, errorCode:%d.\r\n", retVal);
        goto exit;
    }
    app_rtm_thread_flag_set(APP_RTM_THREAD_FLAG_DO_READY);
    for (;;)
    {
        ret = osThreadFlagsWait(APP_RTM_THREAD_FLAG_DO_UPDATE, osFlagsWaitAll, 0xFFFFFFFF);
        if (ret < 0)
        {
            continue;
        }
        osMutexAcquire(self->mutex, osWaitForever);
        memcpy((uint8_t *)(&dido_value) + DO_DATA_OFFSET_START, (uint8_t *)(&self->dido_structure_temp) + DO_DATA_OFFSET_START, DO_DATA_OFFSET_END - DO_DATA_OFFSET_START);
        osMutexRelease(self->mutex);

        if (0 != memcmp((uint8_t *)(&(self->dido_structure)) + DO_DATA_OFFSET_START,
                        (uint8_t *)(&(dido_value)) + DO_DATA_OFFSET_START,
                        DO_DATA_OFFSET_END - DO_DATA_OFFSET_START))
        {
            /*changed DO mcp23017_0x03*/
            if (self->dido_structure.mcp23017_0x03_u.mcp23017_0x03 !=
                dido_value.mcp23017_0x03_u.mcp23017_0x03)
            {
                self->dido_structure.mcp23017_0x03_u.mcp23017_0x03 =
                    dido_value.mcp23017_0x03_u.mcp23017_0x03;
                mcp23017_msg.data = (uint8_t *)&(self->dido_structure.mcp23017_0x03_u.mcp23017_0x03);
                device_err = device_write(self->do_mcp23017_0x03, &mcp23017_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write mcp23017_0x03 fail!\r\n");
                }
            }
            /*changed DO SoftwareMVTreatmentEn*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn !=
                dido_value.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn =
                    dido_value.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareMVTreatmentEn);
                device_err = device_write(self->do_gpio_SoftwareMVTreatmentEn, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write SoftwareMVTreatmentEn fail!\r\n");
                }
            }
            /*changed DO SoftwareKVTreatmentEn*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn !=
                dido_value.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn =
                    dido_value.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareKVTreatmentEn);
                device_err = device_write(self->do_gpio_SoftwareKVTreatmentEn, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write SoftwareKVTreatmentEn fail!\r\n");
                }
            }
            /*changed DO softwareMoveEN*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN !=
                dido_value.gpio_do_u.gpio_do_bit.DO_softwareMoveEN)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN =
                    dido_value.gpio_do_u.gpio_do_bit.DO_softwareMoveEN;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_softwareMoveEN);
                device_err = device_write(self->do_gpio_softwareMoveEN, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write softwareMoveEN fail!\r\n");
                }
            }
            /*changed DO SoftwareHvEn*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn !=
                dido_value.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn =
                    dido_value.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_SoftwareHvEn);
                device_err = device_write(self->do_gpio_SoftwareHvEn, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write SoftwareHvEn fail!\r\n");
                }
            }
            /*changed DO TreatmentMotionEnable*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable !=
                dido_value.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable =
                    dido_value.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_TreatmentMotionEnable);
                device_err = device_write(self->do_gpio_TreatmentMotionEnable, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write TreatmentMotionEnable fail!\r\n");
                }
            }
            /*changed DO ThreePhasePowerOn*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn !=
                dido_value.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn =
                    dido_value.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_ThreePhasePowerOn);
                device_err = device_write(self->do_gpio_ThreePhasePowerOn, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write ThreePhasePowerOn fail!\r\n");
                }
            }
            /*changed DO AsuMotionEnable*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_AsuMotionEnable !=
                dido_value.gpio_do_u.gpio_do_bit.DO_AsuMotionEnable)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_AsuMotionEnable =
                    dido_value.gpio_do_u.gpio_do_bit.DO_AsuMotionEnable;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_AsuMotionEnable);
                device_err = device_write(self->do_gpio_AsuMotionEnable, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write AsuMotionEnable fail!\r\n");
                }
            }
            /*changed DO RUN_LED1*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED1 !=
                dido_value.gpio_do_u.gpio_do_bit.RUN_LED1)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED1 =
                    dido_value.gpio_do_u.gpio_do_bit.RUN_LED1;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED1);
                device_err = device_write(self->do_gpio_RUN_LED1, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write RUN_LED1 fail!\r\n");
                }
            }
            /*changed DO RUN_LED2*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED2 !=
                dido_value.gpio_do_u.gpio_do_bit.RUN_LED2)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED2 =
                    dido_value.gpio_do_u.gpio_do_bit.RUN_LED2;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED2);
                device_err = device_write(self->do_gpio_RUN_LED2, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write RUN_LED2 fail!\r\n");
                }
            }
            /*changed DO RUN_LED3*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED3 !=
                dido_value.gpio_do_u.gpio_do_bit.RUN_LED3)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED3 =
                    dido_value.gpio_do_u.gpio_do_bit.RUN_LED3;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED3);
                device_err = device_write(self->do_gpio_RUN_LED3, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write RUN_LED3 fail!\r\n");
                }
            }
            /*changed DO RUN_LED4*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED4 !=
                dido_value.gpio_do_u.gpio_do_bit.RUN_LED4)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED4 =
                    dido_value.gpio_do_u.gpio_do_bit.RUN_LED4;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED4);
                device_err = device_write(self->do_gpio_RUN_LED4, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write RUN_LED4 fail!\r\n");
                }
            }
            /*changed DO RUN_LED5*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED5 !=
                dido_value.gpio_do_u.gpio_do_bit.RUN_LED5)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED5 =
                    dido_value.gpio_do_u.gpio_do_bit.RUN_LED5;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED5);
                device_err = device_write(self->do_gpio_RUN_LED5, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write RUN_LED5 fail!\r\n");
                }
            }
            /*changed DO RUN_LED6*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED6 !=
                dido_value.gpio_do_u.gpio_do_bit.RUN_LED6)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED6 =
                    dido_value.gpio_do_u.gpio_do_bit.RUN_LED6;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.RUN_LED6);
                device_err = device_write(self->do_gpio_RUN_LED6, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write RUN_LED6 fail!\r\n");
                }
            }
        }
    }
exit:
    osThreadExit();
}
/**
 * @brief
 *
 * @param self
 * @return int32_t
 */
int32_t app_dido_create(app_dido_t *self)
{

    memset(self, 0, sizeof(app_dido_t));
    self->mutex = osMutexNew(NULL);
    if (self->mutex == NULL)
    {
        return -1;
    }
    /*3.创建线程RX、TX*/
    osThreadAttr_t di_poll_thread_attributes = {
        .name = "app_di_poll_thread",
        .stack_size = 1024 * 4,
        .priority = (osPriority_t)osPriorityAboveNormal,
    };
    osThreadId_t threadHandle = osThreadNew(app_di_poll_entry,
                                            self,
                                            &di_poll_thread_attributes);
    if (threadHandle == NULL)
    {
        return -2;
    }

    osThreadAttr_t do_thread_attributes = {
        .name = "app_do_thread",
        .stack_size = 1024 * 4,
        .priority = (osPriority_t)osPriorityAboveNormal,
    };
    self->do_thread_id = osThreadNew(app_do_entry,
                                     self,
                                     &do_thread_attributes);
    if (self->do_thread_id == NULL)
    {
        return -3;
    }
    return 0;
}

void app_di_get(app_dido_t *self, dido_structure_t *dido_value)
{
    osMutexAcquire(self->mutex, osWaitForever);
    memcpy(dido_value, &self->dido_structure_temp, DI_DATA_OFFSET_END - DI_DATA_OFFSET_START);
    osMutexRelease(self->mutex);
}

void app_do_set(app_dido_t *self, dido_structure_t *dido_value)
{
    osMutexAcquire(self->mutex, osWaitForever);
    if (memcmp((uint8_t *)(&self->dido_structure_temp) + DO_DATA_OFFSET_START, (uint8_t *)dido_value + DO_DATA_OFFSET_START, DO_DATA_OFFSET_END - DO_DATA_OFFSET_START) != 0)
    {
        memcpy((uint8_t *)(&self->dido_structure_temp) + DO_DATA_OFFSET_START, (uint8_t *)dido_value + DO_DATA_OFFSET_START, DO_DATA_OFFSET_END - DO_DATA_OFFSET_START);
        osThreadFlagsSet(self->do_thread_id, APP_RTM_THREAD_FLAG_DO_UPDATE);
    }
    osMutexRelease(self->mutex);
}
void app_do_get(app_dido_t *self, dido_structure_t *dido_value)
{
    osMutexAcquire(self->mutex, osWaitForever);
    memcpy((uint8_t *)dido_value + DO_DATA_OFFSET_START, (uint8_t *)(&self->dido_structure_temp) + DO_DATA_OFFSET_START, DO_DATA_OFFSET_END - DO_DATA_OFFSET_START);
    osMutexRelease(self->mutex);
}