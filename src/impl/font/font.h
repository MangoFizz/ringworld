#ifndef RINGWORLD__TEXT_FONT_H
#define RINGWORLD__TEXT_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/tag.h"
#include "../tag/definitions/font.h"
#include "../tag/definitions/vector_font.h"

typedef enum PACKED_ENUM FontStyle {
    FONT_STYLE_PLAIN = -1,
    FONT_STYLE_BOLD,
    FONT_STYLE_ITALIC,
    FONT_STYLE_CONDENSE,
    FONT_STYLE_UNDERLINE,
    FONT_STYLE_SIZE = 0x7FFF
} FontStyle;

/**
 * Get the font data for a given font tag handle and style.
 * If the font is a bitmap font, it retrieves the data for the specified 
 * style; if the font is a vector font, it retrieves the data which 
 * contains all styles.
 */
void *font_get_data(TagHandle font_tag_handle, FontStyle style);

/**
 * Check if a character exists in the specified font.
 * @param font_tag_handle The tag handle of the font.
 * @param character The character to check for existence.
 * @return True if the character exists in the font, false otherwise.
 */
bool font_character_exists(TagHandle font_tag_handle, wchar_t character);

/**
 * Retrieve the tag handle of a font by its file path.
 * This function first attempts to find the font in the TAG_GROUP_FONT group,
 * and if not found, it tries the TAG_GROUP_VECTOR_FONT group.
 * @param font_path The file path of the font.
 * @return The tag handle of the font.
 */
static inline TagHandle font_find_by_path(const char *font_path) {
    TagHandle font_tag = tag_lookup(font_path, TAG_GROUP_FONT);
    if(HANDLE_IS_NULL(font_tag)) {
        font_tag = tag_lookup(font_path, TAG_GROUP_VECTOR_FONT);
    }
    return font_tag;
}

/**
 * Retrieve the default small font tag handle.
 * @return The tag handle of the default small font.
 */
static inline TagHandle font_get_default_small(void) {
    return font_find_by_path("ui\\small_ui");
}

/**
 * Retrieve the default large font tag handle.
 * @return The tag handle of the default large font.
 */
static inline TagHandle font_get_default_large(void) {
    return font_find_by_path("ui\\large_ui");
}

/**
 * Retrieve the default terminal font tag handle.
 * @return The tag handle of the default terminal font.
 */
static inline TagHandle font_get_default_terminal(void) {
    return font_find_by_path("ui\\ticker");
}

/**
 * Calculate the height of a font characters.
 * @param font_tag_handle The tag handle of the font.
 * @return The height of the font characters.
 */
static inline uint16_t font_get_height(TagHandle font_tag_handle) {
    TagEntry *tag_entry = tag_get_entry(font_tag_handle);
    if(tag_entry->primary_group == TAG_GROUP_FONT) {
        Font *font = tag_get_data(TAG_GROUP_FONT, font_tag_handle);
        return font->ascending_height + font->descending_height;
    }
    else {
        VectorFont *font = tag_get_data(TAG_GROUP_VECTOR_FONT, font_tag_handle);
        return font->font_size;
    }
}

#ifdef __cplusplus
}
#endif

#endif
