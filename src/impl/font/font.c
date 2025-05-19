#include "bitmap_font.h"
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
