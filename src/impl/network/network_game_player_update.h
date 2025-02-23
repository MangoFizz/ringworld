#ifndef RINGWORLD__NETGAME__NETWORK_PLAYER_UPDATE_H
#define RINGWORLD__NETGAME__NETWORK_PLAYER_UPDATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../types/types.h"
#include "../object/object.h"
#include "../unit/unit.h"

typedef struct NetworkGamePlayerUpdate {
    int32_t id;
    int32_t tick_count;
    uint8_t action_collection[32];
    bool driving_vehicle;
    struct {
        ObjectHandle parent_object_index;
        VectorXYZ position;
        VectorIJK transitional_velocity;
        VectorIJK forward;
        ObjectAnimationData animation;
    } object;
    struct {
        UnitAnimationData animation;
        VectorIJK unit_34C;
        VectorIJK unit_358;
        VectorIJK unit_364;
        VectorIJK unit_370;
    } unit;
    struct {
        uint32_t flags;
        int8_t biped_503;
        int8_t airborne_ticks; 
        int8_t slipping_ticks; 
        int8_t biped_504;
        int16_t biped_508;
        float biped_50C;
        VectorIJK biped_514;
        uint32_t unk1;
        uint8_t biped_4D0;
        uint8_t biped_4D1;
        uint8_t movement_state; // biped_movement_state enum
        uint32_t biped_4D8;
    } biped;
    struct { 
        VectorXYZ position;
        VectorIJK transitional_velocity;
        VectorIJK angular_velocity;
        uint8_t object_data[sizeof(DynamicObjectBase) - FIELD_OFFSET(DynamicObjectBase, network_role)];
        float driver_power;
        float gunner_power;
        int32_t unk1;
        VehicleObject vehicle_data; // not sure about this, open sauce is weird
    } vehicle;
    struct NetworkGamePlayerUpdate *next;
} NetworkGamePlayerUpdate; 
// _Static_assert(sizeof(NetworkGamePlayerUpdate) == 0x418);

typedef struct NetworkGamePlayerUpdateHistory {
    int32_t next_update_history_id; // maximum = 64
    NetworkGamePlayerUpdate *updates;
    uint32_t pad1;
    int32_t number_of_playbacks;
    int32_t total_ticks;
    int32_t total_updates;
    uint32_t pad2;
    uint32_t pad3;
    uint32_t pad4;
    float avg_prediction_error;
    float avg_ticks_played_back;
} NetworkGamePlayerUpdateHistory; 
_Static_assert(sizeof(NetworkGamePlayerUpdateHistory) == 0x2C);

#ifdef __cplusplus
}
#endif

#endif
