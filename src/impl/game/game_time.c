#include "game_time.h"

extern GameTimeGlobals **game_time_globals;

GameTimeGlobals *game_time_get_globals(void) {
    return *game_time_globals;
}

bool game_time_get_paused(void) {
    return (*game_time_globals)->paused;
}

void game_time_set_paused(bool paused) {
    GameTimeGlobals *globals = game_time_get_globals();
    if(globals->initialized != false) {
        globals->active = !paused;
    }
    globals->paused = paused;
}

int32_t game_time_get_local_time(void) {
    GameTimeGlobals *globals = game_time_get_globals();
    return globals->local_time;
}
