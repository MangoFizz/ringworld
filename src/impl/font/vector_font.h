#ifndef RINGWORLD__FONT__VECTOR_FONT_H
#define RINGWORLD__FONT__VECTOR_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../memory/dynamic_buffer.h"
#include "../text/text.h"
#include "../tag/tag.h"

typedef struct DynamicArray DynamicArray;

typedef struct VectorFontTextRect {
    DynamicBuffer text;
    bool is_unicode;
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    TextJustification justification;
} VectorFontTextRect;

/**
 * Get the vector font style of a given font.
 * @param font The vector font to get the style from.
 * @param style The font style to retrieve.
 * @return A pointer to the VectorFontStyle structure corresponding to the requested style.
 */
VectorFontStyle *vector_font_get_style(VectorFont *font, FontStyle style);

/**
 * Calculate the draw bounds for a Unicode string.
 * @param string The Unicode string to calculate bounds for.
 * @param position The position rectangle.
 * @param first_character_position Pointer to store the first character position.
 * @param text_bounds Pointer to store the text bounds.
 */
void vector_font_calculate_unicode_string_draw_bounds(const wchar_t *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds);

/**
 * Handle string formatting for vector fonts.
 * @param str The string to format.
 * @param x The x position.
 * @param y The y position.
 * @param width The width of the text area.
 * @param height The height of the text area.
 * @param text_recs Pointer to store the formatted text rectangles.
 * @param count Pointer to store the number of formatted text rectangles.
 */
void vector_font_handle_string_formatting(const char *text, int16_t x, int16_t y, int16_t width, int16_t height, DynamicArray *text_recs);

/**
 * Handle Unicode string formatting for vector fonts.
 * @param str The Unicode string to format.
 * @param x The x position.
 * @param y The y position.
 * @param width The width of the text area.
 * @param height The height of the text area.
 * @param text_recs Pointer to store the formatted text rectangles.
 * @param count Pointer to store the number of formatted text rectangles.
 */
void vector_font_handle_unicode_string_formatting(const wchar_t *text, int16_t x, int16_t y, int16_t width, int16_t height, DynamicArray *text_recs);

/**
 * Calculate the width of a Unicode string.
 * @param string The Unicode string to calculate the width for.
 * @param font The vector font to be used.
 * @param style The style of the font.
 * @return The calculated width of the string.
 */
uint32_t vector_font_calculate_unicode_string_width(wchar_t *string, VectorFont *font, FontStyle style);

#ifdef __cplusplus
}
#endif

#endif
