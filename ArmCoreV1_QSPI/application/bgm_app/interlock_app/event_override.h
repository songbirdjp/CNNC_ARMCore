#ifndef __EVENT_OVERRIDE_H__
#define __EVENT_OVERRIDE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum not_ready_event
{
    NOT_READY_EVENT_PLAN_DATA = 0,
    NOT_READY_EVENT_MAX,
};

enum interlock_event
{
    INTERLOCK_EVENT_PLAN_DATA,
    INTERLOCK_EVENT_MAX,
};

uint32_t not_ready_event_get(void);
int8_t not_ready_event_with_override_get(enum not_ready_event type);
int8_t interlock_with_override_get(enum interlock_event type);

#ifdef __cplusplus
}
#endif
#endif /* __EVENT_OVERRIDE_H__ */