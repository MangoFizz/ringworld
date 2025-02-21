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
 * This function draws a unicode string on the screen.
 * @param draw_character A function pointer to the character drawing function.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param color The color of the text in ARGB format.
 * @param draw_rect A pointer to the Rectangle2D structure defining the screen bounds.
 * @param flags Flags to control the text drawing behavior.
 * @param string The Unicode string to be drawn.
 */
bool text_draw_unicode_string(void (*draw_character)(), Rectangle2D *position, ColorARGBInt *color, Rectangle2D *draw_rect, uint32_t flags, wchar_t *string);

/**
 * This function draws a string on the screen.
 * @param draw_character A function pointer to the character drawing function.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param color The color of the text in ARGB format.
 * @param draw_rect A pointer to the Rectangle2D structure defining the screen bounds.
 * @param flags Flags to control the text drawing behavior.
 * @param string The string to be drawn.
 */
bool text_draw_string(void (*draw_character)(), Rectangle2D *position, ColorARGBInt *color, Rectangle2D *draw_rect, uint32_t flags, char *string);

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
 * This function sets the shadow color for text.
 * @param color The color of the shadow in ARGB format.
 */
void text_set_shadow_color(ColorARGBInt color);

#ifdef __cplusplus
}
#endif

#endif


