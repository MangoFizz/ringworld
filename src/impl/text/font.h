#ifndef RINGWORLD__TEXT_FONT_H
#define RINGWORLD__TEXT_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/tag.h"
#include "../tag/definitions/font.h"

/**
 * This function retrieves the default small font tag handle.
 * @return The tag handle of the default small font.
 */
static inline TagHandle font_get_default_small(void) {
    return lookup_tag("ui\\small_ui", TAG_GROUP_FONT);
}

/**
 * This function retrieves the default large font tag handle.
 * @return The tag handle of the default large font.
 */
static inline TagHandle font_get_default_large(void) {
    return lookup_tag("ui\\large_ui", TAG_GROUP_FONT);
}

/**
 * This function retrieves the default terminal font tag handle.
 * @return The tag handle of the default terminal font.
 */
static inline TagHandle font_get_default_terminal(void) {
    return lookup_tag("ui\\ticker", TAG_GROUP_FONT);
}

/**
 * Calculate the height of a font characters.
 * @param font_tag_handle The tag handle of the font.
 * @return The height of the font characters.
 */
static inline uint16_t font_get_height(TagHandle font_tag_handle) {
    Font *font = tag_get_data(TAG_GROUP_FONT, font_tag_handle);
    return font->ascending_height + font->descending_height;
}

#ifdef __cplusplus
}
#endif

#endif
