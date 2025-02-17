#include "game_globals.h"

extern Globals **game_globals;

void *game_globals_get(void) {
    return *game_globals;
}
