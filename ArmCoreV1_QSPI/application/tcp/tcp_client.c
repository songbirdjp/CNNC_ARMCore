#include "w5500_port.h"
#include "socket.h"
#include "tcp_client.h"
#include "stdbool.h"
#include "init_call.h"
#include "main.h"

#define SOCK_TCPS   0

static uint8_t remote_ip[4] = {192, 168, 10, 100};
static uint16_t remote_port = 8000;

static wiz_NetInfo local_net_info = {
#ifdef BANKA
        .mac = {0x78, 0x83, 0x68, 0x88, 0x56, 0x72},
        .ip =  {192, 168, 10, 71},
#else
        .mac = {0x78, 0x83, 0x68, 0x88, 0x56, 0x71},
        .ip =  {192, 168, 10, 72},
#endif
        .sn =  {255, 255, 255, 0},
        .gw =  {192, 168, 0, 1},
        .dns = {180, 76, 76, 76},
        .dhcp = NETINFO_DHCP
};

static wiz_NetInfo *local_netinfo_get(void)
{
    return &local_net_info;
}

static uint8_t *remote_ip_get(void)
{
    return remote_ip;
}

static uint16_t *remote_port_get(void)
{
    return &remote_port;
}


static TCP_DATA_t recvInfo = {0};

static volatile uint8_t tcp_link_state = false;
static uint8_t tcp_link_status_get(void)
{
    return tcp_link_state;
}

static void (*fun_ptr)(void);

static void tcp_establish_cb(void)
{
    if (fun_ptr != NULL)
    {
        fun_ptr();
    }
}

int8_t tcp_establish_cb_register(void (*fun_cb)(void))
{
    fun_ptr = fun_cb;

    return 0;
}

int8_t tcp_recv_data_callback_register(void (*fun_cb)(void *arg))
{
    return device_w5500_rx_callback_register(fun_cb);
}

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
            tcp_establish_cb();
            break;

        case SOCK_CLOSE_WAIT:        /*socket处于等待关闭状态*/
            close(sn);
            printf("SOCK_CLOSE_WAIT\r\n");
            break;
    }

    return ret;
}
#if 0
void do_tcp_server(void)
{
    uint8_t flag = 0;
    uint16_t len;
    switch (getSn_SR(SOCK_TCPS))
    {
        case SOCK_INIT:
            listen(SOCK_TCPS);
            printf("SERVER_SOCK_INIT\r\n");
            break;
        case SOCK_ESTABLISHED:
            if (getSn_IR(SOCK_TCPS) & Sn_IR_CON)
            {
                setSn_IR(SOCK_TCPS, Sn_IR_CON);
            }

            len = 4096;//TODO 这里是个坑，getSn_RX_RSR(SOCK_TCPS)取数总是问题，目前把长度写成4096各方面都是正常的，如果有问题先查这里。
            memset(gDATABUF, 0, sizeof(gDATABUF));
            recv(SOCK_TCPS, gDATABUF, len);
            if (len)
            {
                // 解析HTTP请求
                int http_request_type = parse_http_request(gDATABUF);
                printf("%s\r\n",gDATABUF);
                // 根据HTTP请求的类型，发送HTTP响应
                if (http_request_type == 1)
                {
                    printf("http_request_type = %x\r\n", http_request_type);
                    if(Get_IO_Flag(gDATABUF) == 1)//抓取HTTP响应头是否有io-state标志
                    {
                        handleIOStatusRequest(SOCK_TCPS);
                    }
                    else//没有io位即正常响应标准页面
                    {
                        SendHttpResponse_Get(SOCK_TCPS);
                    }

                }
                else if (http_request_type == 2)
                {
                    printf("http_request_type22222 = %x\r\n", http_request_type);

                    handle_request(gDATABUF);
                }
                else if (http_request_type == 4)
                {
                    // 处理DELETE请求的响应
                    // ...
                }
                else if (http_request_type == 5)
                {
                    // 处理HEAD请求的响应
                    // ...
                }
                else if (http_request_type == 6)
                {
                    // 处理OPTIONS请求的响应
                    // ...

                }
                else if (http_request_type == 7)
                {
                    // 处理PATCH请求的响应
                    // ...
                }
                else if (http_request_type == 8)
                {
                    // 处理TRACE请求的响应
                    // ...
                }
                else if (http_request_type == 9)
                {
                    // 处理CONNECT请求的响应
                    // ...
                }
                else if(http_request_type == 10)
                {
                    char httpResponse[] = "HTTP/1.1 Option type\r\n\r\n";
                    send(SOCK_TCPS, (uint8_t *) httpResponse, sizeof(httpResponse));
                    printf("%s\r\n",gDATABUF);
                    http_String2Numbers(gDATABUF);
                }
                else
                {
                    // 对于我们不支持的HTTP请求，返回"404 Not Found"错误
                    char httpResponse[] = "HTTP/1.1 404 Not Found\r\n\r\n";
                    send(SOCK_TCPS, (uint8_t *) httpResponse, sizeof(httpResponse));
                }
            }
            printf("SERVER_SOCKSOCK_ESTABLISHED\r\n");
            disconnect(SOCK_TCPS);
            break;
        case SOCK_CLOSE_WAIT:
            disconnect(SOCK_TCPS);
            printf("SERVER_SOCK_CLOSED_WAIT\r\n");
            break;
        case SOCK_CLOSED:
            socket(SOCK_TCPS, Sn_MR_TCP, 5000, 0x00);
            printf("SERVER_SOCK_CLOSED\r\n");
            break;
    }
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

    device_w5500_interrupt_init(socket_num_get());

    device_w5500_rx_buffer_init(recvInfo.gDATABUF, sizeof(recvInfo.gDATABUF));

    device_w5500_rx_queue_init(queue);

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
    return device_w5500_link_state_recover(socket_num_get());
}

static int32_t tcp_data_recv_with_block(void)
{
    return device_w5500_data_recv_with_block();
}

/*
 * tcp client init
*/

static osMessageQueueId_t tcp_rx_queueHandle = NULL;
static osMutexId_t tcp_access_mutexHandle = NULL;

static void TCPClientTask(void *argument)
{
  /* USER CODE BEGIN TCPClientTask */

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

        ret = do_tcp_client(socket_num_get());
        if (ret != 0)
        {
            printf("do_tcp_client err:%d\r\n", ret);
        }

        osMutexRelease(tcp_access_mutexHandle);

        osDelay(100);
    }
  /* USER CODE END TCPClientTask */
}

static void tcp_client_entry(void *argument)
{
  /* USER CODE BEGIN tcp_client_entry */
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
  /* USER CODE END tcp_client_entry */
}

static int8_t tcp_client_thread_init(void)
{
    osThreadAttr_t tcp_irq_thread_attributes = {
    .name = "tcp_irq_thread",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };
    osThreadAttr_t TCPClient_attributes = {
    .name = "TCPClient",
    .stack_size = 1024 * 4,
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

    osThreadId_t tcp_irq_threadHandle = osThreadNew(tcp_client_entry, NULL, &tcp_irq_thread_attributes);
    if (tcp_irq_threadHandle == NULL)
    {
        printf("thread tcp irq create failed\r\n");
        return -1;
    }

    osThreadId_t TCPClientHandle = osThreadNew(TCPClientTask, NULL, &TCPClient_attributes);
    if (TCPClientHandle == NULL)
    {
        printf("thread tcp client create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(tcp_client_thread_init);

osStatus_t tcp_client_data_recv_get(TCP_DATA_t *buf)
{
    return osMessageQueueGet(tcp_rx_queueHandle, buf, 0, 0);
}

int32_t tcp_client_data_send(uint8_t *buf, uint16_t len)
{
    osMutexAcquire(tcp_access_mutexHandle, osWaitForever);

    int32_t ret = send(socket_num_get(), buf, len);
    if (ret <= SOCK_BUSY)
    {
        printf("tcp client send err:%d\r\n", ret);
    }

    osMutexRelease(tcp_access_mutexHandle);

    return ret;
}