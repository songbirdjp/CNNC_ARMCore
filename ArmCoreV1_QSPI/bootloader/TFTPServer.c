#include "wizchip_conf.h"
#include "socket.h"
#include "drv_flash.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "init_call.h"
#include "flash_port.h"
#include <stdbool.h>
#include "stm32h7xx_hal.h"

#define TFTP_PORT 69
#define BUFFER_SIZE 512
#define APP_START_ADDRESS 0x08040000
#define FLASH_END_ADDRESS 0x080FFFFF // 根据MCU的具体Flash大小调整
void configure_socket_buffers() {
    // 设置Socket 0的缓冲区大小
    setSn_TXBUF_SIZE(0, 2);  // 2 KB
    setSn_RXBUF_SIZE(0, 2);  // 2 KB

    // 设置Socket 1的缓冲区大小
    setSn_TXBUF_SIZE(1, 0);  // 0 KB
    setSn_RXBUF_SIZE(1, 2);  // 2 KB

    // 设置Socket 2的缓冲区大小
    setSn_TXBUF_SIZE(2, 0);  // 0 KB
    setSn_RXBUF_SIZE(2, 0);  // 0 KB

    // 设置Socket 3的缓冲区大小
    setSn_TXBUF_SIZE(3, 0);  // 0 KB
    setSn_RXBUF_SIZE(3, 0);  // 0 KB

    // 设置Socket 4的缓冲区大小
    setSn_TXBUF_SIZE(4, 2);  // 6 KB
    setSn_RXBUF_SIZE(4, 2);  // 6 KB

    // 设置Socket 5的缓冲区大小
    setSn_TXBUF_SIZE(5, 0);  // 6 KB
    setSn_RXBUF_SIZE(5, 0);  // 6 KB

    // 设置Socket 6的缓冲区大小
    setSn_TXBUF_SIZE(6, 4);  // 0 KB
    setSn_RXBUF_SIZE(6, 4);  // 0 KB

    // 设置Socket 7的缓冲区大小
    setSn_TXBUF_SIZE(7, 3);  // 0 KB
    setSn_RXBUF_SIZE(7, 0);  // 0 KB
}
// 发送ACK数据包，带有块编号
void send_ack(int sock, uint16_t block_number, uint8_t *destip, uint16_t destport) 
{
    uint8_t ack_packet[4];
    ack_packet[0] = 0x00; // TFTP ACK opcode high byte
    ack_packet[1] = 0x04; // TFTP ACK opcode low byte
    ack_packet[2] = (block_number >> 8) & 0xFF; // Block number high byte
    ack_packet[3] = block_number & 0xFF; // Block number low byte
    int ret = sendto(sock, ack_packet, sizeof(ack_packet), destip, destport);
    if (ret < 0) 
    {
        printf("Failed to send ACK = %d\n", ret);
    }
    else
    {
        printf("sendto success = %d\n", ret);
    }
}
#define FLASH_ADDRESS_BASE  (FLASH_BASE + FLASH_SECTOR_SIZE * 6)
#define FLASH_VALID_SIZE    (FLASH_SECTOR_SIZE * 2)
// 检查是否是写请求（WRQ）
bool is_write_request(uint8_t *buffer) {
    return buffer[0] == 0x00 && buffer[1] == 0x02; // WRQ opcode
}

static DEVICE_FLASH flash_bank1 = {0};
static DEVICE_FLASH *device_flash_get(void)
{
    return &flash_bank1;
}
static void TFTPServer_thread_entry(void *argument) 
{
    uint8_t buffer[BUFFER_SIZE];
    int32_t len;
    uint16_t block_number = 0; // 初始化块编号
    int8_t sock;
    uint8_t client_ip[4];
    uint16_t client_port;    
    uint32_t flash_addr = FLASH_ADDRESS_BASE;
    uint32_t flash_cfg[2] = {FLASH_ADDRESS_BASE, FLASH_VALID_SIZE};
    DEVICE_FLASH *flash = device_flash_get();
     int8_t session_sock;
    flash_init(flash, "DEVICE_NAME_FLASH_BANK1");
    flash_operation_address_set(flash, flash_cfg[0], flash_cfg[1]);
    flash->ioctl(flash, FLASH_CMD_ERASE_SECTOR, (void *)flash_cfg);
    osDelay(5000);
    configure_socket_buffers();
    sock = socket(4, Sn_MR_UDP, TFTP_PORT, 0);
    if (sock < 0) {
        printf("Failed to open socket on port %d\n", TFTP_PORT);
        return;
    }
    printf("Socket 4 current state: 0x%x\n", getSn_SR(4));
    
    
    while (1) 
    {
        len = recvfrom(4, buffer, BUFFER_SIZE, client_ip, &client_port);
        if (len > 0) 
        {   
            // printf("len: %d\n",len);
            if (is_write_request(buffer)) 
            {
                session_sock = socket(6, Sn_MR_UDP, 1025, 0); // 动态分配端口
                if (session_sock < 0) {
                    printf("Failed to open session socket\n");
                    continue;
                }
                send_ack(session_sock, block_number, client_ip, client_port);
            }
            while (1) 
            {
                len = recvfrom(session_sock, buffer, BUFFER_SIZE, NULL, NULL);
                if (len > 0) 
                {
                if (flash_addr + len > FLASH_END_ADDRESS)
                {
                    printf("Flash address out of range\n");
                    break;
                }
                flash->write(flash, 0, buffer, len, 1000);
                flash_addr += len;
                send_ack(session_sock, ++block_number, client_ip, client_port); // 发送当前块编号的ACK
                     
            }
            else if (len < 0) 
            {
                printf("Error receiving data, len: %d. Retrying...\n", len);
                printf("Retrying block number: %d\n", block_number);
                socket(6, Sn_MR_UDP,1025, 0); 
                continue; // 继续尝试接收数据
            }
            else 
            {
                // printf("Error receiving data, len: %d. Retrying...\n", len);
                // printf("Retrying block number: %d\n", block_number);
                // socket(5, Sn_MR_UDP,1025, 0); 
                // continue; // 继续尝试接收数据
                printf("No more data, len: %d, block_number: %d\n", len, block_number);
                printf("Socket 4 current state: 0x%x\n", getSn_SR(4));
                printf("Socket 2 current state: 0x%x\n", getSn_SR(2));
                break; // 退出循环
                }
            }
            //close(session_sock); // 关闭会话socket
        }
        osDelay(1); // 延迟以降低CPU占用
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
// INIT_APP_EXPORT(TFTPServer_thread_init);

