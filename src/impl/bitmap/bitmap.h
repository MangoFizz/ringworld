#ifndef RINGWORLD__BITMAP__BITMAP_H
#define RINGWORLD__BITMAP__BITMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <d3d9.h>
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
 * @return Pointer to the bitmap loaded data.
 */
void *bitmap_load(bool block, bool load, BitmapData *bitmap);

/**
 * Validate the width of a bitmap based on the format and type.
 * @param format The format of the bitmap.
 * @param type The type of the bitmap.
 * @param width The width of the bitmap.
 * @return True if the width is valid, false otherwise.
 */
bool bitmap_format_type_valid_width(BitmapFormat format, BitmapType type, uint16_t width);

/**
 * Validate the height of a bitmap based on the format and type.
 * @param format The format of the bitmap.
 * @param type The type of the bitmap.
 * @param height The height of the bitmap.
 * @return True if the height is valid, false otherwise.
 */
bool bitmap_format_type_valid_height(BitmapFormat format, BitmapType type, uint16_t height);

/**
 * Get the number of pixels in a mipmap level.
 * @param mipmap_index The index of the mipmap level.
 * @param bitmap_data The bitmap data.
 * @return The number of pixels in the mipmap level.
 */
uint32_t bitmap_mipmap_get_pixel_count(uint8_t mipmap_index, BitmapData *bitmap_data);

/**
 * Get the size of the pixel data in a bitmap.
 * @param bitmap_data The bitmap data.
 * @return The size of the pixel data.
 */
uint32_t bitmap_get_pixel_data_size(BitmapData *bitmap_data);

/**
 * Create a new 2D bitmap data.
 * @param width The width of the bitmap.
 * @param height The height of the bitmap.
 * @param mipmap_count The number of mipmaps.
 * @param format The format of the bitmap.
 * @return The new bitmap data.
 */
BitmapData *bitmap_new_2d_bitmap_data(uint16_t width, uint16_t height, uint16_t mipmap_count, BitmapDataFormat format);

/**
 * Get the address of a pixel in a bitmap.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param mipmap_level The mipmap level of the pixel. // @todo Confirm this
 * @param bitmap The bitmap data.
 * @return The address of the pixel.
 */
void *bitmap_address_for_pixel(uint16_t x, uint16_t y, uint8_t mipmap_level, BitmapData *bitmap);

/**
 * Draw a bitmap in a specified rectangle with a color mask.
 * @param screen_rect The position and size of the screen rectangle to draw the bitmap.
 * @param texture_rect The position and size of the texture rectangle to use from the bitmap.
 * @param bitmap The bitmap data to draw.
 * @param rect Unknown.
 * @param color_mask The color mask to apply to the bitmap.
 * @todo Verify the parameters.
 */
void bitmap_draw_in_rect(BitmapData *bitmap, const Rectangle2D *rect, ColorARGBInt color_mask, const Rectangle2D *screen_rect, const Rectangle2D *texture_rect);

/**
 * Draw a bitmap with specified parameters.
 * @param bitmap The bitmap data to draw.
 * @param bounds The bounds within which to draw the bitmap.
 * @param scale The scale factor to apply to the bitmap.
 * @param rotation The rotation angle to apply to the bitmap.
 * @param fade The fade factor to apply to the bitmap.
 * @param offset The offset to apply to the bitmap.
 */
void bitmap_draw(BitmapData *bitmap, const Bounds2D *bounds, float scale, float rotation, float fade, const VectorXYInt *offset);

#ifdef __cplusplus
}
#endif

#endif
