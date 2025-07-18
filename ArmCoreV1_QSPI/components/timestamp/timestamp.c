#include "timestamp.h"
#include "tim.h"
#include "init_call.h"
#include "cmsis_os2.h"

/**
  * @brief This function handles TIM24 global interrupt.
  */
void TIM24_IRQHandler(void)
{
  /* USER CODE BEGIN TIM24_IRQn 0 */

  /* USER CODE END TIM24_IRQn 0 */
  HAL_TIM_IRQHandler(&htim24);
  /* USER CODE BEGIN TIM24_IRQn 1 */

  /* USER CODE END TIM24_IRQn 1 */
}

struct timestamp_data
{
    uint64_t timestamp_ns;
    osMutexId_t mutex;
    osEventFlagsId_t event;
    #define TIMESTAMP_EVENT (1 << 0)
};

static struct timestamp_data timestamp_obj = {0};
static struct timestamp_data *timestamp_obj_get(void)
{
    return &timestamp_obj;
};


static void PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    osEventFlagsSet(timestamp_obj_get()->event, TIMESTAMP_EVENT);
}

static int8_t timestamp_init(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    MX_TIM24_Init();

    status = HAL_TIM_RegisterCallback(&htim24, HAL_TIM_PERIOD_ELAPSED_CB_ID, PeriodElapsedCallback);
    if (status != HAL_OK)
    {
        printf("tim24 register callback err: %d\r\n", status);
        return -1;
    }

    status = HAL_TIM_Base_Start_IT(&htim24);
    if (status != HAL_OK)
    {
        printf("tim24 start err: %d\r\n", status);
        return -2;
    }

    return 0;
}

static int8_t timestamp_entry(void *argument)
{
    int8_t ret = 0;
    struct timestamp_data *obj = (struct timestamp_data *)argument;

    ret = timestamp_init();
    if (ret != 0)
    {
        printf("timestamp init err: %d\r\n", ret);
        osThreadExit();
    }

    for (;;)
    {
        osEventFlagsWait(obj->event, TIMESTAMP_EVENT, osFlagsWaitAny, osWaitForever);

        osMutexAcquire(obj->mutex, osWaitForever);
        obj->timestamp_ns += (uint64_t)__HAL_TIM_GET_AUTORELOAD(&htim24) * 1000;
        osMutexRelease(obj->mutex);
    }

    return 0;
}


static int8_t timestamp_thread_init(void)
{
    struct timestamp_data *obj = timestamp_obj_get();

    obj->event = osEventFlagsNew(NULL);
    if (obj->event == NULL)
    {
        printf("event timestamp create err\r\n");
        return -1;
    }

    osMutexAttr_t mutex_attributes = {
    .name = "timestamp_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };
    obj->mutex = osMutexNew(&mutex_attributes);
    if (obj->mutex == NULL)
    {
        printf("mutex timestamp create err\r\n");
        return -2;
    }

    osThreadAttr_t thread_attributes = {
    .name = "timestamp_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityHigh,
    };
    
    osThreadId_t tid = osThreadNew(timestamp_entry, obj, &thread_attributes);
    if (tid == NULL)
    {
        printf("thread timestamp create err\r\n");
        return -3;
    }

    return 0;
}
INIT_APP_EXPORT(timestamp_thread_init);

int8_t timestamp_ns_set(uint64_t timestamp_ns)
{
    struct timestamp_data *obj = timestamp_obj_get();

    osMutexAcquire(obj->mutex, osWaitForever);
    __disable_irq();
    __HAL_TIM_CLEAR_IT(&htim24, TIM_IT_UPDATE);
    __HAL_TIM_SET_COUNTER(&htim24, 0);
    obj->timestamp_ns = timestamp_ns;
    __enable_irq();
    osMutexRelease(obj->mutex);

    return 0;
}

uint64_t timestamp_ns_get(void)
{
    struct timestamp_data *obj = timestamp_obj_get();
    uint64_t timestamp_ns = 0;

    osMutexAcquire(obj->mutex, osWaitForever);
    timestamp_ns = obj->timestamp_ns + (uint64_t)__HAL_TIM_GET_COUNTER(&htim24) * 1000;
    osMutexRelease(obj->mutex);

    return timestamp_ns;
}
