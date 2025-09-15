#include "../debug/assertion.h"
#include "../render/render.h"
#include "../rng/rng.h"
#include "rasterizer_dx9_vertex_shader_constants.h"
#include "rasterizer_dx9.h"
#include "rasterizer_screen_effect.h"
#include "rasterizer_config.h"

void rasterizer_screen_effect_set_texture_transforms(RasterizerScreenEffectParameters *effect_params, int16_t base_conv_passes, int16_t extra_conv_passes, bool override_transforms, int32_t screen_width, int32_t screen_height) {
    bool linear_texture_addressing = rasterizer_config_linear_texture_addressing_sun();
    uint16_t canvas_width = render_get_screen_width();
    uint16_t canvas_height = render_get_screen_height();

    BitmapData fallback_bitmap;
    fallback_bitmap.width = screen_width;
    fallback_bitmap.height = screen_height;
    fallback_bitmap.format = linear_texture_addressing == 0 ? BITMAP_DATA_FORMAT_A8 : BITMAP_DATA_FORMAT_DXT5;

    BitmapData *conv_mask_source = effect_params->convolution_mask;
    BitmapData *scanlines_map = effect_params->video_on ? effect_params->scanlines_map : &fallback_bitmap;
    BitmapData *noise_map = effect_params->video_on ? effect_params->noise_map : &fallback_bitmap;

    BitmapData *conv_mask;
    if(effect_params->convolution_mask == NULL || (base_conv_passes < 1 && (extra_conv_passes != 1 && effect_params->convolution_type == 0))) {
        conv_mask = &fallback_bitmap;
    } 
    else {
        conv_mask = effect_params->convolution_mask;
    }

    float convolution_screen_width = canvas_width;
    float convolution_screen_height = canvas_height;
    if(effect_params->convolution_mask == NULL || (base_conv_passes < 1 && (extra_conv_passes != 1 && effect_params->convolution_type == 0))) {
        convolution_screen_width = screen_width;
        convolution_screen_height = screen_height;
    }
    
    float scanlines_screen_width = screen_width;
    float scanlines_screen_height = screen_height;
    if(effect_params->video_on) {
        // don't fuck up scanlines at c10
        scanlines_screen_width = RASTERIZER_SCREEN_BASE_WIDTH;
        scanlines_screen_height = RASTERIZER_SCREEN_BASE_HEIGHT;
    }

    float noise_screen_width = screen_width;
    float noise_screen_height = screen_height;
    if(effect_params->video_on) {
        noise_screen_width = canvas_width;
        noise_screen_height = canvas_height;
    }

    float inv_conv_mask_width = 1.0f;
    float inv_conv_mask_height = 1.0f;
    if(conv_mask->format != BITMAP_DATA_FORMAT_DXT5) {
        inv_conv_mask_width = 1.0f / conv_mask->width;
        inv_conv_mask_height = 1.0f / conv_mask->height;
    }

    float inv_scanlines_map_width = 1.0f;
    float inv_scanlines_map_height = 1.0f;
    if(scanlines_map->format != BITMAP_DATA_FORMAT_DXT5) {
        inv_scanlines_map_width = 1.0f / scanlines_map->width;
        inv_scanlines_map_height = 1.0f / scanlines_map->height;
    }

    float inv_noise_map_width = 1.0f;
    float inv_noise_map_height = 1.0f;
    if(noise_map->format != BITMAP_DATA_FORMAT_DXT5) {
        inv_noise_map_width = 1.0f / noise_map->width;
        inv_noise_map_height = 1.0f / noise_map->height;
    }

    float inv_screen_width = 1.0f;
    float inv_screen_height = 1.0f;
    if(linear_texture_addressing == 0) {
        inv_screen_width = 1.0f / screen_width;
        inv_screen_height = 1.0f / screen_height;
    }

    VertexShaderTexanimConstants constants = {0};

    constants.tex0_transform_x.i = (convolution_screen_width / screen_width) * inv_conv_mask_width;
    constants.tex0_transform_x.j = 0.0f;
    constants.tex0_transform_x.k = 0.0f;
    constants.tex0_transform_x.w = ((conv_mask->width + 1.0f) - convolution_screen_width) * inv_conv_mask_width * 0.5f;
    
    constants.tex0_transform_y.i = 0.0f;
    constants.tex0_transform_y.j = (convolution_screen_height / screen_height) * inv_conv_mask_height;
    constants.tex0_transform_y.k = 0.0f;
    constants.tex0_transform_y.w = ((conv_mask->height + 1.0f) - convolution_screen_height) * inv_conv_mask_height * 0.5f;
    
    constants.tex1_transform_x.i = (scanlines_screen_width / screen_width) * inv_scanlines_map_width;
    constants.tex1_transform_x.j = 0.0;
    constants.tex1_transform_x.k = 0.0;
    constants.tex1_transform_x.w = ((scanlines_map->width + 1.0f) - scanlines_screen_width) * inv_scanlines_map_width * 0.5f;

    constants.tex1_transform_y.i = 0.0;
    constants.tex1_transform_y.j = (scanlines_screen_height / screen_height) * inv_scanlines_map_height;
    constants.tex1_transform_y.k = 0.0;
    constants.tex1_transform_y.w = ((scanlines_map->height + 1.0f) - scanlines_screen_height) * inv_scanlines_map_height * 0.5f;
    
    constants.tex2_transform_x.i = (noise_screen_width / screen_width) * inv_noise_map_width;
    constants.tex2_transform_x.j = 0.0;
    constants.tex2_transform_x.k = 0.0;
    constants.tex2_transform_x.w = ((noise_map->width + 1.0f) - noise_screen_width) * inv_noise_map_width * 0.5f;

    constants.tex2_transform_y.i = 0.0;
    constants.tex2_transform_y.j = (noise_screen_height / screen_height) * inv_noise_map_height;
    constants.tex2_transform_y.k = 0.0; 
    constants.tex2_transform_y.w = ((noise_map->height + 1.0f) - noise_screen_height) * inv_noise_map_height * 0.5f;

    constants.tex3_transform_x.i = inv_screen_width;
    constants.tex3_transform_x.j = 0.0;
    constants.tex3_transform_x.k = 0.0;
    constants.tex3_transform_x.w = ((screen_width + 1.0) - screen_width) * inv_screen_width * 0.5f;
    
    constants.tex3_transform_y.i = 0.0;
    constants.tex3_transform_y.j = inv_screen_height;
    constants.tex3_transform_y.k = 0.0;
    constants.tex3_transform_y.w = ((screen_height + 1.0) - screen_height) * inv_screen_height * 0.5f;

    if(effect_params->convolution_type == 1) {
        ASSERT(!effect_params->video_on);

        float convolution_radius = effect_params->convolution_radius;
        float conv_offset_x = 0.0f;
        float conv_offset_y = 0.0f;
        if(effect_params->convolution_mask == NULL) {
            conv_offset_x = convolution_radius * inv_conv_mask_width;
            conv_offset_y = convolution_radius * inv_conv_mask_height;
        }

        constants.tex0_transform_x.w = conv_offset_x + constants.tex0_transform_x.w;
        constants.tex0_transform_y.w = conv_offset_y + constants.tex0_transform_y.w;

        constants.tex1_transform_x.w = (convolution_radius * -1.0f) * inv_scanlines_map_width + constants.tex1_transform_x.w;
        constants.tex1_transform_y.w = (convolution_radius * -1.0f) * inv_scanlines_map_height + constants.tex1_transform_y.w;

        constants.tex2_transform_x.w = convolution_radius * inv_noise_map_width + constants.tex2_transform_x.w;
        constants.tex2_transform_y.w = (convolution_radius * -1.0f) * inv_noise_map_height + constants.tex2_transform_y.w;

        constants.tex3_transform_x.w = (convolution_radius * -1.0f) * constants.tex3_transform_x.i + constants.tex3_transform_x.w;
        constants.tex3_transform_y.w = convolution_radius * constants.tex3_transform_y.j + constants.tex3_transform_y.w;
    }
    else if(effect_params->convolution_type == 2) {
        ASSERT(!effect_params->video_on);

        float convolution_radius = effect_params->convolution_radius;
        float conv_mask_offset = 0.0f;
        float screen_offset = convolution_radius * -1.0f;
        float scanlines_offset = 0.0f;
        if(effect_params->convolution_mask == NULL) {
            conv_mask_offset = convolution_radius * -1.0f;
            screen_offset = convolution_radius * 2.0f;
        }

        constants.tex0_transform_x.i = (1.0 - conv_mask_offset / conv_mask->width) * constants.tex0_transform_x.i;
        constants.tex0_transform_x.w = conv_mask_offset * 0.5 * inv_conv_mask_width + constants.tex0_transform_x.w;
        constants.tex0_transform_y.j = (1.0 - conv_mask_offset / conv_mask->height) * constants.tex0_transform_y.j;
        constants.tex0_transform_y.w = conv_mask_offset * 0.5 * inv_conv_mask_height + constants.tex0_transform_y.w;

        constants.tex1_transform_x.i = (1.0 - scanlines_offset / scanlines_map->width) * constants.tex1_transform_x.i;
        constants.tex1_transform_x.w = scanlines_offset * 0.5 * inv_scanlines_map_width + constants.tex1_transform_x.w;
        constants.tex1_transform_y.j = (1.0 - scanlines_offset / scanlines_map->height) * constants.tex1_transform_y.j;
        constants.tex1_transform_y.w = scanlines_offset * 0.5 * inv_scanlines_map_height + constants.tex1_transform_y.w;
        
        constants.tex2_transform_x.i = (1.0 - convolution_radius / noise_map->width) * constants.tex2_transform_x.i;
        constants.tex2_transform_x.w = convolution_radius * 0.5 * inv_noise_map_width + constants.tex2_transform_x.w;
        constants.tex2_transform_y.j = (1.0 - convolution_radius / noise_map->height) * constants.tex2_transform_y.j;
        constants.tex2_transform_y.w = convolution_radius * 0.5 * inv_noise_map_height + constants.tex2_transform_y.w;

        constants.tex3_transform_x.i = (1.0 - screen_offset / screen_width) * constants.tex3_transform_x.i;
        constants.tex3_transform_x.w = screen_offset * 0.5 * constants.tex3_transform_x.i + constants.tex3_transform_x.w;
        constants.tex3_transform_y.j = (1.0 - screen_offset / screen_height) * constants.tex3_transform_y.j;
        constants.tex3_transform_y.w = screen_offset * 0.5 * constants.tex3_transform_y.j + constants.tex3_transform_y.w;
    }
    else if(base_conv_passes == 1 && effect_params->video_on) {
        uint32_t *global_random_seed = rng_get_global_seed();
        constants.tex2_transform_x.w = rng_random_seed_float(global_random_seed) * inv_noise_map_width * noise_map->width + constants.tex2_transform_x.w;
        constants.tex2_transform_y.w = rng_random_seed_float(global_random_seed) * inv_noise_map_height * noise_map->height + constants.tex2_transform_y.w;
    }

    if(override_transforms) {
        constants.tex0_transform_x.i = constants.tex1_transform_x.i;
        constants.tex0_transform_x.j = 0.0;
        constants.tex0_transform_x.k = 0.0;
        constants.tex0_transform_x.w = constants.tex1_transform_x.w;

        constants.tex0_transform_y.i = 0.0;
        constants.tex0_transform_y.j = constants.tex1_transform_y.j;
        constants.tex0_transform_y.k = 0.0;
        constants.tex0_transform_y.w = constants.tex1_transform_y.w;

        constants.tex1_transform_x.i = constants.tex2_transform_x.i;
        constants.tex1_transform_x.j = 0.0;
        constants.tex1_transform_x.k = 0.0;
        constants.tex1_transform_x.w = constants.tex2_transform_x.w;

        constants.tex1_transform_y.i = 0.0;
        constants.tex1_transform_y.j = constants.tex2_transform_y.j;
        constants.tex1_transform_y.k = 0.0;
        constants.tex1_transform_y.w = constants.tex2_transform_y.w;
    }

    rasterizer_dx9_set_vertex_shader_constant_f(VSH_CONSTANTS_TEXANIM_OFFSET, &constants, VSH_CONSTANTS_TEXANIM_COUNT);
}
