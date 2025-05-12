#ifndef RINGWORLD__TEXT_TEXT_H
#define RINGWORLD__TEXT_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/tag.h"
#include "../types/types.h"

typedef struct TextDrawGlobals {
    TagHandle font;
    uint32_t flags;
    int16_t style;
    int16_t justification;
    ColorARGB color;
    int16_t tabs[0x12];
    VectorXYInt offset;
} TextDrawGlobals;

/**
 * This function retrieves the global text drawing settings.
 * @return A pointer to the TextDrawGlobals structure containing the current text drawing settings.
 */
TextDrawGlobals *text_get_drawing_globals(void);

/**
 * Initialize the text drawing system for a new map.
 */
void text_draw_initialize_for_new_map(void);

/**
 * This function sets the drawing parameters for text.
 * @param style The style of the text.
 * @param justification The justification of the text.
 * @param flags Flags to control the text drawing behavior.
 * @param font_tag The tag handle of the font to be used.
 * @param color The color of the text in ARGB format.
 */
void text_set_drawing_parameters(int16_t style, int16_t justification, uint32_t flags, TagHandle font_tag, ColorARGB *color);

/**
 * This function sets the tab stops for text.
 * @param tab_stops An array of tab stops.
 * @param count The number of tab stops.
 */
void text_set_tab_stops(uint16_t *tab_stops, size_t count);

/**
 * This function resets the tab stops for text.
 */
void text_reset_tab_stops(void);

/**
 * This function sets the shadow color for text.
 * @param color The color of the shadow in ARGB format.
 */
void text_set_shadow_color(ColorARGBInt color);

/**
 * Draw a unicode string with the given parameters.
 * @param position The position to draw the string.
 * @param dest_rect The destination rectangle.
 * @param color The color of the text.
 * @param flags Additional flags for drawing.
 * @param string The Unicode string to draw.
 */
void text_draw_unicode_string(Rectangle2D *position, Rectangle2D *dest_rect, ColorARGBInt *color, uint32_t flags, const wchar_t *string);

/**
 * Draw a string with the given parameters.
 * @param position The position to draw the string.
 * @param dest_rect The destination rectangle.
 * @param color The color of the text.
 * @param flags Additional flags for drawing.
 * @param string The string to draw.
 */
void text_draw_string(Rectangle2D *position, Rectangle2D *dest_rect, ColorARGBInt *color, uint32_t flags, const char *string);

/**
 * Calculate the bounds of a Unicode string to be drawn.
 * @param string The Unicode string to calculate bounds for.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param first_character_position A pointer to the Rectangle2D structure to store the position of the first character.
 * @param text_bounds A pointer to the Rectangle2D structure to store the overall bounds of the text.
 */
void text_calculate_unicode_string_draw_bounds(const wchar_t *string, Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds);

#ifdef __cplusplus
}
#endif

#endif
