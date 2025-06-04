#include "drv_octospi.h"
#include "lan9252_port.h"
#include "tim.h"
#include "applInterface.h"

static DEVICE_OSPI device_lan9252 = {0};

DEVICE_OSPI *device_lan9252_get(void)
{
    return &device_lan9252;
}

// void TIM2_IRQHandler(void)
// {
//   /* USER CODE BEGIN TIM2_IRQn 0 */

//   /* USER CODE END TIM2_IRQn 0 */
//   HAL_TIM_IRQHandler(&htim2);
//   /* USER CODE BEGIN TIM2_IRQn 1 */

//   /* USER CODE END TIM2_IRQn 1 */
// }

void OCTOSPI1_IRQHandler(void)
{
  /* USER CODE BEGIN OCTOSPI1_IRQn 0 */

  /* USER CODE END OCTOSPI1_IRQn 0 */
  HAL_OSPI_IRQHandler((OSPI_HandleTypeDef *)device_lan9252_get());
  /* USER CODE BEGIN OCTOSPI1_IRQn 1 */

  /* USER CODE END OCTOSPI1_IRQn 1 */
}

#ifdef USING_OSPI_SLAVE_TO_MASTER_INTERRUPT
static void EscIsr_callback(void)
{
    struct node_info
    {
        uint8_t *name;
        DEVICE_IRQ_LIST *node
    }info = {LAN9252_INTN_LINE_NAME, NULL};

    if (device_lan9252_get()->ioctl(device_lan9252_get(), OSPI_CMD_IRQ_NODE_FIND, &info) == 0)
    {
        osEventFlagsSet(info.node->node_data->irq_event, info.node->node_data->irq_event_flag);
    }
}

static void Sync0Isr_callback(void)
{
    struct node_info
    {
        uint8_t *name;
        DEVICE_IRQ_LIST *node
    }info = {LAN9252_SYNC0_IRQ_LINE_NAME, NULL};

    if (device_lan9252_get()->ioctl(device_lan9252_get(), OSPI_CMD_IRQ_NODE_FIND, &info) == 0)
    {
        osEventFlagsSet(info.node->node_data->irq_event, info.node->node_data->irq_event_flag);
    }
}

static void Sync1Isr_callback(void)
{
    struct node_info
    {
        uint8_t *name;
        DEVICE_IRQ_LIST *node
    }info = {LAN9252_SYNC1_IRQ_LINE_NAME, NULL};

    if (device_lan9252_get()->ioctl(device_lan9252_get(), OSPI_CMD_IRQ_NODE_FIND, &info) == 0)
    {
        osEventFlagsSet(info.node->node_data->irq_event, info.node->node_data->irq_event_flag);
    }
}
#endif

#undef USING_OSPI_OPTION_FUNCTION
#ifdef USING_OSPI_OPTION_FUNCTION
static DEVICE_OSPI_OPT device_lan9252_opt = {0};
static int8_t lan9252_opt_before_write(DEVICE_OSPI *spi)
{
    // printf("write: set cs pin low\r\n");
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);

    return 0;
}
static int8_t lan9252_opt_after_write(DEVICE_OSPI *spi)
{
    // printf("after write\r\n");

    return 0;
}
static int8_t lan9252_opt_complete_write(DEVICE_OSPI *spi)
{
    // printf("write: set cs pin high\r\n");
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET);

    return 0;
}
static int8_t lan9252_opt_before_read(DEVICE_OSPI *spi)
{
    // printf("read: set cs pin low\r\n");
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);

    return 0;
}
static int8_t lan9252_opt_after_read(DEVICE_OSPI *spi)
{
    // printf("after read\r\n");

    return 0;
}
static int8_t lan9252_opt_complete_read(DEVICE_OSPI *spi)
{
    // printf("read: set cs pin high\r\n");
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET);

    return 0;
}
static int8_t device_lan9252_opt_init(DEVICE_OSPI *ospi, DEVICE_OSPI_OPT *ospi_opt)
{
    ospi_opt->before_write = lan9252_opt_before_write;
    ospi_opt->after_write = lan9252_opt_after_write;
    ospi_opt->complete_write = lan9252_opt_complete_write;
    ospi_opt->before_read = lan9252_opt_before_read;
    ospi_opt->after_read = lan9252_opt_after_read;
    ospi_opt->complete_read = lan9252_opt_complete_read;

    return ospi->ioctl(ospi, OSPI_CMD_SET_OPT_FUNC, ospi_opt);
}
#endif

#ifdef USING_OSPI_SLAVE_TO_MASTER_INTERRUPT
static int8_t device_lan9252_irq_init(DEVICE_OSPI *ospi, uint8_t *node_name, char splitter)
{
    if (ospi == NULL || node_name == NULL)
    {
        return -1;
    }

    uint8_t name_buf[50] = {0};
    uint8_t *argv[5] = {NULL};
    memcpy(name_buf, node_name, strlen(node_name));
    uint8_t name_num = split_string(name_buf, splitter, argv);

#if 0
    for (uint8_t i = 0; i < name_num; i++)
    {
        printf("node_name:%s\r\n", argv[i]);
    }
#endif

    osEventFlagsAttr_t lan9252_irq_event_attributes = {
    .name = "lan9252_irq_event"
    };

    osEventFlagsId_t irq_event = osEventFlagsNew(&lan9252_irq_event_attributes);
    if (irq_event == NULL)
    {
        printf("device irq event malloc err\r\n");
        return -2;
    }

    IRQ_INFO_NODE *node = NULL;
    int8_t ret = 0;
    for (uint8_t i = 0; i < name_num; i++)
    {
        node = (IRQ_INFO_NODE *)pvPortMalloc(sizeof(IRQ_INFO_NODE));
        if (node == NULL)
        {
            printf("device irq node %s malloc err\r\n", argv[i]);
            return -3;
        }

        memcpy(node->node_name, argv[i], NODE_NAME_LENGTH);
        
        node->irq_event = irq_event;

        switch (i)
        {
            case 0:
                node->irq_pin = LAN9252_INTn_Pin;
                node->irq_event_flag = LAN9252_IRQ_EVENT;
                break;
            case 1:
                node->irq_pin = LAN9252_SYN0_INTn_Pin;
                node->irq_event_flag = LAN9252_SYNC0_IRQ_EVENT;
                break;
            case 2:
                node->irq_pin = LAN9252_SYN1_INTn_Pin;
                node->irq_event_flag = LAN9252_SYNC1_IRQ_EVENT;
                break;
            default:
                printf("device irq node %s err\r\n", argv[i]);
                return -4;
        }

        ret = ospi->ioctl(ospi, OSPI_CMD_IRQ_NODE_ADD, node);
        if (ret != 0)
        {
            printf("device irq node %s add err\r\n", argv[i]);
            return -5;
        }
    }
    
    return 0;
}
#endif


int8_t device_lan9252_init(uint8_t *device_name)
{
    if (device_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

    int8_t ret = 0;

    ret = ospi_init(device_lan9252_get(), device_name);
    if (ret != 0)
    {
        printf("device %s init err:%d\r\n", device_name, ret);
        return ret;
    }

#ifdef USING_OSPI_OPTION_FUNCTION
    ret = device_lan9252_opt_init(device_lan9252_get(), &device_lan9252_opt);
    if (ret != 0)
    {
        printf("device %s opt init err:%d\r\n", device_name, ret);
        return ret;
    }
#endif

#ifdef USING_OSPI_SLAVE_TO_MASTER_INTERRUPT
    ret = device_lan9252_irq_init(device_lan9252_get(), LAN9252_IRQ_LINE_NAME, ' '); /* separate with space for node_name */
    if (ret != 0)
    {
        printf("device %s irq init err:%d\r\n", device_name, ret);
        return ret;
    }

    ret = gpio_pin_irq_callback_register(LAN9252_INTN_NAME, EscIsr_callback);
    ret |= gpio_pin_irq_callback_register(LAN9252_SYNC0_IRQ_NAME, Sync0Isr_callback);
    ret |= gpio_pin_irq_callback_register(LAN9252_SYNC1_IRQ_NAME, Sync1Isr_callback);
    if (ret != 0)
    {
        printf("device %s irq callback register err:%d\r\n", device_name, ret);
        return ret;
    }
#endif

    ret = device_lan9252_get()->open(device_lan9252_get());
    if (ret != 0)
    {
        printf("device %s open err:%d\r\n", device_name, ret);
        return ret;
    }

    HAL_StatusTypeDef status = HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID, ECAT_CheckTimer);
    if (status != HAL_OK)
    {
        printf("device %s tim callback register err:%d\r\n", device_name, status);
        return -2;
    }

    return ret;
}

int8_t device_lan9252_rx_buffer_init(uint8_t *buf, uint16_t len)
{
    struct dma_rx_buf_info
    {
        uint8_t *buf;
        uint16_t len;
    }info = {buf, len};

    printf("buf:%p, len:%d\r\n", buf, len);

    return device_lan9252_get()->ioctl(device_lan9252_get(), OSPI_CMD_SET_DMA_RX_BUF, (void *)&info);
}

int8_t device_lan9252_rx_queue_init(osMessageQueueId_t queue)
{
    return device_lan9252_get()->ioctl(device_lan9252_get(), OSPI_CMD_SET_DMA_RX_QUEUE, queue);
}

int32_t device_lan9252_data_recv_with_block(void)
{
    struct wait_info
    {
        uint8_t *name;
        char splitter;
        uint32_t timeout
    }info = {"irq_line_3 irq_line_13 irq_line_2", ' ', osWaitForever};

    return device_lan9252_get()->ioctl(device_lan9252_get(), OSPI_CMD_IRQ_WAIT_WITH_BLOCK, (void *)&info);
}

int8_t device_lan9252_data_read(uint16_t address, uint8_t *buf, uint32_t num)
{
    OSPI_RegularCmdTypeDef sCommand = {0};

    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction = 0X6B;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;

    sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = HAL_OSPI_ADDRESS_16_BITS;
    sCommand.Address = address;

    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DataMode = HAL_OSPI_DATA_4_LINES;
    sCommand.NbData = num;

    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles = 8;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;

    return device_lan9252_get()->read(device_lan9252_get(), &sCommand, buf, 5000);
}

int8_t device_lan9252_data_write(uint16_t address, uint8_t *buf, uint32_t num)
{
    OSPI_RegularCmdTypeDef sCommand = {0};

    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction = 0X62;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;

    sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = HAL_OSPI_ADDRESS_16_BITS;
    sCommand.Address = address;

    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DataMode = HAL_OSPI_DATA_4_LINES;
    sCommand.NbData = num;

    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;

    return device_lan9252_get()->write(device_lan9252_get(), &sCommand, buf, 5000);
}

int8_t device_lan9252_sqi_mode_set(uint8_t cmd)
{
    OSPI_RegularCmdTypeDef sCommand = {0};

    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction = cmd;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;

    sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;

    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DataMode = HAL_OSPI_DATA_NONE;
    sCommand.NbData = 0;

    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;

    return device_lan9252_get()->write(device_lan9252_get(), &sCommand, NULL, 5000);
}

int8_t device_lan9252_sqi_data_read(uint16_t address, uint8_t *buf, uint32_t num)
{
    OSPI_RegularCmdTypeDef sCommand = {0};

    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction = 0X0B;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_4_LINES;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;

    sCommand.AddressMode = HAL_OSPI_ADDRESS_4_LINES;
    sCommand.AddressSize = HAL_OSPI_ADDRESS_16_BITS;
    sCommand.Address = address;

    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DataMode = HAL_OSPI_DATA_4_LINES;
    sCommand.NbData = num;

    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles = 6;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;

    return device_lan9252_get()->read(device_lan9252_get(), &sCommand, buf, 5000);
}

int8_t device_lan9252_sqi_data_write(uint16_t address, uint8_t *buf, uint32_t num)
{
    OSPI_RegularCmdTypeDef sCommand = {0};

    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction = 0X02;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_4_LINES;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;

    sCommand.AddressMode = HAL_OSPI_ADDRESS_4_LINES;
    sCommand.AddressSize = HAL_OSPI_ADDRESS_16_BITS;
    sCommand.Address = address;

    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DataMode = HAL_OSPI_DATA_4_LINES;
    sCommand.NbData = num;

    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;

    return device_lan9252_get()->write(device_lan9252_get(), &sCommand, buf, 5000);
}