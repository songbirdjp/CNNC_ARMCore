#ifndef __FLASH_PORT_H__
#define __FLASH_PORT_H__



#ifdef __cplusplus
extern "C" {
#endif


/* NOTE：bank1 and bank2 correspond to 2 devices */
#define DEVICE_NAME_FLASH_BANK1     "bank1"
#define DEVICE_NAME_FLASH_BANK2     "bank2"


/*
* bank1:
* sector 0: 0x08000000 - 0x0801FFFF (128k)  -> bootloader 1
* sector 1: 0x08020000 - 0x0803FFFF (128k)  -> bootloader 2
* sector 2: 0x08040000 - 0x0805FFFF (128k)  -> application
* sector 3: 0x08060000 - 0x0807FFFF (128k)  -> reserved
* sector 4: 0x08080000 - 0x0809FFFF (128k)  -> reserved
* sector 5: 0x080A0000 - 0x080BFFFF (128k)  -> config data, such as log info, etc.
* sector 6: 0x080C0000 - 0x080DFFFF (128k)  -> log 1
* sector 7: 0x080E0000 - 0x080FFFFF (128k)  -> log 2

* bank2: not support in stm32h723
* sector 0: 0x08100000 - 0x0811FFFF (128k)  -> reserved
* sector 1: 0x08120000 - 0x0813FFFF (128k)  -> reserved
* sector 2: 0x08140000 - 0x0815FFFF (128k)  -> reserved
* sector 3: 0x08160000 - 0x0817FFFF (128k)  -> reserved
* sector 4: 0x08180000 - 0x0819FFFF (128k)  -> reserved
* sector 5: 0x081A0000 - 0x081BFFFF (128k)  -> reserved
* sector 6: 0x081C0000 - 0x081DFFFF (128k)  -> reserved
* sector 7: 0x081E0000 - 0x081FFFFF (128k)  -> reserved
*/

int8_t device_flash_erase(uint8_t *device_name, uint32_t offset, uint32_t size);
int8_t device_flash_read(uint8_t *device_name, uint32_t offset, uint8_t *buf, uint32_t len, uint32_t timeout);
int8_t device_flash_write(uint8_t *device_name, uint32_t offset, uint8_t *buf, uint32_t len, uint32_t timeout);


#ifdef __cplusplus
}
#endif

#endif /* __FLASH_PORT_H__ */