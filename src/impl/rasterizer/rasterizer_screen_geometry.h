#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_SCREEN_GEOMETRY_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_SCREEN_GEOMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"
#include "../tag/definitions/bitmap.h"
#include "rasterizer_dx9_vertex.h"

typedef struct RasterizerMeterParams {
    ColorARGBInt gradient_min_color;
    ColorARGBInt gradient_max_color;
    ColorARGBInt background_color;
    ColorARGBInt flash_color;
    uint8_t flash_color_is_negative;
    uint8_t tint_mode_2;
    ColorARGBInt tint_color;
    float gradient;
} RasterizerMeterParams;

typedef struct RasterizerDynamicScreenGeometryParams {
    RasterizerMeterParams *meter_parameters;
    VectorXY *offset;
    bool map_anchor_screen[3];
    BitmapData *map[3];
    bool map_wrapped[3];
    VectorXY *map_offset[3];
    VectorXY map_scale[3];
    VectorXY map_texture_scale[3];
    ColorRGB *map_tint[3];
    ColorARGB plasma_fade;
    uint8_t doing_plasma_effect;
    float *map_fade[3];
    int16_t map0_to_1_blend_function;
    int16_t map1_to_2_blend_function;
    int16_t framebuffer_blend_function;
    bool point_sampled;
} RasterizerDynamicScreenGeometryParams;
_Static_assert(sizeof(RasterizerDynamicScreenGeometryParams) == 140);

/**
 * Renders a screen geometry quad.
 * @param params Pointer to the structure containing the rendering parameters.
 * @param vertices Pointer to an array of vertex structures that define the corners of the screen quad.
 */
void rasterizer_screen_geometry_draw(RasterizerDynamicScreenGeometryParams *params, RasterizerDynamicVertex *vertices);

/**
 * Renders a screen geometry quad.
 * @param rect Pointer to the rectangle structure that defines the screen quad.
 * @param color The color of the quad.
 */
void rasterizer_screen_geometry_draw_quad(Rectangle2D *rect, ColorARGBInt color);

#ifdef __cplusplus
}
#endif

#endif
