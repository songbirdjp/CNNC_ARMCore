#ifndef NET_BOOTLOADER_H
#define NET_BOOTLOADER_H

#include <stdint.h>
#include "wizchip_conf.h"

// 定义网络引导加载程序的接口
int8_t NetBootloader_Init(wiz_NetInfo *net_info);
int8_t NetBootloader_ReceiveAndFlash(void);
int8_t NetBootloader_VerifyAndJump(void);

#endif // NET_BOOTLOADER_H