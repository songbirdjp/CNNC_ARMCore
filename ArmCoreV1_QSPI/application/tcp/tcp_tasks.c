#include "w5500_port.h"
#include "socket.h"
#include "tcp_tasks.h"
#include <stdbool.h>
#include "init_call.h"
#include <stdio.h>

#define SOCK_TCPS   0

#ifndef IS_TCP_SERVER
static uint8_t remote_ip[4] = {192, 168, 10, 110};
static uint16_t remote_port = 8000;

static uint8_t *remote_ip_get(void)
{
    return remote_ip;
}

static uint16_t remote_port_get(void)
{
    return remote_port;
}
#endif

static wiz_NetInfo local_net_info = {
        .mac = {0x78, 0x83, 0x68, 0x88, 0x56, 0x76},
        .ip =  {192, 168, 10, 76},
        .sn =  {255, 255, 255, 0},
        .gw =  {192, 168, 10, 1},
        .dns = {180, 76, 76, 76},
        .dhcp = NETINFO_DHCP
};

static wiz_NetInfo *local_netinfo_get(void)
{
    return &local_net_info;
}

static TCP_DATA_t recvInfo = {0};
static volatile uint8_t tcp_link_state = false;
static uint8_t tcp_link_status_get(void)
{
    return tcp_link_state;
}

static void (*fun_ptr)(uint8_t sn);

static void tcp_establish_cb(uint8_t sn)
{
    if (fun_ptr != NULL)
    {
        fun_ptr(sn);
    }
}

int8_t tcp_establish_cb_register(void (*fun_cb)(uint8_t sn))
{
    fun_ptr = fun_cb;

    return 0;
}

int8_t tcp_recv_data_callback_register(void (*fun_cb)(void *arg))
{
    return device_w5500_rx_callback_register(fun_cb);
}

uint8_t tcp_socket_state_get(uint8_t sn)
{
    return getSn_SR(sn);
}

#ifdef IS_TCP_SERVER
CLIENT_INFO client[MAX_CLIENT_NUM] = {-1};

static void tcp_server_init(void)
{
    for (uint8_t i = 0; i < MAX_CLIENT_NUM; i++)
    {
        client[i].socketNum = -1;
        client[i].clientType = 0;
    }
}

void clearClientInfo(uint8_t s)
{
    client[s].connectStatus = 0;
    client[s].clientType = 0;
    client[s].socketNum = -1;
    client[s].loopCnt = 0; 
}

static int8_t do_tcp_server_send(uint8_t sn)
{
    int8_t ret = 0;

    switch (getSn_SR(sn))
    {
    case SOCK_INIT:
        ret = listen(sn);
        break;
    case SOCK_ESTABLISHED:
        tcp_establish_cb(sn); // period feedback here
        break;
    case SOCK_CLOSE_WAIT:
        osDelay(500);
        ret = disconnect(sn);
        break;
    case SOCK_CLOSED:
        clearClientInfo(sn);
        ret = socket(sn, Sn_MR_TCP, 80, 0);
        break;
    default:    break;
    }

    return ret;
}
#else 
static int8_t do_tcp_client(uint8_t sn)
{
    int8_t ret = 0;

    switch (getSn_SR(sn))                  /*获取socket的状态*/
    {
        case SOCK_CLOSED:/*socket处于关闭状态*/
            ret = socket(sn, Sn_MR_TCP, 8123, Sn_MR_ND);
            if (ret < 0)
            {
                printf("tcp socket err:%d\r\n", ret);
            }
            break;
            
        case SOCK_INIT:                      /*socket处于初始化状态*/
            ret = connect(sn, remote_ip, remote_port);/*socket连接服务器*/
            if (ret != SOCK_OK)
            {
                printf("tcp connect err:%d\r\n", ret);
            }
            break;

        case SOCK_ESTABLISHED:               /*socket处于连接建立状态*/
            tcp_establish_cb(sn);
            break;

        case SOCK_CLOSE_WAIT:        /*socket处于等待关闭状态*/
            close(sn);
            printf("SOCK_CLOSE_WAIT\r\n");
            break;
    }

    return ret;
}
#endif

static uint8_t socket_num_get(void)
{
    return SOCK_TCPS;
}

static int8_t tcp_init(osMessageQueueId_t queue)
{
    int8_t ret = 0;

    ret = device_w5500_init(local_netinfo_get(), DEVICE_NAME_DEFAULT);
    if (ret != 0)
    {
        printf("device w5500 init err\r\n");
        return ret;
    }

    device_w5500_interrupt_init(MAX_CLIENT_NUM);

    device_w5500_rx_buffer_init(recvInfo.gDATABUF, sizeof(recvInfo.gDATABUF));

    device_w5500_rx_queue_init(queue);

#ifdef IS_TCP_SERVER
    tcp_server_init();
#endif

    return 0;
}

static uint8_t tcp_link_detect(void)
{
    uint8_t ret = device_w5500_phy_link_status_get();
    tcp_link_state =  (ret == PHY_LINK_OFF) ? false : true;
    return tcp_link_state;
}

static int8_t tcp_link_state_recover(void)
{
#ifdef IS_TCP_SERVER
    int8_t ret = 0;
    for (uint8_t sn = 0; sn < MAX_CLIENT_NUM; sn++)
    {
        ret |= device_w5500_link_state_recover(sn);
		clearClientInfo(sn);
    }
    return ret;
#else
    return device_w5500_link_state_recover(socket_num_get());
#endif
}

static int8_t tcp_data_recv_with_block(void)
{
    return device_w5500_data_recv_with_block();
}

/*
 * tcp init
*/

static osMessageQueueId_t tcp_rx_queueHandle = NULL;
static osMutexId_t tcp_access_mutexHandle = NULL;

static void TCPSendTask(void *argument)
{
  /* USER CODE BEGIN TCPSendTask */

    int8_t ret = 0;

    ret = tcp_init(tcp_rx_queueHandle);
    if (ret != 0)
    {
        printf("tcp init err\r\n");
        return;
    }
    /* Infinite loop */
    for(;;)
    {
        osMutexAcquire(tcp_access_mutexHandle, osWaitForever);

        while(tcp_link_detect() == false)
        {
            // printf("tcp link off\r\n");

            tcp_link_state_recover();

            osDelay(100);
        }
 
    #ifdef IS_TCP_SERVER
        for(uint8_t i = 0; i < MAX_CLIENT_NUM; i++)
        {
            ret = do_tcp_server_send(i);
            if (ret < 0)
            {
                printf("do_tcp_server_send err:%d sn = %d\r\n", ret, i);
            }
        }
    #else
        ret = do_tcp_client(socket_num_get());
    #endif
        osMutexRelease(tcp_access_mutexHandle);

        osDelay(1);
    }
  /* USER CODE END TCPSendTask */
}

static void tcp_recv_entry(void *argument)
{
  /* USER CODE BEGIN tcp_recv_entry */
  /* Infinite loop */
  int32_t ret = 0;

  for(;;)
  {
        while(tcp_link_status_get() == false)
        {
            osDelay(100);
        }

        ret = tcp_data_recv_with_block();
        if (ret < 0)
        {
            printf("tcp recv data err:%d\r\n", ret);
        }

        osMutexAcquire(tcp_access_mutexHandle, osWaitForever);

        ret = device_w5500_irq_process();
        if (ret < 0)
        {
            printf("irq process err:%d\r\n", ret);
        }

        osMutexRelease(tcp_access_mutexHandle);

  }
  /* USER CODE END tcp_recv_entry */
}

static int8_t tcp_thread_init(void)
{
    osThreadAttr_t tcp_irq_thread_attributes = {
    .name = "tcp_irq_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };
     osThreadAttr_t tcp_send_attributes = {
    .name = "tcp_send_thread",
    .stack_size = 2048 * 4,
    .priority = (osPriority_t) osPriorityNormal,
    };
    osMutexAttr_t tcp_access_mutex_attributes = {
    .name = "tcp_access_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    osMessageQueueAttr_t tcp_rx_queue_attributes = {
    .name = "tcp_rx_queue"
    };

    tcp_access_mutexHandle = osMutexNew(&tcp_access_mutex_attributes);
    if (tcp_access_mutexHandle == NULL)
    {
        printf("mutex tcp access create failed\r\n");
        return -1;
    }

    tcp_rx_queueHandle = osMessageQueueNew (3, sizeof(TCP_DATA_t), &tcp_rx_queue_attributes);
    if (tcp_rx_queueHandle == NULL)
    {
        printf("queue tcp rx create failed\r\n");
        return -1;
    }

    osThreadId_t tcp_irq_threadHandle = osThreadNew(tcp_recv_entry, NULL, &tcp_irq_thread_attributes);
    if (tcp_irq_threadHandle == NULL)
    {
        printf("thread tcp irq create failed\r\n");
        return -1;
    }
    osThreadId_t tcp_sendHandle = osThreadNew(TCPSendTask, NULL, &tcp_send_attributes);
    if (tcp_sendHandle == NULL)
    {
        printf("thread tcp create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(tcp_thread_init);

osStatus_t tcp_data_recv_get_with_block(TCP_DATA_t *buf, uint32_t timeout)
{
    return osMessageQueueGet(tcp_rx_queueHandle, buf, 0, timeout);
}

int32_t tcp_data_send(uint8_t s, uint8_t *buf, uint16_t len)
{
    osMutexAcquire(tcp_access_mutexHandle, osWaitForever);

    int32_t ret = send(s, buf, len);
    if (ret <= SOCK_BUSY)
    {
        printf("tcp send err:%d\r\n", ret);
    }

    osMutexRelease(tcp_access_mutexHandle);

    return ret;
}