#include "game_engine.h"

extern GameEngineInterface **current_game_engine_interface;
extern GameEngineGlobals *game_engine_globals;

GameEngineInterface *game_engine_get_current_interface(void) {
    return *current_game_engine_interface;
}

GameEngineGlobals *game_engine_get_globals(void) {
    return game_engine_globals;
}
