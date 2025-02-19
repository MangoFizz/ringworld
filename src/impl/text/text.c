#include "text.h"

extern TextDrawGlobals *text_drawing_globals;

TextDrawGlobals *text_get_drawing_globals(void) {
    return text_drawing_globals;
}

void text_set_drawing_parameters(int16_t style, int16_t justification, uint32_t flags, TagHandle font_tag, ColorARGB *color) {
    TextDrawGlobals *globals = text_get_drawing_globals();
    globals->style = style;
    globals->justification = justification;
    globals->flags = flags;
    globals->font = font_tag;
    globals->color = *color;
}
