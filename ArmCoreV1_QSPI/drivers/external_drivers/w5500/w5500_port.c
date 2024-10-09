#include "drv_spi.h"
#include "stdbool.h"
#include "socket.h"

static DEVICE_SPI device_w5500 = {0};

DEVICE_SPI *device_w5500_get(void)
{
    return &device_w5500;
}

void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */

  /* USER CODE END SPI1_IRQn 0 */
  HAL_SPI_IRQHandler((SPI_HandleTypeDef *)device_w5500_get());
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}

static void w5500_irq_callback(void)
{
    struct node_info
    {
        uint8_t *name;
        DEVICE_IRQ_LIST *node
    }info = {"irq_line_4", NULL};

    if (device_w5500_get()->ioctl(device_w5500_get(), SPI_CMD_IRQ_NODE_FIND, (void *)&info) == 0)
    {
        osEventFlagsSet(info.node->node_data->irq_event, info.node->node_data->irq_event_flag);
    }
    
}


static void w5500_select(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
}
static void w5500_deselect(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
}
static void w5500_reset_ctrl(uint8_t flag)
{
    if (flag)
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
    }
}
static void w5500_write_dma(uint8_t* buf, uint16_t len)
{
    device_w5500_get()->write(device_w5500_get(), buf, len, 5000);
}
static void w5500_read_dma(uint8_t* buf, uint16_t len)
{
    device_w5500_get()->read(device_w5500_get(), buf, len, 5000);
}
static uint8_t w5500_phy_link_status_get(void)
{
    uint8_t tmp;

    if (ctlwizchip(CW_GET_PHYLINK, (void *)&tmp) == -1)
    {
        printf("get phylink err\n");
        return -1;
    }

    return tmp;
}

static int8_t w5500_chip_init(wiz_NetInfo *net_info)
{
    int8_t ret = 0;

    if (net_info == NULL)
    {
        return -1;
    }

    /* 1. reset chip */
    w5500_reset_ctrl(true);
    osDelay(100);
    w5500_reset_ctrl(false);
    osDelay(100);

    /* 2. register callback function when using spi */
    reg_wizchip_cs_cbfunc(w5500_select, w5500_deselect);
    reg_wizchip_spiburst_cbfunc(w5500_read_dma, w5500_write_dma);

    printf("getVERSIONR:%d\r\n", getVERSIONR());

    /* 3. set tx and rx buffer size */
    uint8_t memsize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2},
                             {2, 2, 2, 2, 2, 2, 2, 2}};
    if (ctlwizchip(CW_INIT_WIZCHIP, (void *) memsize) == -1)
    {
        printf("w5500 chip init err\r\n");
        return -2;
    }

    /* 4. network init */
    ctlnetwork(CN_SET_NETINFO, (void *)net_info);
    
    uint8_t tmpstr[6];
    wiz_NetInfo tempINFO;
    ctlwizchip(CW_GET_ID, (void *)tmpstr);
    ctlnetwork(CN_GET_NETINFO, (void *)&tempINFO);

    printf("chip id:%s\r\n", tmpstr);
    printf("local ip:%d.%d.%d.%d\r\n", tempINFO.ip[0], tempINFO.ip[1], tempINFO.ip[2], tempINFO.ip[3]);

    return 0;
}

#define USING_SENDOK_INTERRUPT
static void W5500_interrupt_init(uint8_t max_interrupt)
{
    uint8_t sn;

    intr_kind int_mask = IK_WOL | IK_PPPOE_TERMINATED | IK_DEST_UNREACH | IK_IP_CONFLICT ;
    for(sn = 0; sn < max_interrupt; sn++)  int_mask |= (1 << (8 + sn));
    ctlwizchip(CW_SET_INTRMASK, (void *)&int_mask);
    
#ifdef USING_SENDOK_INTERRUPT
    setSn_IMR(sn, Sn_IR_SENDOK | Sn_IR_TIMEOUT | Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON); // enable socket 0 all interrupt source
#else
    setSn_IMR(sn, Sn_IR_TIMEOUT | Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON); // enable socket 0 all interrupt source
#endif

    uint16_t intlevel = 2000;
    ctlwizchip(CW_SET_INTRTIME, (void *)&intlevel);

    /*set tcp timeout value*/
    wiz_NetTimeout net_timeout = {1, 16384};
    wizchip_settimeout(&net_timeout);

    for(sn = 0; sn < max_interrupt; sn++)   setSn_KPALVTR(sn, 1);
}

static void W5500_interrupt_status_print(uint8_t sn)
{
    printf("getIMR:%x\r\n", getIMR());
    printf("getSIMR:%x\r\n", getSIMR());
    printf("getSn_IMR:%x\r\n", getSn_IMR(sn));   
    printf("getIR:%x\r\n", getIR());
    printf("getSIR:%x\r\n", getSIR());
    printf("getSn_IR:%x\r\n", getSn_IR(sn));
    
    // if (HAL_GPIO_ReadPin(W5500_INTn_GPIO_Port, W5500_INTn_Pin) == GPIO_PIN_RESET)
    // {
    //     printf("---W5500 INTn pin reset---\r\n");
    // }
}
#include "tcp_tasks.h"
static int32_t w5500_irq_process(void)
{
    uint16_t interrupt_type = 0; // SIR << 8 | IR
    uint8_t reg_ir = 0, reg_sir = 0, reg_sn_ir = 0;
    uint8_t sn = 0;
    uint32_t clr_cnt = 0;
    int32_t recv_ret = 0;
   
    if (ctlwizchip(CW_GET_INTERRUPT, &interrupt_type) != 0)
    {
        printf("W5500 get interrupt err\r\n");
    }

    reg_ir = interrupt_type & 0xFF;
    reg_sir = interrupt_type >> 8;

    if (reg_ir)
    {
        printf("IR interrupt:%x\r\n", reg_ir);
        setIR(reg_ir);
        while(getIR() & reg_ir);
    }

    for (uint8_t i = 0; i < _WIZCHIP_SOCK_NUM_; i++)
    {
        if (reg_sir & (1 << i))
        {
            sn = i;
            break;
        }
    }

    reg_sn_ir = getSn_IR(sn);  

    if (reg_sn_ir) // 清除中断标志位
    {
        // printf("reg_sn_ir:%x\r\n", reg_sn_ir);
        (reg_sn_ir & Sn_IR_CON) ? printf("socket %d: Connected to peer succeed\r\n", sn) : NULL;
        (reg_sn_ir & Sn_IR_DISCON) ? printf("disconnect to peer\r\n") : NULL;
        // (reg_sn_ir & Sn_IR_RECV) ? printf("tcp recv interrupt\r\n") : NULL;
        (reg_sn_ir & Sn_IR_TIMEOUT) ? printf("tcp timeout interrupt\r\n") : NULL;
        // (reg_sn_ir & Sn_IR_SENDOK) ? socket_sending_status_set(socket_sending_status_get() & (~(1<<sn))) : NULL;

        if (reg_sn_ir & Sn_IR_SENDOK)
        {
            uint8_t sending_state = 0;
            ctlsocket(sn, CS_GET_SENDING_STATE, &sending_state);
            sending_state &= ~(1<<sn);
            ctlsocket(sn, CS_SET_SENDING_STATE, &sending_state);
        }

        setSn_IR(sn, reg_sn_ir); // 清除中断标志位

        while(getSn_IR(sn))
        {
            if (++clr_cnt % 5 == 0)
            {
                reg_sn_ir = getSn_IR(sn);
                setSn_IR(sn, reg_sn_ir);
            }
        }
        (clr_cnt != 0) ? printf("clr_cnt: %u\r\n", clr_cnt) : NULL;
        
    }

    DEVICE_SPI *dev = device_w5500_get();
    int32_t recv_len = 0;

    // printf("sn:%d\r\n", sn);

    switch (getSn_SR(sn))                  /*获取socket的状态*/
    {
        case SOCK_CLOSED:/*socket处于关闭状态*/
            break;

        case SOCK_INIT:                      /*socket处于初始化状态*/
            break;

        case SOCK_ESTABLISHED:               /*socket处于连接建立状态*/
            recv_len = getSn_RX_RSR(sn);            /*获取接收的数据长度*/
            if (recv_len > 0)  //接收到数据
            {
                recv_ret = recv(sn, dev->rx_buf, recv_len);     /*接收来自对端的数据*/
                if (recv_ret <= SOCK_BUSY)
                {
                    printf("tcp receive err:%d\r\n", recv_ret);
                    return recv_ret;
                }
               // *(uint16_t *)&dev->rx_buf[dev->rx_buf_len] = recv_len;  /* NOTE: must according to static TCP_DATA_t */
                TCP_DATA_t rxBufTmp = {0};
                rxBufTmp.sn = sn;
                rxBufTmp.Len = recv_len;
                memcpy(rxBufTmp.gDATABUF, dev->rx_buf, recv_len);
                recv_ret = osMessageQueuePut(dev->rx_queue, &rxBufTmp, 0, 100);
                if (recv_ret != osOK)
                {
                    printf("w5500 queue put err:%d\r\n", recv_ret);
                    return recv_ret;
                }
                
                if (dev->rx_cb != NULL)
                {
                    dev->rx_cb(dev);
                }
            }
            break;

        case SOCK_CLOSE_WAIT:        /*socket处于等待关闭状态*/
            break;

        default:
            break;
    }

    return recv_len;
}

/*
 * device relate functions below
*/

#ifdef USING_SPI_OPTION_FUNCTION
static DEVICE_SPI_OPT device_w5500_opt = {0};
static int8_t w5500_opt_before_write(DEVICE_SPI *spi)
{
    // printf("before write\r\n");

    return 0;
}
static int8_t w5500_opt_after_write(DEVICE_SPI *spi)
{
    // printf("after write\r\n");

    return 0;
}
static int8_t w5500_opt_complete_write(DEVICE_SPI *spi)
{
    // printf("complete write\r\n");

    return 0;
}
static int8_t w5500_opt_before_read(DEVICE_SPI *spi)
{
    // printf("before read\r\n");

    return 0;
}
static int8_t w5500_opt_after_read(DEVICE_SPI *spi)
{
    // printf("after read\r\n");

    return 0;
}
static int8_t w5500_opt_complete_read(DEVICE_SPI *spi)
{
    // printf("complete read\r\n");

    return 0;
}
static int8_t device_w5500_opt_init(DEVICE_SPI *spi, DEVICE_SPI_OPT *spi_opt)
{
    spi_opt->before_write = w5500_opt_before_write;
    spi_opt->after_write = w5500_opt_after_write;
    spi_opt->complete_write = w5500_opt_complete_write;
    spi_opt->before_read = w5500_opt_before_read;
    spi_opt->after_read = w5500_opt_after_read;
    spi_opt->complete_read = w5500_opt_complete_read;

    return spi->ioctl(spi, SPI_CMD_SET_OPT_FUNC, (void *)spi_opt);
}
#endif

#ifdef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
#define W5500_INTn_Pin GPIO_PIN_4
#define W5500_IRQ_EVENT      (1<<0)

static int8_t device_w5500_irq_init(DEVICE_SPI *spi, uint8_t *node_name)
{
    IRQ_INFO_NODE *node = (IRQ_INFO_NODE *)pvPortMalloc(sizeof(IRQ_INFO_NODE));
    if (node == NULL)
    {
        printf("device irq node %s malloc err\r\n", node_name);
        return -1;
    }
    
    const osEventFlagsAttr_t w5500_irq_event_attributes = {
    .name = "w5500_irq_event"
    };
    memcpy(node->node_name, node_name, NODE_NAME_LENGTH);
    node->irq_pin = W5500_INTn_Pin;
    node->irq_event = osEventFlagsNew(&w5500_irq_event_attributes);
    node->irq_event_flag = W5500_IRQ_EVENT;

    return spi->ioctl(spi, SPI_CMD_IRQ_NODE_ADD, (void *)node);
}
#endif

int8_t device_w5500_init(wiz_NetInfo *net_info, uint8_t *device_name)
{
    if (net_info == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    int8_t ret = 0;

    ret = spi_init(&device_w5500, device_name, SPI_MASTER);
    if (ret != 0)
    {
        printf("device %s init err:%d\r\n", device_name, ret);
        return ret;
    }

#ifdef USING_SPI_OPTION_FUNCTION
    device_w5500_opt_init(&device_w5500, &device_w5500_opt);
#endif

#ifdef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
    device_w5500_irq_init(&device_w5500, "irq_line_4");
    gpio_pin_irq_callback_register("GPIOD_4", w5500_irq_callback);
#endif

    ret = device_w5500_get()->open(device_w5500_get());
    if (ret != 0)
    {
        printf("device %s open err:%d\r\n", device_name, ret);
        return ret;
    }

    return w5500_chip_init(net_info);;
}

int8_t device_w5500_rx_buffer_init(uint8_t *buf, uint16_t len)
{
    struct dma_rx_buf_info
    {
        uint8_t *buf;
        uint16_t len;
    }info = {buf, len};

    return device_w5500_get()->ioctl(device_w5500_get(), SPI_CMD_SET_DMA_RX_BUF, (void *)&info);
}

int8_t device_w5500_rx_queue_init(osMessageQueueId_t queue)
{
    return device_w5500_get()->ioctl(device_w5500_get(), SPI_CMD_SET_DMA_RX_QUEUE, (void *)queue);
}

int8_t device_w5500_rx_callback_register(void (*callback)(void *arg))
{
    return device_w5500_get()->ioctl(device_w5500_get(), SPI_CMD_SET_RX_CALLBACK, (void *)callback);
}

int8_t device_w5500_interrupt_init(uint8_t max_interrupt)
{
    if (max_interrupt > 8)
    {
        return -1;
    }

    W5500_interrupt_init(max_interrupt);

    return 0;
}

uint8_t device_w5500_phy_link_status_get(void)
{
    return w5500_phy_link_status_get();
}

int8_t device_w5500_link_state_recover(uint8_t sn)
{
    if (sn > 7)
    {
        return -1;
    }

    uint8_t sock_open_status = 0;

    ctlsocket(sn, CS_GET_USE_STATUS, &sock_open_status);

    if (sock_open_status == true)
    {
        close(sn);
        printf("close socket:%d\r\n", sn);
    }

    return 0;
}
int8_t device_w5500_data_recv_with_block(void)
{
    struct wait_info
    {
        uint8_t *name;
        char splitter;
        uint32_t timeout
    }info = {"irq_line_4", ' ', osWaitForever};

    return device_w5500_get()->ioctl(device_w5500_get(), SPI_CMD_IRQ_WAIT_WITH_BLOCK, (void *)&info);
}

int32_t device_w5500_irq_process(void)
{
    return w5500_irq_process();
}
