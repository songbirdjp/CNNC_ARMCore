#include "main_app.h"
#include "websocket.h"
#include "init_call.h"
#include "ethercat.h"
#include "tcp_tasks.h"
#include "websocket_console.h"
#include "lan9252_app.h"
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

static void tcp_recv_data_process(APP_DATA_RECV *info)
{
    int8_t ret = 0;

    ret = websocket_shell_cmd_parse(info->sn, info->tcpData, info->length);
    if (ret != 0)
    {
        printf("websocket shell cmd parse err: %d\r\n", ret);
    }

    /* add other process here */
    // ret = websocket_cmd_parse(info);
    // if (ret != 0)
    // {
    //     printf("websocket cmd parse err: %d\r\n", ret);
    // }
}

static int8_t data_process_init(void)
{
    int8_t ret = 0;
    ret = ws_data_process_callback_register(tcp_recv_data_process);
    if (ret != 0)
    {
        printf("websocket data process callback register err: %d\r\n", ret);
        return ret;
    }
    ret = tcp_establish_cb_register(non_realtime_tcp_callback);
    if (ret != 0)
    {
        printf("tcp callback register err: %d\r\n", ret);
        return ret;
    }
    ret = tcp_recv_data_callback_register(non_realtime_tcp_recv_data_callback);
    if (ret != 0)
    {
        printf("tcp recv data callback register err: %d\r\n", ret);
        return ret;
    }
    ret = ethercat_slave_appl_cb_register(data_process_eventHandle, DATA_PROCESS_LAN_EVENT, realtime_ethercat_data_process);
    if (ret != 0)
    {
        printf("ethercat callback register err: %d\r\n", ret);
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
                printf("no msg in tcp rx queue: %d\r\n", stat);
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