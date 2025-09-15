/**
 * @file app_fkp.c
 * @author SI (siyunlong@cnncpm.com)
 * @brief
 * @version 0.1
 * @date 2025-09-04
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "app_fkp.h"
#include "rtm_main.h"
#include "ulog.h"
#include "init_call.h"
int32_t fkp_set_rx_callback(struct uart_protocol *const self,
                            uint32_t ID,
                            const uint8_t *data,
                            uint16_t *len,
                            void *arg)
{
    return 0;
}
int32_t fkp_reboot_rx_callback(struct uart_protocol *const self,
                               uint32_t ID,
                               const uint8_t *data,
                               uint16_t *len,
                               void *arg)
{
    app_rtm_main_t *app_rtm_main = (app_rtm_main_t *)arg;

    uart_protocol_reboot(&(app_rtm_main->rtm_module_info[ID].uart_protocol), ID, 1000);
    return 0;
}
int32_t fkp_heartbeat_rx_callback(struct uart_protocol *const self,
                                  uint32_t ID,
                                  const uint8_t *data,
                                  uint16_t *len,
                                  void *arg)
{
    rtm_module_info_t *rtm_module_info = (rtm_module_info_t *)arg;
    heartbeat_t *heartbeat = (heartbeat_t *)data;

    if (*len > sizeof(heartbeat_t))
    {
        LOG_E("%s heartbeat rx len err!\r\n", rtm_module_info->module_name);
    }

    if (memcmp(&(rtm_module_info->heartbeat_info_rx), heartbeat, sizeof(heartbeat_t)) != 0)
    {
        // LOG_E("%s heartbeat rx err!\r\n", rtm_module_info->module_name);
    }
    return 0;
}
int32_t fkp_heartbeat_rx_timeout_callback(struct uart_protocol *const self,
                                          uint32_t ID,
                                          const uint8_t *data,
                                          uint16_t *len,
                                          void *arg)
{
    rtm_module_info_t *rtm_module_info = (rtm_module_info_t *)arg;
    bit_set(rtm_module_info->manage_info.status_word, MODULE_LINK_STATE_BIT);
    return 0;
}
void app_fkp_rx_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    int32_t ret = 0;
    uint16_t len = 0;
    payload_t payload;
    uint32_t ID = 0;
    uint8_t cmd = 0;
    button_state_t button_state = {0};
    button_state_t button_state_bak = button_state;
    button_state_t filtered_button_state = {0};
    uint32_t current_time = 0;
    uint32_t last_time = 0;
    ret = uart_protocol_rx_RegisterCallback(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol,
                                            UART_PROTOCOL_SET_RX_CB_ID,
                                            fkp_set_rx_callback,
                                            NULL);
    if (ret != 0)
    {
        LOG_E("%s register callback error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
        goto exit;
    }
    ret = uart_protocol_rx_RegisterCallback(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol,
                                            UART_PROTOCOL_HEARTBEAT_RX_CB_ID,
                                            fkp_heartbeat_rx_callback,
                                            &self->rtm_module_info[RTM_MODULE_FKP]);
    if (ret != 0)
    {
        LOG_E("%s register callback error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
        goto exit;
    }
    ret = uart_protocol_rx_RegisterCallback(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol,
                                            UART_PROTOCOL_HEARTBEAT_RX_TIMEOUT_CB_ID,
                                            fkp_heartbeat_rx_timeout_callback,
                                            &self->rtm_module_info[RTM_MODULE_FKP]);
    if (ret != 0)
    {
        LOG_E("%s register callback error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
        goto exit;
    }
    ret = uart_protocol_open(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol);
    if (ret != 0)
    {
        LOG_E("%s open error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
        goto exit;
    }
    for (;;)
    {
        ret = uart_protocol_recv(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol, (uint8_t *)&payload, &len, 0xFFFFFFFF);
        if (ret != 0)
        {
            LOG_E("%s recv error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
            continue;
        }
        switch (payload.type)
        {
        case 0x03: /*参数配置帧*/
        case 0x04: /*参数获取帧*/
        case 0x05: /*SET帧*/
        case 0x06: /*GET帧*/
        {
            ID = payload.id_ack >> 1;
            cmd = payload.data[0];
            switch (cmd)
            {
            case SEND_FKP_BUTTON_CMD: // 按键原始键值
                uint32_t key_value = *(uint32_t *)(payload.data + 1);

                ret = app_keyboard_fault_get(&self->app_keyboard, BUTTON_TYPE_FKP);
                if (ret != 0)
                {
                    bit_set(self->rtm_module_info[RTM_MODULE_FKP].manage_info.status_word, MODULE_FAULT_STATE_BIT);
                    LOG_E("%s app_keyboard_fault_check error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
                }

                app_keyboard_button_state_get(&self->app_keyboard,
                                              BUTTON_TYPE_FKP,
                                              key_value,
                                              &button_state);
                // LOG_I("%s key_value = %x\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, key_value);
                // LOG_I("%s button_state function_state = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, button_state.function_state);
                // LOG_I("%s button_state special_button = %x\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, button_state.special_button);
                break;
            default:
                break;
            }
        }
        break;
        default:
            break;
        }
        if (memcmp(&button_state, &button_state_bak, sizeof(button_state)) != 0)
        {
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

int32_t fkp_heartbeat_tx_callback(struct uart_protocol *const self,
                                            uint8_t *data,
                                            uint16_t *len,
                                            void *arg)
{
    rtm_module_info_t *rtm_module_info = (rtm_module_info_t *)arg;
    memcpy(data, &(rtm_module_info->heartbeat_info_tx), sizeof(uint64_t));
    *len = sizeof(rtm_module_info->heartbeat_info_tx);
    return 0;
}
void app_fkp_tx_thread(void *argument)
{
    app_rtm_main_t *self = (app_rtm_main_t *)argument;
    osStatus_t status = osOK;
    queue_frame_t queue_frame;
    int32_t ret = 0;
    original_prompt_t original_prompt;
    ret = uart_protocol_tx_RegisterCallback(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol,
                                            UART_PROTOCOL_HEARTBEAT_TX_CB_ID,
                                            fkp_heartbeat_tx_callback,
                                            &self->rtm_module_info[RTM_MODULE_FKP]);
    if (ret != 0)
    {
        LOG_E("%s register callback error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
        goto exit;
    }

    ret = uart_protocol_open(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol);
    if (ret != 0)
    {
        LOG_E("%s open error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
        goto exit;
    }
    for (;;)
    {
        status = osMessageQueueGet(self->rtm_module_info[RTM_MODULE_FKP].module_queue, &queue_frame, NULL, 0xFFFFFFFF);
        if (status != osOK)
        {
            LOG_E("%s queue get error, status = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, status);
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
                prompt_state_t prompt_state = *(prompt_state_t *)(queue_frame.payload.data + 1);
                // LOG_I("prompt_state:0x%x\r\n", *(uint32_t *)&prompt_state);
                ret = app_keyboard_prompt_state_get(&self->app_keyboard,
                                                    BUTTON_TYPE_FKP,
                                                    prompt_state,
                                                    &original_prompt);
                queue_frame.payload.length = sizeof(original_prompt.Led_blink) + 1;
                queue_frame.payload.data[0] = SEND_RTM_OFF_BLINK_CMD;
                memcpy(queue_frame.payload.data + 1, &original_prompt.Led_blink, sizeof(original_prompt.Led_blink));
                queue_frame.length = queue_frame.payload.length + sizeof(payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH;
                ret = uart_protocol_send(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol, (uint8_t *)&queue_frame, queue_frame.length, 100);
                if (ret != 0)
                {
                    bit_set(self->rtm_module_info[RTM_MODULE_FKP].manage_info.status_word, MODULE_LINK_STATE_BIT);
                    LOG_E("%s send error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
                }
                // LOG_I("Led_blink:0x%x\r\n", queue_frame.payload.data[1]);
                queue_frame.payload.length = sizeof(original_prompt.userPrompt) + 1;
                queue_frame.payload.data[0] = SEND_RTM_OFF_USER_PROMPT_CMD;
                memcpy(queue_frame.payload.data + 1, &original_prompt.userPrompt, sizeof(original_prompt.userPrompt));
                queue_frame.length = queue_frame.payload.length + sizeof(payload_t) - UART_PROTOCOL_DATA_MAX_LENGTH;
                ret = uart_protocol_send(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol, (uint8_t *)&queue_frame, queue_frame.length, 100);
                if (ret != 0)
                {
                    bit_set(self->rtm_module_info[RTM_MODULE_FKP].manage_info.status_word, MODULE_LINK_STATE_BIT);
                    LOG_E("%s send error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
                }
                // LOG_I("userPrompt:0x%x\r\n", queue_frame.payload.data[1]);
                continue;
                break;
            case RECEIVE_FKP_TIMESTAMP_CMD:
            case RECEIVE_SYSTEM_STATE_CMD:
            case SEND_FKP_POWER_OFF_CMD:
            case RECEIVE_FKP_DOSE_CMD:
                ret = uart_protocol_send(&self->rtm_module_info[RTM_MODULE_FKP].uart_protocol, (uint8_t *)&queue_frame, queue_frame.length, 100);
                if (ret != 0)
                {
                    bit_set(self->rtm_module_info[RTM_MODULE_FKP].manage_info.status_word, MODULE_LINK_STATE_BIT);
                    LOG_E("%s send error, ret = %d\r\n", self->rtm_module_info[RTM_MODULE_FKP].module_name, ret);
                    continue;
                }
                break;
            default:
                continue;
                break;
            }
        }
    }
exit:
    osThreadExit();
}