#include <string.h>
#include "../game/game_state.h"
#include "first_person_weapon.h"

extern FirstPersonWeapons **first_person_weapons;

FirstPersonWeapons *first_person_weapons_get(void) {
    return *first_person_weapons;
}

void first_person_weapon_initialize(void) {
    *first_person_weapons = game_state_allocate_heap(sizeof(FirstPersonWeapons));
}

void first_person_weapon_initialize_for_new_map(void) {
    FirstPersonWeapons *first_person_weapons = first_person_weapons_get();
    for(int i = 0; i < MAX_LOCAL_PLAYERS; i++) {
        memset(&first_person_weapons->local_players[i], 0, sizeof(FirstPersonWeapon));
        first_person_weapons->local_players[i].weapon_object = NULL_HANDLE;
        first_person_weapons->local_players[i].animation_sound_index = -1;
        first_person_weapons->local_players[i].animation_sound_state = -1;
    }
}
