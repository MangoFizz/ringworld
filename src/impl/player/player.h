#ifndef RINGWORLD__PLAYER__PLAYER_H
#define RINGWORLD__PLAYER__PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../memory/table.h"
#include "../object/object.h"
#include "../types/types.h"

#define MAX_LOCAL_PLAYERS 1

typedef TableResourceHandle PlayerHandle;

typedef struct PlayersGlobals {
    int32_t local_player_network_id; 
    PlayerHandle local_players[MAX_LOCAL_PLAYERS]; 
    ObjectHandle local_player_dead_units[MAX_LOCAL_PLAYERS]; 
    int16_t local_player_count; 
    int16_t double_speed_ticks_remaining; 
    bool are_all_dead; 
    bool input_disabled; 
    int16_t bsp_switch_trigger_index; 
    int16_t respawn_failure; 
    bool was_teleported; 
    uint8_t pad_2;
    uint32_t combined_pvs[16]; 
    uint32_t combined_pvs_local[16]; 
} PlayersGlobals; 
_Static_assert(sizeof(PlayersGlobals) == 0x98);

typedef struct Team {
    char pad[0x40];
} Team; 
_Static_assert(sizeof(Team) == 0x40);

typedef struct PlayerControl {
    ObjectHandle unit_handle; 
    uint32_t control_flags; 
    uint16_t pad_1;
    uint16_t pad_2;
    VectorPY desired_angles; 
    VectorIJ throttle; 
    float primary_trigger; 
    int16_t weapon_index; 
    int16_t grenade_index; 
    int16_t zoom_level; 
    uint8_t weapon_swap_ticks;
    uint8_t pad_3;
    ObjectHandle target_object_index; 
    float autoaim_level; 
    float magnetism_level;
    float look_acceleration_time;
    float pitch_minimum;
    float pitch_maximum;
} PlayerControl; 
_Static_assert(sizeof(PlayerControl) == 0x40);

typedef struct PlayerControlGlobals {
    uint32_t action_test_flags; 
    uint32_t testing_for_action_flags; 
    uint32_t disabled_button_flags;
    uint32_t control_flags; // FLAG(0) = camera control
    PlayerControl local_players[MAX_LOCAL_PLAYERS];
} PlayerControlGlobals; 
_Static_assert(sizeof(PlayerControlGlobals) == 0x50);

typedef union PlayerMultiplayerStatistics {
    char raw[8];

    struct Ctf {
        int16_t flag_grabs;
        int16_t flag_returns;
        int16_t flag_scores;
    } ctf;

    struct Slayer {
    } slayer;

    struct Oddball {
        int16_t time;
        int16_t carrier_kills;
        int16_t kills;
    } oddball;

    struct King {
        int16_t hill_score;
    } king;

    struct Race {
        int16_t time;
        int16_t laps;
        int16_t best_time;
    } race;
} PlayerMultiplayerStatistics; 
_Static_assert(sizeof(PlayerMultiplayerStatistics) == 8);

typedef struct NetworkPlayer {
    wchar_t name[12];
    uint16_t color_index;
    int16_t icon_index;
    uint8_t machine_index;
    uint8_t controller_index;
    uint8_t team_index;
    uint8_t index;
} NetworkPlayer;

typedef struct MultiplayerPlayerInfo {
    float speed_multiplier;
    int32_t teleporter_index;
    int32_t objective_mode;
    int32_t objective_player_handle;
    int32_t target_player;
    TickCount32 target_time;
    TickCount32 last_death_time;
    uint32_t slayer_target;
    bool odd_man_out;
} MultiplayerPlayerInfo;

typedef struct Player {
    uint16_t player_id;
    uint16_t local_player_index;
    wchar_t name[12];
    uint32_t squad_index;
    uint32_t team_index;
    ObjectHandle interaction_object_handle;
    uint16_t interaction_object_type;
    uint16_t interaction_object_seat;
    TickCount32 respawn_time;
    TickCount32 respawn_time_penalty;
    ObjectHandle unit_handle;
    ObjectHandle dead_unit_handle;
    int16_t bsp_cluster_index;
    bool weapon_swap_result;
    char pad_1;
    ObjectHandle auto_aim_target_object;
    TickCount32 last_fire_time;
    NetworkPlayer network_player_data;
    uint16_t power_up_durations[2];
    MultiplayerPlayerInfo multiplayer_info;
    char pad_3[6];
    uint16_t kill_streak;
    uint16_t multikill;
    uint16_t last_kill_time;
    int16_t kills[4];
    int16_t assists[4];
    uint16_t betrays;
    uint16_t deaths;
    uint16_t suicides;
    char pad_4[14];
    uint16_t team_kills;
    char pad_5[2];
    PlayerMultiplayerStatistics multiplayer_statistics;
    TickCount32 telefrag_timer;
    TickCount32 quit_time;
    bool telefrag_danger;
    bool quit;
    char pad_6[6];
    uint32_t ping;
    uint32_t team_kill_count;
    TickCount32 team_kill_ticks_since_last;
    char pad_7[16];
    VectorXYZ position;
    char pad_8[24];
    uint16_t melee : 1;
    uint16_t action : 1;
    uint16_t pad_9 : 1;
    uint16_t flashlight : 1;
    uint16_t pad_10 : 9;
    uint16_t reload : 1;
    uint16_t pad_11 : 2;
    char pad_12[2];
    char pad_13[24];
    float baseline_update_xy_aim;
    float baseline_update_z_aim;
    float baseline_update_forward;
    float baseline_update_left;
    float baseline_update_rate_of_fire;
    uint16_t baseline_update_weapon_slot;
    uint16_t baseline_update_grenade_slot;
    char pad_14[4];
    VectorXYZ update_aiming;
    char pad_15[16];
    VectorXYZ update_position;
    char pad_16[132];
} Player;
_Static_assert(sizeof(Player) == 0x200);

MAKE_TABLE_STRUCT(TeamsTable, Team);
MAKE_TABLE_STRUCT(PlayersTable, Player);

/**
 * Initialize all player data structures.
 */
void players_data_initialize(void);

/**
 * Get the players globals
 */
PlayersGlobals *player_get_globals(void);

/**
 * Get the players table
 */
PlayersTable *player_get_table(void);

#ifdef __cplusplus
}
#endif

#endif
