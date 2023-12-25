#ifndef __FINSH_CONFIG_H__
#define __FINSH_CONFIG_H__


/* Command shell */
#define RT_USING_FINSH
// #define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
// #define FINSH_USING_MSH_ONLY
#define FINSH_ARG_MAX 10
#define RT_USING_HEAP
#define RT_CONSOLEBUF_SIZE 10


typedef int             rt_bool_t;      /**< boolean type */
typedef unsigned int    size_t;
typedef size_t          rt_size_t;      /**< Type for size number */
// typedef rt_ubase_t      rt_size_t;      /**< Type for size number */

typedef signed long                     rt_base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   rt_ubase_t;     /**< Nbit unsigned CPU related data type */
typedef rt_base_t                       rt_err_t;       /**< Type for error number */

typedef signed   char                   rt_int8_t;      /**<  8bit integer type */
typedef signed   short                  rt_int16_t;     /**< 16bit integer type */
typedef signed   int                    rt_int32_t;     /**< 32bit integer type */
typedef unsigned char                   rt_uint8_t;     /**<  8bit unsigned integer type */
typedef unsigned short                  rt_uint16_t;    /**< 16bit unsigned integer type */
typedef unsigned int                    rt_uint32_t;    /**< 32bit unsigned integer type */

/* boolean type definitions */
#define RT_TRUE                         1               /**< boolean true  */
#define RT_FALSE                        0               /**< boolean fails */

/* null pointer definition */
#define RT_NULL                         0

#define rt_section(x)               __attribute__((section(x)))
#define rt_used                     __attribute__((used))
#define rt_inline                   static __inline

// #define printf(x)       printf(x)
#define RT_ASSERT(EX)                                                         \
if (!(EX))                                                                    \
{                                                                             \
    printf("assert err\r\n");                                                 \
}

#define rt_memset  memset

/* RT-Thread error code definitions */
#define RT_EOK                          0               /**< There is no error */
#define RT_ERROR                        1               /**< A generic error happens */
#define RT_ETIMEOUT                     2               /**< Timed out */
#define RT_EFULL                        3               /**< The resource is full */
#define RT_EEMPTY                       4               /**< The resource is empty */
#define RT_ENOMEM                       5               /**< No memory */
#define RT_ENOSYS                       6               /**< No system */
#define RT_EBUSY                        7               /**< Busy */
#define RT_EIO                          8               /**< IO error */
#define RT_EINTR                        9               /**< Interrupted system call */
#define RT_EINVAL                       10              /**< Invalid argument */
#define RT_ETRAP                        11              /**< Trap event */
#define RT_ENOENT                       12              /**< No entry */
#define RT_ENOSPC                       13              /**< No space left */

#endif