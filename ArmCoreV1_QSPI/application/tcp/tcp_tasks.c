#include "tcp_tasks.h"
#include "w5500_port.h"
#include "init_call.h"
#include "ulog.h"

#define MAX_CLIENT_NUM   (0)

struct socket_info
{
    int8_t sn;
    uint8_t local_type;     /* 0: client, 1: service */
    uint8_t connect_status; /* 0: not connected, 1: connected */
    uint8_t reserved;
    struct
    {
        uint8_t protocol;   /* tcp、udp、raw */
        uint8_t ip[4];
        uint16_t port;
    }net_info;

    uint8_t name[16];       /* socket name for remote */
    int8_t (*callback_period)(uint8_t sn);
};

static struct socket_info socket_info[MAX_SOCKET_NUM] = 
{
    /* client sn must assign by user */
    // [0] = {0, 0, 0, 0, .net_info = {Sn_MR_TCP, {192, 168, 10, 95}, 8123}, TCP_CLIENT_NAME, NULL},
    // [1] = {1, 0, 0, 0, .net_info = {Sn_MR_TCP, {192, 168, 10, 95}, 8125}, TCP_CLIENT_NAME, NULL},

    /* service sn is assigned by w5500 */
    [MAX_CLIENT_NUM + 0] = {-1, 1, 0, 0, .net_info = {Sn_MR_TCP, {0}, 80}, {0}, NULL},
    [MAX_CLIENT_NUM + 1] = {-1, 1, 0, 0, .net_info = {Sn_MR_TCP, {0}, 80}, {0}, NULL},
    [MAX_CLIENT_NUM + 2] = {-1, 1, 0, 0, .net_info = {Sn_MR_TCP, {0}, 80}, {0}, NULL},
    [MAX_CLIENT_NUM + 3] = {-1, 1, 0, 0, .net_info = {Sn_MR_TCP, {0}, 80}, {0}, NULL},
    [MAX_CLIENT_NUM + 4] = {-1, 1, 0, 0, .net_info = {Sn_MR_TCP, {0}, 80}, {0}, NULL},
    [MAX_CLIENT_NUM + 5] = {-1, 1, 0, 0, .net_info = {Sn_MR_TCP, {0}, 80}, {0}, NULL},
    [MAX_CLIENT_NUM + 6] = {-1, 1, 0, 0, .net_info = {Sn_MR_TCP, {0}, 80}, {0}, NULL},
    [MAX_CLIENT_NUM + 7] = {-1, 1, 0, 0, .net_info = {Sn_MR_TCP, {0}, 80}, {0}, NULL},
};

static wiz_NetInfo local_net_info = 
{
    .mac = {0x78, 0x83, 0x68, 0x88, 0x56, 0x70},
    .ip =  {192, 168, 10, 70},
    .sn =  {255, 255, 255, 0},
    .gw =  {192, 168, 10, 1},
    .dns = {180, 76, 76, 76},
    .dhcp = NETINFO_DHCP
};


/*********************************************************************************************************************/
/* private code, user should not modify it */
__attribute__((weak)) int8_t socket_connect_event_cb(uint8_t sn, uint8_t connect_status)
{
    return 0;
}

static int8_t (*callback_period[MAX_SOCKET_NUM])(uint8_t sn) = {NULL};
static int8_t (*callback_period_get(uint8_t *name))(uint8_t sn)
{
    if (memcmp(name, TCP_CLIENT_NAME, strlen(TCP_CLIENT_NAME)) == 0)
    {
        return callback_period[0];
    }
    else if (memcmp(name, TCP_SHELL_NAME, strlen(TCP_SHELL_NAME)) == 0)
    {
        return callback_period[1];
    }
    else if (memcmp(name, TCP_SERVICE_NAME, strlen(TCP_SERVICE_NAME)) == 0)
    {
        return callback_period[2];
    }
    else if (memcmp(name, TCP_CONTROLLER_NAME, strlen(TCP_CONTROLLER_NAME)) == 0)
    {
        return callback_period[3];
    }
    else
    {
        return NULL;
    }
}

static osMessageQueueId_t tcp_rx_queueHandle = NULL;
static osMutexId_t tcp_access_mutexHandle = NULL;
static struct socket_info *socket_info_get(uint8_t sn)
{
    if (sn >= MAX_SOCKET_NUM)
    {
        return NULL;
    }

    return &socket_info[sn];
}
static int8_t socket_connect_process(uint8_t sn)
{
    uint8_t ip[4] = {0};
    uint16_t port = 0;
    struct socket_info *info = NULL;

    getsockopt(sn, SO_DESTIP, ip);
    getsockopt(sn, SO_DESTPORT, &port);

    uint8_t i = 0;
    for (i = 0; i < MAX_SOCKET_NUM; i++)
    {
        info = socket_info_get(i);
        if (info->local_type == 0 && memcmp(ip, info->net_info.ip, sizeof(ip)) == 0 && port == info->net_info.port) /* local is client */
        {
            info->callback_period = callback_period_get(TCP_CLIENT_NAME);
            break;
        }
        else if (info->local_type == 1 && info->sn == sn)
        {
            break;
        }
    }
    if (i == MAX_SOCKET_NUM)
    {
#if 0
        for (i = 0; i < MAX_SOCKET_NUM; i++)
        {
            info = socket_info_get(i);
            if (info->local_type == 1 && info->connect_status == 0 && info->name[0] == 0)
            {
                break;
            }
        }
        if (i == MAX_SOCKET_NUM)
        {
            return -1;
        }
#endif
        return -1;
    }

    info->sn = sn;
    info->connect_status = 1;

    return socket_connect_event_cb(sn, 1);
}
static int8_t socket_disconnect_process(uint8_t sn)
{
    uint8_t ip[4] = {0};
    uint16_t port = 0;
    struct socket_info *info = NULL;

    getsockopt(sn, SO_DESTIP, ip);
    getsockopt(sn, SO_DESTPORT, &port);

    uint8_t i = 0;
    for (i = 0; i < MAX_SOCKET_NUM; i++)
    {
        info = socket_info_get(i);
        if (info->local_type == 0 && memcmp(ip, info->net_info.ip, sizeof(ip)) == 0 && port == info->net_info.port) /* local is client */
        {
            info->sn = sn;
            info->connect_status = 0;
            info->callback_period = NULL;
            break;
        }
        else if (info->local_type == 1 && info->sn == sn)
        {
            info->sn = -1;
            info->connect_status = 0;
            info->callback_period = NULL;
            memset(info->net_info.ip, 0, sizeof(info->net_info.ip));
            memset(info->name, 0, sizeof(info->name));
            break;
        }
    }
    if (i == MAX_SOCKET_NUM)
    {
        return -1;
    }

    return socket_connect_event_cb(sn, 0);
}
static int8_t socket_data_receive_process(uint8_t sn, uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;
    struct tcp_data recv = {0};

    recv.sn = sn;
    recv.len = len;
    memcpy(recv.buf, buf, len);

    ret = osMessageQueuePut(tcp_rx_queueHandle, &recv, 0, 100);
    if (ret != osOK)
    {
        LOG_E("tcp rx queue put err: %d\r\n", ret);
        return ret;
    }

    return 0;
}

static wiz_NetInfo *local_netinfo_get(void)
{
    return &local_net_info;
}

static volatile uint8_t tcp_link_state = PHY_LINK_OFF;
static uint8_t tcp_link_status_get(void)
{
    return tcp_link_state;
}

static void tcp_establish_cb(uint8_t sn)
{
    if (socket_info_get(sn)->callback_period != NULL)
    {
        socket_info_get(sn)->callback_period(sn);
    }
}

int8_t tcp_establish_cb_register(uint8_t *name, int8_t (*fun_cb)(uint8_t sn))
{
    if (memcmp(name, TCP_CLIENT_NAME, strlen(TCP_CLIENT_NAME)) == 0)
    {
        callback_period[0] = fun_cb;
    }
    else if (memcmp(name, TCP_SHELL_NAME, strlen(TCP_SHELL_NAME)) == 0)
    {
        callback_period[1] = fun_cb;
    }
    else if (memcmp(name, TCP_SERVICE_NAME, strlen(TCP_SERVICE_NAME)) == 0)
    {
        callback_period[2] = fun_cb;
    }
    else if (memcmp(name, TCP_CONTROLLER_NAME, strlen(TCP_CONTROLLER_NAME)) == 0)
    {
        callback_period[3] = fun_cb;
    }
    else
    {
        return -1;
    }

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

static int8_t do_tcp_server(uint8_t sn)
{
    int8_t ret = 0;
    struct socket_info *info = socket_info_get(sn);

    switch (getSn_SR(sn))
    {
    case SOCK_CLOSED:
        ret = socket(sn, info->net_info.protocol, info->net_info.port, SF_TCP_NODELAY);
        break;
    case SOCK_INIT:
        ret = listen(sn);
        break;
    case SOCK_ESTABLISHED:
        tcp_establish_cb(sn); // period feedback here
        break;
    case SOCK_CLOSE_WAIT:
        // ret = info->net_info.protocol == Sn_MR_TCP ? disconnect(sn) : close(sn);
        ret = disconnect(sn);
        break;
    default:
        break;
    }

    return ret;
}
static int8_t do_tcp_client(uint8_t sn)
{
    int8_t ret = 0;
    struct socket_info *info = socket_info_get(sn);

    switch (getSn_SR(info->sn))
    {
    case SOCK_CLOSED:
        ret = socket(info->sn, info->net_info.protocol, info->net_info.port, SF_TCP_NODELAY);
        if (ret < 0)
        {
            LOG_E("tcp socket err: %d\r\n", ret);
        }
        break;
    case SOCK_INIT:
        ret = connect(info->sn, info->net_info.ip, info->net_info.port);
        if (ret != SOCK_OK)
        {
            LOG_E("tcp connect err: %d\r\n", ret);
        }
        break;
    case SOCK_ESTABLISHED:
        tcp_establish_cb(info->sn);
        break;
    case SOCK_CLOSE_WAIT:
        ret = info->net_info.protocol == Sn_MR_TCP ? disconnect(info->sn) : close(info->sn);
        break;
    default:
        break;
    }

    return ret;
}


static uint8_t recv_buf[DATA_BUF_SIZE] = {0};
static int8_t tcp_init(osMessageQueueId_t queue)
{
    int8_t ret = 0;

    ret = device_w5500_init(local_netinfo_get(), DEVICE_NAME_DEFAULT);
    if (ret != 0)
    {
        LOG_E("device w5500 init err: %d\r\n", ret);
        return ret;
    }

    device_w5500_interrupt_init(MAX_SOCKET_NUM);

    device_w5500_rx_buffer_init(recv_buf, sizeof(recv_buf));

    device_w5500_rx_queue_init(queue);

    device_w5500_register_interrupt_callback(socket_connect_process, socket_disconnect_process, socket_data_receive_process);

    return 0;
}

static uint8_t tcp_link_detect(void)
{
    uint8_t ret = device_w5500_phy_link_status_get();
    tcp_link_state =  (ret == PHY_LINK_OFF) ? PHY_LINK_OFF : PHY_LINK_ON;
    return tcp_link_state;
}

static int8_t tcp_link_state_recover(void)
{
    int8_t ret = 0;
    for (uint8_t sn = 0; sn < MAX_SOCKET_NUM; sn++)
    {
        ret |= device_w5500_link_state_recover(sn);
    }

    return ret;
}

static int8_t tcp_data_recv_with_block(void)
{
    return device_w5500_data_recv_with_block();
}

static void tcp_send_entry(void *argument)
{
    int8_t ret = 0;

    ret = tcp_init(NULL);
    if (ret != 0)
    {
        LOG_E("tcp init err: %d\r\n", ret);
        osThreadExit();
    }

    for (;;)
    {
        osMutexAcquire(tcp_access_mutexHandle, osWaitForever);

        while(tcp_link_detect() == 0)
        {
            // LOG_E("tcp link off\r\n");

            tcp_link_state_recover();

            osDelay(100);
        }

        for(uint8_t i = 0; i < MAX_SOCKET_NUM; i++)
        {
            ret = i < MAX_CLIENT_NUM ? do_tcp_client(i) : do_tcp_server(i);
            if (ret < 0)
            {
                LOG_E("do tcp sn[%d] err: %d\r\n", i, ret);
            }
        }

        osMutexRelease(tcp_access_mutexHandle);

        osDelay(1);
    }
}

static void tcp_recv_entry(void *argument)
{
    int32_t ret = 0;

    for(;;)
    {
        while(tcp_link_status_get() == 0)
        {
            osDelay(100);
        }

        ret = tcp_data_recv_with_block();
        if (ret < 0)
        {
            LOG_E("tcp data recv with block err: %d\r\n", ret);
        }

        osMutexAcquire(tcp_access_mutexHandle, osWaitForever);
        ret = device_w5500_irq_process();
        if (ret < 0)
        {
            LOG_E("irq process err: %d\r\n", ret);
        }
        osMutexRelease(tcp_access_mutexHandle);
    }
}

static int8_t tcp_thread_init(void)
{
    osMutexAttr_t tcp_access_mutex_attributes = {
        .name = "tcp_access_mutex",
        .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    tcp_access_mutexHandle = osMutexNew(&tcp_access_mutex_attributes);
    if (tcp_access_mutexHandle == NULL)
    {
        printf("mutex tcp access create failed\r\n");
        return -1;
    }

    tcp_rx_queueHandle = osMessageQueueNew (3, sizeof(struct tcp_data), NULL);
    if (tcp_rx_queueHandle == NULL)
    {
        printf("queue tcp rx create failed\r\n");
        return -2;
    }

    osThreadAttr_t tcp_irq_thread_attributes = {
        .name = "tcp_irq_thread",
        .stack_size = 1024 * 4,
        .priority = (osPriority_t) osPriorityAboveNormal,
    };
    osThreadId_t tcp_irq_threadHandle = osThreadNew(tcp_recv_entry, NULL, &tcp_irq_thread_attributes);
    if (tcp_irq_threadHandle == NULL)
    {
        printf("thread tcp irq create failed\r\n");
        return -3;
    }

     osThreadAttr_t tcp_send_attributes = {
        .name = "tcp_send_thread",
        .stack_size = 2048 * 4,
        .priority = (osPriority_t) osPriorityNormal,
    };
    osThreadId_t tcp_sendHandle = osThreadNew(tcp_send_entry, NULL, &tcp_send_attributes);
    if (tcp_sendHandle == NULL)
    {
        printf("thread tcp create failed\r\n");
        return -4;
    }

    return 0;
}
INIT_APP_EXPORT(tcp_thread_init);

osStatus_t tcp_data_recv_get_with_block(struct tcp_data *buf, uint32_t timeout)
{
    return osMessageQueueGet(tcp_rx_queueHandle, buf, 0, timeout);
}

int32_t tcp_data_send(uint8_t s, uint8_t *buf, uint16_t len)
{
    osMutexAcquire(tcp_access_mutexHandle, osWaitForever);

    int32_t ret = send(s, buf, len);
    if (ret <= SOCK_BUSY)
    {
        LOG_E("tcp send err: %d\r\n", ret);
    }

    osMutexRelease(tcp_access_mutexHandle);

    return ret;
}


static struct socket_info *socket_info_get_by_name(uint8_t *name)
{
    struct socket_info *info = NULL;

    uint8_t i = 0;
    for (i = 0; i < MAX_SOCKET_NUM; i++)
    {
        info = socket_info_get(i);
        if (memcmp(info->name, name, strlen(name)) == 0)
        {
            break;
        }
    }
    if (i == MAX_SOCKET_NUM)
    {
        return NULL;
    }

    return info;
}
static struct socket_info *socket_info_get_by_sn(uint8_t sn)
{
    struct socket_info *info = NULL;

    uint8_t i = 0;
    for (i = 0; i < MAX_SOCKET_NUM; i++)
    {
        info = socket_info_get(i);
        if (info->sn == sn)
        {
            break;
        }
    }
    if (i == MAX_SOCKET_NUM)
    {
        return NULL;
    }

    return info;
}

uint8_t *socket_name_get_by_sn(uint8_t sn)
{
    struct socket_info *info = socket_info_get_by_sn(sn);
    if (info == NULL)
    {
        return NULL;
    }

    return (info->name);
}

int8_t socket_server_info_set_by_ws(uint8_t s, uint8_t *name, uint8_t connection, uint8_t multi_enable)
{
    int8_t ret = 0;
    struct socket_info *info = NULL;

    if (connection == 0)    /* disconnect */
    {
        info = socket_info_get_by_sn(s);
        if (info == NULL)
        {
            return -1;
        }

        info->sn = -1;
        info->connect_status = 0;
        info->callback_period = NULL;
        memset(info->net_info.ip, 0, sizeof(info->net_info.ip));
        memset(info->name, 0, sizeof(info->name));

        // info->net_info.protocol == Sn_MR_TCP ? disconnect(s) : close(s);
    }
    else
    {
        info = socket_info_get_by_name(name);
        if (info == NULL)   /* socket name not exist */
        {
            info = socket_info_get_by_sn(s);
            if (info == NULL)
            {
                uint8_t i = 0;
                for (i = 0; i < MAX_SOCKET_NUM; i++)
                {
                    info = socket_info_get(i);
                    if (info->local_type == 1 && info->connect_status == 0) /* search an empty socket info */
                    {
                        break;
                    }
                }
                if (i == MAX_SOCKET_NUM)
                {
                    return -1;
                }
            }
        }
        else
        {
            if (multi_enable == 0)
            {
                if (info->sn != s && info->sn != -1 && info->connect_status == 1)
                {
                    info->net_info.protocol == Sn_MR_TCP ? disconnect(info->sn) : close(info->sn);  /* disconnect old socket */
                }
            }
            else
            {
                if (info->sn != s && info->sn != -1 && info->connect_status == 1)
                {
                    uint8_t i = 0;
                    for (i = 0; i < MAX_SOCKET_NUM; i++)
                    {
                        info = socket_info_get(i);
                        if (info->local_type == 1 && info->connect_status == 0)
                        {
                            break;
                        }
                    }
                    if (i == MAX_SOCKET_NUM)
                    {
                        return -1;
                    }
                }
            }
        }

        info->sn = s;
        info->connect_status = 1;
        info->callback_period = callback_period_get(name);
        memset(info->name, 0, sizeof(info->name));
        memcpy(info->name, name, strlen(name));
        getsockopt(s, SO_DESTIP, info->net_info.ip);
    }

    return 0;
}

#ifndef TCP_TASK_TEST
#include "shell.h"
static int8_t socket_info_output(uint8_t argc, uint8_t **argv)
{
    struct socket_info *info = NULL;

    LOG_I("name\t sn\t local_type\t connect_status\t protocol\t ip\t\t port\r\n");

    for (uint8_t i = 0; i < MAX_SOCKET_NUM; i++)
    {
        info = socket_info_get(i);

        LOG_I("%s\t %d\t     %d\t             %d\t            %d\t         %d.%d.%d.%d\t %d\r\n", 
                info->name, info->sn, info->local_type, info->connect_status, info->net_info.protocol, 
                info->net_info.ip[0], info->net_info.ip[1], info->net_info.ip[2], info->net_info.ip[3], 
                info->net_info.port);
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(socket_info_output, socket_info_output, output socket info);
#endif
