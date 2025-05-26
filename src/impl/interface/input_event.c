#include <string.h>
#include "input_event.h"

extern InputEventsGlobals *input_events_globals;

InputEventsGlobals *input_events_get_globals(void) {
    return input_events_globals;
}

void input_events_flush(void) {
    InputEventsGlobals *globals = input_events_get_globals();
    memset(globals->event_queue, 0, sizeof(globals->event_queue));
}
