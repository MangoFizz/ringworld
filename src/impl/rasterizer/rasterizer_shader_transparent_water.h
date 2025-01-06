#ifndef RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_WATER_H
#define RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_WATER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../tag/definitions/shader_transparent_water.h"
#include "rasterizer_geometry_group.h"

/**
 * Draw the transparent water shader.
 * @param group The transparent geometry group.
 */
void rasterizer_shader_transparent_water_draw(TransparentGeometryGroup *group);

/**
 * Render the bumpmap for the transparent water shader.
 * @param shader The transparent water shader.
 */
void rasterizer_shader_transparent_water_render_bumpmap(ShaderTransparentWater *shader);

#ifdef __cplusplus
}
#endif

#endif 
