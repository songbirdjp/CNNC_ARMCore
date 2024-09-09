#include "ltc2632.h"
#include "ltc2632_port.h"
#include "cmsis_os2.h"
#include "init_call.h"

static osMessageQueueId_t ltc2632_queue = NULL;
struct ltc2632_object ltc2632_data = {.value.bits.cmd = 0x02};

struct ltc2632_object *ltc2632_object_data_get(void)
{
    return &ltc2632_data;
}

int8_t ltc2632_data_write(struct ltc2632_object *buf)
{
    if (buf == NULL)
    {
        printf("buf is NULL\r\n");
        return -1;
    }

    osStatus_t stat = osMessageQueuePut(ltc2632_queue, buf, 0, 0);
    if (stat != osOK)
    {
        printf("ltc2632_data_write err: %d\r\n", stat);
        return -2;
    }

    return 0;
}

static int8_t ltc2632_init(void)
{
    int8_t ret = 0;

    ret = device_ltc2632_init(DEVICE_LTC2632_NAME_DEFAULT);
    if (ret != 0)
    {
        printf("device_ltc2632_init err: %d\r\n", ret);
        return -1;
    }

    ret = device_ltc2632_open();
    if (ret != 0)
    {
        printf("device_ltc2632_open err: %d\r\n", ret);
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
        printf("ltc2632_init err: %d\r\n", ret);
        return -1;
    }

    for (;;)
    {
        osMessageQueueGet(ltc2632_queue, &buf, 0, osWaitForever);

        ret = device_ltc2632_write(&buf, 1, 1000);
        if (ret != 0)
        {
            printf("device_ltc2632_write err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t ltc2632_thread_init(void)
{
    ltc2632_queue = osMessageQueueNew(15, sizeof(struct ltc2632_object), NULL);
    if (ltc2632_queue == NULL)
    {
        printf("ltc2632_queue create failed\r\n");
        return -1;
    }

    osThreadAttr_t thread_attr = {
    .name = "ltc2632_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityAboveNormal,
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