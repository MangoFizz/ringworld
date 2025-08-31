#ifndef RINGWORLD__TEXT_TEXT_H
#define RINGWORLD__TEXT_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../font/font.h"
#include "../tag/tag.h"
#include "../tag/definitions/font.h"
#include "../types/types.h"

typedef enum PACKED_ENUM TextJustification {
    TEXT_JUSTIFICATION_LEFT,
    TEXT_JUSTIFICATION_RIGHT,
    TEXT_JUSTIFICATION_CENTER,
    NUMBER_OF_TEXT_JUSTIFICATIONS,
    TEXT_JUSTIFICATION_MAX = 0xFFFF
} TextJustification;

typedef struct TextDrawGlobals {
    TagHandle font;
    uint32_t flags;
    FontStyle style;
    TextJustification justification;
    ColorARGB color;
    int16_t tabs_count;
    int16_t tabs[0x10];
    int16_t unk1;
    int16_t unk2;
    int16_t first_line_indentation;
    int16_t indentation; // to be confirmed
} TextDrawGlobals;
_Static_assert(sizeof(TextDrawGlobals) == 72);

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
 * This function retrieves the shadow color for text.
 * @return The shadow color in ARGB format.
 */
Pixel32 text_get_shadow_color(void);

/**
 * This function sets the shadow color for text.
 * @param color The color of the shadow in ARGB format.
 */
void text_set_shadow_color(Pixel32 color);

/**
 * Draw a Unicode string within the specified bounds and text rectangle.
 * @param bounds The bounding rectangle where the string will be drawn.
 * @param text_rect The rectangle defining the text area.
 * @param offset_out Pointer to store the output offset after drawing the string.
 * @param flags Flags specifying additional drawing options (e.g., style).
 * @param string The Unicode string to be drawn.
 */
void text_draw_unicode_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const wchar_t *string);

/**
 * Draw an ASCII string within the specified bounds and text rectangle.
 * @param bounds The bounding rectangle where the string will be drawn.
 * @param text_rect The rectangle defining the text area.
 * @param offset_out Pointer to store the output offset after drawing the string.
 * @param flags Flags specifying additional drawing options (e.g., style).
 * @param string The ASCII string to be drawn.
 */
void text_draw_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const char *string);

/**
 * Calculate the bounds of a Unicode string to be drawn.
 * @param string The Unicode string to calculate bounds for.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param first_character_position A pointer to the Rectangle2D structure to store the position of the first character.
 * @param text_bounds A pointer to the Rectangle2D structure to store the overall bounds of the text.
 */
void text_calculate_unicode_string_draw_bounds(const wchar_t *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds);

/**
 * Calculate the bounds of an ASCII string to be drawn.
 * @param string The ASCII string to calculate bounds for.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param first_character_position A pointer to the Rectangle2D structure to store the position of the first character.
 * @param text_bounds A pointer to the Rectangle2D structure to store the overall bounds of the text.
 */
void text_calculate_string_draw_bounds(const char *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds);

#ifdef __cplusplus
}
#endif

#endif
