#ifndef __SYS_LOG_H__
#define __SYS_LOG_H__

#include "fmc_sdram_port.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_LOG_READ_WRITE_TIMEOUT  1000    /* timeout for read/write operation：1s */
#define SYS_LOG_OVERWRITE_MODE              /* overwrite the oldest log when the log region is full */

#define SYS_LOG_PER_LENGTH  (80)            /* length of each log：80B, must be 4 byte alignment */
#define SYS_LOG_ADDR_BEGIN  (0x3C00000)     /* address offset of system log：60MB */
#define SYS_LOG_SIZE        (1024 * 1024 * 4)   /* size of system log：4MB */
#define SYS_LOG_ADDR_END    (SYS_LOG_ADDR_BEGIN + SYS_LOG_SIZE)

#ifdef __cplusplus
}
#endif
#endif /* __SYS_LOG_H__ */