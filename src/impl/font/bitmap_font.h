#ifndef RINGWORLD__FONT__BITMAP_FONT_H
#define RINGWORLD__FONT__BITMAP_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"

/**
 * This function draws a unicode string on the screen.
 * @param draw_character A function pointer to the character drawing function.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param color The color of the text in ARGB format.
 * @param draw_rect A pointer to the Rectangle2D structure defining the screen bounds.
 * @param flags Flags to control the text drawing behavior.
 * @param string The Unicode string to be drawn.
 */
bool bitmap_font_draw_unicode_string(void (*draw_character)(), Rectangle2D *position, ColorARGBInt *color, Rectangle2D *draw_rect, uint32_t flags, const wchar_t *string);

/**
 * This function draws a string on the screen.
 * @param draw_character A function pointer to the character drawing function.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param color The color of the text in ARGB format.
 * @param draw_rect A pointer to the Rectangle2D structure defining the screen bounds.
 * @param flags Flags to control the text drawing behavior.
 * @param string The string to be drawn.
 */
bool bitmap_font_draw_string(void (*draw_character)(), Rectangle2D *position, ColorARGBInt *color, Rectangle2D *draw_rect, uint32_t flags, const char *string);

/**
 * Calculate the bounds of a string to be drawn.
 * @todo Find out what the parameters are.
 */
void bitmap_font_calculate_draw_bounds();

/**
 * Calculate the bounds of a Unicode string to be drawn.
 * @param string The Unicode string to calculate bounds for.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param first_character_position A pointer to the Rectangle2D structure to store the position of the first character.
 * @param text_bounds A pointer to the Rectangle2D structure to store the overall bounds of the text.
 */
void bitmap_font_calculate_unicode_string_draw_bounds(const wchar_t *string, Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds);

#ifdef __cplusplus
}
#endif

#endif
