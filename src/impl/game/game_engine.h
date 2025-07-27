#ifndef RINGWORLD__GAME__GAME_ENGINE_H
#define RINGWORLD__GAME__GAME_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../types/types.h"
#include "../player/player.h"

typedef enum GameEngineMode {
    GAME_ENGINE_MODE_ACTIVE,
    GAME_ENGINE_MODE_POSTGAME_DELAY,
    GAME_ENGINE_MODE_POSTGAME_RASTERIZE_DELAY,
    GAME_ENGINE_MODE_POSTGAME_RASTERIZE,
    NUM_OF_GAME_ENGINE_MODES
} GameEngineMode;

typedef struct GameEngineGlobals {
    uint32_t unk1;
    uint32_t unk2;
    float fade_unk1;
    float fade_unk2;
    GameEngineMode mode;
    uint32_t pad1[3];
} GameEngineGlobals;
_Static_assert(sizeof(GameEngineGlobals) == 0x20);

typedef enum PACKED_ENUM GameEngineType {
    GAME_ENGINE_TYPE_NONE,
    GAME_ENGINE_TYPE_CTF,
    GAME_ENGINE_TYPE_SLAYER,
    GAME_ENGINE_TYPE_ODDBALL,
    GAME_ENGINE_TYPE_KING,
    GAME_ENGINE_TYPE_RACE,
    GAME_ENGINE_TYPE_MAX_VALUE,
    GAME_ENGINE_TYPE_SIZE = 0xFFFF
} GameEngineType;

typedef struct GameEngineInterface {
    const char *name;
    GameEngineType type;
    void (*dispose)();
    bool (*initialize_for_new_map)();
    void (*dispose_from_old_map)();
    void (*player_added)();
    void (*player_removed)(); // makes sense
    void (*game_ending)();
    void (*game_starting)();
    void (*unk1)();
    void (*unk2)();
    void (*unk3)();
    void (*unk4)();
    void (*post_rasterize)();
    void (*player_update)();
    void (*weapon_update)();
    void (*weapon_pickup)();
    void (*weapon_drop)();
    void (*update)();
    int32_t (*get_score)(uint16_t player_index, bool team_score);
    int32_t (*get_team_score)(uint16_t team_index);
    wchar_t *(*get_score_string)(uint16_t player_index, wchar_t *out_buffer);
    wchar_t *(*get_score_header_string)(wchar_t *out_buffer);
    wchar_t *(*get_team_score_string)(uint16_t team_index, wchar_t *out_buffer);
    bool (*allow_pick_up)();
    void (*player_damaged_player)();
    void (*player_killed_player)();
    bool (*display_score)();
    float (*starting_location_rating)();
    void (*prespawn_player_update)();
    void (*postspawn_player_update)();
    void (*unk5)();
    bool (*goal_matches_player)();
    bool (*test_flag)();
    bool (*test_trait)();
    bool (*test_player_won)();
    void (*state_to_network)();
    void (*state_from_network)();
    void (*player_reset_score)();
    void (*get_gamespy_packet_field_string)();
    void (*create_player_score_strings)();
    void (*create_team_score_strings)();
    void (*get_gamespy_key_count)();
    void (*initialize_for_reset_map)();
} GameEngineInterface;
_Static_assert(sizeof(GameEngineInterface) == 0xB0);

/**
 * Get the current game type engine.
 * @return Pointer to the current game type engine interface.
 */
GameEngineInterface *game_engine_get_current_interface(void);

/**
 * Get the game engine globals.
 * @return Pointer to the game engine globals.
 */
GameEngineGlobals *game_engine_get_globals(void);

#ifdef __cplusplus
}
#endif

#endif
