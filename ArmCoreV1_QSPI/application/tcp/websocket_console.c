#include "websocket_console.h"
#include "websocket.h"
#include "init_call.h"
#include "ulog.h"
#include "shell.h"
#include <stddef.h>

static uint8_t websocket_console_sn = 0;
static int8_t websocket_write(uint8_t *buf, uint32_t len)
{
    return ws_send(websocket_console_sn, buf, len, 1, 0, WDT_TXTDATA);
}

int8_t websocket_cmd_parse(uint8_t sn, uint8_t *buf, uint16_t len)
{
    uint8_t *header = "[shell]";

    if (memcmp(buf, header, strlen(header)) == 0)
    {
        websocket_console_sn = sn;
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
    .index = 2};

    int8_t ret = ulog_write_func_register(&info);
    if (ret != 0)
    {
        printf("console log register err:%d\r\n", ret);
        return ret;
    }
#endif

    return 0;
}
// INIT_COMPONENT_EXPORT(websocket_log_init);