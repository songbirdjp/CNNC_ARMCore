#include "drv_uart.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtc.h"

#define CONSOLE_CMD_MAX_LENGTH      128
struct CmdMessage
{
    uint8_t buf[CONSOLE_CMD_MAX_LENGTH];
    uint16_t len;
};

static DEVICE_UART console = {0};

void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler((UART_HandleTypeDef *)&console);

  /* USER CODE BEGIN USART1_IRQn 1 */
  
  /* USER CODE END USART1_IRQn 1 */
}


#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit((UART_HandleTypeDef *)&console, (uint8_t *) &ch, 1, 0xffff);
    return ch;
}

#ifdef USING_UART_OPTION_FUNCTION
static DEVICE_UART_OPT console_opt = {0};
static int8_t console_opt_before_write(DEVICE_UART *uart)
{
    // printf("before write\r\n");

    return 0;
}
static int8_t console_opt_after_write(DEVICE_UART *uart)
{
    // printf("after write\r\n");

    return 0;
}
static int8_t console_opt_complete_write(DEVICE_UART *uart)
{
    // printf("complete write\r\n");

    return 0;
}
static int8_t console_opt_before_read(DEVICE_UART *uart)
{
    // printf("before read\r\n");

    return 0;
}
static int8_t console_opt_after_read(DEVICE_UART *uart)
{
    // printf("after read\r\n");

    return 0;
}
static int8_t console_opt_complete_read(DEVICE_UART *uart)
{
    // printf("complete read\r\n");

    return 0;
}
static int8_t device_console_opt_init(DEVICE_UART *console, DEVICE_UART_OPT *console_opt)
{
    console_opt->before_write = console_opt_before_write;
    console_opt->after_write = console_opt_after_write;
    console_opt->complete_write = console_opt_complete_write;
    console_opt->before_read = console_opt_before_read;
    console_opt->after_read = console_opt_after_read;
    console_opt->complete_read = console_opt_complete_read;

    return uart_opt_init(console, console_opt);
}
#endif

int8_t device_console_init(uint8_t *device_name)
{
#ifdef USING_UART_OPTION_FUNCTION
    device_console_opt_init(&console, &console_opt);
#endif

    osMessageQueueAttr_t CmdQueue_attributes = {
    .name = "uart_rx_queue"
    };
    osMessageQueueId_t CmdQueueHandle = osMessageQueueNew (16, sizeof(struct CmdMessage), &CmdQueue_attributes);
    uart_rx_queue_init(&console, CmdQueueHandle);

    uint8_t *rx_buf = (uint8_t *)pvPortMalloc(sizeof(struct CmdMessage));  /* here should check when use dma mode */
    if (rx_buf == NULL)
    {
        return -1;
    }
    uint16_t rx_buf_len = sizeof(struct CmdMessage) - sizeof(uint16_t); /* indicate rx buf max len */
    uart_dma_rx_buf_init(&console, rx_buf, rx_buf_len);

    int8_t ret = uart_init(&console, device_name);
    if (ret != 0)
    {
        return ret;
    }

    return console.open(&console);
}

int8_t console_cmd_process(void)
{
    struct CmdMessage msg = {0};

    console.read(&console, &msg, osWaitForever);

    finsh_cmd_parse_entry(msg.buf, msg.len - 2);

    // console.write(&console, msg.buf, msg.len, 5000);
 
    return 0;
}

static void cmd_system_reset(uint8_t argc, uint8_t **argv)
{
    HAL_NVIC_SystemReset();
}
MSH_CMD_EXPORT_ALIAS(cmd_system_reset, system_reset, reset system);

static void cmd_thread_info(uint8_t argc, uint8_t **argv)
{
    uint8_t thread_total_num = 20;
    HeapStats_t *heap_stats = (HeapStats_t *)pvPortMalloc(sizeof( HeapStats_t ));
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

    uint8_t *buf = (uint8_t *)pvPortMalloc(thread_total_num * sizeof( TaskStatus_t ));
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
        printf("%x  ", *(uint32_t *)(opt_addr + i * 4));
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

static void cmd_rtc_test(uint8_t argc, uint8_t **argv)
{
    uint32_t bkp_data = 0;

    bkp_data = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
    printf("bkp_data:%x\r\n", bkp_data);

    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0xA5A5A5A5);

    bkp_data = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
    printf("bkp_data:%x\r\n", bkp_data);

    /* backup sram: 4KB */
    printf("bkp sram:%x\r\n", *(__IO uint32_t *)D3_BKPSRAM_BASE);
    *(__IO uint32_t *)D3_BKPSRAM_BASE = 0x88234567;
    printf("bkp sram:%x\r\n", *(__IO uint32_t *)D3_BKPSRAM_BASE);

}
MSH_CMD_EXPORT_ALIAS(cmd_rtc_test, rtc_test, rtc tset);