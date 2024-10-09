#include "main_app.h"
#include "websocket.h"
#include "init_call.h"
#include "ethercat.h"
#include "tcp_tasks.h"
#include "websocket_console.h"
#include "lan9252_app.h"

#define DATA_PROCESS_LAN_EVENT      (1<<0)
#define DATA_PROCESS_TCP_EVENT      (1<<1)
#define DATA_PROCESS_FPGA_EVENT     (1<<2)
static osEventFlagsId_t data_process_eventHandle = NULL;

static int8_t realtime_ethercat_data_process(void)
{
    TOBJ7010 recv_data = {0};
    TOBJ6000 send_data = {0};

    TOBJ7010 *recv = (TOBJ7010 *)ethercat_recv_data_get((uint16_t *)&recv_data, sizeof(recv_data));
    TOBJ6000 *send = (TOBJ6000 *)ethercat_send_data_get((uint16_t *)&send_data, sizeof(send_data));
    if (recv == NULL || send == NULL)
    {
        printf("ethercat data get failed\r\n");
        return -1;
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

    ret = websocket_cmd_parse(info->sn, info->tcpData, info->length);
    if (ret != 0)
    {
        printf("websocket cmd parse err: %d\r\n", ret);
    }

    /* add other process here */
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
    .stack_size = 1024 * 4,
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