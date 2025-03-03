#ifndef RINGWORLD__RASTERIZER__RASTERIZER_SCREEN_EFFECT_H
#define RINGWORLD__RASTERIZER__RASTERIZER_SCREEN_EFFECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../bitmap/bitmap.h"

typedef struct RasterizerScreenEffectParameters {
    int16_t convolution_extra_passes;
    int16_t convolution_type;
    float convolution_radius;
    BitmapData *convolution_mask;
    float filter_light_enhancement_intensity;
    float filter_desaturation_intensity;
    ColorRGB filter_desaturation_tint;
    bool filter_desaturation_is_additive;
    bool filter_light_enhancement_uses_convolution_mask;
    bool filter_desaturation_uses_convolution_mask;
    bool video_on;
    int32_t overbright_mode;
    BitmapData *scanlines_map;
    float noise_intensity;
    float noise_scale;
    BitmapData *noise_map;
} RasterizerScreenEffectParameters;
_Static_assert(sizeof(RasterizerScreenEffectParameters) == 0x38); 

struct CinematicScreenEffectGlobals {
    RasterizerScreenEffectParameters parameters;
    uint8_t has_control;
    uint8_t initialized;
    float convolution_radius[2];
    float convolution_time[2];
    float filter_light_enhancement_intensity[2];
    float filter_desaturation_intensity[2];
    float filter_time[2];
    float script_values[4];
    float near_clip_distance;
};
_Static_assert(sizeof(struct CinematicScreenEffectGlobals) == 0x78);

#ifdef __cplusplus
}
#endif

#endif
