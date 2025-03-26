#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


int8_t timestamp_ns_set(uint64_t timestamp_ns);
uint64_t timestamp_ns_get(void);



#ifdef __cplusplus
}
#endif

#endif /* __TIMESTAMP_H__ */