#ifndef RINGWORLD__RASTERIZER__RASTERIZER_MODEL_H
#define RINGWORLD__RASTERIZER__RASTERIZER_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../tag/definitions/shader_model.h"
#include "rasterizer_geometry_group.h"

typedef struct RasterizerModelRenderParameters {
    GeometryFlags flags;
    uint32_t count;
    Matrix4x3 *nodes_matrix;
    int16_t nodes_count;
    RenderLighting lighting;
    RenderAnimation animation;
    RenderModelEffect effect;
    VectorXYZ center_point;
    float scale;
    VectorXY texture_scale;
} RasterizerModelRenderParameters;

/**
 * Get the pixel shader for the given shader model.
 * @param shader The shader model to get the pixel shader for.
 * @return The pixel shader for the given shader model, or NULL if not found.
 */
IDirect3DPixelShader9 *rasterizer_model_get_pixel_shader(ShaderModel *shader);

#ifdef __cplusplus
}
#endif

#endif
