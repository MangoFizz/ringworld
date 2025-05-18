#ifndef RINGWORLD__RASTERIZER__VECTOR_FONT_H
#define RINGWORLD__RASTERIZER__VECTOR_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"
#include "../font/vector_font.h"

/**
 * Release the cached vector font resources.
 */
void rasterizer_vector_fonts_flush(void);

/**
 * Draw a Unicode string within the specified bounds and text rectangle.
 * @param bounds The bounding rectangle where the string will be drawn.
 * @param text_rect The rectangle defining the text area.
 * @param offset_out Pointer to store the output offset after drawing the string.
 * @param flags Flags specifying additional drawing options (e.g., style).
 * @param string The Unicode string to be drawn.
 */
void rasterizer_vector_font_draw_unicode_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const wchar_t *string);

/**
 * Draw an ASCII string within the specified bounds and text rectangle.
 * @param bounds The bounding rectangle where the string will be drawn.
 * @param text_rect The rectangle defining the text area.
 * @param offset_out Pointer to store the output offset after drawing the string.
 * @param flags Flags specifying additional drawing options (e.g., style).
 * @param string The ASCII string to be drawn.
 */
void rasterizer_vector_font_draw_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const char *string);

/**
 * Calculate the bounds of a Unicode string to be drawn.
 * @param string The Unicode string to calculate bounds for.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param first_character_position A pointer to the Rectangle2D structure to store the position of the first character.
 * @param text_bounds A pointer to the Rectangle2D structure to store the overall bounds of the text.
 */
void rasterizer_vector_font_calculate_unicode_string_draw_bounds(const wchar_t *string, Rectangle2D *bounds);

#ifdef __cplusplus
}
#endif

#endif
