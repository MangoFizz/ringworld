#include <ringworld/game/game_time.h>

extern GameTimeGlobals *game_time_globals;

bool game_time_get_paused(void) {
    return game_time_globals->paused;
}

void game_time_set_paused(bool paused) {
    if (game_time_globals->initialized != false) {
        game_time_globals->active = !paused;
    }
    game_time_globals->paused = paused;
}
