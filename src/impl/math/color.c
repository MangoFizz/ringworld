#include "color.h"

void color_decode_r8g8b8(uint32_t rgb, ColorRGB *output) {
    output->r = ((rgb >> 16) & 0xFF) / 255.0;
    output->g = ((rgb >> 8) & 0xFF) / 255.0;
    output->b = ((rgb >> 0) & 0xFF) / 255.0;
}

void color_decode_a8r8g8b8(uint32_t argb, ColorARGB *output) {
    output->a = ((argb >> 24) & 0xFF) / 255.0;
    output->r = ((argb >> 16) & 0xFF) / 255.0;
    output->g = ((argb >> 8) & 0xFF) / 255.0;
    output->b = ((argb >> 0) & 0xFF) / 255.0;
}

uint32_t color_encode_r8g8b8(const ColorRGB *rgb) {
    uint32_t output = 0;
    output = (uint32_t)(rgb->r * 255.0) << 16;
    output |= (uint32_t)(rgb->g * 255.0) << 8;
    output |= (uint32_t)(rgb->b * 255.0) << 0;
    return output;
}

uint32_t color_encode_a8r8g8b8(const ColorARGB *argb) {
    uint32_t output;
    output = (uint32_t)(argb->a * 255.0) << 24;
    output |= (uint32_t)(argb->r * 255.0) << 16;
    output |= (uint32_t)(argb->g * 255.0) << 8;
    output |= (uint32_t)(argb->b * 255.0) << 0;
    return output;
}
