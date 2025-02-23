#ifndef RINGWORLD__GAME__GAME_ENGINE_H
#define RINGWORLD__GAME__GAME_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct GameTypeEngineInterface {
    const char *name;
    uint16_t type;
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
    int32_t (*get_score)();
    int32_t (*get_team_score)();
    wchar_t *(*get_score_string)();
    wchar_t *(*get_score_header_string)();
    wchar_t *(*get_team_score_string)();
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
} GameTypeEngineInterface;
_Static_assert(sizeof(GameTypeEngineInterface) == 0xB0);

/**
 * Get the current game type engine.
 * @return Pointer to the current game type engine interface.
 */
GameTypeEngineInterface *game_engine_get_current_interface(void);

#ifdef __cplusplus
}
#endif

#endif
