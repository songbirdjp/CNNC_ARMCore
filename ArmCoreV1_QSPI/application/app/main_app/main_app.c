#include "main_app.h"
#include "init_call.h"
#include "cmsis_os2.h"
#include "radiation_app.h"

static osEventFlagsId_t adcs7476_event = NULL;
#define ADCS7476_CONVERT_COMPLETE_EVENT   (1 << 0)
static int8_t adcs7476_object_data_complete_callback(uint8_t *name, uint16_t *data, uint16_t len)
{
    osEventFlagsSet(adcs7476_event, ADCS7476_CONVERT_COMPLETE_EVENT);

    return 0;
}

static int8_t data_process_entry(void *argument)
{
    int8_t ret = adcs7476_object_data_callback_register(adcs7476_object_data_complete_callback);
    if (ret != 0)
    {
        printf("adcs7476 callback register err: %d\r\n", ret);
        return -2;
    }

    for (;;)
    {
        osEventFlagsWait(adcs7476_event, ADCS7476_CONVERT_COMPLETE_EVENT, osFlagsWaitAny, osWaitForever);

        ret = adcs7476_value_process();
        if (ret != 0)
        {
            printf("data process err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t main_app_thread_init(void)
{
    adcs7476_event = osEventFlagsNew(NULL);
    if (adcs7476_event == NULL)
    {
        printf("adcs7476 event create failed\r\n");
        return -1;
    }

    osThreadAttr_t data_process_thread_attributes = {
    .name = "data_process_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal7,
    };

    osThreadId_t data_process_threadHandle = osThreadNew(data_process_entry, NULL, &data_process_thread_attributes);
    if (data_process_threadHandle == NULL)
    {
        printf("thread data process create failed\r\n");
        return -2;
    }

    return 0;
}
INIT_APP_EXPORT(main_app_thread_init);

