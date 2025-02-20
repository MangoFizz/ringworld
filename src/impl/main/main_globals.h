#ifndef RINGWORLD__MAIN__MAIN_GLOBALS_H
#define RINGWORLD__MAIN__MAIN_GLOBALS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include "../memory/memory.h"
#include "../types/types.h"

typedef enum PACKED_ENUM GameConnectionType {
    GAME_CONNECTION_LOCAL,
    GAME_CONNECTION_NETWORK_CLIENT,
    GAME_CONNECTION_NETWORK_SERVER,
    GAME_CONNECTION_FILM_PLAYBACK,
    NUM_OF_GAME_CONNECTIONS,
    GAME_CONNECTION_SIZE = 0xFFFF
} GameConnectionType;

typedef struct MainGlobalsMap {
    bool reset_map;
    bool switch_to_campaign;
    bool revert_map;
    bool skip_cinematic;
    bool save_map;
    bool save_map_nonsafe;
    bool save_map_with_timeout;
    bool is_saving_map;
    int32_t saving_map_timeout_countdown;
    int32_t saving_map_timeout_timer;
    int32_t unk1;
    int16_t unk2;
    bool won_map;
    bool lost_map;
    bool respawn_coop_players;
    bool save_core;
    bool load_core;
    bool load_core_at_startup;
    int16_t switch_to_structure_bsp; // if not NONE, switches to the scenario's bsp by index
    bool main_menu_scenario_loaded;
    bool main_menu_scenario_load;
} MainGlobalsMap;

typedef struct MainGlobals {
    uint64_t unk1; // this must be the field that aligns the struct to 8 bytes
    LARGE_INTEGER performance_counter; 
    bool unk2; 
    bool is_taking_screenshot; 
    uint16_t pad1; 
    float delta_time; 
    GameConnectionType game_connection; 
    int16_t screenshot_count;
    void *movie; // screenshot bitmap
    uint32_t pad1;
    uint32_t pad2;
    int32_t movie_frame_index;
    float screenshot_delta_time;
    MainGlobalsMap map;
    bool unk3;
    bool unk4;
    bool unk5;
    bool quit;
    int32_t unk6;
    int32_t unk7;
    int32_t unk8;
    bool set_game_connection_to_film_playback;
    bool time_is_stopped;
    bool start_time;
    bool unk9;
    bool skip_frames;
    bool pad2;
    int16_t skip_frames_count;
    int16_t lost_map_count;
    int16_t respawn_count;
    bool unk10;
    bool pad3;
    bool pad4;
    bool pad5;
    bool unk11;
    char scenario_tag_path[256];
    char multiplayer_map_name[256];
    char queued_map[256];
    bool unk12;
    char connect_address[32];
    char connect_password[9];
    bool pad6;
    uint32_t pad7;
} MainGlobals; 
_Static_assert(sizeof(MainGlobals) == 0x3A0);

/**
 * Get the main globals.
 * @return pointer to the main globals
 */
MainGlobals *main_get_globals(void);

#ifdef __cplusplus
}
#endif

#endif
