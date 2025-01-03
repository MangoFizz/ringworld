#ifndef RINGWORLD__EVENT_BUS__EVENTS_H
#define RINGWORLD__EVENT_BUS__EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../ringworld.h"

typedef void (*RingworldEventCallback)(void *);

typedef enum RingworldEventID {
    RW_EVENT_WIDGET_LOADED
} RingworldEventID;

/**
 * Subscribe to an event
 * @param event The event id to subscribe to
 * @param callback The callback to call when the event is triggered
 */
RINGWORLD_API void ringworld_event_subscribe(RingworldEventID event, RingworldEventCallback callback);

/**
 * Trigger an event
 * @param event_id The event id to trigger
 * @param data The data to pass to the event
 */
void ringworld_event_trigger(enum RingworldEventID event, void *data);

#ifdef __cplusplus
}
#endif

#endif
