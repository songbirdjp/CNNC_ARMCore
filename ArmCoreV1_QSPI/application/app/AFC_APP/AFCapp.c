#include "adcs7476.h"
#include "drv_spi.h"
#include "init_call.h"
#include "ulog.h"
#include "drv_flash.h"
#include "flash_port.h"
#include <stdbool.h>
#include "stm32h7xx_hal.h"
#include "drv_gpio.h"
#include "lptim.h"
#include "tim.h"
#include "shell.h"
#include "AFCapp.h"
#include "FreeRTOS.h"
#include "task.h"
#define FLASH_ADDRESS_BASE  (FLASH_BASE + FLASH_SECTOR_SIZE * 6)//0x08000000UL + 0x00020000UL* 6 = 0x080C0000UL
#define FLASH_VALID_SIZE    (FLASH_SECTOR_SIZE * 2) //0x00020000UL * 2 = 0x00040000UL

static DEVICE_FLASH flash_bank1 = {0};
static DEVICE_FLASH *device_flash_get(void)
{
    return &flash_bank1;
}
int8_t Shell_ReadFlash(uint8_t argc, char *argv[])
{
    int8_t ret = 0;
    DEVICE_FLASH *flash = device_flash_get();
    uint8_t data[1024] = {0};
    uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE};
    ret = flash->read(flash, 0, data, sizeof(data), 1000);
    if (ret != 0)
    {
        printf("flash read err:%d\r\n", ret);
        return -3;
    }

    for (uint32_t i = 0; i < 256; i += 16) 
    {
        LOG_E("0x%08x: ", FLASH_ADDRESS_BASE + i);
        for (uint32_t j = 0; j < 32; j++)
        {
            LOG_E("%02x ", data[i + j]);
        }
        LOG_E("\r\n");
    }
    return 0;
}
MSH_CMD_EXPORT_ALIAS(Shell_ReadFlash, ReadFlash, flash test);

int8_t WriteArrayToFlash(uint16_t *data, uint32_t len)
{
    int8_t ret = 0;
    DEVICE_FLASH *flash = device_flash_get();
    uint32_t flash_AFC_Offset = 0;

    ret = flash->write(flash, flash_AFC_Offset, data, len * sizeof(uint16_t), 1000);
    flash_AFC_Offset += 16 * sizeof(uint16_t);
    if (ret != 0)
    {
        printf("flash write err:%d\r\n", ret);
        return -1;
    }

    return 0;
}
AFCApplicationParam_t AFCApplicationParam = {   .whichData = 1,
                                                .positionDeadzone = 5,
                                                .A1In_Para = 1,
                                                .A2In_Para = 1,
                                                .B1In_Para = 0,
                                                .B2In_Para = 0,
                                                .positionStep = 10,
                                                .positionUpperLimit = 19890,
                                                .positionLowerLimit = 18210};
uint16_t *AFCApplicationParamGet(void)
{
    return &AFCApplicationParam;
}
#include "motorCtrl.h"
void MagMotorCtrlbyADC(uint16_t *data)
{
    AFCApplicationParam_t *obj = AFCApplicationParamGet();
    obj->positionCurrent = __HAL_TIM_GET_COUNTER(&htim2); 
    uint16_t dataADC1[8], dataADC2[8] = {0};
    memcpy(dataADC1, data, 1 * sizeof(uint16_t));
    memcpy(dataADC2, data + 1, 1 * sizeof(uint16_t));
    uint16_t phaseA = obj->A1In_Para * dataADC1[0] + obj->B1In_Para;
    uint16_t phaseB = obj->A2In_Para * dataADC2[0] + obj->B2In_Para;
    LOG_I("phaseA = %d\r\n",phaseA);
    LOG_I("phaseB = %d\r\n",phaseB);
    LOG_I("A - B = %d\r\n",phaseA - phaseB);

    if(phaseA > (phaseB + obj->positionDeadzone))
    {
        obj->positionCalculated = obj->positionCurrent + obj->positionStep;
    }
    else if(phaseA <( phaseB - obj->positionDeadzone))
    {
        obj->positionCalculated = obj->positionCurrent - obj->positionStep;
    }
    else
    {
        obj->positionCalculated = obj->positionCurrent;
    }
   
    
    if(obj->positionCurrent > obj->positionUpperLimit)
    {
        obj->positionCalculated = obj->positionUpperLimit - 10;
    }
    else if(obj->positionCurrent < obj->positionLowerLimit)
    {
        obj->positionCalculated = obj->positionLowerLimit + 10;
    }
    // LOG_I("posCalculated = %d\r\n\r\n",obj->positionCalculated);
    // LOG_I("pos = %d\r\n\r\n",obj->positionCurrent);
}
void Shell_ChangeCalPos(uint8_t argc, char *argv[])
{
    
    AFCApplicationParam_t *obj = AFCApplicationParamGet();
    obj->positionCurrent = getEncodeValue(MOTOR_MAG);
    uint16_t dir = atoi(argv[1]);
    uint16_t posStep = atoi(argv[2]);
    if(dir == 1)
    {
        obj->positionCalculated = obj->positionCurrent + posStep;
    }
    else if(dir == 2)
    {
        obj->positionCalculated = obj->positionCurrent - posStep;
    }
}
MSH_CMD_EXPORT_ALIAS(Shell_ChangeCalPos, ChangeCalPos, Change Cal Position);

// FreeRTOS任务统计监控相关变量
static TaskHandle_t Mag_MotorCtrl_task_handle = NULL;
static uint32_t last_check_time_ms = 0;
static uint32_t last_run_time_counter = 0;
static uint32_t loop_count = 0;  // 循环计数器
static uint32_t last_loop_count = 0;  // 上次循环计数
#define TASK_MONITOR_INTERVAL_MS 1000  // 监控间隔1秒

// Get Mag_MotorCtrl task statistics
static void Mag_MotorCtrl_GetTaskStats(void)
{
    if (Mag_MotorCtrl_task_handle == NULL)
    {
        LOG_E("Mag_MotorCtrl task handle not set!\r\n");
        return;
    }
    
    TaskStatus_t task_status;
    uint32_t current_time_ms = HAL_GetTick();
    
    // Get task detailed information
    vTaskGetInfo(Mag_MotorCtrl_task_handle, &task_status, pdTRUE, eInvalid);
    
    // Calculate runtime delta
    uint32_t run_time_delta = task_status.ulRunTimeCounter - last_run_time_counter;
    uint32_t time_delta_ms = current_time_ms - last_check_time_ms;
    
    // Calculate loop frequency (loops per second)
    uint32_t loop_delta = loop_count - last_loop_count;
    uint32_t loops_per_second = 0;
    if (time_delta_ms > 0)
    {
        loops_per_second = (loop_delta * 1000) / time_delta_ms;
    }
    
    // Calculate CPU usage percentage (runtime delta / total time delta * 100)
    uint32_t cpu_usage_percent = 0;
    if (time_delta_ms > 0)
    {
        cpu_usage_percent = (run_time_delta * 100) / time_delta_ms;
    }
    
    // Check if loop frequency is too high (indicating AFC_ADCSampleRecvProcess is not blocking)
    if (loops_per_second > 250)  // 250 loops per second threshold
    {
        LOG_E("Mag_MotorCtrl task loop frequency too high! Loops/sec: %lu\r\n", loops_per_second);
    }
    // else
    // {
    //     LOG_I("Mag_MotorCtrl task loops/sec: %lu\r\n", loops_per_second);
    // }
    
    // Update last recorded values
    last_run_time_counter = task_status.ulRunTimeCounter;
    last_check_time_ms = current_time_ms;
    last_loop_count = loop_count;
}
uint16_t MagMotorADCValue[2] = {0};
static void Mag_MotorCtrl_thread_entry(void *argument)
{
    // Save task handle for statistics monitoring
    Mag_MotorCtrl_task_handle = xTaskGetCurrentTaskHandle();
    
    MX_TIM1_Init();
    MX_TIM4_Init();
    MX_TIM8_Init();
    //MX_TIM24_Init();
    MX_TIM23_Init();
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_Base_Start(&htim4);
    HAL_TIM_Base_Start(&htim8);
    HAL_TIM_Base_Start(&htim23);
   // HAL_TIM_Base_Start(&htim24);

    // flash = device_flash_get();
    // uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE}; 
    // flash_init(flash, "DEVICE_NAME_FLASH_BANK1");
    // flash_operation_address_set(flash, flash_cfg[0], flash_cfg[1]);
    // flash->ioctl(flash, FLASH_CMD_ERASE_SECTOR, (void *)flash_cfg);
    uint16_t data[2] = {0};
    
    // Initialize monitoring time
    last_check_time_ms = HAL_GetTick();
    
    for (;;)
    {   
        // Increment loop counter for frequency monitoring
        loop_count++;
        
        uint16_t *ADCDData = AFC_ADCSampleRecvProcess();
        memcpy(MagMotorADCValue, ADCDData, 2 * sizeof(uint16_t));
        
        // Periodically check task statistics
        static uint32_t last_monitor_time = 0;
        uint32_t current_time = HAL_GetTick();
        if ((current_time - last_monitor_time) >= TASK_MONITOR_INTERVAL_MS)
        {
            Mag_MotorCtrl_GetTaskStats();
            last_monitor_time = current_time;
        }
        
        LOG_I("1111MagMotorADCValue = %d\r\n",MagMotorADCValue[0]);
        LOG_I("222222MagMotorADCValue = %d\r\n",MagMotorADCValue[1]); 
    }
}
static void AFC_DataTransmit_thread_entry(void *argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

static int8_t Mag_MotorCtrl_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "Mag_MotorCtrl_thread",
    .stack_size = 2048 * 4,
    .priority = osPriorityNormal,
    };

    osThreadId_t thread_id = osThreadNew(Mag_MotorCtrl_thread_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread Mag_MotorCtrl create failed\r\n");
        return -1;
    }
    return 0;
}

static int8_t AFC_DataTransmit_thread_init(void)
{
    osThreadAttr_t thread_attr = {
    .name = "AFC_DataTransmit_thread",
    .stack_size = 2048 * 4,
    .priority = osPriorityNormal,
    };

    osThreadId_t thread_id = osThreadNew(AFC_DataTransmit_thread_entry, NULL, &thread_attr);
    if (thread_id == NULL)
    {
        printf("thread AFC_DataTransmit create failed\r\n");
        return -1;
    }
    return 0;
}
INIT_APP_EXPORT(Mag_MotorCtrl_thread_init);
INIT_APP_EXPORT(AFC_DataTransmit_thread_init);