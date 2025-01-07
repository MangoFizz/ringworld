#include "../player/player.h"
#include "player_profile.h"

extern uint32_t *saved_games_current_profile_id;
extern PlayerProfile *saved_games_player_profiles;

uint32_t saved_games_get_current_profile_id(uint16_t local_player_index) {
    ASSERT(local_player_index < MAX_LOCAL_PLAYERS);
    return &saved_games_current_profile_id[local_player_index];
}

PlayerProfile *saved_games_get_player_profile(uint16_t local_player_index) {
    ASSERT(local_player_index < MAX_LOCAL_PLAYERS);
    return &saved_games_player_profiles[local_player_index];
}
