#include "rasterizer_screen.h"

uint16_t rasterizer_screen_width = 640.0f;
uint16_t rasterizer_screen_height = 480.0f;

uint16_t rasterizer_screen_get_width(void) {
    return rasterizer_screen_width;
}

uint16_t rasterizer_screen_get_height(void) {
    return rasterizer_screen_height;
}
