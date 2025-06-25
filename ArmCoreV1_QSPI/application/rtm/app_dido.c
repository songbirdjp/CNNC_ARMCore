#include "app_dido.h"
#include "drv_pin.h"
#include "drv_i2c.h"
#include "init_call.h"
#include "shell.h"
#include "ulog.h"
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

    self->do_gpio_Pulse_Inhibit = device_find(DEVICE_NAME_PIN_DO_PULSE_INHIBIT);
    if (self->do_gpio_Pulse_Inhibit == NULL)
    {
        return -1;
    }
    device_err = device_open(self->do_gpio_Pulse_Inhibit);
    if (device_err != DEV_EOK)
    {
        return -2;
    }

    self->do_gpio_Emergency = device_find(DEVICE_NAME_PIN_DO_EMERGENCY);
    if (self->do_gpio_Emergency == NULL)
    {
        return -3;
    }
    device_err = device_open(self->do_gpio_Emergency);
    if (device_err != DEV_EOK)
    {
        return -4;
    }

    self->do_gpio_KV_TreatmentEN = device_find(DEVICE_NAME_PIN_DO_KV_TREATMENT_EN);
    if (self->do_gpio_KV_TreatmentEN == NULL)
    {
        return -5;
    }
    device_err = device_open(self->do_gpio_KV_TreatmentEN);
    if (device_err != DEV_EOK)
    {
        return -6;
    }

    self->do_gpio_MV_TreatmentEN = device_find(DEVICE_NAME_PIN_DO_MV_TREATMENT_EN);
    if (self->do_gpio_MV_TreatmentEN == NULL)
    {
        return -7;
    }
    device_err = device_open(self->do_gpio_MV_TreatmentEN);
    if (device_err != DEV_EOK)
    {
        return -8;
    }
    self->do_gpio_RUN_LED1 = device_find(DEVICE_NAME_PIN_RUN_LED_1);
    if (self->do_gpio_RUN_LED1 == NULL)
    {
        return -9;
    }
    device_err = device_open(self->do_gpio_RUN_LED1);
    if (device_err != DEV_EOK)
    {
        return -10;
    }
    self->do_gpio_RUN_LED2 = device_find(DEVICE_NAME_PIN_RUN_LED_2);
    if (self->do_gpio_RUN_LED2 == NULL)
    {
        return -11;
    }
    device_err = device_open(self->do_gpio_RUN_LED2);
    if (device_err != DEV_EOK)
    {
        return -12;
    }
    self->do_gpio_RUN_LED3 = device_find(DEVICE_NAME_PIN_RUN_LED_3);
    if (self->do_gpio_RUN_LED3 == NULL)
    {
        return -13;
    }
    device_err = device_open(self->do_gpio_RUN_LED3);
    if (device_err != DEV_EOK)
    {
        return -14;
    }
    self->do_gpio_RUN_LED4 = device_find(DEVICE_NAME_PIN_RUN_LED_4);
    if (self->do_gpio_RUN_LED4 == NULL)
    {
        return -15;
    }
    device_err = device_open(self->do_gpio_RUN_LED4);
    if (device_err != DEV_EOK)
    {
        return -16;
    }
    self->do_gpio_RUN_LED5 = device_find(DEVICE_NAME_PIN_RUN_LED_5);
    if (self->do_gpio_RUN_LED5 == NULL)
    {
        return -17;
    }
    device_err = device_open(self->do_gpio_RUN_LED5);
    if (device_err != DEV_EOK)
    {
        return -18;
    }
    self->do_gpio_RUN_LED6 = device_find(DEVICE_NAME_PIN_RUN_LED_6);
    if (self->do_gpio_RUN_LED6 == NULL)
    {
        return -19;
    }
    device_err = device_open(self->do_gpio_RUN_LED6);
    if (device_err != DEV_EOK)
    {
        return -20;
    }
}
static int32_t di_device_init(app_dido_t *self)
{
    device_err_t device_err = DEV_EIO;

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
    uint8_t mcp23017_iocon = 0x44;
    device_err = device_ioctl(self->di_mcp23017_0x00,
                              DRIVER_MCP23017_REG_IOCON,
                              &mcp23017_iocon);
    if (device_err != DEV_EOK)
    {
        return -3;
    }
    uint8_t mcp23017_iocon = 0x44;

    self->di_gpio_gating = device_find(DEVICE_NAME_PIN_DI_GATING);
    if (self->di_gpio_gating == NULL)
    {
        return -4;
    }
    device_err = device_open(self->di_gpio_gating);
    if (device_err != DEV_EOK)
    {
        return -5;
    }

    self->di_tca9535_INT0 = device_find(DEVICE_NAME_PIN_DI_INT);
    if (self->di_tca9535_INT0 == NULL)
    {
        return -6;
    }
    device_err = device_open(self->di_tca9535_INT0);
    if (device_err != DEV_EOK)
    {
        return -7;
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

    tca9535_msg_t tca9535_msg = {
        .driver_tca9535_reg = DRIVER_TCA9535_REG_INPUT_PORT_1,
        .dataLen = 1};
    pin_msg_t pin_msg = PIN_STATE_NONE;
    // osDelay(100);//规避内部I2C解锁延时切换打断ethercat初始化过程，造成safe op
    int32_t retVal = di_device_init(self);
    if (retVal != 0)
    {
        LOG_I("di device init fail, errorCode:%d.\r\n", retVal);
        goto exit;
    }
    app_rtm_thread_flag_set(APP_RTM_THREAD_FLAG_DI);
    for (;;)
    {
        /*读取9535di*/
        tca9535_msg.data = (uint8_t *)(&(dido_value_cur.tca9535_0x00_u.tca9535_0x00));
        device_err = device_read(self->di_tca9535_0x00,
                                 &tca9535_msg,
                                 0,
                                 1000);
        if (device_err != DEV_EOK)
        {
            self->dido_enable_mask.tca9535_0x00_u.tca9535_0x00 = 0x00;
            LOG_I("read tca9535_0 fail, errorCode:%d\r\n", device_err);
            device_ioctl(self->di_tca9535_0x00,
                         I2C_CMD_INIT,
                         NULL);
        }
        else
        {
            self->dido_enable_mask.tca9535_0x00_u.tca9535_0x00 = 0xff;
        }
        /*读取gpio di*/
        device_err = device_read(self->di_gpio_gating,
                                 &pin_msg,
                                 0,
                                 1000);
        if (device_err != DEV_EOK)
        {
            self->dido_enable_mask.gpio_di_u.gpio_di = 0x00;
            LOG_I("read gating fail!\r\n");
        }
        else
        {
            self->dido_enable_mask.gpio_di_u.gpio_di = 0xff;
        }

        dido_value_cur.gpio_di_u.gpio_di_bit.DI_GATING = pin_msg;
        /*mask为1有效*/
        // for (uint8_t i = 0; i < (DI_DATA_OFFSET_END - DI_DATA_OFFSET_START); i++)
        // {
        //     *((uint8_t *)(&dido_value_cur) + DI_DATA_OFFSET_START + i) &=
        //         *((uint8_t *)&(self->dido_enable_mask) + DI_DATA_OFFSET_START + i);
        // }

        /*判断当前9535di状态，异或，与上次不同则为不确定状态1*/
        dido_state.tca9535_0x00_u.tca9535_0x00 = ((dido_value_cur.tca9535_0x00_u.tca9535_0x00) ^
                                                  (dido_value_last.tca9535_0x00_u.tca9535_0x00));
        dido_value_last.tca9535_0x00_u.tca9535_0x00 =
            dido_value_cur.tca9535_0x00_u.tca9535_0x00;

        /*判断当前gpio di状态，异或，与上次不同则为不确定状态1*/
        dido_state.gpio_di_u.gpio_di = ((dido_value_cur.gpio_di_u.gpio_di) ^
                                        (dido_value_last.gpio_di_u.gpio_di));
        dido_value_last.gpio_di_u.gpio_di = dido_value_cur.gpio_di_u.gpio_di;

        /*di 改变*/
        if ((0 != memcmp((uint8_t *)(&(self->dido_structure)) + DI_DATA_OFFSET_START,
                         (uint8_t *)(&dido_value_cur) + DI_DATA_OFFSET_START,
                         DI_DATA_OFFSET_END - DI_DATA_OFFSET_START)))
        {
            self->dido_structure.tca9535_0x00_u.tca9535_0x00 = (self->dido_structure.tca9535_0x00_u.tca9535_0x00 & dido_state.tca9535_0x00_u.tca9535_0x00) |
                                                               (dido_value_cur.tca9535_0x00_u.tca9535_0x00 & ~dido_state.tca9535_0x00_u.tca9535_0x00);
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
    pin_msg_t gpio_msg = PIN_STATE_NONE;
    uint32_t ret = 0;
    // osDelay(100);//规避内部I2C解锁延时切换打断ethercat初始化过程，造成safe op
    int32_t retVal = do_device_init(self);
    if (retVal != 0)
    {
        LOG_I("do device init fail, errorCode:%d.\r\n", retVal);
        goto exit;
    }
    app_rtm_thread_flag_set(APP_RTM_THREAD_FLAG_DO);
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
            /*changed DO Emergency*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_Emergency !=
                dido_value.gpio_do_u.gpio_do_bit.DO_Emergency)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_Emergency =
                    dido_value.gpio_do_u.gpio_do_bit.DO_Emergency;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_Emergency);
                device_err = device_write(self->do_gpio_Emergency, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write Emergency fail!\r\n");
                }
            }
            /*changed DO KV_TreatmentEN*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_KV_TreatmentEN !=
                dido_value.gpio_do_u.gpio_do_bit.DO_KV_TreatmentEN)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_KV_TreatmentEN =
                    dido_value.gpio_do_u.gpio_do_bit.DO_KV_TreatmentEN;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_KV_TreatmentEN);
                device_err = device_write(self->do_gpio_KV_TreatmentEN, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write KV_TreatmentEN fail!\r\n");
                }
            }
            /*changed DO MV_TreatmentEN*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_MV_TreatmentEN !=
                dido_value.gpio_do_u.gpio_do_bit.DO_MV_TreatmentEN)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_MV_TreatmentEN =
                    dido_value.gpio_do_u.gpio_do_bit.DO_MV_TreatmentEN;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_MV_TreatmentEN);
                device_err = device_write(self->do_gpio_MV_TreatmentEN, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write MV_TreatmentEN fail!\r\n");
                }
            }
            /*changed DO Pulse_Inhibit*/
            if (self->dido_structure.gpio_do_u.gpio_do_bit.DO_Pulse_Inhibit !=
                dido_value.gpio_do_u.gpio_do_bit.DO_Pulse_Inhibit)
            {
                self->dido_structure.gpio_do_u.gpio_do_bit.DO_Pulse_Inhibit =
                    dido_value.gpio_do_u.gpio_do_bit.DO_Pulse_Inhibit;
                gpio_msg = (pin_msg_t)(self->dido_structure.gpio_do_u.gpio_do_bit.DO_Pulse_Inhibit);
                device_err = device_write(self->do_gpio_Pulse_Inhibit, &gpio_msg, 0, 1000);
                if (device_err != DEV_EOK)
                {
                    LOG_I("write Pulse_Inhibit fail!\r\n");
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
            /*changed DO RUN_LED1*/
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
            /*changed DO RUN_LED1*/
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