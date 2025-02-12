#ifndef RINGWORLD__MATH__COLOR_H
#define RINGWORLD__MATH__COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"

static ColorRGB color_rgb_white = {1.0f, 1.0f, 1.0f};

/**
 * Decode a 24-bit RGB color into a ColorRGB structure.
 */
void color_decode_r8g8b8(uint32_t rgb, ColorRGB *output);

/**
 * Decode a 32-bit ARGB color into a ColorARGB structure.
 */
void color_decode_a8r8g8b8(uint32_t argb, ColorARGB *output);

/**
 * Encode a ColorRGB structure into a 24-bit RGB color.
 */
uint32_t color_encode_r8g8b8(ColorRGB *rgb);

/**
 * Encode a ColorARGB structure into a 32-bit ARGB color.
 */
uint32_t color_encode_a8r8g8b8(ColorARGB *argb);

#ifdef __cplusplus
}
#endif

#endif
