#include "../exception/exception.h"
#include "../font/bitmap_font.h"
#include "../tag/definitions/font.h"
#include "../text/text.h"
#include "../text/string.h"
#include "bitmap_font.h"

extern Rectangle2D *text_draw_bounds;
extern Font **text_draw_font;

TagHandle bitmap_font_get_style(TagHandle font_tag, FontStyle style) {
    Font *font_data = tag_get_data(TAG_GROUP_FONT, font_tag);
    TagHandle font_handle = NULL_HANDLE;
    switch(style) {
        case FONT_STYLE_BOLD:
            font_handle = font_data->bold.tag_handle;
        case FONT_STYLE_ITALIC:
            font_handle = font_data->italic.tag_handle;
        case FONT_STYLE_CONDENSE:
            font_handle = font_data->condense.tag_handle;
        case FONT_STYLE_UNDERLINE:
            font_handle = font_data->underline.tag_handle;
        default:
            font_handle = font_tag;
    }
    if(HANDLE_IS_NULL(font_handle)) {
        font_handle = font_tag;
    }
    return font_handle;
}

FontCharacter *bitmap_font_get_character(Font *font, wchar_t character) {
    FontCharacterTables *character_table = TAG_BLOCK_GET_ELEMENT(font->character_tables, character >> 8);
    size_t characters_count = character_table->character_table.count;
    if(characters_count > 0) {
        if(characters_count == 0x100) {
            FontCharacterIndex *character_index = TAG_BLOCK_GET_ELEMENT(character_table->character_table, character & 0xFF);
            if(character_index->character_index != 0xFFFF) {
                return TAG_BLOCK_GET_ELEMENT(font->characters, character_index->character_index);
            }
        }
    }
    return NULL;
}

void bitmap_font_calculate_unicode_string_draw_bounds(const wchar_t *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds) {
    text_draw_bounds->top = INT16_MAX;
    text_draw_bounds->left = INT16_MAX;
    text_draw_bounds->bottom = INT16_MIN;
    text_draw_bounds->right = INT16_MIN;

    TextDrawGlobals *globals = text_get_drawing_globals();
    TagHandle font_tag = bitmap_font_get_style(globals->font, globals->style);
    Font *font_data = tag_get_data(TAG_GROUP_FONT, font_tag);
    *text_draw_font = font_data;

    VectorXYInt offset; 
    bitmap_font_draw_unicode_string(bitmap_font_calculate_draw_bounds, position, &offset, NULL, 0, string); // @todo Check dest_rect parameter

    if(first_character_position) {
        first_character_position->left = offset.x;
        first_character_position->right = offset.x + 1;
        first_character_position->top = offset.y - font_data->ascending_height;
        first_character_position->bottom = font_data->descending_height + offset.y;
    }

    if(text_bounds) {
        text_bounds->left = text_draw_bounds->left;
        text_bounds->top = position->top;
        text_bounds->right = text_draw_bounds->right;
        text_bounds->bottom = font_data->descending_height + offset.y;
    }
}

void bitmap_font_calculate_string_draw_bounds(const char *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds) {
    text_draw_bounds->top = INT16_MAX;
    text_draw_bounds->left = INT16_MAX;
    text_draw_bounds->bottom = INT16_MIN;
    text_draw_bounds->right = INT16_MIN;

    TextDrawGlobals *globals = text_get_drawing_globals();
    TagHandle font_tag = bitmap_font_get_style(globals->font, globals->style);
    Font *font_data = tag_get_data(TAG_GROUP_FONT, font_tag);
    *text_draw_font = font_data;

    VectorXYInt offset; 
    bitmap_font_draw_string(bitmap_font_calculate_draw_bounds, position, &offset, NULL, 0, string); // @todo Check dest_rect parameter

    if(first_character_position) {
        first_character_position->left = offset.x;
        first_character_position->right = offset.x + 1;
        first_character_position->top = offset.y - font_data->ascending_height;
        first_character_position->bottom = font_data->descending_height + offset.y;
    }

    if(text_bounds) {
        text_bounds->left = text_draw_bounds->left;
        text_bounds->top = position->top;
        text_bounds->right = text_draw_bounds->right;
        text_bounds->bottom = font_data->descending_height + offset.y;
    }
}
