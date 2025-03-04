#ifndef RINGWORLD__INTERFACE__SCOREBOARD_H
#define RINGWORLD__INTERFACE__SCOREBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../player/player.h"

typedef struct ScoreboardRowData {
    PlayerHandle player_handle;
    uint16_t place;
    wchar_t *name;
    wchar_t *score;
    uint16_t kills;
    uint16_t deaths;
    uint16_t assists;
    uint8_t team;
    uint16_t ping;
    int16_t lives;
    bool tied;
} ScoreboardRowData;

/**
 * Build the rows for the scoreboard
 * @param player_handle Handle of local player
 * @param scoreboard_rows Array of ScoreboardRowData
 * @param count Number of rows to build
 * @param player_team_first If true, the local player's team will be first
 */
void scoreboard_build_rows(PlayerHandle player_handle, ScoreboardRowData *scoreboard_rows, uint16_t count, bool player_team_first, bool network_players_only);

/**
 * Draw the scoreboard
 * @param player_handle Handle of local player
 * @param fade Fade to apply to the scoreboard
 */
void scoreboard_render(PlayerHandle player_handle, float fade);

#ifdef __cplusplus
}
#endif

#endif
