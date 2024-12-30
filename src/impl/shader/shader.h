#ifndef RINGWORLD__SHADER__SHADER_H
#define RINGWORLD__SHADER__SHADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/shader.h"
#include "../rasterizer/rasterizer_geometry_group.h"

/**
 * Assert that the shader is of the given type.
 * @param shader The shader to assert.
 * @param type The type to assert.
 * @return Pointer to the shader data.
 */
void *shader_type_assert(Shader *shader, ShaderType type);

/**
 * Get the vertex shader permutation index of a shader.
 * @param shader The shader to get the permutation of.
 * @return The permutation index of the shader.
 */
uint16_t shader_get_vertex_shader_permutation(Shader *shader);

/**
 * Apply a shader texture animation.
 * @param map_u_scale The U scale of the map.
 * @param map_v_scale The V scale of the map.
 * @param map_u_offset The U offset of the map.
 * @param map_v_offset The V offset of the map.
 * @param map_rotation The rotation of the map.
 * @param frame_params The frame parameters.
 * @param texture_animation The texture animation data.
 * @param render_animation The render animation data.
 * @param u_transform_reference The U transform reference.
 * @param v_transform_reference The V transform reference.
 */
void shader_texture_animation_evaluate(float map_u_scale, float map_v_scale, float map_u_offset, float map_v_offset, float map_rotation, float frame_params, void *texture_animation, RenderAnimation *render_animation, float *u_transform_reference, float *v_transform_reference);

#ifdef __cplusplus
}
#endif

#endif
