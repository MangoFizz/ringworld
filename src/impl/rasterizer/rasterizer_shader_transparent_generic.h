#ifndef RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_GENERIC_H
#define RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_GENERIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "rasterizer_geometry_group.h"

/**
 * Draw the shader transparent generic geometry.
 * @param group The transparent geometry group.
 * @param param_2 Lightmap related parameter.
 */
void rasterizer_shader_transparent_generic_draw(TransparentGeometryGroup *group, uint32_t *param_2);

/**
 * Set up the pixel shader constants for rendering the shader transparent generic.
 * @param group The transparent geometry group.
 */
void rasterizer_dx9_transparent_generic_preprocess(TransparentGeometryGroup *group);

#ifdef __cplusplus
}
#endif

#endif
