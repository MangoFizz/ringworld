#include "../font/bitmap_font.h"
#include "../rasterizer/rasterizer_bitmap_font.h"
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

void text_draw_unicode_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const wchar_t *string) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    if(HANDLE_IS_NULL(text_globals->font)) {
        return;
    }

    TagEntry *entry = tag_get_entry(text_globals->font);
    if(entry->primary_group == TAG_GROUP_FONT) {
        rasterizer_bitmap_font_draw_unicode_string(bounds, text_rect, offset_out, flags, string);
    }
    else {
        
    }
}

void text_draw_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const char *string) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    if(HANDLE_IS_NULL(text_globals->font)) {
        return;
    }

    TagEntry *entry = tag_get_entry(text_globals->font);
    if(entry->primary_group == TAG_GROUP_FONT) {
        rasterizer_bitmap_font_draw_string(bounds, text_rect, offset_out, flags, string);
    }
    else {
        
    }
}

void text_calculate_unicode_string_draw_bounds(const wchar_t *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    if(HANDLE_IS_NULL(text_globals->font)) {
        return;
    }

    TagEntry *entry = tag_get_entry(text_globals->font);
    if(entry->primary_group == TAG_GROUP_FONT) {
        bitmap_font_calculate_unicode_string_draw_bounds(string, position, first_character_position, text_bounds);
    }
    else {
        
    }
}
