#ifndef __WEBSOCKET_CONSOLE_H__
#define __WEBSOCKET_CONSOLE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t *get_websocket_console_sn(void);
int8_t websocket_cmd_parse(uint8_t sn, uint8_t *buf, uint16_t len);


#ifdef __cplusplus
}
#endif

#endif /* __WEBSOCKET_CONSOLE_H__ */