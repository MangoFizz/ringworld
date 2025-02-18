#include <math.h>
#include "../types/types.h"
#include "rasterizer_dx9_render_target.h"
#include "rasterizer_screen.h"

extern bool *rasterizer_enable_user_interface_render;

bool rasterizer_enable_widescreen_support = true;

uint16_t rasterizer_screen_get_width(void) {
    RasterizerDx9RenderTarget *render_target = rasterizer_dx9_render_target_get(RENDER_TARGET_BACK_BUFFER);
    if(rasterizer_enable_widescreen_support && render_target->width > render_target->height) {
        float aspect_ratio = (float)render_target->width / (float)render_target->height;
        float width = ceil(RASTERIZER_SCREEN_BASE_HEIGHT * aspect_ratio);
        return (uint16_t)width;
    }
    return RASTERIZER_SCREEN_BASE_WIDTH;
}

uint16_t rasterizer_screen_get_height(void) {
    RasterizerDx9RenderTarget *render_target = rasterizer_dx9_render_target_get(RENDER_TARGET_BACK_BUFFER);
    if(rasterizer_enable_widescreen_support && render_target->height > render_target->width) {
        float aspect_ratio = (float)render_target->height / (float)render_target->width;
        float height = ceil(RASTERIZER_SCREEN_BASE_WIDTH * aspect_ratio);
        return (uint16_t)height;
    }
    return RASTERIZER_SCREEN_BASE_HEIGHT;
}

bool rasterizer_screen_user_interface_render_enabled(void) {
    return *rasterizer_enable_user_interface_render;
}

bool rasterizer_screen_widescreen_support_enabled(void) {
    return rasterizer_enable_widescreen_support;
}
