#ifndef RINGWORLD__NETGAME__NETGAME_H
#define RINGWORLD__NETGAME__NETGAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../memory/memory.h"
#include "../types/types.h"
#include "network_transport.h"

enum {
    NETWORK_GAME_MAX_PLAYERS = 16
};

typedef enum PACKED_ENUM NetworkGamePlayerColor {
    NETWORK_GAME_PLAYER_COLOR_WHITE,
    NETWORK_GAME_PLAYER_COLOR_BLACK,
    NETWORK_GAME_PLAYER_COLOR_RED,
    NETWORK_GAME_PLAYER_COLOR_BLUE,
    NETWORK_GAME_PLAYER_COLOR_GRAY,
    NETWORK_GAME_PLAYER_COLOR_YELLOW,
    NETWORK_GAME_PLAYER_COLOR_GREEN,
    NETWORK_GAME_PLAYER_COLOR_PINK,
    NETWORK_GAME_PLAYER_COLOR_PURPLE,
    NETWORK_GAME_PLAYER_COLOR_CYAN,
    NETWORK_GAME_PLAYER_COLOR_COBALT,
    NETWORK_GAME_PLAYER_COLOR_ORANGE,
    NETWORK_GAME_PLAYER_COLOR_TEAL,
    NETWORK_GAME_PLAYER_COLOR_SAGE,
    NETWORK_GAME_PLAYER_COLOR_BROWN,
    NETWORK_GAME_PLAYER_COLOR_TAN,
    NETWORK_GAME_PLAYER_COLOR_MAROON,
    NETWORK_GAME_PLAYER_COLOR_SALMON,
    NETWORK_GAME_PLAYER_COLOR_RANDOM,
    NUM_OF_NETWORK_GAME_PLAYER_COLORS,
    NETWORK_GAME_PLAYER_COLOR_SIZE = 0xFF
} NetworkGamePlayerColor;

typedef enum PACKED_ENUM NetworkGameTeam {
    NETWORK_GAME_TEAM_RED,
    NETWORK_GAME_TEAM_BLUE,
    NUM_OF_NETWORK_GAME_TEAMS,
    NETWORK_GAME_TEAM_SIZE = 0xFF
} NetworkGameTeam;

typedef struct NetworkGamePlayer {
    wchar_t name[12];
    int16_t color_index;
    int16_t unk1;
    int8_t machine_index;
    int8_t unk2;
    int8_t team_index;
    int8_t player_list_index;
} NetworkGamePlayer;
_Static_assert(sizeof(NetworkGamePlayer) == 32);

typedef struct NetworkGameUniversalVariant {
    bool teams;
    uint32_t flags;
    uint32_t goal_rader;
    bool odd_man_out;
    int32_t respawn_time_growth;
    int32_t respawn_time;
    int32_t suicide_penalty;
    int32_t lives;
    float health; 
    int32_t score_to_win;
    uint32_t weapon_set;
    int32_t vehicle_set[NUM_OF_NETWORK_GAME_TEAMS];
    int32_t vehicles_respawn;
    bool friendly_fire; // need an enum for this
    int32_t friendly_fire_penalty;
    bool team_autobalance;
    int32_t time_limit;
} NetworkGameUniversalVariant; 
_Static_assert(sizeof(NetworkGameUniversalVariant) == 0x48);

typedef struct NetworkGameVariant {
    wchar_t name[24]; 
    uint32_t game_engine_index;
    NetworkGameUniversalVariant universal_variant; 
    char pad1[32];
    char game_engine_variant[0x38];	// Add this union later
    uint32_t flags; 
    //uint32 crc; // not actually part of the structure, only "used" when presisting to/from files
} NetworkGameVariant;
_Static_assert(sizeof(NetworkGameVariant) == 0xD8);

typedef struct NetworkGame {
    wchar_t server_name[64];
    uint32_t map_version; 
    char map_name[128];
    NetworkGameVariant gametype_variant;
    uint8_t unk1;
    uint8_t max_players;
    int16_t unk2;
    int16_t players_count;
    NetworkGamePlayer players[NETWORK_GAME_MAX_PLAYERS];
    int16_t pad1;
    int32_t network_game_random_seed;
    int32_t number_of_games_played; 
    int32_t local_data; 
} NetworkGame;
_Static_assert(sizeof(NetworkGame) == 0x3F0);

/**
 * Get the network game data.
 */
NetworkGame *network_game_get(void);

/**
 * Sets the flag to disconnect from the network game.
 */
void network_game_issue_disconnect(void);

/**
 * Get the player color for the given player color index.
 * @param color player color index
 * @return Pointer to the color
 */
const ColorRGB *network_game_get_player_color(NetworkGamePlayerColor color);

/**
 * Get the team color for the given team index.
 * @param team team index
 * @return Pointer to the color
 */
const ColorRGB *network_game_get_team_color(NetworkGameTeam team);

/**
 * Get the server address.
 */
uint32_t network_game_get_server_address(void);

/**
 * Get the server port.
 */
uint16_t network_game_get_server_port(void);

#ifdef __cplusplus
}
#endif

#endif
