#include "../font/bitmap_font.h"
#include "../tag/tag.h"
#include "../tag/definitions/font.h"
#include "../text/text.h"
#include "bitmap_font.h"

extern Rectangle2D *text_draw_bounds;
extern Font **text_draw_font;

void bitmap_font_calculate_unicode_string_draw_bounds(const wchar_t *string, Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds) {
    text_draw_bounds->top = INT16_MIN;
    text_draw_bounds->left = INT16_MIN;
    text_draw_bounds->bottom = INT16_MAX;
    text_draw_bounds->right = INT16_MAX;

    TextDrawGlobals *globals = text_get_drawing_globals();
    TagHandle font_tag = globals->font;
    Font *font_data = tag_get_data(TAG_GROUP_FONT, font_tag);
    if(globals->style != -1) {
        switch(globals->style) {
            case 0:
                font_tag = font_data->bold.tag_handle;
                break;
            case 1:
                font_tag = font_data->italic.tag_handle;
                break;
            case 2:
                font_tag = font_data->condense.tag_handle;
                break;
            case 3:
                font_tag = font_data->underline.tag_handle;
                break;
        }
        if(HANDLE_IS_NULL(font_tag)) {
            font_tag = globals->font;
        }
    }
    *text_draw_font = font_data;

    VectorXYInt offset; // Maybe it's a Vector2DInt instead of Rectangle2D
    bitmap_font_draw_unicode_string(bitmap_font_calculate_draw_bounds, position, NULL, (Rectangle2D *)&offset, 0, string); // @todo Check dest_rect parameter

    first_character_position->left = offset.x;
    first_character_position->right = offset.x + 1;
    first_character_position->top = offset.y - font_data->ascending_height;
    first_character_position->bottom = font_data->descending_height + offset.y;
    text_bounds->left = text_draw_bounds->left;
    text_bounds->top = position->top;
    text_bounds->right = text_draw_bounds->right;
    text_bounds->bottom = first_character_position->bottom;
}
