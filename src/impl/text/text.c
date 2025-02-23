#include "text.h"

extern TextDrawGlobals *text_drawing_globals;
extern ColorARGBInt *text_shadow_color;

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

void text_set_tab_stops(uint16_t *tab_stops, size_t count) {
    TextDrawGlobals *globals = text_get_drawing_globals();
    for(size_t i = 0; i < count; i++) {
        globals->tabs[i] = tab_stops[i];
    }
}

void text_reset_tab_stops(void) {
    TextDrawGlobals *globals = text_get_drawing_globals();
    for(size_t i = 0; i < sizeof(globals->tabs) / sizeof(globals->tabs[0]); i++) {
        globals->tabs[i] = 0;
    }
}

void text_set_shadow_color(ColorARGBInt color) {
    *text_shadow_color = color;
}
