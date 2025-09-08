#include <vector>
#include <unordered_map>
#include "event.h"

#ifdef RINGWORLD_ENABLE_EVENTS_BUS

static std::unordered_map<RingworldEventID, std::vector<RingworldEventCallback>> event_callbacks;

extern "C" void ringworld_event_subscribe(RingworldEventID event, RingworldEventCallback callback) {
    if(event_callbacks.find(event) == event_callbacks.end()) {
        event_callbacks[event] = std::vector<RingworldEventCallback>();
    }
    event_callbacks[event].push_back(callback);
}

extern "C" bool ringworld_event_dispatch(RingworldEventID event, void *data) {
    if(event_callbacks.find(event) == event_callbacks.end()) {
        return false;
    }
    bool result = false;
    for(auto callback : event_callbacks[event]) {
        result = result || callback(data);
    }
    return result;
}

#endif
