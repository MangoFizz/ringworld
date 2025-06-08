#ifndef RINGWORLD__PARTICLE__PARTICLE_H
#define RINGWORLD__PARTICLE__PARTICLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../types/types.h"
#include "../object/object.h"
#include "../tag/tag.h"

struct ParticleObject {
    int16_t id;
    uint16_t flags;
    TagHandle definition_handle;
    ObjectHandle object_index;
    int16_t node_index;
    uint8_t state;
    uint8_t local_player_index;
    int32_t last_rendered_frame_index;
    float life_time;
    float life_span;
    float frame_time;
    float frame_span;
    int16_t sequence_index;
    int16_t frame_index;
    int32_t bsp_leaf_index;
    int16_t bsp_cluster_index;
    uint16_t unk1;
    VectorXYZ position;
    VectorIJK direction;
    VectorIJK translational_velocity;
    float rotation;
    float angular_velocity;
    float radius;
    ColorARGB color;
};

#ifdef __cplusplus
}
#endif

#endif 
