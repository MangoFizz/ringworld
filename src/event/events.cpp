#include <vector>
#include <unordered_map>

#include "events.h"

static std::unordered_map<RingworldEventID, std::vector<RingworldEventCallback>> event_callbacks;

extern "C" void ringworld_event_subscribe(RingworldEventID event, RingworldEventCallback callback) {
    if(event_callbacks.find(event) == event_callbacks.end()) {
        event_callbacks[event] = std::vector<RingworldEventCallback>();
    }
    event_callbacks[event].push_back(callback);
}

extern "C" void ringworld_event_trigger(RingworldEventID event, void *data) {
    if(event_callbacks.find(event) == event_callbacks.end()) {
        return;
    }
    for(auto callback : event_callbacks[event]) {
        reinterpret_cast<void(*)(void *)>(callback)(data);
    }
}
