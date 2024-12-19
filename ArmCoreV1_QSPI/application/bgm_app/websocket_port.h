#ifndef __WEBSOCKET_PORT_H__
#define __WEBSOCKET_PORT_H__

#include "websocket.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TAG_CALI_DATA_SETTING   0x0010
#define TAG_PLAN_DATA_SETTING   0x0020
#define TAG_PLAN_DATA_CLEAR     0x0021
#define TAG_BGM_ARM_CMD         0x0030
#define TAG_BGM_AFC_CMD         0x0031
#define TAG_BGM_DOSE1_CMD       0x0032
#define TAG_BGM_DOSE2_CMD       0x0033




int8_t websocket_cmd_parse(APP_DATA_RECV *info);


#ifdef __cplusplus
}
#endif

#endif /* __WEBSOCKET_PORT_H__ */