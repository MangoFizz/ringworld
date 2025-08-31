#include "../font/bitmap_font.h"
#include "../rasterizer/rasterizer_bitmap_font.h"
#include "../rasterizer/rasterizer_vector_font.h"
#include "../render/render.h"
#include "text.h"

extern TextDrawGlobals *text_drawing_globals;
extern Pixel32 *text_shadow_color;

/**
 * @todo Make the return type const.
 */
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
    globals->tabs_count = count;
    for(size_t i = 0; i < count; i++) {
        globals->tabs[i] = tab_stops[i];
    }
}

void text_reset_tab_stops(void) {
    TextDrawGlobals *globals = text_get_drawing_globals();
    globals->tabs_count = 0;
}

Pixel32 text_get_shadow_color(void) {
    return *text_shadow_color;
}

void text_set_shadow_color(Pixel32 color) {
    *text_shadow_color = color;
}

void text_draw_unicode_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const wchar_t *string) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    if(HANDLE_IS_NULL(text_globals->font)) {
        return;
    }

    RenderGlobals *render_globals = render_get_globals();
    Rectangle2D render_bounds;
    if(bounds != NULL) {
        render_bounds = *bounds;
    }
    else {
        render_bounds = render_globals->camera.window_bounds;
        math_rectangle_2d_translate(&render_bounds, render_globals->camera.viewport_bounds.left * -1, render_globals->camera.viewport_bounds.top * -1);
    }

    Rectangle2D text_draw_rect;
    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();
    if(text_rect != NULL) {
        text_draw_rect.bottom = clamp_i32(text_rect->bottom, 0, screen_height);
        text_draw_rect.right = clamp_i32(text_rect->right, 0, screen_width);
        text_draw_rect.top = clamp_i32(text_rect->top, 0, screen_height);
        text_draw_rect.left = clamp_i32(text_rect->left, 0, screen_width);
    }
    else {
        // @todo use viewport bounds for calculating this
        text_draw_rect.bottom = screen_height;
        text_draw_rect.right = screen_width;
        text_draw_rect.top = 0;
        text_draw_rect.left = 0;
    }

    TagEntry *entry = tag_get_entry(text_globals->font);
    if(entry->primary_group == TAG_GROUP_FONT) {
        rasterizer_bitmap_font_draw_unicode_string(&render_bounds, &text_draw_rect, offset_out, flags, string);
    }
    else {
        rasterizer_vector_font_draw_unicode_string(&render_bounds, &text_draw_rect, offset_out, flags, string);
    }
}

void text_draw_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const char *string) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    if(HANDLE_IS_NULL(text_globals->font)) {
        return;
    }

    RenderGlobals *render_globals = render_get_globals();
    Rectangle2D render_bounds;
    if(bounds != NULL) {
        render_bounds = *bounds;
    }
    else {
        render_bounds = render_globals->camera.window_bounds;
        math_rectangle_2d_translate(&render_bounds, render_globals->camera.viewport_bounds.left * -1, render_globals->camera.viewport_bounds.top * -1);
    }

    Rectangle2D text_draw_rect;
    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();
    if(text_rect != NULL) {
        text_draw_rect.bottom = clamp_i32(text_rect->bottom, 0, screen_height);
        text_draw_rect.right = clamp_i32(text_rect->right, 0, screen_width);
        text_draw_rect.top = clamp_i32(text_rect->top, 0, screen_height);
        text_draw_rect.left = clamp_i32(text_rect->left, 0, screen_width);
    }
    else {
        // @todo use viewport bounds for calculating this
        text_draw_rect.bottom = screen_height;
        text_draw_rect.right = screen_width;
        text_draw_rect.top = 0;
        text_draw_rect.left = 0;
    }

    TagEntry *entry = tag_get_entry(text_globals->font);
    if(entry->primary_group == TAG_GROUP_FONT) {
        rasterizer_bitmap_font_draw_string(&render_bounds, &text_draw_rect, offset_out, flags, string);
    }
    else {
        rasterizer_vector_font_draw_string(&render_bounds, &text_draw_rect, offset_out, flags, string);
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
        vector_font_calculate_unicode_string_draw_bounds(string, position, first_character_position, text_bounds);
    }
}

void text_calculate_string_draw_bounds(const char *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    if(HANDLE_IS_NULL(text_globals->font)) {
        return;
    }

    TagEntry *entry = tag_get_entry(text_globals->font);
    if(entry->primary_group == TAG_GROUP_FONT) {
        bitmap_font_calculate_string_draw_bounds(string, position, first_character_position, text_bounds);
    }
    else {
        vector_font_calculate_string_draw_bounds(string, position, first_character_position, text_bounds);
    }
}
