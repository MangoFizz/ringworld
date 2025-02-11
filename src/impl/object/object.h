#ifndef RINGWORLD__OBJECT__OBJECT_H
#define RINGWORLD__OBJECT__OBJECT_H

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
    TickCount16 ticks_until_garbage_collection;
    char pad_1[2];
    char pad_2[20];
} GarbageObject;
_Static_assert(sizeof(GarbageObject) == 0x18 + sizeof(ItemObject));

typedef struct WeaponTrigger {
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
} WeaponTrigger;
_Static_assert(sizeof(WeaponTrigger) == 0x28);

typedef struct WeaponMagazine {
    int16_t state;
    TickCount16 reload_ticks_remaining;
    TickCount16 reload_ticks;
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
    WeaponTrigger triggers[2];
    WeaponMagazine magazines[2]; 
    TickCount32 last_trigger_fire_tick;
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
    Bool baseline_valid;
    int8_t baseline_index;
    int8_t message_index;
    char pad_1;
    EquipmentNetworkData update_baseline;
    Bool delta_valid; 
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
    Bool tracer : 1;
    Bool projectile_unknown_bit : 1;
    Bool attached : 1;
    Bool pad_1 : 5;
    char pad_2[3];
} ProjectileObjectFlags;

typedef struct ProjectileNetworkData {
    VectorXYZ position;
    VectorXYZ transitional_velocity;
} ProjectileNetworkData; 
_Static_assert(sizeof(ProjectileNetworkData) == 0x18);

typedef struct ProjectileNetwork {
    Bool unknown;
    Bool baseline_valid;
    int8_t baseline_index;
    int8_t message_index;
    ProjectileNetworkData update_baseline;
    Bool delta_valid;
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
    Bool position_reversed : 1;
    Bool not_usable_from_any_side : 1;
    Bool pad_1 : 6;
    char pad_2[3];
    DeviceObjectState power;
    DeviceObjectState position;
    Bool one_sided : 1;
    Bool operates_automatically : 1;
    Bool pad_3 : 6;
    char pad_4[3];
} DeviceObject;
_Static_assert(sizeof(DeviceObject) == 0x20 + sizeof(DynamicObjectBase));

typedef struct DeviceMachineObjectFlags {
    Bool does_not_operate_automatically : 1;
    Bool machine_one_sided : 1;
    Bool never_appears_locked : 1;
    Bool opened_by_melee_attack : 1;
    Bool pad_1 : 4;
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
    Bool usable_from_both_sides : 1;
    Bool pad_1 : 7;
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

enum {
    NUMBER_OF_OBJECT_CHANGE_COLORS = 4
};

typedef struct ObjectPlacementData {
  TagHandle tag_handle;
  uint32_t flags;
  PlayerHandle player;
  ObjectHandle parent_object_handle;
  TagHandle parent_tag_handle; 
  int16_t team_index; 
  int16_t region_permutation;
  VectorXYZ position; 
  float unk1; 
  VectorIJK translational_velocity; 
  VectorIJK forward; 
  VectorIJK up; 
  VectorIJK angular_velocity; 
  ColorRGB change_colors[NUMBER_OF_OBJECT_CHANGE_COLORS]; 
} ObjectCreationData;
_Static_assert(sizeof(ObjectCreationData) == 0x88);

typedef struct DynamicObjectHeader {
  int16_t id;
  uint8_t flags;
  uint8_t object_type;
  int16_t cluster_index;
  int16_t data_size;
  void *address;
} DynamicObjectHeader;
_Static_assert(sizeof(DynamicObjectHeader) == 0xC);

MAKE_TABLE_STRUCT(ObjectTable, DynamicObjectHeader);

/**
 * Initialize object placement data.
 * @param data The object placement data to initialize.
 * @param tag_handle The tag handle of the object.
 * @param parent_handle The parent object handle.
 */ 
void object_placement_data_new(ObjectCreationData *data, TagHandle tag_handle, ObjectHandle parent_handle);

/**
 * Attach an object to another object by its markers.
 * @param object The object to attach.
 * @param object_marker The marker on the object to attach.
 * @param attachment The object to attach to.
 * @param attachment_marker The marker on the attachment to attach to.
 */
void object_attach_to_marker(ObjectHandle object, const char *object_marker, ObjectHandle attachment, const char *attachment_marker);

#ifdef __cplusplus
}
#endif

#endif
