#ifndef RINGWORLD__OBJECT__OBJECT_DEVICE_H
#define RINGWORLD__OBJECT__OBJECT_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "object_base.h"

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

#ifdef __cplusplus
}
#endif

#endif
