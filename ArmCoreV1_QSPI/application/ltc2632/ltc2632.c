#include "ltc2632.h"
#include "ltc2632_port.h"
#include "cmsis_os2.h"
#include "init_call.h"
#include "ulog.h"

static osMessageQueueId_t ltc2632_queue = NULL;
struct ltc2632_object ltc2632_data = {.value.bits.cmd = 0x02, .out_a_value = 1500, .out_b_value = 1500};

struct ltc2632_object *ltc2632_object_data_get(void)
{
    return &ltc2632_data;
}

int8_t ltc2632_data_write(struct ltc2632_object *buf)
{
    if (buf == NULL)
    {
        LOG_E("buf is NULL\r\n");
        return -1;
    }

    osStatus_t stat = osMessageQueuePut(ltc2632_queue, buf, 0, 0);
    if (stat != osOK)
    {
        LOG_E("ltc2632_data_write err: %d\r\n", stat);
        return -2;
    }

    return 0;
}

uint16_t ltc2632_data_value_get(uint8_t channel)
{
    uint16_t value = 0;
    struct ltc2632_object *ltc2632 = ltc2632_object_data_get();

    switch (channel)
    {
    case LTC2632_CHANNEL_OUTA:
        value = ltc2632->out_a_value;
        break;
    case LTC2632_CHANNEL_OUTB:
        value = ltc2632->out_b_value;
        break;
    default:
        LOG_E("invalid channel: %d\r\n", channel);
        value = 0;
        break;
    }

    return value;
}

static int8_t ltc2632_init(void)
{
    int8_t ret = 0;

    ret = device_ltc2632_init(DEVICE_LTC2632_NAME_DEFAULT);
    if (ret != 0)
    {
        LOG_E("device_ltc2632_init err: %d\r\n", ret);
        return -1;
    }

    ret = device_ltc2632_open();
    if (ret != 0)
    {
        LOG_E("device_ltc2632_open err: %d\r\n", ret);
        return -2;
    }

    return 0;
}

static int8_t ltc2632_process_entry(void *argument)
{
    int8_t ret = 0;

    struct ltc2632_object buf = {0};

    ret = ltc2632_init();
    if (ret != 0)
    {
        LOG_E("ltc2632_init err: %d\r\n", ret);
        return -1;
    }

    for (;;)
    {
        osMessageQueueGet(ltc2632_queue, &buf, 0, osWaitForever);

        ret = device_ltc2632_write(&buf, 1, 1000);
        if (ret != 0)
        {
            LOG_E("device_ltc2632_write err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t ltc2632_thread_init(void)
{
    ltc2632_queue = osMessageQueueNew(32, sizeof(struct ltc2632_object), NULL);
    if (ltc2632_queue == NULL)
    {
        printf("ltc2632_queue create failed\r\n");
        return -1;
    }

    osThreadAttr_t thread_attr = {
    .name = "ltc2632_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityHigh,
    };

    osThreadId_t thread_id = osThreadNew(ltc2632_process_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread ltc2632 create failed\r\n");
        return -2;
    }

    return 0;
}
INIT_APP_EXPORT(ltc2632_thread_init);

#ifdef LTC2632_TEST
#include "shell.h"

static int8_t ltc2632_test(uint8_t argc, char **argv)
{
    struct ltc2632_object *ltc2632 = ltc2632_object_data_get();

    ltc2632->value.bits.addr = atoi(argv[1]);
    ltc2632->value.bits.data = atoi(argv[2]);

    ltc2632_data_write(ltc2632);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(ltc2632_test, ltc2632_test, ltc2632 test);
#endif