#include "rasterizer_screen.h"

extern bool *rasterizer_enable_user_interface_render;

uint16_t rasterizer_screen_width = 640.0f;
uint16_t rasterizer_screen_height = 480.0f;

uint16_t rasterizer_screen_get_width(void) {
    return rasterizer_screen_width;
}

uint16_t rasterizer_screen_get_height(void) {
    return rasterizer_screen_height;
}

bool rasterizer_screen_user_interface_render_enabled(void) {
    return *rasterizer_enable_user_interface_render;
}
