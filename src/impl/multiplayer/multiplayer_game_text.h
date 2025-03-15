#ifndef RINGWORLD__MULTIPLAYER__MULTIPLAYER_TAGS_H
#define RINGWORLD__MULTIPLAYER__MULTIPLAYER_TAGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/tag.h"

typedef enum MultiplayerGameTextIndex {
    MULTIPLAYER_GAME_TEXT_PLACE_COLUMN_HEADER = 67,
    MULTIPLAYER_GAME_TEXT_NAME_COLUMN_HEADER = 68,
    MULTIPLAYER_GAME_TEXT_KILLS_COLUMN_HEADER = 69,
    MULTIPLAYER_GAME_TEXT_ASSISTS_COLUMN_HEADER = 70,
    MULTIPLAYER_GAME_TEXT_DEATHS_COLUMN_HEADER = 71,
    MULTIPLAYER_GAME_TEXT_PING_COLUMN_HEADER = 191,
    MULTIPLAYER_GAME_TEXT_SCORE_DEAD = 138,
    MULTIPLAYER_GAME_TEXT_SCORE_QUIT = 139,
    MULTIPLAYER_GAME_TEXT_HEADER_NO_LIVES = 52,
    MULTIPLAYER_GAME_TEXT_HEADER_ONE_LIFE = 53,
    MULTIPLAYER_GAME_TEXT_HEADER_LIVES = 54,
    MULTIPLAYER_GAME_TEXT_HEADER_DRAW = 55,
    MULTIPLAYER_GAME_TEXT_HEADER_TEAM_LOST = 56,
    MULTIPLAYER_GAME_TEXT_HEADER_YOU_LOST = 57,
    MULTIPLAYER_GAME_TEXT_HEADER_TEAM_WON = 58,
    MULTIPLAYER_GAME_TEXT_HEADER_YOU_WON = 59,
    MULTIPLAYER_GAME_TEXT_HEADER_RED_LEADS_BLUE = 60,
    MULTIPLAYER_GAME_TEXT_HEADER_BLUE_LEADS_RED = 61,
    MULTIPLAYER_GAME_TEXT_HEADER_TEAMS_TIED = 62,
    MULTIPLAYER_GAME_TEXT_HEADER_TIED_FOR_PLACE = 63,
    MULTIPLAYER_GAME_TEXT_HEADER_IN_PLACE = 64,
    MULTIPLAYER_GAME_TEXT_SERVER_ADDRESS_PREFIX = 190,
    MULTIPLAYER_GAME_TEXT_RED_TEAM_SCORE = 65,
    MULTIPLAYER_GAME_TEXT_BLUE_TEAM_SCORE = 66,
    MULTIPLAYER_GAME_TEXT_QUIT_CONTINUE = 72,
    MULTIPLAYER_GAME_TEXT_QUIT = 73,
} MultiplayerGameTextIndex;

/**
 * Get the string for the scoreboard.
 * @param index Index of the string.
 * @return Pointer to the wide character string for the scoreboard.
 */
const wchar_t *multiplayer_game_text_get_string(MultiplayerGameTextIndex index);

/**
 * Get the place string for the scoreboard.
 * @param place Place to get the string for.
 * @return Pointer to the wide character string for the place.
 */
const wchar_t *multiplayer_game_text_get_place_string(uint16_t place);

#ifdef __cplusplus
}
#endif

#endif
