#ifndef RINGWORLD__INTERFACE__INPUT_EVENT_H
#define RINGWORLD__INTERFACE__INPUT_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../types/types.h"

typedef struct InputEvent {
    int16_t type;
    int16_t controller_index;
    VectorXYInt position; // I think this can handle buttons and mouse position
} InputEvent;
_Static_assert(sizeof(InputEvent) == 0x8);

typedef struct InputEventsGlobals {
    bool initialized;
    bool unk1;
    uint32_t ms_since_last_event;
    uint32_t ms_since_last_check;
    InputEvent event_queue[4][8];
} InputEventsGlobals;
_Static_assert(sizeof(InputEventsGlobals) == 0x10C);

/**
 * Get the global input events structure.
 */
InputEventsGlobals *input_events_get_globals(void);

/**
 * Flush the input events queue.
 */
void input_events_flush(void);

#ifdef __cplusplus
}
#endif

#endif
