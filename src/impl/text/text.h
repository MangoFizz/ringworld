#ifndef RINGWORLD__TEXT_TEXT_H
#define RINGWORLD__TEXT_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/tag.h"
#include "../types/types.h"

typedef struct TextDrawGlobals {
    uint32_t string_list_index;
    TagHandle font;
    uint32_t unknown1;
    int16_t unknown_ffff;
    int16_t justification;
    ColorARGB color;
    int16_t tabs[0x12];
    VectorXYInt offset;
} TextDrawGlobals;

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
 * This function retrieves the global text drawing settings.
 * @return A pointer to the TextDrawGlobals structure containing the current text drawing settings.
 */
TextDrawGlobals *text_get_drawing_globals(void);

#ifdef __cplusplus
}
#endif

#endif


