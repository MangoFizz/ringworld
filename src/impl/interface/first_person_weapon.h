#ifndef RINGWORLD__INTERFACE__FIRST_PERSON_WEAPON_H
#define RINGWORLD__INTERFACE__FIRST_PERSON_WEAPON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../player/player.h"
#include "../types/types.h"
#include "../object/object.h"

typedef struct FirstPersonWeapon {
    bool visible;
    ObjectHandle unit_object;
    ObjectHandle weapon_object;
    int16_t state;
    int16_t unk1;
    TickCount16 ticks_idle;
    ObjectAnimationState base_animation;
    ObjectAnimationState animation;
    ObjectAnimationState moving_overlay_animation;
    AnimationStateInterpolation overcharged_animation;
    char pad1[0x28]; // figure out this later
    bool rendered;
    VectorIJK forward;
    VectorPY facing_angle;
    VectorPY last_facing_angle;
    VectorXYZ position;
    VectorXYZ last_position;
    int16_t interpolation_frame_index;
    int16_t interpolation_frame_count;
    NodeOrientation node_orientations[128];
    Matrix4x3 node_matrices[64];
    bool remapped_weapon;
    int16_t weapon_node_remappings[64];
    bool remapped_hands;
    int16_t hands_node_remappings[64];
    bool shotgun_is_empty;
    int16_t shotgun_shells_to_reload;
    int16_t shotgun_reload_type;
    int32_t animation_sound_index;
    int16_t animation_sound_state;
} FirstPersonWeapon;
_Static_assert(sizeof(FirstPersonWeapon) == 0x1EA0);

typedef struct FirstPersonWeapons {
    FirstPersonWeapon local_players[MAX_LOCAL_PLAYERS];
} FirstPersonWeapons;

/**
 * Initialize the first person weapons resources.
 */
void first_person_weapon_initialize(void);

/**
 * Initialize the first person weapon data for a new map.
 */
void first_person_weapon_initialize_for_new_map(void);

#ifdef __cplusplus
}
#endif

#endif 
