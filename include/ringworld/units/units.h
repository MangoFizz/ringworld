#ifndef RINGWORLD__UNITS__UNITS_H
#define RINGWORLD__UNITS__UNITS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../object/object.h"

typedef struct UnitRecentDamager {
    uint32_t last_damage_time;
    float total_damage;
    ObjectHandle object;
    TableResourceHandle player;
} UnitRecentDamager;
_Static_assert(sizeof(UnitRecentDamager) == 0x10);

typedef struct UnitFlags {
    bool unknown_biped_speech_related : 1;
    bool pad_1 : 3;
    bool power_up : 1;
    bool power_up_addition : 1; 
    bool controllable : 1;
    bool berserking : 1;
    bool pad_2 : 8;
    bool pad_3 : 3;
    bool unknown_integrated_light_related : 1;
    bool will_not_drop_items : 1;
    bool unknown : 1;
    bool can_blink : 1;
    bool impervious : 1;
    bool suspended : 1;
    bool blind : 1;
    bool unknown_nv_related : 1; // when this is on, the integrated NV power increases. rate is 2x the speed it leaks when on. Wtf is an NV?? I asked the same thing, maybe night vision?
    bool possessed : 1;
    bool desires_flashlight_on : 1;
    bool desires_flashlight_off : 1;
    bool pad_4 : 2;
} UnitFlags;
_Static_assert(sizeof(UnitFlags) == 0x4);

typedef struct UnitControlFlags {
    bool crouch : 1;
    bool jump : 1;
    bool user1 : 1;
    bool user2 : 1;
    bool light : 1;
    bool exact_facing : 1;
    bool action : 1;
    bool melee : 1;
    bool look_dont_turn : 1;
    bool force_alert : 1;
    bool reload : 1;
    bool primary_trigger : 1;
    bool secondary_trigger : 1;
    bool grenade : 1;
    bool exchange_weapon : 1;
    bool pad_1 : 1;
} UnitControlFlags;
_Static_assert(sizeof(UnitControlFlags) == 0x2);

typedef struct UnitAnimationStateData {
    int16_t animation_index;
    int16_t frame_index;
} UnitAnimationStateData;
_Static_assert(sizeof(UnitAnimationStateData) == 0x4);

typedef struct UnitAnimationFlags {
    bool animation_bit0_unknown : 1;
    bool animation_bit1_unknown : 1;
    bool animation_bit2_unknown : 1;
    bool animation_bit3_unknown : 1;
    bool pad_1 : 4;
    bool pad_2 : 1;
} UnitAnimationFlags;
_Static_assert(sizeof(UnitAnimationFlags) == 0x2);

typedef struct UnitAnimationData {
    UnitAnimationFlags flags;
    int16_t unknown_some_animation_index_maybe;
    int16_t unknown_some_animation_index;
    int16_t pad_1;
    uint8_t seat_index;
    uint8_t seat_weapon_index;
    uint8_t weapon_type_index;
    int8_t state;
    int8_t replacement_state;
    int8_t overlay_state;
    int8_t desired_state;
    int8_t base_seat;
    int8_t emotion;
    char pad_2;
    UnitAnimationStateData replacement_animation;
    UnitAnimationStateData overlay_state_animation;
    UnitAnimationStateData weapon_ik;
    bool update_look;
    bool update_aim;
    Rectangle2DF looking_bounds;
    Rectangle2DF aiming_bounds;
    char pad_3[8];
} UnitAnimationData;
_Static_assert(sizeof(UnitAnimationData) == 0x48);

typedef struct AiCommunicationPacket {
    char pad_1[6]; 
    uint16_t type;
    char pad_2[2]; 
    char pad_3[2]; 
    char pad_4[2]; 
    char pad_5[6]; 
    char pad_6[2]; 
    char pad_7[2]; 
    char pad_8[4]; 
    bool broken;
    char pad_9[3]; 
} AiCommunicationPacket;
_Static_assert(sizeof(AiCommunicationPacket) == 0x20);

typedef struct UnitSpeech {
    uint16_t priority;
    uint16_t scream_type;
    TagHandle sound_tag;
    uint16_t ticks;
    int16_t unknown2;
    int32_t unknown;
    AiCommunicationPacket ai_communication_info;
} UnitSpeech;
_Static_assert(sizeof(UnitSpeech) == 0x30);

typedef struct UnitSpeechData {
    UnitSpeech current;
    UnitSpeech next;
    int16_t unknown0; 
    int16_t unknown1;
    int16_t unknown2; 
    int16_t unknown3;
    int32_t unknown4;
    bool unknown6;
    bool unknown7;
    bool unknown8;
    bool pad_1;
    int16_t unknown9;
    int16_t unknown10;
    int16_t unknown11; 
    int16_t unknown12;
    int32_t unknown13; 
} UnitSpeechData;

typedef struct UnitControlData {
    int8_t animation_state;
    int8_t aiming_speed;
    UnitControlFlags control_flags;
    int16_t weapon_index;
    int16_t grenade_index;
    int16_t zoom_index;
    int16_t pad_1;
    VectorIJK throttle;
    float primary_trigger;
    VectorIJK facing_vector;
    VectorIJK aiming_vector;
    VectorIJK looking_vector;
} UnitControlData;
_Static_assert(sizeof(UnitControlData) == 0x40);

typedef struct UnitObject {
    DynamicObjectBase base;
    TagHandle actor_tag;
    TagHandle swarm_actor_tag;
    ObjectHandle swarm_next_unit;
    ObjectHandle swarm_previous_unit;
    UnitFlags unit_flags;
    UnitControlFlags unit_control_flags;
    char pad_1[4];
    int8_t shield_snapping;
    int8_t base_seat_index;
    struct {
        uint32_t ticks_remaining;
        UnitControlFlags control_flags;
        uint16_t pad_1;
    } persistent_control;
    TableResourceHandle controlling_player;
    int16_t ai_effect_type;
    int16_t emotion_animation_index;
    uint32_t next_ai_effect_tick;
    VectorIJK desired_facing_vector;
    VectorIJK desired_aiming_vector;
    VectorIJK aiming_vector;
    VectorIJK aiming_velocity;
    VectorPYR looking_angles;
    VectorIJK looking_vector;
    VectorIJK looking_velocity;
    VectorIJK throttle;
    float primary_trigger;
    int8_t aiming_speed;
    int8_t melee_state;
    int8_t melee_timer;
    int8_t ticks_until_flame_to_death;
    int8_t ping_animation_ticks_left; 
    uint8_t grenade_state;
    int16_t unknown_725;
    int16_t unknown_726;
    uint16_t pad_2;
    TagHandle grenade_projectile; 
    UnitAnimationData animation;
    float ambient;
    float illumination;
    float mouth_factor;
    uint32_t pad_3;
    int16_t vehicle_seat_id;
    int16_t current_weapon_id;
    int16_t next_weapon_id;
    uint16_t pad_4;
    ObjectHandle weapons[4];
    uint32_t weapon_ready_ticks[4];
    ObjectHandle equipment_handle; 
    int8_t current_grenade_index;
    int8_t next_grenade_index;
    uint8_t grenade_counts[2];
    uint8_t zoom_level;
    uint8_t desired_zoom_level;
    int8_t ticks_since_last_vehicle_speech;
    uint8_t aiming_change;
    ObjectHandle powered_seats_riders[2];
    TableResourceHandle _unknown22;
    int32_t _some_tick_time;
    int16_t encounter_id;
    int16_t squad_id;
    float powered_seats_power[2];
    float integrated_light_power;
    float integrated_light_toggle_power;
    float integrated_night_vision_toggle_power;
    VectorXYZ seat_related[4];
    float camo_power;
    float full_spectrum_vision_power; 
    TagHandle dialogue_definition;
    UnitSpeechData speech;
    struct {
        uint16_t category;
        uint16_t ai_ticks_until_handle;
        float amount;
        TagHandle responsible_unit;
    } damage_result;
    ObjectHandle object_flame_causer; 
    float unknown23;
    char pad_5[4];
    uint32_t died_at_tick;
    uint16_t feign_death_timer;
    bool camo_regrowth;
    char pad_6;
    float stun;
    uint16_t stun_ticks;
    int16_t spree_count;
    int32_t spree_starting_time;
    UnitRecentDamager recent_damage[4];
    char pad_7[4];
    char pad_8[2];
    uint8_t opensauce_zoom_level;
    uint8_t opensauce_desired_zoom_level;
    UnitControlData control_data;
    bool last_completed_client_update_valid;
    char pad_9[3];
    int32_t last_completed_client_update_id;
    char pad_10[12];
} UnitObject;
_Static_assert(sizeof(UnitObject) == 0x4CC);

typedef struct BipedFlags {
    bool airborne : 1;
    bool slipping : 1;
    bool absolute_movement : 1;
    bool no_collision : 1;
    bool passes_through_other_bipeds : 1;
    bool limping2 : 1;
    bool pad_1 : 2;
    bool pad_2[3];
} BipedFlags;
_Static_assert(sizeof(BipedFlags) == 0x4);

typedef struct BipedNetworkDelta {
    int8_t grenade_counts[2];
    char pad_1[2];
    float body_vitality;
    float shield_vitality; 
    bool shield_stun_ticks_greater_than_zero;
    char pad_2[3];
} BipedNetworkDelta;
_Static_assert(sizeof(BipedNetworkDelta) == 0x10);

typedef struct BipedNetwork {
    char pad_1[2];
    bool baseline_valid;
    int8_t baseline_id;
    int8_t message_id;
    char pad_2[3];
    BipedNetworkDelta update_baseline;
    bool delta_valid;
    char pad_3[3];
    BipedNetworkDelta update_delta;
} BipedNetwork;
_Static_assert(sizeof(BipedNetwork) == 0x2C);

typedef struct BipedObject {
    UnitObject base;
    BipedFlags biped_flags;
    int8_t landing_timer; 
    int8_t landing_force; 
    uint8_t movement_state;
    char pad_1;
    int32_t _biped_unknown3;
    uint32_t action_flags; 
    int32_t _biped_unknown4;
    VectorXYZ biped_position;
    int32_t walking_counter; 
    char pad_2[12];
    ObjectHandle bump_object; 
    int8_t ticks_since_last_bump;
    int8_t airborne_ticks;
    int8_t slipping_ticks; 
    int8_t digital_throttle;
    int8_t jump_ticks;
    int8_t melee_ticks;
    int8_t melee_inflict_ticks;
    char pad_3;
    int16_t unknown_biped2;
    char pad_4[2];
    float crouch_scale;
    float unknown_biped1;
    Plane3D unknown_biped_physics_related;
    BipedNetwork network;
} BipedObject;
_Static_assert(sizeof(BipedObject) == 0x84 + sizeof(UnitObject));

typedef struct VehicleFlags {
    bool vehicle_unknown0 : 1; 
    bool hovering : 1;
    bool crouched : 1;
    bool jumping : 1;
    bool unknown_vehicle1 : 1;
    bool unknown_vehicle2 : 3;
    bool pad_1;
} VehicleFlags;
_Static_assert(sizeof(VehicleFlags) == 0x2);

typedef struct VehicleNetworkData {
    bool at_rest;
    char pad_1[3];
    VectorXYZ position;
    VectorXYZ transitional_velocity;
    VectorXYZ angular_velocity;
    VectorXYZ forward;
    VectorXYZ up;
} VehicleNetworkData;
_Static_assert(sizeof(VehicleNetworkData) == 0x40);

typedef struct VehicleNetwork {
    bool time_valid;
    bool baseline_valid;
    int8_t baseline_id;
    int8_t message_id;
    VehicleNetworkData update_baseline;
    bool delta_valid; 
    char pad_1[3];
    VehicleNetworkData update_delta;
    uint32_t last_moved_at_tick; 
    int16_t scenario_respawn_id; 
    char pad_2[2];
    VectorXYZ respawn_position; 
} VehicleNetwork;

typedef struct VehicleObject {
    UnitObject base;
    VehicleFlags vehicle_flags;
    char pad_1[2];
    char pad_2[4];
    float speed;
    float slide;
    float turn;
    float tire_position;
    float thread_position_left;
    float thread_position_right;
    float hover;
    float thrust;
    int8_t suspension_states[8];
    VectorXYZ hover_position;
    VectorXYZ unknown_vehicle3;
    VectorXYZ unknown_vehicle4;
    int32_t unknown_vehicle5;
    VehicleNetwork network;
} VehicleObject; 
_Static_assert(sizeof(VehicleObject) == 0xF4 + sizeof(UnitObject));

/**
 * Delete all weapons from a unit.
 * @param unit The handle of the unit to delete all weapons from.
 */
void unit_delete_all_weapons(ObjectHandle unit);

#ifdef __cplusplus
}
#endif

#endif 
