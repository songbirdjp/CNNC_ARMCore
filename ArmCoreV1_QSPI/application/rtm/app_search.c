#include "app_search.h"
#include "ulog.h"
enum search_state_machine
{
    SEARCH_STATE_INIT = 0,
    SEARCH_STATE_DELAY,
    SEARCH_STATE_IDLE,
    SEARCH_STATE_WAIT,
    SEARCH_STATE_START,
    SEARCH_STATE_SEARCH,
    SEARCH_STATE_END
};
typedef struct app_search_t
{
    uint8_t search_state_machine;
    app_dido_t *app_dido;
    osTimerId_t search_osTimerId;
    uint32_t search_timeout_tick;
} app_search_t;

static app_search_t app_search;
void search_timeout_handle(void *argument)
{
    app_search_t *self = (app_search_t *)argument;
    self->search_state_machine = SEARCH_STATE_IDLE;
    dido_structure_t dido_structure = {0};
    app_di_get(app_search.app_dido, &dido_structure);
    app_do_get(app_search.app_dido, &dido_structure);

    dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_SearchTreatmentRoomRelay = 0;

    app_do_set(app_search.app_dido, &dido_structure);
    LOG_I("search timeout!\r\n");
    // search_state_machine(self->app_dido);
}
int32_t search_timeout_set(uint32_t timeout_tick)
{
    if(timeout_tick < SEARCH_MIN_TIMEOUT_MS)
    {
        timeout_tick = SEARCH_MIN_TIMEOUT_MS;
    }
    app_search.search_timeout_tick = timeout_tick;
    return 0;
}
static uint32_t search_timeout_get(void)
{

    if(app_search.search_timeout_tick < SEARCH_MIN_TIMEOUT_MS)
    {
        return SEARCH_MIN_TIMEOUT_MS;
    }
    return app_search.search_timeout_tick;
}
/**
 * @brief
 *
 * @param app_rtm
 * @return int32_t
 */
int32_t search_state_machine(app_dido_t *app_dido)
{
    osStatus_t osStatus = osError;
    dido_structure_t dido_structure = {0};
    app_search.app_dido = app_dido;
    app_di_get(app_search.app_dido, &dido_structure);
    app_do_get(app_search.app_dido, &dido_structure);
    static uint32_t last_tick = 0;
    uint32_t current_tick = 0;
    switch (app_search.search_state_machine)
    {
    case SEARCH_STATE_INIT:
    {
        osTimerAttr_t search_osTimerAttr = {
            .name = "search_timeout"};
        app_search.search_osTimerId = osTimerNew(search_timeout_handle,
                                                 osTimerOnce,
                                                 &app_search,
                                                 &search_osTimerAttr);
        if (app_search.search_osTimerId == NULL)
        {
            LOG_I("search timeout timer create fail!\r\n");
            return -1;
        }
        app_search.search_state_machine = SEARCH_STATE_DELAY;
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_SearchTreatmentRoomRelay = 0;
        app_do_set(app_search.app_dido, &dido_structure);
        last_tick = osKernelGetTickCount();
    }
    break;
    case SEARCH_STATE_DELAY:
    {
        current_tick = osKernelGetTickCount();
        if(current_tick - last_tick >= 100)
        {
            app_search.search_state_machine = SEARCH_STATE_IDLE;
        }
    }
    break;
    case SEARCH_STATE_IDLE:
    {
        if ((dido_structure.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR1 == 0) ||
            (dido_structure.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR2 == 0))
        {
            app_search.search_state_machine = SEARCH_STATE_WAIT;
            LOG_I("search door open\r\n");
        }
    }
    break;
    case SEARCH_STATE_WAIT:
    {
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_SearchTreatmentRoomRelay = 0;
        app_do_set(app_search.app_dido, &dido_structure);
        if (dido_structure.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_SEARCH_TREATMENT_ROOM == 1)
        {
            app_search.search_state_machine = SEARCH_STATE_START;
            LOG_I("search start\r\n");
        }
    }
    break;
    case SEARCH_STATE_START:
    {
        osStatus = osTimerStart(app_search.search_osTimerId, search_timeout_get());
        if (osStatus != osOK)
        {
            LOG_I("search timeout timer start fail!\r\n");
            return -2;
        }
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_SearchTreatmentRoomRelay = 1;
        app_do_set(app_search.app_dido, &dido_structure);
        app_search.search_state_machine = SEARCH_STATE_SEARCH;
        LOG_I("search timer start\r\n");
    }
    break;
    case SEARCH_STATE_SEARCH:
    {
        if ((dido_structure.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR1 == 1) &&
            (dido_structure.mcp23017_0x01_u.mcp23017_0x01_bit.DI_CITB_TREATMENT_ROOM_DOOR2 == 1))
        {
            app_search.search_state_machine = SEARCH_STATE_END;
            LOG_I("search door close\r\n");
        }
    }
    break;
    case SEARCH_STATE_END:
    {
        osStatus = osTimerStop(app_search.search_osTimerId);
        if (osStatus != osOK)
        {
            LOG_I("search timeout timer stop fail!\r\n");
            return -3;
        }
        LOG_I("search end\r\n");
        dido_structure.mcp23017_0x03_u.mcp23017_0x03_bit.DO_SearchTreatmentRoomRelay = 0;
        app_do_set(app_search.app_dido, &dido_structure);
        app_search.search_state_machine = SEARCH_STATE_IDLE;
    }
    break;
    default:
        break;
    }
}