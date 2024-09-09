#include "dose_uart.h"
#include "dose_uart_port.h"
#include "init_call.h"
#include "cmsis_os2.h"

static int8_t dose_handshake_frame_parse(struct dose_object *cmd)
{
    int8_t ret = 0;

    return ret;
}

static int8_t dose_command_frame_parse(struct dose_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])   /* first cmd */
    {
    case 0x40:
        break;
    case 0x41:
        break;
    case 0x42:
        break;
    default:
        break;
    }

    return ret;
}

static int8_t dose_realtime_frame_parse(struct dose_object *cmd)
{
    int8_t ret = 0;

    return ret;
}

static int8_t dose_cmd_parse(struct dose_object *cmd)
{
    if (cmd == NULL)
    {
        printf("cmd is NULL\r\n");
        return -1;
    }

    /* 1. check cmd id */
    if (cmd->id.bits.cmd_id != DOSE_UART_ID)
    {
        return 0;
    }

    int8_t ret = 0;

    /* 2. parse cmd type */
    switch (cmd->type)
    {
    case 0x01:  /* handshake frame */
        ret = dose_handshake_frame_parse(cmd);
        if (ret != 0)
        {
            printf("dose_uart_handshake_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x02:  /* command frame */
        ret = dose_command_frame_parse(cmd);
        if (ret != 0)
        {
            printf("dose_command_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x03:  /* realtime frame */
        ret = dose_realtime_frame_parse(cmd);
        if (ret != 0)
        {
            printf("dose_uart_realtime_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    default:
        printf("invalid cmd type: %d\r\n", cmd->type);
        return -2;
        break;
    }

    if (cmd->id.bits.cmd_ack != 0)  /* need ack */
    {
        cmd->id.bits.cmd_ack = 0;

        ret = dose_uart_cmd_write(cmd);
        if (ret != 0)
        {
            printf("dose_uart_cmd_write err: %d\r\n", ret);
        }
    }

    return ret;
}

osMessageQueueId_t dose_uart_send_queue = NULL;
int8_t dose_uart_cmd_write(struct dose_object *cmd)
{
    osStatus_t stat = osOK;
    struct dose_uart send_buf = {0};
    uint8_t offset = sizeof(struct dose_object) - sizeof(uint8_t *);

    memcpy(send_buf.buf, cmd, sizeof(struct dose_object));
    memcpy(&send_buf.buf[offset], cmd->data, cmd->len);
    send_buf.len = offset + cmd->len;

    stat = osMessageQueuePut(dose_uart_send_queue, &send_buf, 0, 0);
    if (stat != osOK)
    {
        printf("dose uart send queue put err: %d\r\n", stat);
        return -1;
    }

    return 0;
}

static int8_t dose_uart_send_entry(void *argument)
{
    int8_t ret = 0;
    struct dose_uart send_buf = {0};

    for (;;)
    {
        osMessageQueueGet(dose_uart_send_queue, &send_buf, NULL, osWaitForever);

#if 0
        printf("send_buf len: %d\r\n", send_buf.len);
        for (uint8_t i = 0; i < send_buf.len; i++)
        {
            printf("%02x ", send_buf.buf[i]);
        }
        printf("\r\n");
#endif

        ret = device_dose_uart_data_write(&send_buf, send_buf.len, 1000);
        if (ret != 0)
        {
            printf("device_dose_uart_data_write err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t dose_uart_cmd_process(struct dose_uart *buf)
{
    if (buf == NULL)
    {
        return -1;
    }

#if 0
    printf("recv_buf len: %d\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        printf("%02x ", buf->buf[i]);
    }
    printf("\r\n");
#endif

    struct dose_object cmd = {0};
    memcpy(&cmd, buf->buf, sizeof(struct dose_object));
    cmd.data = &buf->buf[sizeof(struct dose_object) - sizeof(uint8_t *)];

    return dose_cmd_parse(&cmd);
}

static int8_t dose_uart_init(void)
{
    int8_t ret = device_dose_uart_init(DEVICE_DOSE_UART_NAME_DEFAULT);
    if (ret != 0)
    {
        printf("device_dose_uart_init err: %d\r\n", ret);
        return -1;
    }

    ret = device_dose_uart_open();
    if (ret != 0)
    {
        printf("device_dose_uart_open err: %d\r\n", ret);
        return -2;
    }

    return 0;
}

static int8_t dose_uart_recv_entry(void *argument)
{
    int8_t ret = 0;
    struct dose_uart recv_buf = {0};

    ret = dose_uart_init();
    if (ret != 0)
    {
        printf("dose_uart_init err: %d\r\n", ret);
        return -1;
    }

    for (;;)
    {
        ret = device_dose_uart_data_read(&recv_buf, osWaitForever);
        if (ret != 0)
        {
            printf("device_dose_uart_data_read err: %d\r\n", ret);
            continue;
        }

        ret = dose_uart_cmd_process(&recv_buf);
        if (ret!= 0)
        {
            printf("dose_uart_cmd_process err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t dose_uart_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "dose_uart_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityAboveNormal7,
    };

    osThreadId_t thread_id = osThreadNew(dose_uart_recv_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread dose uart create failed\r\n");
        return -1;
    }

    dose_uart_send_queue = osMessageQueueNew(5, sizeof(struct dose_uart), NULL);
    if (dose_uart_send_queue == NULL)
    {
        printf("message queue create failed\r\n");
        return -2;
    }

    thread_id = osThreadNew(dose_uart_send_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread dose uart create failed\r\n");
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(dose_uart_thread_init);


#ifdef dose_uart_TEST
#include "shell.h"

static int8_t dose_uart_cmd_send(uint8_t argc, char **argv)
{
    int8_t ret = 0;
    struct dose_object cmd = {0};
    uint8_t data[10] = {0};

    cmd.id.bits.cmd_id = DOSE_UART_ID;
    cmd.id.bits.cmd_ack = 1;
    cmd.type = 0x40;
    cmd.len = 10;
    cmd.data = data;

    for (uint8_t i = 0; i < cmd.len; i++)
    {
        cmd.data[i] = i;
    }

    ret = dose_uart_cmd_write(&cmd);
    if (ret != 0)
    {
        printf("dose_uart_cmd_write err: %d\r\n", ret);
        return -1;
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(dose_uart_cmd_send, dose_uart_cmd_send, send dose uart cmd);
#endif