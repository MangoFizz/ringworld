#include "types.h"

void decode_r8g8b8(uint32_t rgb, ColorRGB *output) {
    output->r = ((rgb >> 16) & 0xFF) / 255.0;
    output->g = ((rgb >> 8) & 0xFF) / 255.0;
    output->b = ((rgb >> 0) & 0xFF) / 255.0;
}

void decode_a8r8g8b8(uint32_t argb, ColorARGB *output) {
    output->a = ((argb >> 24) & 0xFF) / 255.0;
    output->r = ((argb >> 16) & 0xFF) / 255.0;
    output->g = ((argb >> 8) & 0xFF) / 255.0;
    output->b = ((argb >> 0) & 0xFF) / 255.0;
}
