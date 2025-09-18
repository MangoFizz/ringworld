#ifndef RINGWORLD__EVENT__EVENT_H
#define RINGWORLD__EVENT__EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RINGWORLD_ENABLE_EVENTS_BUS

#include <stdbool.h>
#include "../ringworld.h"

#define RINGWORLD_EVENT_DISPATCH(event, data) ringworld_event_dispatch(event, data)

typedef bool (*RingworldEventCallback)(void *);

typedef enum RingworldEventID {
    RW_EVENT_WIDGET_LOADED,
    RW_EVENT_CONSOLE_COMMAND_NOT_FOUND,
    RW_EVENT_HSC_FUNCTION_HELP_NOT_FOUND
} RingworldEventID;

/**
 * Subscribe to an event
 * @param event The event id to subscribe to
 * @param callback The callback to call when the event is triggered
 */
void ringworld_event_subscribe(RingworldEventID event, RingworldEventCallback callback);

/**
 * Trigger an event
 * @param event_id The event id to trigger
 * @param data The data to pass to the event
 */
bool ringworld_event_dispatch(enum RingworldEventID event, void *data);

#else

#define RINGWORLD_EVENT_DISPATCH(event, data) true

#endif

#ifdef __cplusplus
}
#endif

#endif
