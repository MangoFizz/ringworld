#ifndef RINGWORLD__GAME__GAME_TIME_H
#define RINGWORLD__GAME__GAME_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../types/types.h"

typedef struct GameTimeGlobals {
    Bool initialized;
    Bool active;
    Bool paused;
    char unk_3[9];
    uint32_t local_time;
    uint16_t elapsed;
    char unk_18[6];
    float speed;
    uint32_t unk_28;
} GameTimeGlobals;

/**
 * Get the game time globals.
 * @return the game time globals
 */
GameTimeGlobals *game_time_get_globals(void);

/**
 * Get whether the game is paused.
 * @return true if the game is paused, false otherwise
 */
bool game_time_get_paused(void);

/**
 * Set whether the game is paused.
 * @param paused true to pause the game, false to unpause the game
 */
void game_time_set_paused(bool paused);

#ifdef __cplusplus
}
#endif

#endif
