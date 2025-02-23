#ifndef RINGWORLD__NETGAME__NETWORK_GAME_CLIENT_H
#define RINGWORLD__NETGAME__NETWORK_GAME_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "network_transport.h"
#include "network_game_player_update.h"
#include "network_game.h"

enum {
    NETWORK_SERVER_PASSWORD_LENGTH = 8,
    NETWORK_GAME_JOIN_TOKEN_SIZE = 16
};

typedef struct NetworkGameClientJoinParameters {
    int16_t unk1;
    wchar_t password[NETWORK_SERVER_PASSWORD_LENGTH + 1]; 
    char token[NETWORK_GAME_JOIN_TOKEN_SIZE]; 
} NetworkGameClientJoinParameters; 
_Static_assert(sizeof(NetworkGameClientJoinParameters) == 0x24);

typedef struct NetworkGameClientJoinData {
    int32_t unk1;
    int32_t join_process_start_time;
    int32_t unk2;
    bool unk3;
    bool pad1;
    NetworkGameClientJoinParameters parameters; 
    uint16_t pad2;
} NetworkGameClientJoinData; 
_Static_assert(sizeof(NetworkGameClientJoinData) == 0x34);

typedef struct NetworkGamePingTarget {
    int32_t last_ping_time; 
    int16_t samples_sent; 
    int16_t samples_received; 
    int16_t ping_time; 
    bool unk3;
    bool pad1;
    void* tc;
} NetworkGamePingTarget; 
_Static_assert(sizeof(NetworkGamePingTarget) == 0x10);

typedef enum PACKED_ENUM NetworkGameClientState {
    NETWORK_GAME_CLIENT_STATE_SEARCHING,
    NETWORK_GAME_CLIENT_STATE_JOINING,
    NETWORK_GAME_CLIENT_STATE_PREGAME,
    NETWORK_GAME_CLIENT_STATE_INGAME,
    NETWORK_GAME_CLIENT_STATE_POSTGAME,
    NUM_OF_NETWORK_GAME_CLIENT_STATES,
    NETWORK_GAME_CLIENT_STATE_SIZE = 0xFFFF
} NetworkGameClientState;

typedef struct NetworkGameClient {
    int16_t machine_index; 
    uint16_t pad1;
    char discovered_games[9 * 0x130]; // unimportant
    NetworkTransportAddress server_address; 
    NetworkGamePingTarget current_ping_target; 
    NetworkConnection *connection; 
    NetworkGameClientJoinData join_data; 
    NetworkGame game; 
    int32_t unk1;
    int32_t time_of_last_game_search_packet; 
    int32_t next_update_number; 
    int32_t time_of_last_update; 
    int32_t unk2;
    int16_t seconds_to_game_start; 
    NetworkGameClientState state; 
    uint16_t error; 
    uint16_t flags; 
    bool is_out_of_sync; 
    bool is_lagging_out; 
    uint16_t pad2;
    char pad3[0x64]; // fix this mess later
    NetworkGamePlayerUpdateHistory *update_history; 
    uint8_t connection_class; 
} NetworkGameClient; 
_Static_assert(sizeof(NetworkGameClient) == 0xF90);

/**
 * Get the network game client.
 */
NetworkGameClient *network_game_client_get(void);

#ifdef __cplusplus
}
#endif

#endif
