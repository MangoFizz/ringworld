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
 * Get the bitmap index for a specific frame within a sequence.
 * @param bitmap_tag Handle of the bitmap tag.
 * @param sequence_index The index of the sequence within the bitmap tag.
 * @param frame_index The index of the frame within the sequence.
 * @return The index of the bitmap data corresponding to the specified frame.
 */
int16_t bitmap_group_sequence_get_bitmap_index_for_frame(TagHandle bitmap_tag, uint16_t sequence_index, uint16_t frame_index);

/**
 * Get the bitmap for a specific frame within a sequence.
 * @param bitmap_tag Handle of the bitmap tag.
 * @param sequence_index The index of the sequence within the bitmap tag.
 * @param frame_index The index of the frame within the sequence.
 * @return Pointer to the bitmap data corresponding to the specified frame.
 */
BitmapData *bitmap_group_sequence_get_bitmap_for_frame(TagHandle bitmap_tag, uint16_t sequence_index, uint16_t frame_index);

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
