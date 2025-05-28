#ifndef RINGWORLD__FONT__BITMAP_FONT_H
#define RINGWORLD__FONT__BITMAP_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"
#include "../tag/tag.h"
#include "font.h"

/**
 * Get the bitmap font style of a given font.
 * @param font_tag The tag handle of the font to get the style from.
 * @param style The font style to retrieve.
 * @return The tag handle of the bitmap font style.
 */
TagHandle bitmap_font_get_style(TagHandle font_tag, FontStyle style);

/**
 * Retrieves the font character information for a given Unicode character.
 * @param font The font to search in.
 * @param character The Unicode character to retrieve.
 * @return A pointer to the FontCharacter structure for the specified character, or NULL if not found.
 */
FontCharacter *bitmap_font_get_character(Font *font, wchar_t character);

/**
 * This function draws a Unicode string on the screen.
 * @param draw_character A function pointer to the character drawing function.
 * @param bounds A pointer to the Rectangle2D structure defining the screen bounds.
 * @param offset_out A VectorXYInt structure to store the offset after drawing.
 * @param position A pointer to the VectorXYInt structure defining the starting position.
 * @param flags Flags to control the text drawing behavior.
 * @param string The Unicode string to be drawn.
 */
void bitmap_font_draw_unicode_string(void (*draw_character)(), const Rectangle2D *bounds, VectorXYInt *offset_out, const Rectangle2D *text_rect, uint32_t flags, const wchar_t *string);

/**
 * This function draws an ASCII string on the screen.
 * @param draw_character A function pointer to the character drawing function.
 * @param bounds A pointer to the Rectangle2D structure defining the screen bounds.
 * @param offset_out A VectorXYInt structure to store the offset after drawing.
 * @param position A pointer to the VectorXYInt structure defining the starting position.
 * @param flags Flags to control the text drawing behavior.
 * @param string The ASCII string to be drawn.
 * @return Returns true if the string was drawn successfully, false otherwise.
 */
bool bitmap_font_draw_string(void (*draw_character)(), const Rectangle2D *bounds, VectorXYInt *offset_out, const Rectangle2D *text_rect, uint32_t flags, const char *string);

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
void bitmap_font_calculate_unicode_string_draw_bounds(const wchar_t *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds);

/**
 * Calculate the bounds of an ASCII string to be drawn.
 * @param string The ASCII string to calculate bounds for.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param first_character_position A pointer to the Rectangle2D structure to store the position of the first character.
 * @param text_bounds A pointer to the Rectangle2D structure to store the overall bounds of the text.
 */
void bitmap_font_calculate_string_draw_bounds(const char *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds);

#ifdef __cplusplus
}
#endif

#endif
