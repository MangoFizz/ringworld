#ifndef RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_CHICAGO_H
#define RINGWORLD__RASTERIZER__RASTERIZER_SHADER_TRANSPARENT_CHICAGO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "../tag/definitions/shader_transparent_chicago.h"

/**
 * Set up the texture stages for rendering the shader.
 * @param shader_data   Pointer to the shader transparent chicago data.
 */
bool rasterizer_shader_transparent_chicago_preprocess(ShaderTransparentChicago *shader_data);

#ifdef __cplusplus
}
#endif

#endif
