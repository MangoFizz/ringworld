#include "game_engine.h"

extern GameTypeEngineInterface **current_game_engine_interface;

GameTypeEngineInterface *game_engine_get_current_interface(void) {
    return *current_game_engine_interface;
}
