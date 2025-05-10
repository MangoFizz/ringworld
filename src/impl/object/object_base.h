#ifndef RINGWORLD__OBJECT__OBJECT_BASE_H
#define RINGWORLD__OBJECT__OBJECT_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../memory/table.h"
#include "../types/types.h"
#include "../tag/definitions/object.h"
#include "../tag/tag.h"

typedef union TableResourceHandle ObjectHandle;
typedef union TableResourceHandle PlayerHandle;

typedef struct BaseDynamicObjectFlags {
    Bool no_collision : 1;
    Bool on_ground : 1;
    Bool ignore_gravity : 1;
    Bool in_water : 1;
    Bool pad_1 : 1;
    Bool stationary : 1;
    Bool pad_2 : 1;
    Bool no_collision2 : 1;
    Bool pad_3 : 2;
    Bool has_sound_looping_attachment : 1;
    Bool connected_to_map : 1;
    Bool not_placed_automatically : 1;
    Bool is_device_machine : 1;
    Bool is_elevator : 1;
    Bool is_elevator_2 : 1;
    Bool is_garbage : 1;
    Bool pad_4 : 1;
    Bool no_shadow : 1;
    Bool delete_at_deactivation : 1;
    Bool do_not_reactivate : 1;
    Bool outside_of_map : 1;
    Bool pad_5 : 2;
    Bool collidable : 1;
    Bool has_collision_model : 1;
    Bool network_message_unknown_1 : 1;
    Bool network_message_unknown_2 : 1;
    Bool opensauce_is_transforming_in : 1;
    Bool opensauce_is_transforming_out : 1;
    Bool pad_6 : 2;
} BaseDynamicObjectFlags;
_Static_assert(sizeof(BaseDynamicObjectFlags) == sizeof(uint32_t));

typedef struct BaseObjectNetwork {
    Bool valid_position;
    VectorXYZ position;
    Bool valid_forward_and_up;
    VectorXYZ orientation[2];
    Bool valid_transitional_velocity;
    VectorXYZ transitional_velocity;
    Bool valid_timestamp;
    TickCount32 timestamp;
} BaseObjectNetwork;
_Static_assert(sizeof(BaseObjectNetwork) == 0x44);

typedef struct ScenarioLocation {
    int32_t leaf_id;
    int16_t cluster_id;
    char pad[2];
} ScenarioLocation;
_Static_assert(sizeof(ScenarioLocation) == 0x8);

typedef struct BaseObjectVitalsFlags {
    Bool health_damage_effect_applied : 1;
    Bool shield_damage_effect_applied : 1;
    Bool health_depleted : 1;
    Bool shield_depleted : 1;
    Bool pad_1 : 1;
    Bool killed : 1;
    Bool killed_silent : 1;
    Bool cannot_melee_attack : 1;
    Bool pad_2 : 3;
    Bool invulnerable : 1;
    Bool shield_recharging : 1;
    Bool killed_no_stats : 1;
    Bool pad_3 : 2;
} BaseObjectVitalsFlags;
_Static_assert(sizeof(BaseObjectVitalsFlags) == sizeof(uint16_t));

typedef struct BaseObjectVitals {
    float base_health;
    float base_shield;
    float health;
    float shield;
    float current_shield_damage;
    float current_health_damage;
    ObjectHandle entangled_object_handle;
    float recent_shield_damage;
    float recent_health_damage;
    TickCount32 recent_shield_damage_time;
    TickCount32 recent_health_damage_time;
    TickCount16 shield_stun_time;
    BaseObjectVitalsFlags flags;
} BaseObjectVitals;
_Static_assert(sizeof(BaseObjectVitals) == 0x30);

typedef struct BaseObjectAttachmentsData {
    uint8_t types[8];
    uint32_t attachments[8];
    uint32_t first_widget;
} BaseObjectAttachmentsData;
_Static_assert(sizeof(BaseObjectAttachmentsData) == 0x2C);

typedef struct BaseObjectRegionDestroyeds {
    Bool region_0 : 1;
    Bool region_1 : 1;
    Bool region_2 : 1;
    Bool region_3 : 1;
    Bool region_4 : 1;
    Bool region_5 : 1;
    Bool region_6 : 1;
    Bool region_7 : 1;
    Bool pad_1;
} BaseObjectRegionDestroyeds;
_Static_assert(sizeof(BaseObjectRegionDestroyeds) == sizeof(uint16_t));

typedef struct BaseObjectBlockReference {
    uint16_t size;
    uint16_t offset;
} BaseObjectBlockReference;
_Static_assert(sizeof(BaseObjectBlockReference) == 0x4);

typedef struct ObjectValidOutGoingFunctions {
    Bool a : 1;
    Bool b : 1;
    Bool c : 1;
    Bool d : 1;
    Bool pad_1 : 4;
} ObjectValidOutGoingFunctions;
_Static_assert(sizeof(ObjectValidOutGoingFunctions) == 0x1);

typedef struct ObjectAnimationState {
    uint16_t index;
    uint16_t frame;
} ObjectAnimationState;
_Static_assert(sizeof(ObjectAnimationState) == 0x4);

typedef struct ObjectAnimationData {
    TagHandle animation_tag_handle;
    ObjectAnimationState animation_state;
    uint16_t animation_interpolation_frame;
    uint16_t animation_interpolation_frame_count;
} ObjectAnimationData;
_Static_assert(sizeof(ObjectAnimationData) == 0xC);

typedef struct DynamicObjectBase {
    TagHandle tag_handle;
    uint32_t network_role;
    uint32_t flags_0;
    TickCount32 existence_time;
    BaseDynamicObjectFlags flags_1;
    uint32_t object_marker_id;
    BaseObjectNetwork network;
    VectorXYZ position;
    VectorXYZ velocity;
    VectorIJK rotation[2];
    VectorPYR rotation_velocity;
    ScenarioLocation scenario_location;
    VectorXYZ center;
    float bounding_radius;
    float scale;
    uint32_t object_type;
    uint16_t team_owner;
    uint16_t name_list_index;
    TickCount16 moving_time;
    uint16_t variant_index;
    TableResourceHandle player;
    ObjectHandle owner_object;
    uint32_t pad_2;
    ObjectAnimationData animation_data;
    BaseObjectVitals vitals;
    uint32_t pad_3;
    TableResourceHandle cluster_partition;
    ObjectHandle unknown_object;
    ObjectHandle next_object;
    ObjectHandle first_object;
    ObjectHandle parent_object;
    uint8_t parent_attachment_node;
    uint8_t pad_4;
    char force_shield_update;
    ObjectValidOutGoingFunctions valid_outgoing_functions; 
    float incoming_function_values[4];
    float outgoing_function_values[4];
    BaseObjectAttachmentsData attachment_data;
    TableResourceHandle cached_render_state;
    BaseObjectRegionDestroyeds region_destroyeds;
    int16_t shader_permutation;
    uint8_t region_healths[8];
    int8_t region_permutation_ids[8];
    ColorRGB color_change[4];
    ColorRGB color_change_2[4];
    BaseObjectBlockReference node_orientations[2];
    BaseObjectBlockReference node_matrices_block;
} DynamicObjectBase;
_Static_assert(sizeof(DynamicObjectBase) == 0x1F4);

#ifdef __cplusplus
}
#endif

#endif
