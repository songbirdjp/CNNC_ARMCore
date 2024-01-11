#include "ethercat.h"
#include "applInterface.h"
#include "lan9252_port.h"
#include "lan9252_app.h"
#include "cmsis_os2.h"

static void (*fun_ptr)(void);

static void appl_cb(void)
{
    if (fun_ptr != NULL)
    {
        fun_ptr();
    }
}

int8_t ethercat_slave_appl_cb_register(void (*fun_cb)(void))
{
    fun_ptr = fun_cb;

    return 0;
}

static int8_t ethercat_slave_init(void)
{
    int8_t ret = 0;

    ret = device_lan9252_init(DEVICE_NAME_OSPI1_DEFAULT);
    if (ret != 0)
    {
        printf("device lan9252 init err:%d\r\n", ret);
    }

    extern osEventFlagsId_t data_process_eventHandle;
    ret = lan9252_app_ops_init(data_process_eventHandle, DATA_PROCESS_LAN_EVENT, appl_cb);
    if (ret != 0)
    {
        printf("lan9252 appl ops init err:%d\r\n", ret);
    }

    LAN9252_Init();

    return ret;
}

static uint16_t ethercat_slave_stack_init(void)
{
    return MainInit();
}

static void ethercat_slave_main_loop(void)
{
    MainLoop();
}

static int32_t ethercat_slave_wait_event(void)
{
    int32_t ret = device_lan9252_data_recv_with_block();

    if (ret & LAN9252_IRQ_EVENT)
    {
        PDI_Isr();
    }
    
    if (ret & LAN9252_SYNC0_IRQ_EVENT)
    {
        Sync0_Isr();
    }

    if (ret & LAN9252_SYNC1_IRQ_EVENT)
    {
        Sync1_Isr();
    }

    return ret;
}

int8_t ethercat_recv_data_update(void)
{
    osStatus_t stat = osOK;

    uint8_t pdo_output_data[MAX_PD_OUTPUT_SIZE] = {0};

    stat = osMessageQueueGet(lan9252_app_ops_get()->pdo_output_queue, pdo_output_data, 0, 0);
    if (stat == osOK)
    {
        lan9252_app_ops_get()->appl_output_update((uint16_t *)&pdo_output_data);

        // printf("%x %x %x %x\n", sDOOutputs.InfoOut[0], sDOOutputs.InfoOut[1], sDOOutputs.InfoOut[2], sDOOutputs.InfoOut[3]);
    }
    else
    {
        printf("no msg in lan9252 rx queue:%d\r\n", stat);
    }

    return 0;
}

int8_t ethercat_send_data_update(uint16_t *buf) /* slave to master */
{
    if (buf == NULL)
    {
        return -1;
    }

    osMutexAcquire(lan9252_app_ops_get()->pdo_input_update_mutex, osWaitForever);
    memcpy(&sDIInputs, buf, sizeof(sDIInputs));
    osMutexRelease(lan9252_app_ops_get()->pdo_input_update_mutex);

    return 0;
}

uint16_t *ethercat_recv_data_get(uint16_t *buf) /* master to slave */
{
    if (buf == NULL)
    {
        return NULL;
    }

    osMutexAcquire(lan9252_app_ops_get()->pdo_output_update_mutex, osWaitForever);
    memcpy(buf, &sDOOutputs, sizeof(sDOOutputs));
    osMutexRelease(lan9252_app_ops_get()->pdo_output_update_mutex);

    return buf;
}

uint16_t *ethercat_send_data_get(uint16_t *buf)
{
    if (buf == NULL)
    {
        return NULL;
    }

    osMutexAcquire(lan9252_app_ops_get()->pdo_input_update_mutex, osWaitForever);
    memcpy(buf, &sDIInputs, sizeof(sDIInputs));
    osMutexRelease(lan9252_app_ops_get()->pdo_input_update_mutex);

    return buf;
}

/*
 * ethercat thread init
*/
static void Ethercatfunc(void *argument)
{
  /* USER CODE BEGIN Ethercatfunc */
    ethercat_slave_init();

    ethercat_slave_stack_init();

    /* Infinite loop */
    for(;;)
    {

        ethercat_slave_main_loop();        

        osDelay(1);
    }
  /* USER CODE END Ethercatfunc */
}

static void ethercat_slave_entry(void *argument)
{
  /* USER CODE BEGIN ethercat_slave_entry */
  /* Infinite loop */
  int32_t ret = 0;
  osDelay(100); /* wait ethercat init complete */

  for(;;)
  {
    ret = ethercat_slave_wait_event();
    if (ret < 0)
    {
        printf("ethercat wait err:%d\r\n", ret);
    }
    // osDelay(100);
  }
  /* USER CODE END ethercat_slave_entry */
}

int8_t ethercat_thread_init(void)
{
    osThreadAttr_t EthercatSlave_attributes = {
    .name = "EthercatSlave",
    .stack_size = 4096 * 4,
    .priority = (osPriority_t) osPriorityNormal,
    };

    osThreadAttr_t lan9252_irq_thread_attributes = {
    .name = "lan9252_irq_thread",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityHigh,
    };

    osThreadId_t EthercatSlaveHandle = osThreadNew(Ethercatfunc, NULL, &EthercatSlave_attributes);
    if (EthercatSlaveHandle == NULL)
    {
        printf("thread ethercat slave create failed\r\n");
        return -1;
    }

    osThreadId_t lan9252_irq_threadHandle = osThreadNew(ethercat_slave_entry, NULL, &lan9252_irq_thread_attributes);
    if (lan9252_irq_threadHandle == NULL)
    {
        printf("thread lan9252 irq create failed\r\n");
        return -1;
    }

    return 0;
}