#ifndef RINGWORLD__RASTERIZER__RASTERIZER_TRANSPARENT_GEOMETRY_GROUP_H
#define RINGWORLD__RASTERIZER__RASTERIZER_TRANSPARENT_GEOMETRY_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../object/object.h"
#include "../tag/definitions/shader.h"
#include "../tag/definitions/bitmap.h"
#include "rasterizer_dx9_vertex.h"
#include "rasterizer_dx9.h"

typedef struct RenderAnimation {
    ColorRGB *colors;
    float *values;
} RenderAnimation;
_Static_assert(sizeof(RenderAnimation) == 0x8);

typedef enum PACKED_ENUM RenderModelEffectType {
    RENDER_MODEL_EFFECT_TYPE_NONE,
    RENDER_MODEL_EFFECT_TYPE_ACTIVE_CAMOUFLAGE,
    RENDER_MODEL_EFFECT_TYPE_TRANSPARENT_ZBUFFERED,
    RENDER_MODEL_EFFECT_TYPE_MAX,
    RENDER_MODEL_EFFECT_TYPE_SIZE = 0xFFFF
} RenderModelEffectType;
_Static_assert(sizeof(RenderModelEffectType) == sizeof(uint16_t));

typedef struct RenderModelEffect {
    RenderModelEffectType type;
    float intensity;
    float parameter;
    int source_object_index;
    VectorXYZ source_object_centroid;
    Shader *modifier_shader;
    RenderAnimation modifier_animation;
} RenderModelEffect;
_Static_assert(sizeof(RenderModelEffect) == 0x28);

typedef struct RenderDistantLight {
    ColorRGB color;
    VectorXYZ direction;
} RenderDistantLight;
_Static_assert(sizeof(RenderDistantLight) == 0x18);

typedef struct RenderLighting {
    ColorRGB ambient_color;
    int16_t distant_light_count;
    RenderDistantLight distant_lights[2];
    int16_t point_light_count;
    int32_t point_light_indices[2];
    ColorARGB reflection_tint_color;
    VectorXYZ shadow_vector;
    ColorRGB shadow_color;
} RenderLighting;
_Static_assert(sizeof(RenderLighting) == 0x74);

typedef struct GeometryFlags {
    Bool no_sort : 1;
    Bool no_queue : 1;
    Bool no_fog : 1;
    Bool no_zbuffer : 1;
    Bool sky : 1;
    Bool viewspace : 1;
    Bool atmospheric_fog_but_no_planar_fog : 1;
    Bool first_person : 1;
    Bool parts_define_local_nodes : 1;
    Bool dont_skin : 1;
    Bool pad[2];
} GeometryFlags;
_Static_assert(sizeof(GeometryFlags) == sizeof(uint32_t));

typedef struct TransparentGeometryGroup {
    GeometryFlags geometry_flags;
    ObjectHandle object_index;
    ObjectHandle source_object_index;
    Shader *shader;
    int16_t shader_permutation_index;
    RenderModelEffect effect;
    VectorXY model_base_map_scale;
    uint32_t dynamic_triangle_buffer_index;
    TriangleBuffer *triangle_buffer;
    int first_triangle_index;
    int triangle_count;
    int dynamic_vertex_buffer_index;
    VertexBuffer *vertex_buffers;
    BitmapData *lightmap;
    Matrix4x3 *node_matrices;
    int16_t node_matrix_count;
    uint16_t *local_node_remap_table;
    unsigned int local_node_remap_table_size;
    RenderLighting *lighting;
    RenderAnimation *animation;
    float z_sort;
    VectorXYZ centroid;
    Plane3D plane;
    int sorted_index;
    int16_t prev_group_presorted_index;
    int16_t next_group_presorted_index;
    int active_camouflage_transparent_source_object_index;
    uint8_t sort_last;
    uint8_t cortana_hack;
} TransparentGeometryGroup;
_Static_assert(sizeof(TransparentGeometryGroup) == 0xA8);

/**
 * Get the vertex buffer type of a transparent geometry group
 * @param group transparent geometry group
 * @return vertex buffer type
 */
VertexBufferType rasterizer_geometry_group_get_vertex_buffer_type(TransparentGeometryGroup *group);

/**
 * Draw a transparent geometry group
 * @param group transparent geometry group
 * @param param_2 unknown parameter
 */
void rasterizer_transparent_geometry_group_draw(TransparentGeometryGroup *group, uint32_t *param_2);

/**
 * Draw the vertices of a transparent geometry group
 * @param group transparent geometry group
 * @param has_lightmap whether the group has a lightmap
 */
void rasterizer_transparent_geometry_group_draw_vertices(bool has_lightmap, TransparentGeometryGroup *group);

#ifdef __cplusplus
}
#endif

#endif
