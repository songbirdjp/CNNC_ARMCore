#include "main_app.h"
#include "init_call.h"
#include "cmsis_os2.h"

#define DATA_PROCESS_EVENT      (1<<0)
static osEventFlagsId_t data_process_eventHandle = NULL;

static void data_process_entry(void *argument)
{
    osStatus_t stat = 0;
    uint32_t event_flag = 0;

    for (;;)
    {   
        event_flag = osEventFlagsWait(data_process_eventHandle, DATA_PROCESS_EVENT, osFlagsWaitAny, osWaitForever);
        if (event_flag & DATA_PROCESS_EVENT)
        {

        }
    }
}

static int8_t main_app_thread_init(void)
{
    osThreadAttr_t recv_data_process_thread_attributes = {
    .name = "recv_data_process_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
    };

    data_process_eventHandle = osEventFlagsNew(NULL);
    if (data_process_eventHandle == NULL)
    {
        printf("event data process create failed\r\n");
        return -1;
    }

    osThreadId_t recv_data_process_threadHandle = osThreadNew(data_process_entry, NULL, &recv_data_process_thread_attributes);
    if (recv_data_process_threadHandle == NULL)
    {
        printf("thread recv data process create failed\r\n");
        return -2;
    }

    return 0;
}
INIT_APP_EXPORT(main_app_thread_init);