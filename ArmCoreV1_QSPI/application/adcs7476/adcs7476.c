#include "adcs7476.h"
#include "drv_spi.h"
#include "init_call.h"
#include "ulog.h"
#define ADC7476_MASTER_FLAG (1 << 0)
#define ADC7476_SLAVE_FLAG  (1 << 1)


static uint16_t master_data[BUF_LEN] = {0};
static uint16_t slave_data[BUF_LEN] __attribute__((section(".ram_d3"))) = {0};//modif

struct adcs7476_object
{
    osMutexId_t mutex;
    osMessageQueueId_t queue;
    osEventFlagsId_t event;
    uint32_t event_flags;
    uint16_t data[BUF_LEN];
    uint16_t limit_h;
    uint16_t limit_l;

    uint8_t buf_len;
    uint16_t *buf;
};

static struct adcs7476_object 
adcs7476_object_master = {.event_flags = ADC7476_MASTER_FLAG, .limit_h = 0xFFFF, .limit_l = 0, .buf = master_data, .buf_len = BUF_LEN},
adcs7476_object_slave = {.event_flags = ADC7476_SLAVE_FLAG, .limit_h = 0xFFFF, .limit_l = 0, .buf = slave_data, .buf_len = BUF_LEN};

struct adcs7476_object *adcs7476_object_get(uint8_t *device_name)
{
    if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        //printf("object_getspi1\r\n");
        return &adcs7476_object_master;
    }
    else if (!memcmp(device_name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
       // printf("object_getspi6\r\n");
        return &adcs7476_object_slave;
    }
    else
    {
        printf("invalid device name: %s\r\n", device_name);
    }

    return NULL;
}

static int8_t adcs7476_object_data_recv_callback(DEVICE_SPI *spi)
{
    struct adcs7476_object *obj = NULL;

    if (!memcmp(spi->name, DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT)))
    {
        obj = adcs7476_object_get(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT);
    }
    else if (!memcmp(spi->name, DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, sizeof(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT)))
    {
        obj = adcs7476_object_get(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT);
    }
    else
    {
        return 0;
    }

    osEventFlagsSet(obj->event, obj->event_flags);

    return 0;
}

static int8_t adcs7476_object_init(uint8_t *device_name, osEventFlagsId_t event)
{
    if (device_name == NULL || event == NULL)
    {
        printf("device name or event is NULL\r\n");
        return -1;
    }

    struct adcs7476_object *obj = adcs7476_object_get(device_name);
    if (obj == NULL)
    {
        printf("adcs7476 %s object get failed\r\n", device_name);
    }

    int8_t ret = device_adcs7476_init(device_name);
    // printf("initobject %s ret: %d\r\n", device_name, ret);
    if (ret != 0)
    {   
        printf("adcs7476 %s init failed\r\n", device_name);
        return -2;
    }

    ret = device_adcs7476_buffer_init(device_name, obj->buf, obj->buf_len);
    printf("bufferect %s ret: %d\r\n", device_name, ret);
    if (ret != 0)
    {
        printf("adcs7476 %s buffer init failed\r\n", device_name);
        return -3;
    }

    osMutexAttr_t mutex_attributes = {
    .name = "adcs7476_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };

    obj->mutex = osMutexNew(&mutex_attributes);
    if (obj->mutex == NULL)
    {
        printf("mutex create failed\r\n");
        return -4;
    }

    obj->queue = osMessageQueueNew(15, sizeof(uint16_t) * obj->buf_len, NULL);
    if (obj->queue == NULL)
    {
        printf("queue create failed\r\n");
        return -5;
    }

    obj->event = event;

    ret = device_adcs7476_queue_init(device_name, obj->queue);
    if (ret != 0)
    {
        printf("adcs7476 %s queue init failed\r\n", device_name);
        return -7;
    }

    ret = device_adcs7476_callback_register(device_name, adcs7476_object_data_recv_callback);
    if (ret != 0)
    {
        printf("adcs7476 %s callback register failed\r\n", device_name);
        return -8;
    }

    ret = device_adcs7476_open(device_name);
    if (ret != 0)
    {
        printf("adcs7476 %s open failed\r\n", device_name);
        return -9;
    }

    return 0;
}

int8_t adcs7476_object_data_read(uint8_t *device_name, uint16_t *data, uint8_t len, uint32_t timeout)
{
    if (device_name == NULL || data == NULL)
    {
        printf("device name or data is NULL\r\n");
        return -1;
    }

    if (len > BUF_LEN || len == 0)
    {
        printf("parameter len is invalid: %d\r\n", len);
        return -2;
    }

    struct adcs7476_object *obj = adcs7476_object_get(device_name);
    if (obj == NULL)
    {
        printf("adcs7476 %s object get failed\r\n", device_name);
        return -3;
    }

    osMutexAcquire(obj->mutex, timeout);

    memcpy(data, obj->data, len * sizeof(uint16_t));

    osMutexRelease(obj->mutex);

    return 0;
}

int8_t adcs7476_object_data_limit_set(uint8_t *device_name, uint16_t limit_h, uint16_t limit_l)
{
    if (device_name == NULL)
    {
        printf("device name is NULL\r\n");
        return -1;
    }

    struct adcs7476_object *obj = adcs7476_object_get(device_name);
    if (obj == NULL)
    {
        printf("adcs7476 %s object get failed\r\n", device_name);
        return -2;
    }

    osMutexAcquire(obj->mutex, osWaitForever);

    obj->limit_h = limit_h;
    obj->limit_l = limit_l;

    osMutexRelease(obj->mutex);

    return 0;
}

int8_t adcs7476_object_data_limit_get(uint8_t *device_name, uint16_t *limit_h, uint16_t *limit_l)
{
    if (device_name == NULL || limit_h == NULL || limit_l == NULL)
    {
        printf("device name or limit_h or limit_l is NULL\r\n");
        return -1;
    }

    struct adcs7476_object *obj = adcs7476_object_get(device_name);
    if (obj == NULL)
    {
        printf("adcs7476 %s object get failed\r\n", device_name);
        return -2;
    }

    osMutexAcquire(obj->mutex, osWaitForever);

    *limit_h = obj->limit_h;
    *limit_l = obj->limit_l;

    osMutexRelease(obj->mutex);

    return 0;
}

int8_t adcs7476_object_data_limit_fault_get(uint8_t *device_name)
{
    if (device_name == NULL)
    {
        printf("device name is NULL\r\n");
        return -1;
    }

    uint16_t buf[BUF_LEN] = {0};
    int8_t ret = adcs7476_object_data_read(device_name, buf, BUF_LEN, osWaitForever);
    if (ret != 0)
    {
        printf("adcs7476 %s data read failed\r\n", device_name);
        return -2;
    }

    uint16_t limit_h = 0, limit_l = 0;
    ret = adcs7476_object_data_limit_get(device_name, &limit_h, &limit_l);
    if (ret != 0)
    {
        printf("adcs7476 %s data limit get failed\r\n", device_name);
        return -3;
    }

    for (uint8_t i = 0; i < BUF_LEN; i++)
    {
        if (buf[i] > limit_h)
        {
            return (1 << 1);
        }
        else if (buf[i] < limit_l)
        {
            return (1 << 0);
        }
    }

    return 0;
}

static void (*callback)(void) = NULL;

int8_t adcs7476_object_data_callback_register(void (*cb)(void *arg))
{
    callback = cb;

    return 0;
}

static osEventFlagsId_t adcs7476_event = NULL;
static int8_t adcs7476_sample_init(void)
{
    int8_t ret = 0;

    adcs7476_event = osEventFlagsNew(NULL);
    if (adcs7476_event == NULL)
    {
        printf("osEventFlagsNew failed\r\n");
        return -1;
    }

    ret = adcs7476_object_init(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT, adcs7476_event);
    if (ret != 0)
    {
        printf("adcs7476 master init err: %d\r\n", ret);
        return -2;
    }

    ret = adcs7476_object_init(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT, adcs7476_event);
    if (ret != 0)
    {
        printf("adcs7476 slave init err: %d\r\n", ret);
        return -3;
    }

    return 0;
}

static int8_t adcs7476_sample_interval_set(uint16_t time_interval_10ns)
{
    return device_adcs7476_sample_interval_set(time_interval_10ns);
}

int8_t adcs7476_sample_enable(uint8_t enable)
{
    return device_adcs7476_sample_enable(enable);
}

static int8_t adcs7476_sample_data_recv_process(void)
{
    int8_t ret = 0;
    uint32_t event_flag = 0;
    static uint16_t recv_tmp[BUF_LEN] = {0};
    static uint16_t recv_tmp_1[BUF_LEN] = {0};

    struct adcs7476_object *obj_master = adcs7476_object_get(DEVICE_ADCS7476_MCU_IS_MASTER_NAME_DEFAULT);
    struct adcs7476_object *obj_slave = adcs7476_object_get(DEVICE_ADCS7476_MCU_IS_SLAVE_NAME_DEFAULT);

    event_flag = osEventFlagsWait(adcs7476_event, ADC7476_MASTER_FLAG | ADC7476_SLAVE_FLAG, osFlagsWaitAll, osWaitForever);

    osMessageQueueGet(obj_master->queue, recv_tmp, NULL, 0);
    osMessageQueueGet(obj_slave->queue, recv_tmp_1, NULL, 0);

    osMutexAcquire(obj_master->mutex, osWaitForever);
    memcpy(obj_master->data, recv_tmp, obj_master->buf_len * sizeof(uint16_t));
    osMutexRelease(obj_master->mutex);

    osMutexAcquire(obj_slave->mutex, osWaitForever);
    memcpy(obj_slave->data, recv_tmp_1, obj_slave->buf_len * sizeof(uint16_t));
    osMutexRelease(obj_slave->mutex);

    if (callback != NULL)
    {
        callback();
    }

#if 1
    for (uint8_t i = 0; i < obj_master->buf_len; i++)
    {
        LOG_E("ADC7476_MASTER: recv_tmp[%d] = %d\r\n", i, recv_tmp[i]);
        LOG_E("ADC7476_SLAVE: recv_tmp_1[%d] = %d\r\n", i, recv_tmp_1[i]);
    }
#endif

    return 0;
}

static int8_t adcs7476_sample_entry(void *argument)
{
    int8_t ret = 0;

    ret = adcs7476_sample_init();
    if (ret != 0)
    {
        printf("adcs7476 sample init err: %d\r\n", ret);
        return -1;
    }

    ret = adcs7476_sample_interval_set(100);
    if (ret != 0)
    {
        printf("adcs7476 sample interval set err: %d\r\n", ret);
        return -2;
    }

    ret = adcs7476_sample_enable(1);
    if (ret != 0)
    {
        printf("adcs7476 sample enable err: %d\r\n", ret);
        return -3;
    }

    for (;;)
    {
        ret = adcs7476_sample_data_recv_process();
        if (ret != 0)
        {
            printf("adcs7476 sample data recv process err: %d\r\n", ret);
            return -4;
        }
    }

    return 0;
}

static int8_t adcs7476_sample_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "adcs7476_sample_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityHigh1,
    };

    osThreadId_t thread_id = osThreadNew(adcs7476_sample_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread adcs7476 sample create failed\r\n");
        return -1;
    }

    return 0;
}
INIT_APP_EXPORT(adcs7476_sample_thread_init);
#define ADCS7476_SAMPLE_TEST    
#ifdef ADCS7476_SAMPLE_TEST
#include "shell.h"

static int adcs7476_sample_test(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("adcs7476_sample_test: invalid parameter\r\n");
    }

    adcs7476_sample_interval_set(atoi(argv[1]));

    adcs7476_sample_enable(1);

    return 0;
}
MSH_CMD_EXPORT_ALIAS(adcs7476_sample_test, adcs7476_sample_test, adcs7476 sample test);
#endif