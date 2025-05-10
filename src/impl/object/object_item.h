#ifndef RINGWORLD__OBJECT__OBJECT_ITEM_H
#define RINGWORLD__OBJECT__OBJECT_ITEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "object_base.h"

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

#ifdef __cplusplus
}
#endif

#endif
