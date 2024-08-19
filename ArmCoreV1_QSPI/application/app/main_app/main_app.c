#include "main_app.h"
#include "init_call.h"
#include "cmsis_os2.h"

static int8_t data_process_entry(void *argument)
{
    for (;;)
    {
        osDelay(1000);
    }

    return 0;
}

static int8_t main_app_thread_init(void)
{
    osThreadAttr_t data_process_thread_attributes = {
    .name = "data_process_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    osThreadId_t data_process_threadHandle = osThreadNew(data_process_entry, NULL, &data_process_thread_attributes);
    if (data_process_threadHandle == NULL)
    {
        printf("thread data process create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(main_app_thread_init);