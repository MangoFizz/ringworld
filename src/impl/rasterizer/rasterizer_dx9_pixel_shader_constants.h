#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_PIXEL_SHADER_CONSTANTS_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_PIXEL_SHADER_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../math/math.h"

#define FLOAT4_SIZE (sizeof(float) * 4)
#define FLOAT4_ALIGNED __attribute__((packed, aligned(FLOAT4_SIZE)))

enum PixelShaderConstants {
    PSH_CONSTANTS_SCREEN_OFFSET = 0,
    PSH_CONSTANTS_SCREEN_COUNT = 6,
    PSH_CONSTANTS_MODEL_OFFSET = 0,
    PSH_CONSTANTS_MODEL_COUNT = 5
};

typedef struct FLOAT4_ALIGNED PixelShaderScreenGeometryConstants {
    ColorRGB map_tint_0;
    float map_fade_0;
    ColorRGB map_tint_1;
    float map_fade_1;
    ColorRGB map_tint_2;
    float map_fade_2;
    ColorRGB plasma_fade;
    float plasma_fade_alpha;
    float pad0[3];
    float has_map2;
    float unused[4];
} PixelShaderScreenGeometryConstants;
_Static_assert(sizeof(PixelShaderScreenGeometryConstants) == PSH_CONSTANTS_SCREEN_COUNT * FLOAT4_SIZE);

typedef struct FLOAT4_ALIGNED PixelShaderHUDMeterConstants {
    ColorRGB gradient_max_color;
    float flipped_channels;
    ColorRGB gradient_min_color;
    float progress;
    ColorRGB background_color;
    float background_fade;
    ColorRGB draining_color;
    float gradient;
    ColorRGB opacity_mask;
    float fade;
} PixelShaderHUDMeterConstants;
_Static_assert(sizeof(PixelShaderHUDMeterConstants) == 5 * FLOAT4_SIZE);

typedef struct FLOAT4_ALIGNED PixelShaderModelConstants {
   ColorRGBA primary_color;
   ColorRGBA fog_blend_base;
   ColorRGBA fog_blend_extinction;
   ColorRGBA fog_blend_additive;
   ColorRGBA self_illumination_color;
} PixelShaderModelConstants;
_Static_assert(sizeof(PixelShaderModelConstants) == 5 * FLOAT4_SIZE);

#ifdef __cplusplus
}
#endif

#endif

