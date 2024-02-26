#include "drv_octospi.h"
#include "lan9252_port.h"

static DEVICE_OSPI device_lan9252 = {0};

DEVICE_OSPI *device_lan9252_get(void)
{
    return &device_lan9252;
}

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
    IRQ_INFO_NODE *node = device_ospi_irq_node_find(device_lan9252_get(), "irq_line_3")->node_data;
    osEventFlagsSet(node->irq_event, node->irq_event_flag);
}

static void Sync0Isr_callback(void)
{
    IRQ_INFO_NODE *node = device_ospi_irq_node_find(device_lan9252_get(), "irq_line_13")->node_data;
    osEventFlagsSet(node->irq_event, node->irq_event_flag);
}

static void Sync1Isr_callback(void)
{
    IRQ_INFO_NODE *node = device_ospi_irq_node_find(device_lan9252_get(), "irq_line_2")->node_data;
    osEventFlagsSet(node->irq_event, node->irq_event_flag);    
}
#endif

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

    return ospi_opt_init(ospi, ospi_opt);
}
#endif

#ifdef USING_OSPI_SLAVE_TO_MASTER_INTERRUPT

#define LAN9252_INTn_Pin            GPIO_PIN_3
#define LAN9252_SYN0_INTn_Pin       GPIO_PIN_13
#define LAN9252_SYN1_INTn_Pin       GPIO_PIN_2



static int8_t device_lan9252_node_name_get(uint8_t *name, uint8_t **argv)
{
    uint8_t *ptr = name;

    if (name == NULL)
    {
        return -1;
    }

    uint8_t len = strlen(name);
    uint8_t idx = 0, arg_idx = 0;

    while (idx < len)
    {
        while (ptr[idx] != ' ' && idx < len)
        {
            idx++;            
        }

        if (idx >= len)
        {
            break;
        }

        argv[arg_idx++] = ptr;
    }


    return 0;
}

static int8_t device_lan9252_irq_init(DEVICE_OSPI *ospi, uint8_t *node_name)
{
    osEventFlagsAttr_t lan9252_irq_event_attributes = {
    .name = "lan9252_irq_event"
    };

    osEventFlagsId_t irq_event = NULL;

    IRQ_INFO_NODE *node = (IRQ_INFO_NODE *)pvPortMalloc(sizeof(IRQ_INFO_NODE));
    if (node == NULL)
    {
        printf("device irq node %s malloc err\r\n", node_name);
        return -1;
    }
    memcpy(node->node_name, node_name, NODE_NAME_LENGTH);
    node->irq_pin = LAN9252_INTn_Pin;
    irq_event = osEventFlagsNew(&lan9252_irq_event_attributes);
    node->irq_event = irq_event;
    node->irq_event_flag = LAN9252_IRQ_EVENT;
    device_ospi_irq_node_add(&device_lan9252, node);

    node = (IRQ_INFO_NODE *)pvPortMalloc(sizeof(IRQ_INFO_NODE));
    if (node == NULL)
    {
        printf("device irq node %s malloc err\r\n", node_name);
        return -2;
    }    
    memcpy(node->node_name, "irq_line_13", NODE_NAME_LENGTH);
    node->irq_pin = LAN9252_SYN0_INTn_Pin;
    node->irq_event = irq_event;
    node->irq_event_flag = LAN9252_SYNC0_IRQ_EVENT;
    device_ospi_irq_node_add(&device_lan9252, node);

    node = (IRQ_INFO_NODE *)pvPortMalloc(sizeof(IRQ_INFO_NODE));
    if (node == NULL)
    {
        printf("device irq node %s malloc err\r\n", node_name);
        return -3;
    }    
    memcpy(node->node_name, "irq_line_2", NODE_NAME_LENGTH);
    node->irq_pin = LAN9252_SYN1_INTn_Pin;
    node->irq_event = irq_event;
    node->irq_event_flag = LAN9252_SYNC1_IRQ_EVENT;
    
    return device_ospi_irq_node_add(&device_lan9252, node);
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

#ifdef USING_OSPI_OPTION_FUNCTION
    device_lan9252_opt_init(device_lan9252_get(), &device_lan9252_opt);
#endif

#ifdef USING_OSPI_SLAVE_TO_MASTER_INTERRUPT
    device_lan9252_irq_init(device_lan9252_get(), "irq_line_3"); /* separate with space for node_name*/

    gpio_pin_irq_callback_register("GPIOE_3", EscIsr_callback);
    gpio_pin_irq_callback_register("GPIOC_13", Sync0Isr_callback);
    gpio_pin_irq_callback_register("GPIOB_2", Sync1Isr_callback);
#endif

    ret = ospi_init(device_lan9252_get(), device_name);
    if (ret != 0)
    {
        printf("device %s init err:%d\r\n", device_name, ret);
        return ret;
    }

    ret = device_lan9252_get()->open(device_lan9252_get());
    if (ret != 0)
    {
        printf("device %s open err:%d\r\n", device_name, ret);
        return ret;
    }

    return ret;
}

int8_t device_lan9252_rx_buffer_init(uint8_t *buf, uint16_t len)
{
    return ospi_dma_rx_buf_init(device_lan9252_get(), buf, len);
}

int8_t device_lan9252_rx_queue_init(osMessageQueueId_t queue, int8_t (*cb)(void *arg))
{
    return ospi_rx_queue_init(device_lan9252_get(), queue, cb);
}

int32_t device_lan9252_data_recv_with_block(void)
{
    return device_ospi_irq_wait_with_block(device_lan9252_get(), "irq_line_3 irq_line_13 irq_line_2", ' ', osWaitForever);
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