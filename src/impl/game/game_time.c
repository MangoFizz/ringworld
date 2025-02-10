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
