#include "main_app.h"
#include "websocket.h"
#include "init_call.h"
#include "ethercat.h"
#include "tcp_tasks.h"
#include "websocket_console.h"
#include "lan9252_app.h"
#include "ulog.h"
#include "app_manage.h"

#define DATA_PROCESS_LAN_EVENT (1 << 0)
#define DATA_PROCESS_TCP_EVENT (1 << 1)
#define DATA_PROCESS_FPGA_EVENT (1 << 2)
static osEventFlagsId_t data_process_eventHandle = NULL;

static int8_t realtime_ethercat_data_process(void)
{
#define ETHERCAT_STATE_OP (0x08)
    if (ethercat_state_get() == ETHERCAT_STATE_OP)
    {
        app_rtm_thread_flag_set(APP_RTM_THREAD_FLAG_ETHERCAT_READY);
        app_rtm_ethercat_state_op_set();
        app_rtm_event_output_set();
    }
    else
    {
        app_rtm_ethercat_state_op_clean();
    }
    return 0;
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

static int8_t tcp_recv_data_process(struct tcp_data *info);
static int8_t websocket_recv_data_process(APP_DATA_RECV *info);

static int8_t data_process_init(void)
{
    int8_t ret = 0;
    ret = ws_data_process_callback_register(websocket_recv_data_process);
    if (ret != 0)
    {
        LOG_E("websocket data process callback register err: %d\r\n", ret);
        return ret;
    }
    ret = tcp_establish_cb_register(CONTROLLER_AUTHORIZATION, non_realtime_tcp_callback);   /* NOTE: search by name, bind callback function to sn */
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
    struct tcp_data tcp_info = {0};

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
                tcp_recv_data_process(&tcp_info);
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
        LOG_E("event data process create failed\r\n");
        return -1;
    }

    osThreadId_t recv_data_process_threadHandle = osThreadNew(data_process_entry, NULL, &recv_data_process_thread_attributes);
    if (recv_data_process_threadHandle == NULL)
    {
        LOG_E("thread recv data process create failed\r\n");
        return -2;
    }

    return 0;
}
INIT_APP_EXPORT(main_app_thread_init);


/**********************************************************************************************************************************/
/* add user function below */
static int8_t tcp_recv_data_process(struct tcp_data *info)
{
    int8_t ret = 0;

    uint8_t *name = socket_name_get_by_sn(info->sn);

    if (name == NULL || memcmp(socket_name_get_by_sn(info->sn), CONTROLLER_AUTHORIZATION, strlen(CONTROLLER_AUTHORIZATION)) == 0 ||
        memcmp(socket_name_get_by_sn(info->sn), SERVICE_AUTHORIZATION, strlen(SERVICE_AUTHORIZATION)) == 0 ||
        memcmp(socket_name_get_by_sn(info->sn), SHELL_AUTHORIZATION, strlen(SHELL_AUTHORIZATION)) == 0)
    {
#ifdef TCP_WEBSOCKET
        ws_recv_data_process(info);
#endif
    }
    else
    {
        /* other tcp protocol, user add process here */
    }

    return 0;
}

static int8_t websocket_recv_data_process(APP_DATA_RECV *info)
{
    int8_t ret = 0;

    if (memcmp(socket_name_get_by_sn(info->sn), SHELL_AUTHORIZATION, strlen(SHELL_AUTHORIZATION)) == 0)
    {
        ret = websocket_shell_cmd_parse(info->sn, info->tcpData, info->length);
        if (ret != 0)
        {
            LOG_E("websocket shell cmd parse err: %d\r\n", ret);
        }
    }
    else if (memcmp(socket_name_get_by_sn(info->sn), CONTROLLER_AUTHORIZATION, strlen(CONTROLLER_AUTHORIZATION)) == 0)
    {
        /* add plc cmd process here */
    }
    else if (memcmp(socket_name_get_by_sn(info->sn), SERVICE_AUTHORIZATION, strlen(SERVICE_AUTHORIZATION)) == 0)
    {
        /* add service cmd process here */
    }
    else
    {
        /* do nothing */
    }

    return 0;
}