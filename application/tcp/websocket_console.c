#include "websocket_console.h"
#include "tcp_tasks.h"
#include "websocket.h"
#include "init_call.h"
#include "ulog.h"
#include "shell.h"
#include <stddef.h>

static int8_t websocket_write(uint8_t *buf, uint32_t len)
{
    int8_t ret = 0;
    uint8_t *name = NULL;

    for (uint8_t i = 0; i < 8; i++)
    {
        name = socket_name_get_by_sn(i);
        if (name != NULL && memcmp(name, SHELL_AUTHORIZATION, strlen(SHELL_AUTHORIZATION)) == 0)
        {
            ret = ws_send(i, buf, len, 1, 0, WDT_TXTDATA);
            if (ret <= 0)
            {
                LOG_E("websocket_write sn = %d err: %d\r\n", i, ret);
            }
        }
    }

    return ret;
}

int8_t websocket_shell_cmd_parse(uint8_t sn, uint8_t *buf, uint16_t len)
{
    uint8_t *header = "[shell]";

    if (memcmp(buf, header, strlen(header)) == 0)
    {
        shell_cmd_parse_entry(buf + strlen(header), len - strlen(header));
    }

    return 0;
}

static int8_t websocket_log_init(void)
{
#ifdef TCP_WEBSOCKET
    struct ulog_write_func_info info = {
    .func_init = NULL,
    .func_callback = websocket_write,
    .index = 2,
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
INIT_COMPONENT_EXPORT(websocket_log_init);