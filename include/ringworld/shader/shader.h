#ifndef RINGWORLD__SHADER__SHADER_H
#define RINGWORLD__SHADER__SHADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/shader.h"

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

#ifdef __cplusplus
}
#endif

#endif
