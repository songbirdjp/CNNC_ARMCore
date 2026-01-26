#include "os_tool.h"
#include <stdarg.h>
#include "ulog.h"

uint8_t *os_tool_mutex_holder_get(osMutexId_t mutex_id, ...)
{
    uint8_t *file = NULL, *function = NULL;
    uint32_t line = 0;

    va_list args;
    va_start(args, mutex_id);
    file = va_arg(args, char*);
    line = va_arg(args, uint32_t);
    function = va_arg(args, char*);
    va_end(args);

    osThreadId_t holder_id = osMutexGetOwner(mutex_id);
    uint8_t *holder_name = osThreadGetName(holder_id);

    if (file != NULL && line != 0 && function != NULL)
    {
        LOG_I("[file]: %s\r\n[line]: %d\r\n[function]: %s\r\n", file, line, function);
    }

    LOG_I("mutex holder: %s\r\n", holder_name);
    LOG_I("current thread: %s\r\n", osThreadGetName(osThreadGetId()));

    return holder_name;
}