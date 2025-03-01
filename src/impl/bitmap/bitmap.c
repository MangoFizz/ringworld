#include <math.h>
#include "../exception/exception.h"
#include "../rasterizer/rasterizer_dx9_texture.h"
#include "../rasterizer/rasterizer_dx9_vertex.h"
#include "../rasterizer/rasterizer_screen_geometry.h"
#include "../render/render.h"
#include "bitmap.h"

uint8_t bitmap_format_bits_per_pixel[] = { 0x08, 0x08, 0x08, 0x10, 0x00, 0x00, 0x10, 0x00, 0x10, 0x10, 0x20, 0x20, 0x00, 0x00, 0x04, 0x08, 0x08, 0x08, 0xFF };

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

bool bitmap_format_type_valid_width(BitmapFormat format, BitmapType type, uint16_t width) {
    return width > 0 && width <= 30000;
}

bool bitmap_format_type_valid_height(BitmapFormat format, BitmapType type, uint16_t height) {
    return height > 0 && height <= 30000;
}

uint32_t bitmap_mipmap_get_pixel_count(uint8_t mipmap_index, BitmapData *bitmap_data) {
    uint16_t width = 1;
    if((bitmap_data->width >> (mipmap_index & 0x1F)) >= 2) {
        width = bitmap_data->width >> (mipmap_index & 0x1F);
    }

    uint16_t height = 1;
    if((bitmap_data->height >> (mipmap_index & 0x1F)) >= 2) {
        height = bitmap_data->height >> (mipmap_index & 0x1F);
    }

    uint16_t depth = 1;
    if((bitmap_data->depth >> (mipmap_index & 0x1F)) >= 2) {
        depth = bitmap_data->depth >> (mipmap_index & 0x1F);
    }

    if(bitmap_data->flags.compressed) {
        width -= width & 3;
        height -= height & 3;
    }

    uint32_t size = width * height * depth;
    if(bitmap_data->type == BITMAP_DATA_TYPE_CUBE_MAP) {
        size *= 6;
    }

    return size;
}

size_t bitmap_get_pixel_data_size(BitmapData *bitmap_data) {
    ASSERT(bitmap_data != NULL);
    size_t size = 0;
    for(byte i = 0; i < bitmap_data->mipmap_count; i++) {
        size += bitmap_mipmap_get_pixel_count(i, bitmap_data);
    }
    uint8_t bits_per_pixel = bitmap_format_bits_per_pixel[bitmap_data->format];
    return bits_per_pixel * size + ((bits_per_pixel * size >> 31 & 7) >> 3);
}

BitmapData *bitmap_new_2d_bitmap_data(uint16_t width, uint16_t height, uint16_t mipmap_count, BitmapDataFormat format) {
    ASSERT(bitmap_format_type_valid_width(format, BITMAP_TYPE_2D_TEXTURES, width));
    ASSERT(bitmap_format_type_valid_height(format, BITMAP_TYPE_2D_TEXTURES, height));

    BitmapData *bitmap_data = GlobalAlloc(GPTR, sizeof(BitmapData));
    if(bitmap_data == NULL) {
        CRASHF_DEBUG("Failed to allocate memory for bitmap data");
    }

    bitmap_data->signature = TAG_GROUP_BITMAP;
    bitmap_data->width = width;
    bitmap_data->height = height;
    bitmap_data->depth = 1;
    bitmap_data->type = BITMAP_TYPE_2D_TEXTURES;
    bitmap_data->format = format;
    bitmap_data->flags.runtime_created = true;
    bitmap_data->mipmap_count = mipmap_count;

    if(((width & width - 1) == 0) && ((height & height - 1) == 0)) {
        bitmap_data->flags.power_of_two_dimensions = true;
    }

    if(format == BITMAP_DATA_FORMAT_P8) {
        bitmap_data->flags.palettized = true;
    }

    bitmap_data->bitmap_tag_id = NULL_HANDLE;
    bitmap_data->pointer = (void *)0xFFFFFFFF;
    uint32_t pixel_data_size = bitmap_get_pixel_data_size(bitmap_data);
    bitmap_data->base_address = GlobalAlloc(GPTR, pixel_data_size);
    bitmap_data->hardware_format = NULL;

    return bitmap_data;
}

void bitmap_draw(BitmapData *bitmap, Bounds2D *bounds, float scale, float rotation, float fade, VectorXYInt *offset) {
    float sin_rotation = sin(rotation);
    float cos_rotation = cos(rotation);
    uint32_t color = (uint32_t)(fade * 255.0f) << 24 | 0xFFFFFF;
    float reg_point_x = bitmap->registration_point.x;
    float reg_point_y = bitmap->registration_point.y;
    uint16_t width = bitmap->width;
    uint16_t height = bitmap->height;

    Bounds2D default_bounds;
    default_bounds.left = 0.0f;
    default_bounds.right = 1.0f;
    default_bounds.top = 0.0f;
    default_bounds.bottom = 1.0f;
    if(bounds == NULL) {
        bounds = &default_bounds;
    }

    RasterizerDynamicVertex vertices[4];
    for(size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
        float pos_x, pos_y;
        float screen_pos_x, screen_pos_y;
        if(((i + 1) & 2) == 0) {
            pos_x = bounds->left;
        }
        else {
            pos_x = bounds->right;
        }
        if(i < 2) {
            pos_y = bounds->top;
        }
        else {
            pos_y = bounds->bottom;
        }

        float texture_offset_x = (width * pos_x - reg_point_x) * scale;
        float texture_offset_y = (height * pos_y - reg_point_y) * scale;

        RasterizerDynamicVertex *vertex = &vertices[i];
        vertex->position.x = (texture_offset_x * cos_rotation + offset->x) - texture_offset_y * sin_rotation;
        vertex->position.y = (texture_offset_y * cos_rotation + offset->y) + texture_offset_x * sin_rotation;
        vertex->position.z = 0.0f;
        vertex->texture_pos.x = pos_x;
        vertex->texture_pos.y = pos_y;
        vertex->color = color;
    }

    RasterizerDynamicScreenGeometryParams screen_geometry_parameters;
    memset(&screen_geometry_parameters, 0, sizeof(screen_geometry_parameters));
    screen_geometry_parameters.meter_parameters = NULL;
    screen_geometry_parameters.map_texture_scale[0].x = 1.0f;
    screen_geometry_parameters.map_texture_scale[0].y = 1.0f;
    screen_geometry_parameters.map_scale[0].x = 1.0f;
    screen_geometry_parameters.map_scale[0].y = 1.0f;
    screen_geometry_parameters.point_sampled = false;
    screen_geometry_parameters.framebuffer_blend_function = FRAMEBUFFER_BLEND_FUNCTION_ALPHA_MULTIPLY_ADD;
    screen_geometry_parameters.map[0] = bitmap;
    rasterizer_screen_geometry_draw(&screen_geometry_parameters, vertices);
}

