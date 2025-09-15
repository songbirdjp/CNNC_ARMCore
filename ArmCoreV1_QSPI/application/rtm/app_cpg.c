/**
 * @file app_cpg.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-09-04
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "app_cpg.h"

#include "rtm_main.h"
#include "ulog.h"
#include "init_call.h"
#include "fdcan_port.h"
void app_cpg_rx_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    uint16_t rx_len = sizeof(cpg_recv_structure_t);
    int8_t ret = 0;
    struct fdcan_rx_msg msg = {0};
    uint32_t CpgButton_1 = 0;
    uint32_t CpgButton_2 = 0;
    button_state_t button_state = {0};
    button_state_t button_state_bak = button_state;
    button_state_t filtered_button_state = {0};
    uint32_t current_time = osKernelGetTickCount();
    uint32_t last_time = current_time;
    uint32_t link_timeout_last_time_l = current_time;
    uint32_t link_timeout_last_time_r = current_time;
    fdcan1_enable_switch(1);
    for (;;)
    {
        ret = fdcan1_data_read(&msg, 100);
        if (ret != 0)
        {
            current_time = osKernelGetTickCount();
            if (current_time - link_timeout_last_time_l > 1000)
            {
                link_timeout_last_time_l = current_time;
                bit_set(self->app_cpg.manage_info.status_word, CPG_L_LINK_STATE_BIT);
            }
            if (current_time - link_timeout_last_time_r > 1000)
            {
                link_timeout_last_time_r = current_time;
                bit_set(self->app_cpg.manage_info.status_word, CPG_R_LINK_STATE_BIT);
            }
            LOG_E("fdcan1_data_read error, ret = %d\r\n", ret);
            continue;
        }
        if ((msg.header.Identifier != 0x11) && (msg.header.Identifier != 0x12))
        {
            LOG_E("msg.header.Identifier error, Identifier = 0x%x\r\n", msg.header.Identifier);
            continue;
        }
        if (msg.header.Identifier == 0x11)
        {
            link_timeout_last_time_l = osKernelGetTickCount();
            CpgButton_1 = (*(cpg_recv_structure_t *)&msg.buf).CpgButton;
            // LOG_I("CpgButton_1 = 0x%x\r\n", CpgButton_1);
            ret = app_keyboard_fault_get(&self->app_keyboard, BUTTON_TYPE_CPG_L);
            if (ret != 0)
            {
                bit_set(self->app_cpg.manage_info.status_word, CPG_L_FAULT_STATE_BIT);
                LOG_E("app_keyboard_fault_check error, ret = %d\r\n", ret);
            }

            app_keyboard_button_state_get(&self->app_keyboard,
                                          BUTTON_TYPE_CPG_L,
                                          CpgButton_1,
                                          &button_state);
        }
        else if (msg.header.Identifier == 0x12)
        {
            link_timeout_last_time_r = osKernelGetTickCount();
            CpgButton_2 = (*(cpg_recv_structure_t *)&msg.buf).CpgButton;
            // LOG_I("CpgButton_2 = 0x%x\r\n", CpgButton_2);
            ret = app_keyboard_fault_get(&self->app_keyboard, BUTTON_TYPE_CPG_R);
            if (ret != 0)
            {
                bit_set(self->app_cpg.manage_info.status_word, CPG_R_FAULT_STATE_BIT);
                LOG_E("app_keyboard_fault_check error, ret = %d\r\n", ret);
            }

            app_keyboard_button_state_get(&self->app_keyboard,
                                          BUTTON_TYPE_CPG_R,
                                          CpgButton_2,
                                          &button_state);
        }
        if (memcmp(&button_state, &button_state_bak, sizeof(button_state)) != 0)
        {
            // LOG_I("CpgButton_1 = 0x%x\r\n", CpgButton_1);
            // LOG_I("CpgButton_2 = 0x%x\r\n", CpgButton_2);
            // LOG_I("button_state function_state:%d\r\n", button_state.function_state);
            // LOG_I("button_state special_button:0x%x\r\n", button_state.special_button);
            current_time = osKernelGetTickCount();
            last_time = current_time;
            memcpy(&button_state_bak, &button_state, sizeof(button_state_t));

            app_keyboard_button_state_filter(&self->app_keyboard,
                                             FILTER_TYPE_GMM,
                                             button_state,
                                             &filtered_button_state);
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_OFF_PLC],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          SEND_RTM_OFF_BUTTON_CMD,
                                          &filtered_button_state,
                                          sizeof(button_state_t));
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }

            app_keyboard_button_state_filter(&self->app_keyboard,
                                             FILTER_TYPE_RTM_ON_PLC,
                                             button_state,
                                             &filtered_button_state);
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_ON],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          SEND_RTM_OFF_BUTTON_CMD,
                                          &filtered_button_state,
                                          sizeof(button_state_t));
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
            app_keyboard_button_state_filter(&self->app_keyboard,
                                             FILTER_TYPE_RTM_OFF_ARM,
                                             button_state,
                                             &filtered_button_state);
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_OFF_ARM],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          SEND_RTM_OFF_BUTTON_CMD,
                                          &filtered_button_state,
                                          sizeof(button_state_t));
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
            app_keyboard_button_state_filter(&self->app_keyboard,
                                             FILTER_TYPE_PSM,
                                             button_state,
                                             &filtered_button_state);
            ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_PSM],
                                          GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                          SEND_RTM_OFF_BUTTON_CMD,
                                          &filtered_button_state,
                                          sizeof(button_state_t));
            if (ret != 0)
            {
                LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
            }
        }
        else
        {
            current_time = osKernelGetTickCount();
            if ((current_time - last_time) > 500)
            {
                last_time = current_time;

                app_keyboard_button_state_filter(&self->app_keyboard,
                                                 FILTER_TYPE_GMM,
                                                 button_state,
                                                 &filtered_button_state);
                ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_OFF_PLC],
                                              GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                              SEND_RTM_OFF_BUTTON_CMD,
                                              &filtered_button_state,
                                              sizeof(button_state_t));
                if (ret != 0)
                {
                    LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
                }
                app_keyboard_button_state_filter(&self->app_keyboard,
                                                 FILTER_TYPE_RTM_ON_PLC,
                                                 button_state,
                                                 &filtered_button_state);
                ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_ON],
                                              GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                              SEND_RTM_OFF_BUTTON_CMD,
                                              &filtered_button_state,
                                              sizeof(button_state_t));
                if (ret != 0)
                {
                    LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
                }
                app_keyboard_button_state_filter(&self->app_keyboard,
                                                 FILTER_TYPE_RTM_OFF_ARM,
                                                 button_state,
                                                 &filtered_button_state);
                ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_RTM_OFF_ARM],
                                              GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                              SEND_RTM_OFF_BUTTON_CMD,
                                              &filtered_button_state,
                                              sizeof(button_state_t));
                if (ret != 0)
                {
                    LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
                }
                app_keyboard_button_state_filter(&self->app_keyboard,
                                                 FILTER_TYPE_PSM,
                                                 button_state,
                                                 &filtered_button_state);
                ret = rtm_set_data_distribute(self->rtm_module_info[RTM_MODULE_CPG].queue_group[RTM_MODULE_PSM],
                                              GMM_ID | PSM_ID | RTM_ON_PLC_ID | RTM_OFF_ARM_ID,
                                              SEND_RTM_OFF_BUTTON_CMD,
                                              &filtered_button_state,
                                              sizeof(button_state_t));
                if (ret != 0)
                {
                    LOG_E("rtm_set_data_distribute error, ret = %d\r\n", ret);
                }
            }
        }
    }
exit:
    osThreadExit();
}
void app_cpg_tx_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    queue_frame_t queue_frame;
    osStatus_t status = osOK;
    int8_t ret = 0;
    uint16_t tx_len = sizeof(cpg_send_structure_t);
    cpg_send_structure_t send_data = {0}, send_data_bak = {0};
    memset(&send_data, 0, sizeof(cpg_send_structure_t));
    memset(&send_data_bak, 0, sizeof(cpg_send_structure_t));

    prompt_state_t prompt_state;
    original_prompt_t original_prompt;
    uint8_t led_state = 0;

    send_data.OffGantryUnitInfo = 0xFFFF;
    send_data.BoardID = 0x01;
    send_data.HardwareVersion = 0x01;
    send_data.FirmWareVersion = 0x00000001;
    for (;;)
    {
        status = osMessageQueueGet(self->rtm_module_info[RTM_MODULE_CPG].module_queue, &queue_frame, NULL, 50);
        if (status != osOK)
        {
            ret = fdcan1_data_write(0x01, (uint8_t *)&send_data_bak, sizeof(cpg_send_structure_t));
            if (ret != 0)
            {
                LOG_E("fdcan1_data_write error,id = 0x01, ret = %d\r\n", ret);
            }
            ret = fdcan1_data_write(0x02, (uint8_t *)&send_data_bak, sizeof(cpg_send_structure_t));
            if (ret != 0)
            {
                LOG_E("fdcan1_data_write error,id = 0x02, ret = %d\r\n", ret);
            }
            continue;
        }

        uint8_t type = queue_frame.payload.type;
        uint8_t cmd = queue_frame.payload.data[0];
        uint16_t len = queue_frame.payload.length - 1;
        if (type == 0x05)
        {
            switch (cmd)
            {
            case SEND_RTM_OFF_PROMPT_CMD:
                prompt_state = *(prompt_state_t *)(queue_frame.payload.data + 1);
                app_keyboard_prompt_state_get(&self->app_keyboard,
                                              BUTTON_TYPE_CPG_R,
                                              prompt_state,
                                              &original_prompt);
                // LOG_I("prompt_state:0x%x\r\n", *(uint32_t *)&prompt_state);
                // LOG_I("original_prompt led_blink:0x%x\r\n", original_prompt.Led_blink);
                // LOG_I("original_prompt userPrompt:0x%x\r\n", original_prompt.userPrompt);
                memcpy(&(send_data.CpgLedBlink), &original_prompt, sizeof(original_prompt_t));
                break;
            case RECEIVE_SYSTEM_STATE_CMD:
                memcpy(&(send_data.SystemCurrentState), queue_frame.payload.data + 1, len);
                break;
            case SEND_PSM_BRAKE_CMD:
                led_state = *(uint8_t *)(queue_frame.payload.data + 1);
                app_keyboard_cpg_original_prompt_set(&self->app_keyboard,
                                                     CPG_ORIGINAL_PROMPT_SET_TYPE_Y_BRAKE,
                                                     led_state);
                app_keyboard_prompt_state_get(&self->app_keyboard,
                                              BUTTON_TYPE_CPG_R,
                                              prompt_state,
                                              &original_prompt);
                int32_t retval = app_keyboard_cpg_original_prompt_check(&self->app_keyboard,
                                                                        CPG_ORIGINAL_PROMPT_SET_TYPE_Y_BRAKE,
                                                                        led_state);
                if (retval != 0)
                {
                    bit_set(self->app_cpg.manage_info.status_word, CPG_Y_BRAKE_STATE_BIT);
                    LOG_E("CPG_Y_BRAKE_STATE_BIT error, ret = %d\r\n", retval);
                }
                memcpy(&(send_data.CpgLedBlink), &original_prompt, sizeof(original_prompt_t));
                break;
            default:
                continue;
                break;
            }
        }

        if (memcmp(&send_data, &send_data_bak, sizeof(cpg_send_structure_t)) != 0)
        {
            memcpy(&send_data_bak, &send_data, sizeof(cpg_send_structure_t));

            ret = fdcan1_data_write(0x01, (uint8_t *)&send_data, sizeof(cpg_send_structure_t));
            if (ret != 0)
            {
                LOG_E("fdcan1_data_write error,id = 0x01, ret = %d\r\n", ret);
            }
            ret = fdcan1_data_write(0x02, (uint8_t *)&send_data, sizeof(cpg_send_structure_t));
            if (ret != 0)
            {
                LOG_E("fdcan1_data_write error,id = 0x02, ret = %d\r\n", ret);
            }
        }
    }
exit:
    osThreadExit();
}