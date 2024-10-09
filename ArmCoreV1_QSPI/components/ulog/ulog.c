/**
MIT License

Copyright (c) 2019 R. Dunbar Poor <rdpoor@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * \file ulog.c
 *
 * \brief uLog: lightweight logging for embedded systems
 *
 * See ulog.h for sparse documentation.
 */

#include "ulog.h"
#include "cmsis_os2.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "init_call.h"

// =============================================================================
// types and definitions

typedef struct {
  ulog_function_t fn;
  ulog_level_t threshold;
} subscriber_t;

// =============================================================================
// local storage

static subscriber_t s_subscribers[ULOG_MAX_SUBSCRIBERS];
static char s_message[ULOG_MAX_MESSAGE_LENGTH];

// =============================================================================
// user-visible code

/**
 * @description:
 * @param {ulog_level_t} severity
 * @param {char} *msg
 * @return {*}
 */
void log_printf(ulog_level_t severity, char *msg)
{
#ifdef CFG_PLATFORM_SIMULATOR
    uint32_t tick_pre_second = osKernelGetTickFreq();
    uint32_t ostick = osKernelGetTickCount();
    time_t time_s_cur =  ostick / tick_pre_second;
    uint32_t time_ms_left = ostick % tick_pre_second;

    struct tm tm_temp, *tm;
    
    tm = localtime_r(&time_s_cur, &tm_temp);

    printf("[%04u-%02u-%02u %02u:%02u:%02u.%03u] [%s]: %s",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, time_ms_left,
            ulog_level_name(severity),
            msg);
#endif
}

/**
 * @description:
 * @return {*}
 */
void ulog_init(ulog_level_t threshold) {
    memset(s_subscribers, 0, sizeof(s_subscribers));
    ulog_subscribe(log_printf, threshold);
}

ulog_err_t ulog_level_set(ulog_level_t threshold)
{
    int i;
    for (i=0; i<ULOG_MAX_SUBSCRIBERS; i++) {
        if (s_subscribers[i].fn == log_printf) {
            // already subscribed: update threshold and return immediately.
            s_subscribers[i].threshold = threshold;
        }
    }

    return ULOG_ERR_NONE;
}
ulog_level_t ulog_level_get(ulog_function_t fn)
{
    int i;
    for (i=0; i<ULOG_MAX_SUBSCRIBERS; i++) {
        if (s_subscribers[i].fn == log_printf) {
            return s_subscribers[i].threshold;
        }
    }

    return ULOG_ERR_SUBSCRIBERS_EXCEEDED;
}


// search the s_subscribers table to install or update fn
ulog_err_t ulog_subscribe(ulog_function_t fn, ulog_level_t threshold) {
    int available_slot = -1;
    int i;
    for (i=0; i<ULOG_MAX_SUBSCRIBERS; i++) {
        if (s_subscribers[i].fn == fn) {
            // already subscribed: update threshold and return immediately.
            s_subscribers[i].threshold = threshold;
            return ULOG_ERR_NONE;

        } else if (s_subscribers[i].fn == NULL) {
            // found a free slot
            available_slot = i;
        }
    }
    // fn is not yet a subscriber.  assign if possible.
    if (available_slot == -1) {
        return ULOG_ERR_SUBSCRIBERS_EXCEEDED;
    }
    s_subscribers[available_slot].fn = fn;
    s_subscribers[available_slot].threshold = threshold;
    return ULOG_ERR_NONE;
}

// search the s_subscribers table to remove
ulog_err_t ulog_unsubscribe(ulog_function_t fn) {
    int i;
    for (i=0; i<ULOG_MAX_SUBSCRIBERS; i++) {
        if (s_subscribers[i].fn == fn) {
            s_subscribers[i].fn = NULL;    // mark as empty
            return ULOG_ERR_NONE;
        }
    }
    return ULOG_ERR_NOT_SUBSCRIBED;
}

const char *ulog_level_name(ulog_level_t severity) {
    switch(severity) {
        case ULOG_TRACE_LEVEL: return "TRACE";
        case ULOG_DEBUG_LEVEL: return "DEBUG";
        case ULOG_INFO_LEVEL: return "INFO";
        case ULOG_WARNING_LEVEL: return "WARNING";
        case ULOG_ERROR_LEVEL: return "ERROR";
        case ULOG_CRITICAL_LEVEL: return "CRITICAL";
        case ULOG_ALWAYS_LEVEL: return "ALWAYS";
        default: return "UNKNOWN";
    }
}

void ulog_message(ulog_level_t severity, const char *fmt, ...) {
    va_list ap;
    int i;
    va_start(ap, fmt);
    vsnprintf(s_message, ULOG_MAX_MESSAGE_LENGTH, fmt, ap);
    va_end(ap);

    for (i=0; i<ULOG_MAX_SUBSCRIBERS; i++) {
        if (s_subscribers[i].fn != NULL) {
            if (severity >= s_subscribers[i].threshold) {
                s_subscribers[i].fn(severity, s_message);
            }
        }
    }

}

/***************************************** private code below ******************************************************/
static struct ulog_write_func_info func_info[ULOG_MAX_SUBSCRIBERS] = {NULL};

int8_t ulog_write_func_register(struct ulog_write_func_info *func)
{
    if (func == NULL)
    {
        return -1;
    }

    if (func->index >= ULOG_MAX_SUBSCRIBERS)
    {
        return -2;
    }

    memcpy(&func_info[func->index], func, sizeof(struct ulog_write_func_info));

    return 0;
}

#ifdef USING_ULOG_THREAD
static osMessageQueueId_t ulog_output_queueHandle = NULL;
#endif

static void ulog_output(ulog_level_t severity, char *msg)
{
    uint32_t tick_pre_second = osKernelGetTickFreq();
    uint32_t ostick = osKernelGetTickCount();
    time_t time_s_cur =  ostick / tick_pre_second;
    uint32_t time_ms_left = ostick % tick_pre_second;

    uint8_t msg_buf[ULOG_MAX_MESSAGE_LENGTH] = {0};

#ifdef USING_ULOG_TIMESTAMP
    struct tm tm_temp, *tm;
    
    tm = localtime_r(&time_s_cur, &tm_temp);

    sprintf(msg_buf, "[%04u-%02u-%02u %02u:%02u:%02u.%03u] ",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, time_ms_left);
#endif

#ifdef USING_ULOG_LEVEL_TAG
    sprintf(msg_buf + strlen(msg_buf), "[%s] %s", ulog_level_name(severity), msg);
#else
    sprintf(msg_buf + strlen(msg_buf), "%s", msg);
#endif

#ifdef USING_ULOG_THREAD
    osStatus_t stat = osMessageQueuePut (ulog_output_queueHandle, msg_buf, 0, 1000);
    if (stat != osOK)
    {
        printf("ulog output queue put err:%d\r\n", stat);
    }
#else
    /* 2. callback function process */
    for (uint8_t i = 0; i < sizeof(func_info)/sizeof(func_info[0]); i++)
    {
        if (func_info[i].func_callback == NULL)
        {
            continue;
        }

        switch (func_info[i].index)
        {
        case 0: /* Console */
            func_info[i].func_callback(msg_buf, strlen(msg_buf));
            break;
        
        default:
            func_info[i].func_callback(msg_buf, strlen(msg_buf) + 1);  /* add '\0' at the end, to separate with other log */
            break;
        }
    }
#endif
}

#ifndef USING_ULOG_THREAD
static int8_t ulog_component_init(void)
{
    // ulog_init(ULOG_DEBUG_LEVEL);

    /* TODO: here need read flash, which indicate log info, but depend on os event, so modify logic if needed */

    memset(s_subscribers, 0, sizeof(s_subscribers));
    ulog_subscribe(ulog_output, ULOG_DEBUG_LEVEL);   /* register callback function */
    
    /* 1. callback function init */
    for (uint8_t i = 0; i < sizeof(func_info)/sizeof(func_info[0]); i++)
    {
        if (func_info[i].func_init != NULL)
        {
            func_info[i].func_init();
        }
    }

    return 0;
}
INIT_COMPONENT_EXPORT(ulog_component_init);
#else
static void ulog_output_entry(void *argument)
{
    uint8_t log_buf[ULOG_MAX_MESSAGE_LENGTH] = {0};

    /* 1. callback function init */
    for (uint8_t i = 0; i < sizeof(func_info)/sizeof(func_info[0]); i++)
    {
        if (func_info[i].func_init != NULL)
        {
            func_info[i].func_init();
        }
    }

    while (1)
    {
        osMessageQueueGet (ulog_output_queueHandle, log_buf, 0, osWaitForever);

        /* 2. callback function process */
        for (uint8_t i = 0; i < sizeof(func_info)/sizeof(func_info[0]); i++)
        {
            if (func_info[i].func_callback == NULL)
            {
                continue;
            }

            switch (func_info[i].index)
            {
            case 0: /* Console */
                func_info[i].func_callback(log_buf, strlen(log_buf));
                break;
            
            default:
                func_info[i].func_callback(log_buf, strlen(log_buf) + 1);  /* add '\0' at the end, to separate with other log */
                break;
            }
        }
    }
}

static int8_t ulog_thread_init(void)
{
    osThreadAttr_t ulog_output_thread_attributes = {
    .name = "ulog_output_thread",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityBelowNormal,
    };

    osThreadId_t ulog_output_threadHandle = osThreadNew(ulog_output_entry, NULL, &ulog_output_thread_attributes);
    if (ulog_output_threadHandle == NULL)
    {
        printf("thread ulog output create failed\r\n");
        return -1;
    }

    ulog_output_queueHandle = osMessageQueueNew (16, ULOG_MAX_MESSAGE_LENGTH, NULL);
    if (ulog_output_queueHandle == NULL)
    {
        printf("queue ulog output create failed\r\n");
        return -1;
    }

    memset(s_subscribers, 0, sizeof(s_subscribers));
    ulog_subscribe(ulog_output, ULOG_DEBUG_LEVEL);   /* register callback function */

    return 0;
}
INIT_APP_EXPORT(ulog_thread_init);

#ifdef ULOG_TEST
#include "shell.h"
void ulog_test(uint8_t argc, char **argv)
{
    for(uint16_t i = 0; i < 500; i++)
    {
        LOG_I("ulog test:%d\r\n", atoi(argv[1]) + i);
    }
}
MSH_CMD_EXPORT_ALIAS(ulog_test, ulog_test, ulog test);

#endif
#endif
