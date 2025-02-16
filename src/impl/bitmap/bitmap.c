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

int16_t bitmap_group_sequence_get_bitmap_index_for_frame(TagHandle bitmap_tag, uint16_t sequence_index, uint16_t frame_index) {
    ASSERT(sequence_index >= 0 && frame_index >= 0);
    if(!HANDLE_IS_NULL(bitmap_tag)) {
        Bitmap *bitmap = tag_get_data(TAG_GROUP_BITMAP, bitmap_tag);
        if(bitmap != NULL) {
            if(bitmap->bitmap_group_sequence.count != 0) {
                BitmapGroupSequence *sequence = TAG_BLOCK_GET_ELEMENT(bitmap->bitmap_group_sequence, sequence_index % bitmap->bitmap_group_sequence.count);
                if(sequence->bitmap_count == 0) {
                    if(sequence->sprites.count != 0) {
                        BitmapGroupSprite *sprite = TAG_BLOCK_GET_ELEMENT(sequence->sprites, frame_index);
                        return sprite->bitmap_index;
                    }
                }
                else {
                    return sequence->first_bitmap_index + frame_index % sequence->bitmap_count;
                }
                return frame_index;
            }
            return 0;
        } 
    }
    return -1;
}

BitmapData *bitmap_group_sequence_get_bitmap_for_frame(TagHandle bitmap_tag, uint16_t sequence_index, uint16_t frame_index) {
    int16_t bitmap_index = bitmap_group_sequence_get_bitmap_index_for_frame(bitmap_tag, sequence_index, frame_index);
    if(bitmap_index != -1) {
        Bitmap *bitmap = tag_get_data(TAG_GROUP_BITMAP, bitmap_tag);
        return TAG_BLOCK_GET_ELEMENT(bitmap->bitmap_data, bitmap_index);
    }
    return NULL;
}
