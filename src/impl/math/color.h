#ifndef RINGWORLD__MATH__COLOR_H
#define RINGWORLD__MATH__COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"

static const ColorARGB COLOR_ARGB_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
static const ColorARGB COLOR_ARGB_RED = { 1.0f, 1.0f, 0.1f, 0.1f };
static const ColorARGB COLOR_ARGB_YELLOW = {1.0f, 1.0f, 1.0f, 0.125f};
static const ColorARGB COLOR_ARGB_GRAY = {1.0f, 0.7f, 0.7f, 0.7f};
static const ColorARGB COLOR_ARGB_BLACK = {0.0f, 0.0f, 0.0f, 0.0f};
static const ColorRGB COLOR_RGB_WHITE = {1.0f, 1.0f, 1.0f};
static const ColorRGB COLOR_RGB_BLACK = {0.0f, 0.0f, 0.0f};

enum {
    COLOR_INTERPOLATE_IN_HSV_SPACE = 1,
    COLOR_INTERPOLATE_ALONG_FARTHEST_HUE_PATH = 2
};

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
uint32_t color_encode_r8g8b8(const ColorRGB *rgb);

/**
 * Encode a ColorARGB structure into a 32-bit ARGB color.
 */
uint32_t color_encode_a8r8g8b8(const ColorARGB *argb);

/**
 * Interpolate between two colors.
 * @param a The first color.
 * @param b The second color.
 * @param output Pointer to a ColorRGB structure to store the result.
 * @param flags Interpolation flags.
 * @param t Interpolation value.
 * @return The pointer to the output color.
 */
ColorRGB *color_interpolate(const ColorRGB *a, const ColorRGB *b, ColorRGB *output, uint32_t flags, float t);

#ifdef __cplusplus
}
#endif

#endif
