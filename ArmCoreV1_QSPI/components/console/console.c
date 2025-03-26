#include "console.h"
#include "dev_uart.h"
#include "drv_uart.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "shell.h"
#include "init_call.h"
#include "ulog.h"

#ifndef USING_COM1_FOR_YMODEM
#define CONSOLE_CMD_MAX_LENGTH 128
#else
#define CONSOLE_CMD_MAX_LENGTH 1029
#endif
struct CmdMessage
{
    uint8_t buf[CONSOLE_CMD_MAX_LENGTH];
    uint16_t len;
};

static uart_dev_t *console = NULL;

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE __attribute__((weak)) int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE __attribute__((weak)) int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}

int8_t device_console_init(void)
{
    drv_console_init();
    console = device_uart_find(UART_DEV_NAME_CONSOLE);

    device_err_t ret = dev_uart_init(console,
                                     DEV_UART_IOCTL_USE_DMA,
                                     5,
                                     sizeof(struct CmdMessage));
    if (ret != 0)
    {
        return -1;
    }

    return 0;
}

static int8_t device_console_write(uint8_t *buf, uint16_t len)
{
    return dev_uart_send(console, buf, len, 1000);
}

static int8_t console_log_init(void)
{
#ifdef USING_ULOG_CONSOLE
    struct ulog_write_func_info info = {
        .func_init = NULL,
        .func_callback = device_console_write,
        .index = 0,
        .level = ULOG_INFO_LEVEL};

    int8_t ret = ulog_write_func_register(&info);
    if (ret != 0)
    {
        printf("console log register err:%d\r\n", ret);
        return ret;
    }
#endif

    return 0;
}
INIT_COMPONENT_EXPORT(console_log_init);

static int8_t console_cmd_process(void)
{
    struct CmdMessage msg = {0};

    device_err_t ret = dev_uart_recv(console, msg.buf, sizeof(struct CmdMessage) , osWaitForever);
    if (ret != 0)
    {
        return -1;
    }

    if (msg.len <= 2)
    {
        LOG_E("cmd len must more than 2\r\n");
        return -2;
    }

    msg.buf[msg.len - 2] = 0;

    shell_cmd_parse_entry(msg.buf, msg.len - 2);

    return 0;
}

#ifdef USING_COM1_FOR_YMODEM
static enum com1_mode com_mode = CONSOLE_MODE;
enum com1_mode *com_mode_get(void)
{
    return &com_mode;
}

int8_t ymodem_data_read(uint8_t *buf, uint16_t *len, uint32_t timeout)
{
    if (buf == NULL || len == NULL)
    {
        return -1;
    }

    struct CmdMessage msg = {0};

    device_err_t ret = dev_uart_recv(console, msg.buf, sizeof(struct CmdMessage) , timeout);
    if (ret != 0)
    {
        return -1;
    }

    *len = msg.len;
    memcpy(buf, msg.buf, msg.len);

    return 0;
}

int8_t ymodem_data_write(uint8_t *buf, uint16_t len, uint32_t timeout)
{
    if (buf == NULL || len == NULL)
    {
        return -1;
    }
    device_err_t ret = dev_uart_send(console, buf, len, timeout);
    if (ret != 0)
    {
        return -1;
    }

    return 0;
}
#endif

static void StartConsoleTask(void *argument)
{
    /* USER CODE BEGIN StartConsoleTask */
    /* Infinite loop */
#ifdef USING_COM1_FOR_YMODEM
    enum com1_mode mode = CONSOLE_MODE;
#endif

    for (;;)
    {

#ifdef USING_COM1_FOR_YMODEM
        mode = *com_mode_get();
        if (mode == YMODEM_MODE)
        {
            osDelay(1000);
        }
        else if (mode == CONSOLE_MODE)
#endif
        {
            console_cmd_process();
        }
    }
    /* USER CODE END StartConsoleTask */
}

static int8_t console_thread_init(uint8_t argc, uint8_t **argv)
{
    osThreadAttr_t Console_attributes = {
        .name = "Console",
        .stack_size = 1024 * 4,
        .priority = (osPriority_t)osPriorityLow,
    };

    osThreadId_t ConsoleHandle = osThreadNew(StartConsoleTask, NULL, &Console_attributes);
    if (ConsoleHandle == NULL)
    {
        return -1;
    }

    return 0;
}
INIT_COMPONENT_EXPORT(console_thread_init);

static int8_t cmd_help(uint8_t argc, uint8_t **argv)
{
    extern uint32_t __shell_cmd_start;
    extern uint32_t __shell_cmd_end;

    const struct shell_cmd_desc *desc;

    LOG_I("shell commands:\r\n");

    for (desc = &__shell_cmd_start; desc < &__shell_cmd_end; desc++)
    {
        LOG_I("%-32s - %s\r\n", desc->name, desc->desc);
    }

    LOG_I("\r\n");

    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_help, help, shell help);

static void cmd_system_reset(uint8_t argc, uint8_t **argv)
{
    HAL_NVIC_SystemReset();
}
MSH_CMD_EXPORT_ALIAS(cmd_system_reset, system_reset, reset system);

static void cmd_thread_info(uint8_t argc, uint8_t **argv)
{
    uint8_t thread_total_num = uxTaskGetNumberOfTasks();
    HeapStats_t *heap_stats = (HeapStats_t *)pvPortMalloc(sizeof(HeapStats_t));
    if (heap_stats == NULL)
    {
        printf("heap stats malloc failed\r\n");
        return;
    }

    vPortGetHeapStats(heap_stats);

    printf("\r\n***************************************************************\r\n");

    printf("Heap Stats:\r\n");
    printf("allocation_success: %d\r\n", heap_stats->xNumberOfSuccessfulAllocations);
    printf("free_success: %d\r\n", heap_stats->xNumberOfSuccessfulFrees);
    printf("free_blocks: %d\r\n", heap_stats->xNumberOfFreeBlocks);
    printf("bytes_of_largest_block: %d\r\n", heap_stats->xSizeOfLargestFreeBlockInBytes);
    printf("bytes_of_smallest_block: %d\r\n", heap_stats->xSizeOfSmallestFreeBlockInBytes);
    printf("bytes_of_available: %d\r\n", heap_stats->xAvailableHeapSpaceInBytes);
    printf("bytes_remaining_ever: %d\r\n", heap_stats->xMinimumEverFreeBytesRemaining);

    printf("---------------------------------------------------------------\r\n");

    uint8_t *buf = (uint8_t *)pvPortMalloc(thread_total_num * (configMAX_TASK_NAME_LEN + 15));
    if (buf == NULL)
    {
        printf("buf malloc failed\r\n");
        vPortFree(heap_stats);
        return;
    }

    // taskENTER_CRITICAL();
    vTaskList(buf);
    // taskEXIT_CRITICAL();

    printf("task_name   task_status	task_priority	stack_left	task_num\r\n");
    printf("%s", buf);

    printf("---------------------------------------------------------------\r\n");

    // taskENTER_CRITICAL();
    vTaskGetRunTimeStats(buf);
    // taskEXIT_CRITICAL();

    printf("task_name       run_time        cpu_usage\r\n");
    printf("%s", buf);

    printf("***************************************************************\r\n");

    vPortFree(heap_stats);
    vPortFree(buf);
}
MSH_CMD_EXPORT_ALIAS(cmd_thread_info, thread_info, thread status);

static void cmd_mem_read(uint8_t argc, uint8_t **argv)
{
    uint32_t opt_addr = 0;
    uint8_t read_num = 0;
    char *end_ptr;

    if (argc < 3)
    {
        printf("argv too few\r\n");
        return;
    }

    /* compare address */
    opt_addr = strtoul(argv[1], &end_ptr, 0);
    read_num = strtoul(argv[2], &end_ptr, 0);

    for (uint8_t i = 0; i < read_num; i++)
    {
        printf("%.8x  ", *(uint32_t *)(opt_addr + i * 4));
    }
    printf("\r\n");
}
MSH_CMD_EXPORT_ALIAS(cmd_mem_read, mem_read, read memory);

static void cmd_mem_write(uint8_t argc, uint8_t **argv)
{
    uint32_t opt_addr = 0;
    char *end_ptr;

    if (argc < 3)
    {
        printf("argv too few\r\n");
        return;
    }

    /* compare address */
    opt_addr = strtoul(argv[1], &end_ptr, 0);

    for (uint8_t i = 0; i < argc - 2; i++)
    {
        *(uint32_t *)(opt_addr + i * 4) = strtoul(argv[i + 2], &end_ptr, 0);
    }
}
MSH_CMD_EXPORT_ALIAS(cmd_mem_write, mem_write, write memory);
