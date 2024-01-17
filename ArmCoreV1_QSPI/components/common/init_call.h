#ifndef __INIT_CALL_H__
#define __INIT_CALL_H__

#ifdef __cplusplus
extern "C" {
#endif

struct init_desc
{
    const char* fn_name;
    const int (*init_fn)(void);
};

#define INIT_EXPORT(fn, level)                                                                          \
    const char __init_call_name_##fn[] = #fn;                                                         \
    const struct init_desc __init_call_desc_##fn __attribute__((section(".init_call_fn." #level))) =    \
    { __init_call_name_##fn, fn};


#define INIT_BOARD_EXPORT(fn)           INIT_EXPORT(fn, 1)
#define INIT_PREV_EXPORT(fn)            INIT_EXPORT(fn, 2)
#define INIT_DEVICE_EXPORT(fn)          INIT_EXPORT(fn, 3)
#define INIT_COMPONENT_EXPORT(fn)       INIT_EXPORT(fn, 4)
#define INIT_ENV_EXPORT(fn)             INIT_EXPORT(fn, 5)
#define INIT_APP_EXPORT(fn)             INIT_EXPORT(fn, 6)



#ifdef __cplusplus
}
#endif

#endif