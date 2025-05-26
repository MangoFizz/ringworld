#include "bitmap_font.h"
#include "vector_font.h"
#include "font.h"

void *font_get_data(TagHandle font_tag_handle, FontStyle style) {
    if(tag_get_primary_group(font_tag_handle) == TAG_GROUP_VECTOR_FONT) {
        return tag_get_data(TAG_GROUP_VECTOR_FONT, font_tag_handle);
    }
    else {
        TagHandle font_style_tag = bitmap_font_get_style(font_tag_handle, style);
        return tag_get_data(TAG_GROUP_FONT, font_style_tag);
    }
}

bool font_character_exists(TagHandle font_tag_handle, wchar_t character) {
    TagGroup font_group = tag_get_primary_group(font_tag_handle);
    if(font_group == TAG_GROUP_VECTOR_FONT) {
        VectorFont *font = tag_get_data(TAG_GROUP_VECTOR_FONT, font_tag_handle);
        if(vector_font_glyph_exists(font, FONT_STYLE_REGULAR, character)) {
            return true;
        }
    }
    else {
        Font *font = tag_get_data(TAG_GROUP_FONT, font_tag_handle);
        FontCharacter *font_character = bitmap_font_get_character(font, character);
        if(font_character) {
            return true;
        }
    }
    return false;
}
