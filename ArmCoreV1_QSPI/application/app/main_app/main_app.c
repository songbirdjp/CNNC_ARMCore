#include "main_app.h"
#include "websocket.h"
#include "init_call.h"
// #include "ethercat.h"
#include "tcp_tasks.h"
#include "websocket_console.h"
// #include "lan9252_app.h"
// #include "BGM_def.h"
#include "ulog.h"
#include "planData.h"
#define DATA_PROCESS_LAN_EVENT      (1<<0)
#define DATA_PROCESS_TCP_EVENT      (1<<1)
#define DATA_PROCESS_FPGA_EVENT     (1<<2)
static osEventFlagsId_t data_process_eventHandle = NULL;

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
static uint16_t *feedback;
void AFC_ADCSampleDataFeedback(uint8_t socket)
{
    uint8_t read_data[10] = {1,2,3,4,5,6,7,8,9,10};
    //flash->read(flash, WS_AFC_ADC_Flash_addr, read_data, 10*16 * sizeof(uint16_t), 1000);
    // for (int i = 0; i < 8* 16; i++)     
    // {
    //     //LOG_E("wswswsread_data[%d] = %x\r\n", i, read_data[i]);
    // }
    // LOG_E("socket = %d\r\n", socket);
    uint16_t  *pFDAry = feedback;
    *pFDAry++ = 0xFF;//tag
    *pFDAry++ = 0x00;//control
    *pFDAry++ = sizeof(read_data )+ 6; //  length 
    memcpy(pFDAry, read_data, sizeof(read_data));
    pFDAry += 10; // 16个uint16_t移动16步
    ws_send(socket, feedback,sizeof(read_data )+ 6, true, false, WDT_BINDATA);
}
static void tcp_recv_data_process(APP_DATA_RECV *info)
{
    int8_t ret = 0;

    ret = websocket_cmd_parse(info->sn, info->tcpData, info->length);
    if (ret != 0)
    {
        printf("websocket cmd parse err: %d\r\n", ret);
    }
    uint16_t tag = (info->tcpData[1] << 8) + info->tcpData[0];
    LOG_E("111T %x C%x L%x\r\n", tag, (info->tcpData[3] << 8) + info->tcpData[2], (info->tcpData[5] << 8) + info->tcpData[4]);
    switch(tag)
    {
        case 0x31:
            // nrtRecvCommandParse(info);
            // AFC_MagMotorFeedback(info);
            AFC_ADCSampleDataFeedback(info);
        break;
        case 0xFF:
            //LOG_E("222\r\n");
            // planFeedback(info->sn);       
            // AFC_ADCSampleDataFeedback(info);
            break;
        default:      
        break;
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
    // ret = ethercat_slave_appl_cb_register(data_process_eventHandle, DATA_PROCESS_LAN_EVENT, realtime_ethercat_data_process);
    // if (ret != 0)
    // {
    //     printf("ethercat callback register err: %d\r\n", ret);
    //     return ret;
    // }
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
        // if (event_flag & DATA_PROCESS_LAN_EVENT)
        // {
        //     ethercat_recv_data_update_with_block(0);
        // }
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
    .stack_size = 4096 * 4,
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