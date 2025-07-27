#ifndef RINGWORLD__WEAPON__WEAPON_H
#define RINGWORLD__WEAPON__WEAPON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../object/object_item.h"

typedef struct WeaponObjectTrigger {
    int8_t idle_time;
    int8_t state;
    TickCount16 trigger_time;
    uint32_t not_firing;
    uint32_t auto_reload;
    char pad_1[2];
    uint16_t rounds_since_last_tracer;
    float rate_of_fire;
    float ejection_port_recovery_time;
    float illumination_recovery_time;
    float projectile_error_related;
    TableResourceHandle charing_effect;
    int8_t network_delay_ticks;
    char pad_2[3];
} WeaponObjectTrigger;
_Static_assert(sizeof(WeaponObjectTrigger) == 0x28);

typedef struct WeaponObjectMagazine {
    int16_t state;
    TickCount16 reload_ticks_remaining;
    TickCount16 reload_ticks;
    int16_t rounds_unloaded;
    int16_t rounds_loaded;
    int16_t rounds_left_to_recharge; 
    int16_t unk_1; 
    int16_t unk_2; 
} WeaponObjectMagazine; 
_Static_assert(sizeof(WeaponObjectMagazine) == 0x10);

typedef struct WeaponReloadStartData {
    int16_t total_rounds[2];
    int16_t loaded_rounds[2];
} WeaponReloadStartData;
_Static_assert(sizeof(WeaponReloadStartData) == 8);

typedef struct WeaponNetworkData {
    VectorXYZ position;
    VectorXYZ transitional_velocity;
    VectorXYZ angular_velocity; 
    int16_t magazine_rounds_total[2];
    float age;
} WeaponNetworkData; 
_Static_assert(sizeof(WeaponNetworkData) == 0x2C);

typedef struct WeaponNetwork {
    Bool baseline_valid;
    int8_t baseline_index;
    int8_t message_index;
    char pad_1;
    WeaponNetworkData update_baseline;
    Bool delta_valid;
    char pad_2[3];
    WeaponNetworkData update_delta;
}WeaponNetwork;

typedef struct WeaponObject {
    ItemObject base;
    uint32_t flags;
    uint16_t owner_unit_flags;
    char pad_1[2];
    float primary_trigger;
    int8_t weapon_state;
    char pad_2;
    int16_t ready_ticks;
    float heat;
    float age;
    float illumination_fraction;
    float integrated_light_power;
    char pad_3[4];
    ObjectHandle tracked_object;
    char pad_4[8];
    int16_t alt_shots_loaded;
    char pad_5[2];
    WeaponObjectTrigger triggers[2];
    WeaponObjectMagazine magazines[2]; 
    TickCount32 last_trigger_fire_tick;
    WeaponReloadStartData reload_starting_point;
    char pad_6[4];
    WeaponNetwork network;
} WeaponObject;
_Static_assert(sizeof(WeaponObject) == 0x114 + sizeof(ItemObject));

typedef struct WeaponInterfaceMagazineState {
    bool reloading;
    bool can_fire;
    int16_t rounds_loaded;
    int16_t rounds_loaded_maximum;
    int16_t rounds_remaining;
    int16_t rounds_remaining_maximum;
} WeaponInterfaceMagazineState;
_Static_assert(sizeof(WeaponInterfaceMagazineState) == 10);

typedef struct WeaponInterfaceState {
    float heat;
    float age;
    bool overheated;
    int16_t magazine_count;
    WeaponInterfaceMagazineState magazines[2];
} WeaponInterfaceState;

/**
 * Build the weapon interface state for a weapon object.
 * @param weapon_object_handle The object handle of the weapon object.
 * @param weapon_interface_state Pointer to a WeaponInterfaceState struct to write the weapon interface state to.
 */
void weapon_build_weapon_interface_state(ObjectHandle weapon_object_handle, WeaponInterfaceState *weapon_interface_state);

#ifdef __cplusplus
}
#endif

#endif 
