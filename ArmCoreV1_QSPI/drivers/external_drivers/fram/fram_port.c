#include "fram_port.h"
#include "drv_spi.h"
#include "shell.h"
#include "stdarg.h"
#include "init_call.h"

#ifdef USING_FRAM

/* opcode command */
#define WREN    0x06    /* set write enable */
#define WRDI    0x04    /* reset write enable */
#define RDSR    0x05    /* read status register */
#define WRSR    0x01    /* write status register */
#define READ    0x03    /* read memory data */
#define FSTRD   0x0B    /* fast read memory data */
#define WRITE   0x02    /* write memory data */
#define SLEEP   0xB9    /* enter sleep mode */
#define RDID    0x9F    /* read device id */

/* if use dma mode, must define data buffer in sram d3  */
// static uint8_t opcode_and_addr[4] __attribute__((section(".RAM_D3"))) = {0};
static uint8_t opcode_and_addr_buf[4 + 128] __attribute__((section(".ram_d3"))) = {0};
static uint8_t data_buf[128] __attribute__((section(".ram_d3"))) = {0};

static uint8_t *fram_opcode_and_addr_buf_get(void)
{
    return opcode_and_addr_buf;
}

static uint8_t *fram_data_buf_get(void)
{
    return data_buf;
}

#undef USING_SPI_SLAVE_TO_MASTER_INTERRUPT
static DEVICE_SPI device_fram = {0};
DEVICE_SPI *device_fram_get(void)
{
    return &device_fram;
}

void SPI6_IRQHandler(void)
{
  /* USER CODE BEGIN SPI6_IRQn 0 */

  /* USER CODE END SPI6_IRQn 0 */
  HAL_SPI_IRQHandler((SPI_HandleTypeDef *)device_fram_get());
  /* USER CODE BEGIN SPI6_IRQn 1 */

  /* USER CODE END SPI6_IRQn 1 */
}

#ifdef USING_SPI_OPTION_FUNCTION
static DEVICE_SPI_OPT device_fram_opt = {0};
static int8_t fram_opt_before_write(DEVICE_SPI *spi)
{
    // printf("before write\r\n");
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_RESET);
    return 0;
}
static int8_t fram_opt_after_write(DEVICE_SPI *spi)
{
    // printf("after write\r\n");

    return 0;
}
static int8_t fram_opt_complete_write(DEVICE_SPI *spi)
{
    // printf("complete write\r\n");
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_SET);
    return 0;
}
static int8_t fram_opt_before_read(DEVICE_SPI *spi)
{
    // printf("before read\r\n");
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_RESET);
    return 0;
}
static int8_t fram_opt_after_read(DEVICE_SPI *spi)
{
    // printf("after read\r\n");

    return 0;
}
static int8_t fram_opt_complete_read(DEVICE_SPI *spi)
{
    // printf("complete read\r\n");
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_SET);
    return 0;
}
static int8_t device_fram_opt_init(DEVICE_SPI *spi, DEVICE_SPI_OPT *spi_opt)
{
    spi_opt->before_write = fram_opt_before_write;
    spi_opt->after_write = fram_opt_after_write;
    spi_opt->complete_write = fram_opt_complete_write;
    spi_opt->before_read = fram_opt_before_read;
    spi_opt->after_read = fram_opt_after_read;
    spi_opt->complete_read = fram_opt_complete_read;

    return spi_opt_init(spi, spi_opt);
}
#endif

static int8_t device_fram_write_enable(void)
{
    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();
    opcode_buf[0] = WREN;
    return device_fram_get()->write(device_fram_get(), opcode_buf, 1, 1000);
}
static int8_t device_fram_write_disable(void)
{
    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();
    opcode_buf[0] = WRDI;
    return device_fram_get()->write(device_fram_get(), opcode_buf, 1, 1000);
}
static int8_t device_fram_sr_read(uint8_t *buf)
{
    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();

    if (!buf)
    {
        return -1;
    }

    opcode_buf[0] = RDSR;

    device_fram_get()->write_and_read(device_fram_get(), opcode_buf, data_buf, 2, 1000);

    *buf = data_buf[1];

    return 0;
}
static int8_t device_fram_sr_write(uint8_t *buf)
{
    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();

    if (!buf)
    {
        return -1;
    }

    device_fram_write_enable();

    opcode_buf[0] = WRSR;
    opcode_buf[1] = buf[0];

    return device_fram_get()->write(device_fram_get(), opcode_buf, 2, 1000);
}
static int8_t device_fram_id_read(uint8_t *buf)
{
    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();
    uint8_t *data_buf = fram_data_buf_get();

    if (!buf)
    {
        return -1;
    }

    opcode_buf[0] = RDID;

    device_fram_get()->write_and_read(device_fram_get(), opcode_buf, data_buf, 10, 1000);

    memcpy(buf, &data_buf[1], 9);

    return 0;
}
static int8_t device_fram_sleep_enter(void)
{
    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();

    opcode_buf[0] = SLEEP;

    return device_fram_get()->write(device_fram_get(), opcode_buf, 1, 1000);
}
static int8_t device_fram_sleep_exit(void)
{
    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();

    opcode_buf[0] = WRDI;

    return device_fram_get()->write(device_fram_get(), opcode_buf, 1, 1000);
}

int8_t device_fram_write(uint16_t addr, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    if (addr + len >= FRAM_ADDR_END)
    {
        printf("addr + len must less than 0x%x\r\n", FRAM_ADDR_END);
        return -1;
    }

    if (!buf)
    {
        return -2;
    }

    if (len == 0 || len > 128)
    {
        return -3;
    }

    /* 1. write enable first */
    device_fram_write_enable();

    /* 2. construct opcode and address */
    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();

    opcode_buf[0] = WRITE;
    opcode_buf[1] = addr >> 8;
    opcode_buf[2] = addr;

    memcpy(&opcode_buf[3], buf, len);

    return device_fram_get()->write(device_fram_get(), opcode_buf, 3 + len, timeout);
}

int8_t device_fram_read(uint16_t addr, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    if (addr + len >= FRAM_ADDR_END)
    {
        printf("addr + len must less than 0x%x\r\n", FRAM_ADDR_END);
        return -1;
    }

    if (!buf)
    {
        return -2;
    }

    if (len == 0 || len > 128)
    {
        return -3;
    }

    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();
    uint8_t *data_buf = fram_data_buf_get();

    opcode_buf[0] = READ;
    opcode_buf[1] = addr >> 8;
    opcode_buf[2] = addr;

    device_fram_get()->write_and_read(device_fram_get(), opcode_buf, data_buf, 3 + len, timeout);

    memcpy(buf, &data_buf[3], len);

    return 0;
}

/* fast read mode；SI line is ignored */
int8_t device_fram_fast_read(uint16_t addr, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    if (addr + len >= FRAM_ADDR_END)
    {
        printf("addr + len must less than 0x%x\r\n", FRAM_ADDR_END);
        return -1;
    }

    if (!buf)
    {
        return -2;
    }

    if (len == 0 || len > 128)
    {
        return -3;
    }

    uint8_t *opcode_buf = fram_opcode_and_addr_buf_get();
    uint8_t *data_buf = fram_data_buf_get();

    opcode_buf[0] = FSTRD;
    opcode_buf[1] = addr >> 8;
    opcode_buf[2] = addr;
    opcode_buf[3] = 0x00; /* a dummy data */

    device_fram_get()->write_and_read(device_fram_get(), opcode_buf, data_buf, 4 + len, timeout);

    memcpy(buf, &data_buf[4], len);

    return 0;
}

int8_t device_fram_init(uint8_t *device_name)
{
    int8_t ret = 0;

    if (device_name == NULL)
    {
        printf("ptr is null\r\n");
        return -1;
    }

#ifdef USING_SPI_OPTION_FUNCTION
    ret = device_fram_opt_init(device_fram_get(), &device_fram_opt);
    if (ret != 0)
    {
        printf("device fram opt init err:%d\r\n", ret);
        return ret;
    }
#endif
    
    ret = spi_init(device_fram_get(), device_name, SPI_MASTER);
    if (ret != 0)
    {
        printf("device fram init err:%d\r\n", ret);
        return ret;        
    }

    return device_fram_get()->open(device_fram_get());
}

static int8_t fram_init(void)
{
    return device_fram_init(DEVICE_FRAM_NAME_DEFAULT);
}
INIT_DEVICE_EXPORT(fram_init);

static int8_t device_fram_test(void)
{
    uint8_t data_buf[128] = {0};

    /* 1. sr read test */
    device_fram_sr_read(data_buf);
    printf("initial sr:%.2x\r\n", data_buf[0]);

    /* 2. id read test */
    device_fram_id_read(data_buf);
    printf("device id:");
    for (uint8_t i = 0; i < 9; i++)
    {
        printf("%.2x ", data_buf[i]);
    }
    printf("\r\n");

    /* 3. write enable and disable test */
    device_fram_write_enable();
    device_fram_sr_read(data_buf);
    printf("enable sr:%.2x\r\n", data_buf[0]);

    device_fram_write_disable();
    device_fram_sr_read(data_buf);
    printf("disable sr:%.2x\r\n", data_buf[0]);

    /* 4. write sr test */
    data_buf[0] = 0xFF;
    device_fram_sr_write(data_buf);

    device_fram_sr_read(data_buf);
    printf("write 0xFF sr:%.2x\r\n", data_buf[0]);

    data_buf[0] = 0x00;
    device_fram_sr_write(data_buf);

    device_fram_sr_read(data_buf);
    printf("write 0x00 sr:%.2x\r\n", data_buf[0]);

    /* 5. write and read test */
    device_fram_read(0x0000, data_buf, 16, 1000);
    printf("normal read:");
    for (uint8_t i = 0; i < 16; i++)
    {
        printf("%.2x ", data_buf[i]);
        data_buf[i] = data_buf[i]+1;
    }
    printf("\r\n");

    device_fram_write(0x0000, data_buf, 16, 1000);

    device_fram_read(0x0000, data_buf, 16, 1000);
    printf("normal read:");
    for (uint8_t i = 0; i < 16; i++)
    {
        printf("%.2x ", data_buf[i]);
    }
    printf("\r\n");

    device_fram_fast_read(0x0000, data_buf, 16, 1000);
    printf("fast   read:");
    for (uint8_t i = 0; i < 16; i++)
    {
        printf("%.2x ", data_buf[i]);
    }
    printf("\r\n");

    // /* 6. sleep test */
    device_fram_sleep_enter();
    device_fram_read(0x0000, data_buf, 16, 1000);
    printf("sleep enter read:");
    for (uint8_t i = 0; i < 16; i++)
    {
        printf("%.2x ", data_buf[i]);
    }
    printf("\r\n");

    device_fram_sleep_exit();
    device_fram_read(0x0000, data_buf, 16, 1000);
    printf("sleep exit  read:");
    for (uint8_t i = 0; i < 16; i++)
    {
        printf("%.2x ", data_buf[i]);
    }
    printf("\r\n");
}
MSH_CMD_EXPORT_ALIAS(device_fram_test, fram_test,  fram function test);

struct fram_log
{
    uint16_t log_addr_start;    /* start addr for log record */
    uint16_t log_line_num;      /* valid log num */
    uint16_t log_addr_offset;   /* address to write log, internal use */
};

static struct fram_log fram_log_info __attribute__((section(".ram_d3"))) = {0};  /* at start address, to record log info */

struct fram_log *fram_log_info_get(void)
{
    return &fram_log_info;
}

int8_t fram_log_info_set(uint16_t log_addr_start, uint8_t log_line_num_reset_flag)
{
    struct fram_log *fram_log_info = fram_log_info_get();

    if (log_addr_start < sizeof(struct fram_log))
    {
        printf("log start addr must equal or bigger than %d\r\n", sizeof(struct fram_log));
        return -1;
    }

    if (log_addr_start > FRAM_SIZE - BYTE_LEN_PER_LINE)
    {
        printf("log start addr must less than %d\r\n", FRAM_SIZE - BYTE_LEN_PER_LINE);
        return -2;
    }

    fram_log_info->log_addr_start = log_addr_start;

    if (log_line_num_reset_flag == 1)
    {
        fram_log_info->log_line_num = 0;
        fram_log_info->log_addr_offset = fram_log_info->log_addr_start;

        // memset((uint8_t *)fram_log_info->log_addr_start, '\0', FRAM_SIZE - fram_log_info->log_addr_start + 1);
    }

    return 0;
}

static void fram_addr_offset_update(uint8_t *buf, uint16_t len)
{
    struct fram_log *fram_log_info = fram_log_info_get();

    fram_log_info->log_addr_offset += BYTE_LEN_PER_LINE;

    if (fram_log_info->log_line_num < (FRAM_SIZE - fram_log_info->log_addr_start) / BYTE_LEN_PER_LINE)
    {
        fram_log_info->log_line_num++;
    }
}

int8_t fram_log_write(uint8_t *buf, uint16_t len)
{
    int8_t ret = 0;
    struct fram_log *fram_log_info = fram_log_info_get();

    if (buf == NULL)
    {
        printf("buf is invalid\r\n");
        return -2;
    }

    if (len > BYTE_LEN_PER_LINE)
    {
        printf("write len err:%d\r\n", len);
        return -3;
    }

    if ((fram_log_info->log_addr_offset + BYTE_LEN_PER_LINE) >= FRAM_SIZE)
    {
        fram_log_info->log_addr_offset = fram_log_info->log_addr_start;
        fram_log_info->log_line_num = (FRAM_SIZE - fram_log_info->log_addr_start) / BYTE_LEN_PER_LINE;
    }

    ret = device_fram_write(fram_log_info->log_addr_offset, buf, len, 5000);
    if (ret == 0)
    {
        fram_addr_offset_update(buf, len);
    }

    return ret;
}

int8_t fram_log_console_output(void)
{
    struct fram_log *fram_log_info = fram_log_info_get();
    uint8_t log_buf[BYTE_LEN_PER_LINE] = {0};

    if (fram_log_info->log_line_num == 0)
    {
        printf("there is no valid log\r\n");
        return -1;
    }

    for (uint16_t i = 0; i < fram_log_info->log_line_num; i++)
    {
        device_fram_read(fram_log_info->log_addr_start + BYTE_LEN_PER_LINE * i, log_buf, BYTE_LEN_PER_LINE, 5000);
        printf("%s", log_buf);
    }

    return 0;
}

void fram_log_printf(const char *fmt, ...)
{
    static char log_msg[BYTE_LEN_PER_LINE];
    int ret = 0;

    va_list ap;
    va_start(ap, fmt);

    memset(log_msg, '\0', BYTE_LEN_PER_LINE);
    ret = vsnprintf(log_msg, BYTE_LEN_PER_LINE, fmt, ap);
    va_end(ap);

    if (ret < 0)
    {
        printf("vsnprintf err:%d\r\n", ret);
        return;
    }

    ret = fram_log_write(log_msg, ret + 1); /* write '\0' to the end */
    if (ret != 0)
    {
        printf("fram_log_write err:%d\r\n", ret);
    }
}

void fram_log_test(void)
{
    int8_t ret = 0;

    struct fram_log *fram_log_info = fram_log_info_get();
    printf("addr_start:%x, line_num:%d, addr_offset:%x\r\n", fram_log_info->log_addr_start, fram_log_info->log_line_num, fram_log_info->log_addr_offset);
    
    ret = fram_log_info_set(8, 1);
    if (ret != 0)
    {
        printf("fram_log_info_set err:%d\r\n", ret);
    }

    for (uint16_t i = 0; i < 10; i++)
    {
        fram_log_printf("this is a test demo:%s %s %d\r\n", __DATE__, __TIME__, i);
        fram_log_printf("hello world:%s %s %d\r\n", __DATE__, __TIME__, i);
    }

    ret = fram_log_console_output();
    if (ret != 0)
    {
        printf("fram_log_console_output err:%d\r\n", ret);
    }

}
MSH_CMD_EXPORT_ALIAS(fram_log_test, fram_log_test, fram log record test);

#endif