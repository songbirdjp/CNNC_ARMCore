#include "AFC_uart.h"
#include "uart_port.h"
#include "init_call.h"
#include "sys_cfg.h"
// #include "mcu_adc.h"
#include "ulog.h"
#include "drv_flash.h"
#include "motorctrl.h"
#include "shell.h"
#include "stdint.h"
#include "stm32h7xx.h"
#include "tim.h"
extern TIM_HandleTypeDef htim4;  // TIM4句柄声明
const uint8_t AFC_Version[4] = {0x19,0,0,1};

int8_t control_data_pointer_get(void **ptr)
{
    if (ptr == NULL)
    {
        return -1;
    }

    *ptr = (void *)control_data_get();
    
    return 0;
}


osMessageQueueId_t AFC_uart_send_queue = NULL;
static int8_t AFC_uart_cmd_write(struct AFC_object *cmd)
{
    osStatus_t stat = osOK;
    struct AFC_uart send_buf = {0};
    uint8_t offset = sizeof(struct AFC_object) - sizeof(uint8_t *);

    memcpy(send_buf.buf, cmd, sizeof(struct AFC_object));
    memcpy(&send_buf.buf[offset], cmd->data, cmd->len);
    send_buf.len = offset + cmd->len;
    stat = osMessageQueuePut(AFC_uart_send_queue, &send_buf, 0, 0);
    if (stat != osOK)
    {
        LOG_E("AFC uart send queue put err: %d\r\n", stat);
        return -1;
    }
    return 0;
}
void AFC_UART_SendCmd(UARTCmdType_t cmdType, uint8_t *cmdData,uint8_t len)
{
    struct cmd_object BGMCmdToSend;
    BGMCmdToSend.id.byte = 0x80;
    BGMCmdToSend.type = cmdType;
    BGMCmdToSend.len = len;
    BGMCmdToSend.data = cmdData;
    AFC_uart_cmd_write(&BGMCmdToSend);
}

static int8_t AFC_handshake_frame_parse(struct AFC_object *cmd)
{
    int8_t ret = 0;
    AFCConfigParam_TypeDef *obj = AFC_ConfigParam_get();
    obj->AFCHandShakeOK = 0x01;
#if 1
    LOG_I("bgm arm core handshake frame parse: %d\r\n", cmd->len);
    LOG_I("hardware version: %#.2x\r\n", cmd->data[0]);
    LOG_I("software version: %u.%u.%u\r\n", cmd->data[1], cmd->data[2], cmd->data[3]);
    LOG_I("AFC id: %u\r\n", cmd->data[4]);
#endif

    uint8_t *fw_ver = system_info_get()->fw_version;

    cmd->len = 4;
    cmd->data[0] = 0x01;    /* 0: dose1     1: dose2 */
    cmd->data[1] = strtoul(&fw_ver[0], NULL, 10);  /* software version */
    cmd->data[2] = strtoul(&fw_ver[3], NULL, 10);
    cmd->data[3] = strtoul(&fw_ver[6], NULL, 10);

    return ret;
}
#include "drv_flash.h"
#include "flash_port.h"
#include "AFCapp.h"
extern DEVICE_FLASH *flash;
//#define FLASH_ADDRESS_BASE  (FLASH_BASE + FLASH_SECTOR_SIZE * 6)//0x08000000UL + 0x00020000UL* 6 = 0x080C0000UL
//#define FLASH_VALID_SIZE    (FLASH_SECTOR_SIZE * 2) //0x00020000UL * 2 = 0x00040000UL
uint32_t start_address = 0;
extern uint32_t flash_AFC_Offset;
// uint8_t tim4Delaytimes = 12;
extern MotorFindingZeroFSM_t MagMotorState;
static int8_t  AFC_ParaSet_parse(struct AFC_object *cmd)
{
    AFCConfigParam_TypeDef *objConfig = AFC_ConfigParam_get();
    AFCApplicationParam_t *objAFCApp = AFCApplicationParamGet();
   // uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE};
    //uint32_t start_address = 0; // 定义一个起始地址
    uint16_t temp_data[16] = {0};
    switch (cmd->data[1])//0x01
    {
        case 0x00://Set AFC Control Mode
            objConfig->AFCControlmode = cmd->data[2];
            switch(objConfig->AFCControlmode)
            {
                case 0x00:
                    MagMotorState = MotorFSM_StayAtPresetPos;
                    // objAFCApp->whichData =1;
                    printf("MotorFSM_StayAtPresetPos\r\n");
                    break;
                case 0x01:
                    MagMotorState = MotorFSM_ManualControl;
                    // objAFCApp->whichData =2;
                    printf("MotorFSM_ManualControl\r\n");
                    break;
                case 0x02:
                    MagMotorState = MotorFSM_AutoControl;
                    objAFCApp->positionCalculated = __HAL_TIM_GET_COUNTER(&htim2);
                    printf("MotorFSM_AutoControl\r\n");
                    break;
            }
            break;
        case 0x01://Set AFC Sample Mode
            objConfig->AFCSampleMode = cmd->data[2];
            break;
        case 0x02://Set AFC Sample Delay
            objConfig->AFCSampleDelay = (cmd->data[3] << 8) | cmd->data[2];   
            //LOG_E("AFC Sample Delay: %d\r\n", obj->AFCSampleDelay);
            __HAL_TIM_SET_COUNTER(&htim4, objConfig->AFCSampleDelay);
            break;
        case 0x03://delete sample data
            //flash->ioctl(flash, FLASH_CMD_ERASE_SECTOR, (void *)flash_cfg);
            break;
        case 0x04:
            cmd->len = 0x02;
            cmd->data[0] = 0x01;    
            cmd->data[1] = 0x04;

            //memcpy(&cmd->data[2],AFC_ADCSampleRecvProcess(), sizeof(uint8_t) * 16 * 2);
            break;
        default:
            break;
    }
   return 0;
}
static int8_t  AFC_MagMotorCmd_parse(struct AFC_object *cmd)
{   
    int8_t ret = 0;
    MotorCtrlParam_TypeDef *obj = MAG_motorParam_get();
    obj->encoderValCurrent = __HAL_TIM_GET_COUNTER(&htim2);
    switch (cmd->data[1])
    {
        case 0x00:// Mag Motor find zero ok ACK
            cmd->len = 0x03;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x00;
            cmd->data[2] = obj->motorFindZeroOK;
            break;
        case 0x01://Mag Motor set position 2 bytes no ACK
            obj->encoderValTarget = (cmd->data[3] << 8) | cmd->data[2];
            cmd->len = 0x04;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x01;
            cmd->data[2] = obj->encoderValCurrent & 0xFF;
            cmd->data[3] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x02://Mag Motor run by step no ACK
            if (cmd->data[2] != 0x01 && cmd->data[2] != 0x02) 
            {
                //LOG_E("Mag Motor run by step err: %d\r\n", cmd->data[0]);
                break;
            }
            uint16_t step_value = (cmd->data[4] << 8) | cmd->data[3];
            if (cmd->data[2] == 0x01)
            {
                obj->encoderValTarget += step_value;
                if(obj->encoderValCurrent> 35200)//1024*4.75*5.18 = 25195.5200;
                {
                    obj->encoderValTarget = 35200;
                }
            }
            else if (cmd->data[2] == 0x02)
            {
                obj->encoderValTarget -= step_value;
                if(obj->encoderValCurrent < 10000)
                {
                    obj->encoderValTarget = 10000;
                }
            }
            cmd->len = 0x04;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x02;
            cmd->data[2] = obj->encoderValCurrent & 0xFF;
            cmd->data[3] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x03:
            cmd->len = 0x04;
            cmd->data[0] = 0x40;
            cmd->data[1] = 0x03;
            cmd->data[2] = obj->encoderValCurrent & 0xFF;
            cmd->data[3] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x04:
            obj->presetPos = (cmd->data[0] << 8) | cmd->data[1];
            break;
        default:
            break;
    }
    return ret;
}   
#include "motorCtrl.h"  
static int8_t  AFC_AFTMotorCmd_parse(struct AFC_object *cmd)
{
    int8_t ret = 0;
    MotorCtrlParam_TypeDef *obj = AFT_motorParam_get();
     obj->encoderValCurrent = __HAL_TIM_GET_COUNTER(&htim3);
    switch (cmd->data[1])
    {
         case 0x00:
            obj->motorBrakeStatus = cmd->data[0];
            break;
        case 0x01://enable AFT Motor to init
            obj->motorInitEnable = cmd->data[0];
            break;
        case 0x0F://enable AFT Motor to find zero
            cmd->len = 0x01;
            cmd->data[0] = obj->motorFindZeroOK;
            break;
        case 0x03://Mag Motor set position 2 bytes no ACK
            obj->encoderValTarget = (cmd->data[0] << 8) | cmd->data[1];
            break;
        case 0x02://Mag Motor run by step no ACK
          if (cmd->data[2] != 0x01 && cmd->data[2] != 0x02) 
            {
                LOG_E("AFT Motor run by step err: %d\r\n", cmd->data[0]);
                break;
            }
            uint16_t step_value = (cmd->data[4] << 8) | cmd->data[3];
            printf("step_value = %d\r\n",step_value);
            if (cmd->data[2] == 0x01)
            {
                // obj->encoderValTarget += step_value;
                // if(obj->encoderValCurrent> 65535)//1024*4.75*5.18 = 25195.5200;
                // {
                //     obj->encoderValTarget = 65535;
                // }
                motorCtrlByPWM(MOTOR_AFT, 60);
                osDelay(200);
                motorCtrlByPWM(MOTOR_AFT, 0);
                printf("A1111111111111\r\n");
            }
            else if (cmd->data[2] == 0x02)
            {
                // obj->encoderValTarget -= step_value;
                // if(obj->encoderValCurrent < 100)
                // {
                //     obj->encoderValTarget = 100;
                // }
                motorCtrlByPWM(MOTOR_AFT, -60);
                osDelay(200);
                motorCtrlByPWM(MOTOR_AFT, 0);
                printf("A2222222222222\r\n");
            }
            printf("obj->encoderValTarget = %d\r\n",obj->encoderValTarget);
            cmd->len = 0x04;
            cmd->data[0] = 0x41;
            cmd->data[1] = 0x02;
            cmd->data[2] = obj->encoderValCurrent & 0xFF;
            cmd->data[3] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x05:
            cmd->len = 0x02;
            cmd->data[0] = obj->encoderValCurrent & 0xFF;
            cmd->data[1] = (obj->encoderValCurrent >> 8) & 0xFF;
            break;
        case 0x06:
            obj->presetPos = (cmd->data[0] << 8) | cmd->data[1];
            break;
        default:
            // ret = -1;
            break;
    }
    return ret;
    // return 0;
}    


static int8_t  AFC_ADCSampleSet_parse(struct AFC_object *cmd)//0x60
{
    int8_t ret = 0;
    switch (cmd->data[1])
    {
    case 0x01:
        break;
    case 0x05:
        cmd->len = 0x12;
        cmd->type = 0x02;
        cmd->data[0] = 0x60;
        cmd->data[1] = 0x05;
       // flash->read(flash, 0, &cmd->data[2], cmd->len - 2, 1000);
        // LOG_E("flash read len: %d\r\n", cmd->len);
        break;
    default:
        break;
    }   
   return ret;
}   
  

#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtc.h"
#include "init_call.h"
#include "ulog.h"

static int8_t AFC_command_frame_parse(struct AFC_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])   /* first cmd */
    {
    case 0x01:
        ret = AFC_ParaSet_parse(cmd);
        break;
    case 0x40:
        ret = AFC_MagMotorCmd_parse(cmd);
        break;
    case 0x41:
        ret = AFC_AFTMotorCmd_parse(cmd);//recv original: 16 bytes 55 aa 00 00 06 00 80 02 02 00 60 05 ea 71 0b fe 
        break;
    case 0x60:
        ret = AFC_ADCSampleSet_parse(cmd);
    case 0xEE:
        HAL_NVIC_SystemReset();   
        break;
    default:    
        ret = -1;
        break;
    }

    if (ret != 0)
    {
        printf("AFC uart command [%.2x, %.2x] parse err: %d\r\n", cmd->data[0], cmd->data[1], ret);
    }

    return ret;
}

static int8_t AFC_realtime_frame_parse(struct AFC_object *cmd)
{
    int8_t ret = 0;

    switch (cmd->data[0])
    {

    }

    return ret;
}

static int8_t AFC_cmd_parse(struct AFC_object *cmd)
{
    if (cmd == NULL)
    {
        LOG_E("cmd is NULL\r\n");
        return -1;
    }
    /* 1. check cmd id */
    
    if (cmd->id.bits.cmd_id != AFC_UART_ID)
    {
        return 0;
    }

    int8_t ret = 0;
   
    /* 2. parse cmd type */
    //LOG_I("AFC_cmd_parse  cmd->type: %02x\r\n", cmd->type);
    switch (cmd->type)
    {
    case 0x01:  /* handshake frame */
        ret = AFC_handshake_frame_parse(cmd);
        if (ret != 0)
        {
            LOG_E("AFC_handshake_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x02:  /* command frame */
        ret = AFC_command_frame_parse(cmd);
        LOG_I("AFC_command_frame_parse ret: %d\r\n", ret);
        if (ret != 0)
        {
            LOG_E("AFC_command_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    case 0x03:  /* realtime frame */
        ret = AFC_realtime_frame_parse(cmd);
        if (ret != 0)
        {
            LOG_E("AFC_realtime_frame_parse err: %d\r\n", ret);
            return -2;
        }
        break;
    default:
        LOG_E("invalid cmd type: %d\r\n", cmd->type);
        return -2;
        break;
    }

    if (cmd->id.bits.cmd_ack != 0)  /* need ack */
    {
        cmd->id.bits.cmd_ack = 0;

        cmd->type |= 0x80;
        //LOG_I("414AFC_cmd_parse  cmd->type: %02x\r\n", cmd->type);
        ret = AFC_uart_cmd_write(cmd);
        if (ret != 0)
        {
            LOG_E("AFC uart cmd write err: %d\r\n", ret);
        }
    }

    return ret;
}



static int8_t AFC_uart_send_entry(void *argument)
{
    int8_t ret = 0;
    struct AFC_uart send_buf = {0};

    for (;;)
    {
        osMessageQueueGet(AFC_uart_send_queue, &send_buf, NULL, osWaitForever);

#if 1
        LOG_I("TEST afc send_buf len: %d\r\n", send_buf.len);
        for (uint8_t i = 0; i < send_buf.len; i++)
        {
            LOG_I("%02x ", send_buf.buf[i]);
        }
        LOG_I("\r\n");
#endif

        ret = device_AFC_uart_data_write(&send_buf, send_buf.len, 1000);
        if (ret != 0)
        {
            LOG_E("device_AFC_uart_data_write err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t AFC_uart_cmd_process(struct AFC_uart *buf)
{
    if (buf == NULL)
    {
        return -1;
    }

#if 0
    LOG_I("test len: %d\r\n", buf->len);
    for (uint8_t i = 0; i < buf->len; i++)
    {
        LOG_I("%02x ", buf->buf[i]);
    }
    LOG_I("\r\n");
#endif

    struct AFC_object cmd = {0};
    memcpy(&cmd, buf->buf + 14, sizeof(struct AFC_object));
    cmd.data = &buf->buf[sizeof(struct AFC_object) - sizeof(uint8_t *) + 14];


    // LOG_I("cmd.id.byte: %02x\r\n", cmd.id.byte);
    // LOG_I("cmd.id.bits.cmd_id: %02x\r\n", cmd.id.bits.cmd_id);
    // LOG_I("cmd.id.bits.cmd_ack: %02x\r\n", cmd.id.bits.cmd_ack);
    // LOG_I("cmd.type: %02x\r\n", cmd.type);
    // LOG_I("cmd.len: %d\r\n", cmd.len);
    // LOG_I("cmd.data: ");

    // for (uint16_t i = 0; i <  cmd.len; i++)
    // {
    //     LOG_I("%02x ", cmd.data[i]);
    // }
    // LOG_I("\r\n");

    return AFC_cmd_parse(&cmd);
}

static int8_t AFC_uart_init(void)
{
    int8_t ret = device_AFC_uart_init(DEVICE_AFC_UART_NAME_DEFAULT);
    if (ret != 0)
    {
        LOG_E("device_AFC_uart_init err: %d\r\n", ret);
        return -1;
    }

    ret = device_AFC_uart_open();
    if (ret != 0)
    {
        LOG_E("device_AFC_uart_open err: %d\r\n", ret);
        return -2;
    }

    return 0;
}

static int8_t AFC_uart_recv_entry(void *argument)
{
    int8_t ret = 0;
    struct AFC_uart recv_buf = {0};

    ret = AFC_uart_init();
    if (ret != 0)
    {
        LOG_E("AFC_uart_init err: %d\r\n", ret);
        return -1;
    }

    for (;;)
    {
        ret = device_AFC_uart_data_read(&recv_buf, osWaitForever);
        if (ret != 0)
        {
            LOG_E("device_AFC_uart_data_read err: %d\r\n", ret);
            continue;
        }

        ret = AFC_uart_cmd_process(&recv_buf);
        if (ret!= 0)
        {
            LOG_E("AFC_uart_cmd_process err: %d\r\n", ret);
        }
    }

    return 0;
}

static int8_t AFC_uart_thread_init(void)
{
    osThreadAttr_t thread_recv_attr = {
    .name = "AFC_uart_recv_thread",
    .stack_size = 2048 * 4,
    .priority = osPriorityAboveNormal,
    };

    osThreadId_t thread_id = osThreadNew(AFC_uart_recv_entry, NULL, &thread_recv_attr);
    if (thread_id == NULL)
    {
        LOG_E("thread AFC uart create failed\r\n");
        return -1;
    }

    AFC_uart_send_queue = osMessageQueueNew(5, sizeof(struct AFC_uart), NULL);
    if (AFC_uart_send_queue == NULL)
    {
        LOG_E("message queue create failed\r\n");
        return -2;
    }

    osThreadAttr_t thread_send_attr = {
    .name = "AFC_uart_send_thread",
    .stack_size = 1024 * 4,
    .priority = osPriorityAboveNormal,
    };

    thread_id = osThreadNew(AFC_uart_send_entry, NULL, &thread_send_attr);
    if (thread_id == NULL)
    {
        LOG_E("thread AFC uart create failed\r\n");
        return -3;
    }

    osMutexAttr_t mutex_attributes = {
    .name = "data_mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit
    };


    return 0;
}
INIT_APP_EXPORT(AFC_uart_thread_init);


