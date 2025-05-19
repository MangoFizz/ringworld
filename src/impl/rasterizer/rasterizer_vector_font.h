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
 * @param font The vector font to be used.
 * @param font_style The style of the font.
 * @param bounds A pointer to the Rectangle2D structure defining the screen bounds.
 */
void rasterizer_vector_font_calculate_unicode_string_draw_bounds(const wchar_t *string, VectorFont *font, FontStyle font_style, Rectangle2D *bounds);

#ifdef __cplusplus
}
#endif

#endif
