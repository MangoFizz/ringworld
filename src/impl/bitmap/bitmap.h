#ifndef RINGWORLD__CONSOLE__CONSOLE_H
#define RINGWORLD__CONSOLE__CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/bitmap.h"

/**
 * Get the bitmap data for a bitmap tag.
 * @param bitmap_tag Handle of the bitmap tag.
 * @param bitmap_data_index The index of the bitmap data.
 * @return The bitmap data
 */
BitmapData *bitmap_get_data(TagHandle bitmap_tag, uint16_t bitmap_data_index);

/**
 * Get the bitmap data for a given sequence.
 * @param bitmap_tag Handle of the bitmap tag.
 * @param bitmap_data_index The index of the sequence.
 * @return The bitmap data
 */
BitmapData *bitmap_group_get_bitmap_from_sequence(TagHandle bitmap_tag, uint16_t sequence_index);

/**
 * Load bitmap texture.
 * @param bitmap The bitmap data.
 * @param block True if the load should block the thread, false otherwise.
 * @param load True if the bitmap should be loaded, false otherwise.
 * @return True if the bitmap was loaded successfully, false otherwise.
 */
bool bitmap_load(bool block, bool load, BitmapData *bitmap);

#ifdef __cplusplus
}
#endif

#endif
