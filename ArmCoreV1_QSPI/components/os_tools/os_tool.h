#ifndef __OS_TOOL_H__
#define __OS_TOOL_H__

#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif


uint8_t *os_tool_mutex_holder_get(osMutexId_t mutex_id, ...);

#ifdef __cplusplus
}
#endif
#endif /* __OS_TOOL_H__ */