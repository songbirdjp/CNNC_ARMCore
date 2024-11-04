#include "NetBootloader.h"
#include "bootloader.h"
#include "socket.h"
#include "drv_flash.h"
#include <stdio.h>
#include <string.h>
#include "init_call.h"
#include "main.h"
#include <stdint.h>
#include "cmsis_os2.h"
#include "flash_port.h"


// 定义应用程序的起始地址
#define APP_START_ADDRESS 0x08040000
typedef void (*pFunction)(void);

void jump_to_application(void) {
    // 定义一个函数指针，指向应用程序的复位处理程序
    pFunction application_entry;
    
    // 获取应用程序的堆栈指针（SP）和复位处理程序地址
    uint32_t app_stack_pointer = *(volatile uint32_t *)APP_START_ADDRESS;
    uint32_t app_reset_handler = *(volatile uint32_t *)(APP_START_ADDRESS + 4);

    // 设置主堆栈指针（MSP）
    __set_MSP(app_stack_pointer);

    // 设置程序计数器（PC）为应用程序的复位处理程序地址
    application_entry = (pFunction)app_reset_handler;
    application_entry();
}

// 初始化网络引导加载程序
int8_t NetBootloader_Init(wiz_NetInfo *net_info) {
    // 初始化W5500芯片
    if (device_w5500_init(net_info, "W5500") != 0) {
        printf("W5500 init failed\n");
        return -1;
    }
    return 0;
}
#
// 接收程序数据并写入闪存
int8_t NetBootloader_ReceiveAndFlash(void) {
    uint8_t buffer[1024];
    int32_t len;
    uint32_t flash_addr = APP_START_ADDRESS;

    // 打开UDP socket
    int8_t sock = socket(0, Sn_MR_UDP, 69, 0); // 使用TFTP端口69
    if (sock < 0) {
        printf("Socket open failed\n");
        return -1;
    }

    // // 初始化闪存
    // if (device_flash_init() != 0) {
    //     printf("Flash init failed\n");
    //     return -1;
    // }

    // 接收数据并写入闪存
    while ((len = recvfrom(sock, buffer, sizeof(buffer), NULL, NULL)) > 0) {
        if (device_flash_write(DEVICE_NAME_FLASH_BANK1, flash_addr, buffer, len, 1000) != 0) {
            printf("Flash write failed\n");
            return -1;
        }
        flash_addr += len;
    }

    close(sock);
    return 0;
}

int8_t NetBootloader_VerifyAndJump(void) {
    // 校验程序完整性
    // if (verify_program_crc() != 0) {
    //     printf("Program verification failed\n");
    //     return -1;
    // }
   jump_to_application();
}

static void NetBootloader_thread_entry(void *argument)
{
      wiz_NetInfo net_info = {
        .mac = {0x00, 0x08, 0xdc, 0x00, 0x00, 0x00},
        .ip = {192, 168, 1, 100},
        .sn = {255, 255, 255, 0},
        .gw = {192, 168, 1, 1},
        .dns = {8, 8, 8, 8},
        .dhcp = NETINFO_STATIC
    };
    
    // if (NetBootloader_Init(&net_info) != 0) {
    //     printf("NetBootloader init failed\n");
    //     // osThreadExit();
    // }
    // // 接收和烧录程序
    // if (NetBootloader_ReceiveAndFlash() != 0) {
    //     printf("Receive and flash failed\n");
    //     osThreadExit();
    // }
    // // 校验和跳转到应用程序
    // if (NetBootloader_VerifyAndJump() != 0) {
    //     printf("Verify and jump failed\n");
    //     osThreadExit();
    // }
    for(;;)
    {
        osDelay(100);
    }
}

static int8_t NetBootloader_thread_init(void)
{
    osThreadAttr_t NetBootloader_thread_attributes = {
    .name = "NetBootloader",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityNormal,
    };
    osThreadId_t NetBootloader_threadHandle = osThreadNew(NetBootloader_thread_entry, NULL, &NetBootloader_thread_attributes);
    if (NetBootloader_threadHandle == NULL)
    {
        printf("thread NetBootloader create failed\r\n");
        return -1;
    }
    return 0;
}
// INIT_APP_EXPORT(NetBootloader_thread_init);
