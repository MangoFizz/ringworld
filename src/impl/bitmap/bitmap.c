#include <stdio.h>

#include "../exception/exception.h"
#include "bitmap.h"

BitmapData *bitmap_get_data(TagHandle bitmap_tag, uint16_t bitmap_data_index) {
    Bitmap *bitmap = tag_get_data(TAG_GROUP_BITMAP, bitmap_tag);
    if(bitmap->bitmap_data.count == 0 || bitmap_data_index >= bitmap->bitmap_data.count) {
        CRASHF_DEBUG("Bitmap data index is out of bounds: %d >= %d", bitmap_data_index, bitmap->bitmap_data.count);
    }
    return TAG_BLOCK_GET_ELEMENT(bitmap->bitmap_data, bitmap_data_index);
}
