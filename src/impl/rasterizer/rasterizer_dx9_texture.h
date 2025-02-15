#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_TEXTURE_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../bitmap/bitmap.h"
#include "../tag/tag.h"

enum {
    RASTERIZER_DX9_MAX_TEXTURE_STAGES = 4,
};

/**
 * Calculate the number of mipmaps levels for a texture.
 * @param height The height of the texture.
 * @param bitmap The bitmap data.
 * @param width The width of the texture.
 * @return The number of mipmaps levels.
 */
uint32_t rasterizer_dx9_texture_calculate_mipmaps_levels(uint16_t *height, BitmapData *bitmap, uint16_t *width);

/**
 * Create a texture from a bitmap.
 * @param bitmap The bitmap data.
 * @return True if the texture was created successfully, false otherwise.
 */
bool rasterizer_dx9_texture_create(BitmapData *bitmap);

/**
 * Set the bitmap data texture; if the bitmap is not loaded, it will be loaded.
 * @param stage The stage to set the texture to.
 * @param bitmap_data_index The index of the bitmap data.
 * @param bitmap_tag The tag handle of the bitmap.
 * @return True if the texture was set successfully, false otherwise.
 */
bool rasterizer_dx9_texture_set_bitmap_data_texture_no_assert(uint32_t stage, uint16_t bitmap_data_index, TagHandle bitmap_tag);

/**
 * Set the bitmap data texture; if the bitmap is not loaded, it will be loaded.
 * @param stage The stage to set the texture to.
 * @param bitmap_type The type of the bitmap.
 * @param bitmap_usage The usage of the bitmap.
 * @param bitmap_data_index The index of the bitmap data.
 * @param bitmap_tag The tag handle of the bitmap.
 * @return True if the texture was set successfully, false otherwise.
 */
bool rasterizer_dx9_texture_set_bitmap_data_texture(uint16_t stage, BitmapType bitmap_type, BitmapUsage bitmap_usage, uint16_t bitmap_data_index, TagHandle bitmap_tag);

/**
 * Set the bitmap data texture directly.
 * @param stage The stage to set the texture to.
 * @param bitmap_data The bitmap data.
 * @return True if the texture was set successfully, false otherwise.
 */
bool rasterizer_dx9_texture_set_bitmap_data_directly(uint16_t stage, BitmapData *bitmap_data);

#ifdef __cplusplus
}
#endif

#endif

