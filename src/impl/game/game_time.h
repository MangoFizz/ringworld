#ifndef RINGWORLD__GAME__GAME_TIME_H
#define RINGWORLD__GAME__GAME_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct GameTimeGlobals {
    bool initialized;
    bool active;
    bool paused;
    uint32_t unk1;
    uint32_t unk2;
    int32_t local_time;
    int16_t elapsed;
    char unk_18[6];
    float speed;
    uint32_t unk_28;
} GameTimeGlobals;
_Static_assert(sizeof(GameTimeGlobals) == 0x20);

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

/**
 * Get the local game time.
 * @return the local game time
 */
int32_t game_time_get_local_time(void);

#ifdef __cplusplus
}
#endif

#endif
