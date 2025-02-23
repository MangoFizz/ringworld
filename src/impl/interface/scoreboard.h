#ifndef RINGWORLD__INTERFACE__SCOREBOARD_H
#define RINGWORLD__INTERFACE__SCOREBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../player/player.h"

/**
 * Draw the scoreboard
 */
void scoreboard_render(PlayerHandle player_handle, float fade);

#ifdef __cplusplus
}
#endif

#endif
