#ifndef RINGWORLD__CONSOLE__CONSOLE_H
#define RINGWORLD__CONSOLE__CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/bitmap.h"

/**
 * Get the bitmap data for a bitmap tag.
 * @param bitmap_tag The bitmap tag.
 * @param bitmap_data_index The index of the bitmap data.
 * @return The bitmap data
 */
BitmapData *bitmap_get_data(TagHandle bitmap_tag, uint16_t bitmap_data_index);

#ifdef __cplusplus
}
#endif

#endif
