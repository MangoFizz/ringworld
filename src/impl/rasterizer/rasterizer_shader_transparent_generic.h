#ifndef RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_GENERIC_H
#define RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_GENERIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <d3d9.h>

#include "../tag/definitions/shader_transparent_generic.h"
#include "rasterizer_geometry_group.h"

/**
 * Draw the shader transparent generic geometry.
 * @param group The transparent geometry group.
 * @param param_2 Lightmap related parameter.
 */
void rasterizer_shader_transparent_generic_draw(TransparentGeometryGroup *group, uint32_t *param_2);

/**
 * Get the instance of the shader transparent generic pixel shader.
 * If the instance does not exist, it will be compiled against the given shader parameters.
 * @param tag The shader transparent generic tag.
 * @return The pixel shader instance.
 */
IDirect3DPixelShader9 *rasterizer_shader_transparent_generic_get_instance(ShaderTransparentGeneric *tag);

/**
 * Set up the pixel shader constants for rendering the shader transparent generic.
 * @param group The transparent geometry group.
 */
void rasterizer_dx9_transparent_generic_preprocess(TransparentGeometryGroup *group);

/**
 * Clear the instances of the shader transparent generic.
 */
void rasterizer_shader_transparent_generic_clear_instances(void);

#ifdef __cplusplus
}
#endif

#endif
