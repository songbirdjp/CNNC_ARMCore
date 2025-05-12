#include "main_app.h"
#include "websocket.h"
#include "init_call.h"
#include "ethercat.h"
#include "tcp_tasks.h"
#include "lan9252_app.h"
#include "ulog.h"
#include "bgm_def.h"
#include "websocket_console.h"
#include "websocket_port.h"
#include "bgm_app.h"
#include "io_port.h"
#include "plan_data.h"

#define DATA_PROCESS_LAN_EVENT      (1<<0)
#define DATA_PROCESS_TCP_EVENT      (1<<1)
#define DATA_PROCESS_FPGA_EVENT     (1<<2)
static osEventFlagsId_t data_process_eventHandle = NULL;

static uint8_t bgm_fsm_state_ctrl = 0;
#include "shell.h"
static int8_t bgm_fsm_state_ctrl_switch(uint8_t argc, char *argv[])
{
    bgm_fsm_state_ctrl = atoi(argv[1]);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_fsm_state_ctrl_switch, bgm_fsm_state_ctrl_switch, set bgm fsm state ctrl);

static int8_t bgm_fsm_state_request_set(uint8_t argc, char *argv[])
{
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    obj->fsm_state_request = atoi(argv[1]);
    osMutexRelease(obj->mutex);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_fsm_state_request_set, bgm_fsm_state_request_set, set bgm fsm state request);

static int8_t bgm_fsm_state_set_pre(uint8_t argc, char *argv[])
{
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    obj->fsm_state_request_pre = atoi(argv[1]);
    obj->fsm_state_request_already = atoi(argv[1]);
    osMutexRelease(obj->mutex);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(bgm_fsm_state_set_pre, bgm_fsm_state_set_pre, set bgm fsm state pre);

static int8_t ethercat_recv_data_process(TOBJ7010 *recv)
{
    int8_t ret = 0;
    static enum bgm_fsm_state last_fsm_state = BGM_STATE_MAX;
    static uint16_t last_radiation_index = 0;
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);

    if (last_radiation_index != recv->OutU16_RadiationIndex && obj->fsm_state == BGM_STATE_WORK)
    {
        last_radiation_index = recv->OutU16_RadiationIndex;

        ret = dose_radiation_index_set(BGM_UART_DOSE1, last_radiation_index, 0);
        ret |= dose_radiation_index_set(BGM_UART_DOSE2, last_radiation_index, 0);
    }

    if (bgm_fsm_state_ctrl == 0)
    {
        obj->fsm_state_request = recv->OutU8_RequireState;
    }
    
    if (last_fsm_state != obj->fsm_state_request)
    {
        obj->fsm_state_request_pre = last_fsm_state;
        last_fsm_state = obj->fsm_state_request;
    }
    obj->beam_id = recv->OutU8_BeamId;

    if (obj->fsm_state != BGM_STATE_WORK)
    {
        obj->radiation_index = recv->OutU16_RadiationIndex;
    }
    else
    {
        switch (obj->deliver_type)
        {
        case DELIVER_TYPE_VMAT:
        case DELIVER_TYPE_HiMAT:
        case DELIVER_TYPE_SURVIEW:
        case DELIVER_TYPE_CT:
            obj->radiation_index = recv->OutU16_RadiationIndex;
            break;
        case DELIVER_TYPE_SWIMRT:
        case DELIVER_TYPE_SSIMRT:
        case DELIVER_TYPE_CRT:
            /* radiation index is updated by dose board */
            obj->radiation_index = dose_radiation_index_get(BGM_UART_DOSE1);
            break;

        default:
            LOG_E("invalid deliver type: %d\r\n", obj->deliver_type);
            break;
        }
    }

    osMutexRelease(obj->mutex);

    return ret;
}

static int8_t ethercat_send_data_process(TOBJ6000 *send)
{
    int8_t ret = 0;
    struct bgm_data_info *obj = bgm_data_info_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    send->InU8_FsmState = obj->fsm_state;
    send->InU8_BeamId = obj->beam_id;
    send->InU16_RadiationIndex = obj->radiation_index;
    osMutexRelease(obj->mutex);

    send->InU16_NotReadyEvent = 0;

    struct interlocks interlock = interlock_status_get();
    send->InU32_WaringInterlock = interlock.detect_status.bytes;
    send->InU32_MinorInterlock = interlock.extend_status.interrupt_flag << 16 | interlock.extend_status.interrupt_capture;
    send->InU32_SeriousInterlock = interlock.extend_status.current.bytes;


    send->InF_BeamOnTime = 0;
    send->InF_PrimaryDoseTotalActual = dose_meter_value_get(BGM_UART_DOSE1);
    send->InF_SecondaryDoseTotalActual = dose_meter_value_get(BGM_UART_DOSE2);
    send->InF_PrimaryDoseRateActual = dose_rate_value_get(BGM_UART_DOSE1);
    send->InF_SecondaryDoseRateActual = dose_rate_value_get(BGM_UART_DOSE2);


    send->InU8_DoseAFsmState = (uint8_t)dose_fsm_state_get(BGM_UART_DOSE1);
    send->InU8_DoseBFsmState = (uint8_t)dose_fsm_state_get(BGM_UART_DOSE2);
    send->InU32_DoseAInterlock = dose_interlock_get(BGM_UART_DOSE1);
    send->InU32_DoseBInterlock = dose_interlock_get(BGM_UART_DOSE2);

    send->InU32_AfcState = 0;
    send->InF_AfcPositionCurrent = 0;

    return ret;
}

static int8_t realtime_ethercat_data_process(void)
{
    TOBJ7010 recv_data = {0};
    TOBJ6000 send_data = {0};

    TOBJ7010 *recv = (TOBJ7010 *)ethercat_recv_data_get((uint16_t *)&recv_data, sizeof(recv_data));
    TOBJ6000 *send = (TOBJ6000 *)ethercat_send_data_get((uint16_t *)&send_data, sizeof(send_data));
    if (recv == NULL || send == NULL)
    {
        LOG_E("ethercat data get failed\r\n");
        return -1;
    }

    int8_t ret = ethercat_recv_data_process(recv);
    if (ret != 0)
    {
        LOG_E("ethercat data process err: %d\r\n", ret);
        return -2;
    }

    ret = ethercat_send_data_process(send);
    if (ret != 0)
    {
        LOG_E("ethercat data process err: %d\r\n", ret);
        return -3;
    }

    return ethercat_send_data_update(send, sizeof(send_data));
}

static int8_t non_realtime_tcp_callback(uint8_t sn)
{
#ifdef TCP_WEBSOCKET
    return ws_send_data_process(sn);
#endif

    return 0;
}

static int8_t non_realtime_tcp_recv_data_callback(void)
{
    osEventFlagsSet(data_process_eventHandle, DATA_PROCESS_TCP_EVENT);
    return 0;
}

static void tcp_recv_data_process(APP_DATA_RECV *info)
{
    int8_t ret = 0;

    ret = websocket_shell_cmd_parse(info->sn, info->tcpData, info->length);
    if (ret != 0)
    {
        LOG_E("websocket shell cmd parse err: %d\r\n", ret);
    }

    ret = websocket_cmd_parse(info);
    if (ret != 0)
    {
        LOG_E("websocket cmd parse err: %d\r\n", ret);
    }

    /* add other process here */
}

static int8_t data_process_init(void)
{
    int8_t ret = 0;

    ret = ws_data_process_callback_register(tcp_recv_data_process);
    if (ret != 0)
    {
        LOG_E("websocket data process callback register err: %d\r\n", ret);
        return ret;
    }
    ret = tcp_establish_cb_register(non_realtime_tcp_callback);
    if (ret != 0)
    {
        LOG_E("tcp callback register err: %d\r\n", ret);
        return ret;
    }
    ret = tcp_recv_data_callback_register(non_realtime_tcp_recv_data_callback);
    if (ret != 0)
    {
        LOG_E("tcp recv data callback register err: %d\r\n", ret);
        return ret;
    }
    ret = ethercat_slave_appl_cb_register(data_process_eventHandle, DATA_PROCESS_LAN_EVENT, realtime_ethercat_data_process);
    if (ret != 0)
    {
        LOG_E("ethercat callback register err: %d\r\n", ret);
        return ret;
    }
    return 0;
}

static void data_process_entry(void *argument)
{
    osStatus_t stat = 0;
    uint32_t event_flag = 0;
    TCP_DATA_t tcp_info = {0};

    osDelay(1000);

    data_process_init();  /* register callback functions for tcp 、ethercat、fpga */

    for (;;)
    {   
        event_flag = osEventFlagsWait(data_process_eventHandle, DATA_PROCESS_FPGA_EVENT | DATA_PROCESS_LAN_EVENT | DATA_PROCESS_TCP_EVENT, osFlagsWaitAny, osWaitForever);
        if (event_flag & DATA_PROCESS_LAN_EVENT)
        {
            ethercat_recv_data_update_with_block(0);
        }
        if (event_flag & DATA_PROCESS_TCP_EVENT)
        {
            stat = tcp_data_recv_get_with_block(&tcp_info, 0);
            if (stat == osOK)
            {
#ifdef TCP_WEBSOCKET
                extern int32_t ws_recv_data_process(TCP_DATA_t *recvData);
                ws_recv_data_process(&tcp_info);
#endif
            }
            else
            {
                LOG_E("no msg in tcp rx queue: %d\r\n", stat);
            }
        }
    }
}

static int8_t main_app_thread_init(void)
{
    osThreadAttr_t recv_data_process_thread_attributes = {
    .name = "recv_data_process_thread",
    .stack_size = 2048 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    data_process_eventHandle = osEventFlagsNew(NULL);
    if (data_process_eventHandle == NULL)
    {
        printf("event data process create failed\r\n");
        return -1;
    }

    osThreadId_t recv_data_process_threadHandle = osThreadNew(data_process_entry, NULL, &recv_data_process_thread_attributes);
    if (recv_data_process_threadHandle == NULL)
    {
        printf("thread recv data process create failed\r\n");
        return -2;
    }

    return 0;
}
INIT_APP_EXPORT(main_app_thread_init);