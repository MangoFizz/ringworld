#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_SCREEN_GEOMETRY_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_SCREEN_GEOMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"
#include "../tag/definitions/bitmap.h"

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

typedef struct RasterizerDynamicScreenQuadParams {
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
} RasterizerDynamicScreenQuadParams;
_Static_assert(sizeof(RasterizerDynamicScreenQuadParams) == 140);

#ifdef __cplusplus
}
#endif

#endif
