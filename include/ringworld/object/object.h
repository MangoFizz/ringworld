#ifndef RINGWORLD__OBJECT__OBJECT_H
#define RINGWORLD__OBJECT__OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../memory/table.h"
#include "../types/types.h"
#include "../tag/tag.h"

typedef union TableResourceHandle ObjectHandle;

typedef struct ModelNode {
    float scale;
    VectorXYZ rotation[3];
    VectorXYZ position;
};

typedef struct BaseObjectFlags {
    bool no_collision : 1;
    bool on_ground : 1;
    bool ignore_gravity : 1;
    bool in_water : 1;
    bool pad_1 : 1;
    bool stationary : 1;
    bool pad_2 : 1;
    bool no_collision2 : 1;
    bool pad_3 : 2;
    bool has_sound_looping_attachment : 1;
    bool connected_to_map : 1;
    bool not_placed_automatically : 1;
    bool is_device_machine : 1;
    bool is_elevator : 1;
    bool is_elevator_2 : 1;
    bool is_garbage : 1;
    bool pad_4 : 1;
    bool no_shadow : 1;
    bool delete_at_deactivation : 1;
    bool do_not_reactivate : 1;
    bool outside_of_map : 1;
    bool pad_5 : 2;
    bool collidable : 1;
    bool has_collision_model : 1;
    bool network_message_unknown_1 : 1;
    bool network_message_unknown_2 : 1;
    bool opensauce_is_transforming_in : 1;
    bool opensauce_is_transforming_out : 1;
    bool pad_6 : 2;
} BaseObjectFlags;
_Static_assert(sizeof(BaseObjectFlags) == sizeof(uint32_t));

typedef struct BaseObjectNetwork {
    bool valid_position;
    VectorXYZ position;
    bool valid_forward_and_up;
    VectorXYZ orientation[2];
    bool valid_transitional_velocity;
    VectorXYZ transitional_velocity;
    bool valid_timestamp;
    uint32_t timestamp;
} BaseObjectNetwork;
_Static_assert(sizeof(BaseObjectNetwork) == 0x44);

typedef struct ScenarioLocation {
    int32_t leaf_id;
    int16_t cluster_id;
    char pad[2];
} ScenarioLocation;
_Static_assert(sizeof(ScenarioLocation) == 0x8);

typedef struct BaseObjectVitalsFlags {
    bool health_damage_effect_applied : 1;
    bool shield_damage_effect_applied : 1;
    bool health_depleted : 1;
    bool shield_depleted : 1;
    bool pad_1 : 1;
    bool killed : 1;
    bool killed_silent : 1;
    bool cannot_melee_attack : 1;
    bool pad_2 : 3;
    bool invulnerable : 1;
    bool shield_recharging : 1;
    bool killed_no_stats : 1;
    bool pad_3 : 2;
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
    uint32_t recent_shield_damage_time;
    uint32_t recent_health_damage_time;
    uint16_t shield_stun_time;
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
    bool region_0 : 1;
    bool region_1 : 1;
    bool region_2 : 1;
    bool region_3 : 1;
    bool region_4 : 1;
    bool region_5 : 1;
    bool region_6 : 1;
    bool region_7 : 1;
    bool pad_1 : 8;
} BaseObjectRegionDestroyeds;
_Static_assert(sizeof(BaseObjectRegionDestroyeds) == sizeof(uint16_t));

typedef struct BaseObjectBlockReference {
    uint16_t size;
    uint16_t offset;
} BaseObjectBlockReference;
_Static_assert(sizeof(BaseObjectBlockReference) == 0x4);

typedef struct ObjectValidOutGoingFunctions {
    bool a : 1;
    bool b : 1;
    bool c : 1;
    bool d : 1;
    bool pad_1 : 4;
} ObjectValidOutGoingFunctions;
_Static_assert(sizeof(ObjectValidOutGoingFunctions) == 0x1);

typedef struct DynamicObjectBase {
    TagHandle tag_handle;
    uint32_t network_role;
    uint32_t flags_0;
    uint32_t existence_time;
    BaseObjectFlags flags_1;
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
    uint16_t moving_time;
    uint16_t variant_index;
    TableResourceHandle player;
    ObjectHandle owner_object;
    uint32_t pad_2;
    TagHandle animation_tag_handle;
    uint16_t animation_index;
    uint16_t animation_frame;
    uint16_t animation_interpolation_frame;
    uint16_t animation_interpolation_frame_count;
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

typedef struct ItemObject {
    DynamicObjectBase base;
    uint32_t flags;
    int16_t ticks_until_detonation;
    int16_t bsp_collision_surface_id;
    int16_t bsp_collision_reference_id; 
    char pad_1[2];
    ObjectHandle dropped_by_unit; 
    int32_t last_update_tick;
    ObjectHandle collision_object;
    VectorXYZ collision_object_position;
    VectorXYZ unknown_collision_position; 
    VectorPY unknown_collision_angle; 
} ItemObject;
_Static_assert(sizeof(ItemObject) == 0x38 + sizeof(DynamicObjectBase));

typedef struct GarbageObject {
    ItemObject base;
    uint16_t ticks_until_garbage_collection;
    char pad_1[2];
    char pad_2[20];
} GarbageObject;
_Static_assert(sizeof(GarbageObject) == 0x18 + sizeof(ItemObject));

typedef struct WeaponTrigger {
    int8_t idle_time;
    int8_t state;
    uint16_t trigger_time;
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
} WeaponTrigger;
_Static_assert(sizeof(WeaponTrigger) == 0x28);

typedef struct WeaponMagazine {
    int16_t state;
    uint16_t reload_ticks_remaining;
    uint16_t reload_ticks;
    int16_t rounds_unloaded;
    int16_t rounds_loaded;
    int16_t rounds_left_to_recharge; 
    int16_t unknown; 
    int16_t unknown2; 
} WeaponMagazine; 
_Static_assert(sizeof(WeaponMagazine) == 0x10);

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
    bool baseline_valid;
    int8_t baseline_index;
    int8_t message_index;
    char pad_1;
    WeaponNetworkData update_baseline;
    bool delta_valid;
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
    WeaponTrigger triggers[2];
    WeaponMagazine magazines[2]; 
    uint32_t last_trigger_fire_tick;
    WeaponReloadStartData reload_starting_point;
    char pad_6[4];
    WeaponNetwork network;
} WeaponObject;
_Static_assert(sizeof(WeaponObject) == 0x114 + sizeof(ItemObject));

typedef struct EquipmentNetworkData {
    VectorXYZ position;
    VectorXYZ transitional_velocity;
    VectorXYZ angular_velocity;
} EquipmentNetworkData; 
_Static_assert(sizeof(EquipmentNetworkData) == 0x24);

typedef struct EquipmentNetwork {
    bool baseline_valid;
    int8_t baseline_index;
    int8_t message_index;
    char pad_1;
    EquipmentNetworkData update_baseline;
    bool delta_valid; 
    char pad_2[3];
    EquipmentNetworkData update_delta;
} EquipmentNetwork;

typedef struct EquipmentObject {
    ItemObject base;
    char pad_1[24];
    EquipmentNetwork network;
} EquipmentObject;
_Static_assert(sizeof(EquipmentObject) == 0x68 + sizeof(ItemObject));

typedef struct ProjectileObjectFlags {
    bool tracer : 1;
    bool projectile_unknown_bit : 1;
    bool attached : 1;
    bool pad_1 : 5;
    char pad_2[3];
} ProjectileObjectFlags;

typedef struct ProjectileNetworkData {
    VectorXYZ position;
    VectorXYZ transitional_velocity;
} ProjectileNetworkData; 
_Static_assert(sizeof(ProjectileNetworkData) == 0x18);

typedef struct ProjectileNetwork {
    bool unknown;
    bool baseline_valid;
    int8_t baseline_index;
    int8_t message_index;
    ProjectileNetworkData update_baseline;
    bool delta_valid;
    char pad_1[3];
    ProjectileNetworkData update_delta;
} ProjectileNetwork;

typedef struct ProjectileObject {
    ItemObject base;
    ProjectileObjectFlags projectile_flags;
    int16_t action_enum;
    int16_t material_id; 
    ObjectHandle source_unit;
    ObjectHandle target_object;
    int32_t contrail_attachment_block_id;
    float time_remaining; 
    float arming_rate; 
    float unknown_proj_float1;
    float unknown_proj_float2; 
    float distance_travelled;
    VectorXYZ transitional_velocity;
    float water_damage_upper_bound;
    VectorXYZ angular_velocity;
    VectorPY unknown_euler;
    ProjectileNetwork network;
} ProjectileObject;
_Static_assert(sizeof(ProjectileObject) == 0x84 + sizeof(ItemObject));

typedef struct DeviceObjectState {
    int16_t device_group_id;
    char pad_1[2];
    float value;
    float change;
} DeviceObjectState;

typedef struct DeviceObject {
    DynamicObjectBase base;
    bool position_reversed : 1;
    bool not_usable_from_any_side : 1;
    bool pad_1 : 6;
    char pad_2[3];
    DeviceObjectState power;
    DeviceObjectState position;
    bool one_sided : 1;
    bool operates_automatically : 1;
    bool pad_3 : 6;
    char pad_4[3];
} DeviceObject;
_Static_assert(sizeof(DeviceObject) == 0x20 + sizeof(DynamicObjectBase));

typedef struct DeviceMachineObjectFlags {
    bool does_not_operate_automatically : 1;
    bool machine_one_sided : 1;
    bool never_appears_locked : 1;
    bool opened_by_melee_attack : 1;
    bool pad_1 : 4;
    char pad_2[3];
} DeviceMachineObjectFlags;

typedef struct DeviceMachineObject {
    DeviceObject base;
    DeviceMachineObjectFlags device_flags;
    uint32_t ticks_since_started_opening;
    VectorXYZ elevator_position;
} DeviceMachineObject;
_Static_assert(sizeof(DeviceMachineObject) == 0x14 + sizeof(DeviceObject));

typedef struct DeviceControlObjectFlags {
    bool usable_from_both_sides : 1;
    bool pad_1 : 7;
    char pad_2[3];
} DeviceControlObjectFlags;

typedef struct DeviceControlObject {
    DeviceObject base;
    DeviceControlObjectFlags device_control_flags;
    int16_t custom_name_id;
    char pad_1[2];
} DeviceControlObject;
_Static_assert(sizeof(DeviceControlObject) == 8 + sizeof(DeviceObject));

typedef struct DeviceLightFixtureObject {
    DeviceObject base;
    ColorRGB light_color;
    float light_intensity;
    float light_falloff_angle;
    float light_cutoff_angle;
} DeviceLightFixtureObject;
_Static_assert(sizeof(DeviceLightFixtureObject) == 0x18 + sizeof(DeviceObject));

#ifdef __cplusplus
}
#endif

#endif
