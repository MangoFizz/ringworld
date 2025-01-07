#ifndef RINGWORLD__INTERFACE__PLAYER_UI_H
#define RINGWORLD__INTERFACE__PLAYER_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Set the active player profile for the local player.
 * @param param_1 Unknown parameter; seems to be another index.
 * @param local_player_index The local player index.
 * @param profile_data The profile data.
 */
void player_ui_set_active_player_profile(uint32_t param_1, uint16_t local_player_index, void *profile_data);

#ifdef __cplusplus
}
#endif

#endif
