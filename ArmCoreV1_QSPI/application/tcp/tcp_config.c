#include "tcp_config.h"
#include "w5500_port.h"
#include "socket.h"
#include "nonRealtimeDataProcess.h"

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
uint8_t tcp_link_status(void)
{
    return tcp_link_state;
}

int8_t do_tcp_client(uint8_t sn)
{
    int8_t ret = 0;

    switch (getSn_SR(sn))                  /*获取socket的状态*/
    {
        case SOCK_CLOSED:/*socket处于关闭状态*/
            ret = socket(0, Sn_MR_TCP, 8123, Sn_MR_ND);
            if (ret != 0)
            {
                printf("tcp socket err:%d\r\n", ret);
            }
            break;
        case SOCK_INIT:                      /*socket处于初始化状态*/
            ret = connect(0, remote_ip, remote_port);/*socket连接服务器*/
            if (ret != SOCK_OK)
            {
                printf("tcp connect err:%d\r\n", ret);
            }
            break;
        case SOCK_ESTABLISHED:               /*socket处于连接建立状态*/
            if(beam_cmd_get() == NO_USE)
            {
                sendFeedback();
            }
            
            break;
        case SOCK_CLOSE_WAIT:        /*socket处于等待关闭状态*/
            close(0);
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

uint8_t socket_num_get(void)
{
    return SOCK_TCPS;
}

int8_t tcp_init(osMessageQueueId_t queue)
{
    int8_t ret = 0;

    ret = device_w5500_init(local_netinfo_get());
    if (ret != 0)
    {
        printf("device w5500 init err\r\n");
        return ret;
    }

    device_w5500_interrupt_init(socket_num_get());

    device_w5500_rx_buffer_init(recvInfo.gDATABUF, sizeof(recvInfo.gDATABUF));

    device_w5500_rx_queue_init(queue);

    TCPFeedbackInit();

    return 0;
}

uint8_t tcp_link_detect(void)
{
    uint8_t ret = device_w5500_phy_link_status_get();
    tcp_link_state =  (ret == PHY_LINK_OFF) ? false : true;
    return tcp_link_state;
}

int8_t tcp_link_state_recover(void)
{
    return device_w5500_link_state_recover(socket_num_get());
}

int32_t tcp_data_recv_with_block(void)
{
    return device_w5500_data_recv_with_block();
}