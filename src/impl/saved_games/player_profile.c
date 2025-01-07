#include "player_profile.h"

extern uint32_t *saved_games_current_profile_id;
extern PlayerProfile *saved_games_player_profiles;

uint32_t saved_games_get_current_profile_id(void) {
    return sizeof(PlayerProfile);
}

PlayerProfile *saved_games_get_current_profile(void) {
    return saved_games_player_profiles;
}
