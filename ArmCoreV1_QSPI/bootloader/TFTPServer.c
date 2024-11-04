#include "wizchip_conf.h"
#include "socket.h"
#include "drv_flash.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "init_call.h"
#include "flash_port.h"
#include <stdbool.h>

#define TFTP_PORT 69
#define BUFFER_SIZE 512
#define APP_START_ADDRESS 0x08040000
#define FLASH_END_ADDRESS 0x080FFFFF // 根据MCU的具体Flash大小调整
int8_t sockudp ;
uint8_t destip[4] = {192,168, 10, 80};
// 发送ACK数据包，带有块编号
void send_ack(int sock, uint16_t block_number) 
{
    uint8_t ack_packet[4];
    ack_packet[0] = 0x00; // TFTP ACK opcode high byte
    ack_packet[1] = 0x04; // TFTP ACK opcode low byte
    ack_packet[2] = (block_number >> 8) & 0xFF; // Block number high byte
    ack_packet[3] = block_number & 0xFF; // Block number low byte
    int ret = sendto(sockudp, ack_packet, sizeof(ack_packet), destip, 57931);
    if (ret < 0) 
    {
        printf("Failed to send ACK = %d\n", ret);
    }
    else
    {
        printf("sendto success = %d\n", ret);
    }
}

// 检查是否是写请求（WRQ）
bool is_write_request(uint8_t *buffer) {
    return buffer[0] == 0x00 && buffer[1] == 0x02; // WRQ opcode
}

// TFTP服务器线程入口
static void TFTPServer_thread_entry(void *argument) 
{
    uint8_t buffer[BUFFER_SIZE];
    int32_t len;
    uint32_t flash_addr = APP_START_ADDRESS;
    uint16_t block_number = 0; // 初始化块编号
    int8_t sock ;
    osDelay(5000);
    sock = socket( 4, Sn_MR_UDP, TFTP_PORT, 0);
    printf("Socket 4 current state: 0x%x\n",  getSn_SR(4));
    while (1) 
    {
        len = recvfrom(sock, buffer, BUFFER_SIZE, NULL, NULL);
        if (len > 0) 
        {   
            sockudp = socket( 5, Sn_MR_UDP, 60000, 0);
            printf("Socket 5 current state: 0x%x\n",  getSn_SR(5));
            // 处理TFTP请求和数据包
            if (is_write_request(buffer)) 
            {
                while ((len = recvfrom(sock, buffer, BUFFER_SIZE, NULL, NULL)) > 0) 
                {
                    // 检查Flash地址范围，避免越界
                    if (flash_addr + len > FLASH_END_ADDRESS) {
                        printf("Flash address out of range\n");
                        break;
                    }

                    // 写入Flash
                    if (device_flash_write(DEVICE_NAME_FLASH_BANK1, flash_addr, buffer, len, 1000) != 0) {
                        printf("Flash write failed\n");
                        break;
                    }

                    flash_addr += len;
                    send_ack(sockudp, block_number++); // 发送当前块编号的ACK
                }
            }
             send_ack(sockudp, block_number++); // 发送当前块编号的ACK
        }
       // printf("TFTP server started on port 69\n");
        osDelay(10); // 延迟以降低CPU占用
    }
}

// TFTP服务器线程初始化
static int8_t TFTPServer_thread_init(void) 
{
    osThreadAttr_t TFTPServer_thread_attributes = {
        .name = "TFTPServer",
        .stack_size = 2048 * 4,
        .priority = (osPriority_t) osPriorityNormal,
    };

    osThreadId_t TFTPServer_threadHandle = osThreadNew(TFTPServer_thread_entry, NULL, &TFTPServer_thread_attributes);
    if (TFTPServer_threadHandle == NULL) {
        printf("thread TFTPServer create failed\r\n");
        return -1;
    }
    return 0;
}
// 在系统初始化时启用TFTP服务器
INIT_APP_EXPORT(TFTPServer_thread_init);