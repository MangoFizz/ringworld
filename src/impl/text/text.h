#ifndef RINGWORLD__TEXT_TEXT_H
#define RINGWORLD__TEXT_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"

/**
 * This function draws a Unicode string on the screen.
 * @param draw_character A function pointer to the character drawing function.
 * @param position A pointer to the Rectangle2D structure defining the screen offset.
 * @param color The color of the text in ARGB format.
 * @param draw_rect A pointer to the Rectangle2D structure defining the screen bounds.
 * @param flags Flags to control the text drawing behavior.
 * @param string The Unicode string to be drawn.
 */
bool text_draw_unicode_string(void (*draw_character)(), Rectangle2D *position, ColorARGBInt color, Rectangle2D *draw_rect, uint32_t flags, wchar_t *string);

#ifdef __cplusplus
}
#endif

#endif


