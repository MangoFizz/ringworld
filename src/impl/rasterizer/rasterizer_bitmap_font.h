#ifndef RINGWORLD__RASTERIZER__BITMAP_FONT_H
#define RINGWORLD__RASTERIZER__BITMAP_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/font.h"
#include "../tag/definitions/bitmap.h"
#include "rasterizer_screen_geometry.h"

enum {
    FONT_CHARACTER_CACHE_BITMAP_WIDTH = 512,
    FONT_CHARACTER_CACHE_BITMAP_HEIGHT = 512,
    MAX_FONT_CACHE_CHARACTERS = 512
};

typedef struct BitmapFontCacheCharacter {
    FontCharacter *character;
    VectorXYInt position;
} BitmapFontCacheCharacter;
_Static_assert(sizeof(BitmapFontCacheCharacter) == 0x8);

typedef struct BitmapFontCache {
    bool initialized;
    int16_t read_index;
    int16_t write_index;
    VectorXYInt position;
    int16_t maximum_character_height;
    BitmapData *bitmap;
    BitmapFontCacheCharacter characters[MAX_FONT_CACHE_CHARACTERS];
} BitmapFontCache;
_Static_assert(sizeof(BitmapFontCache) == 0x10 + MAX_FONT_CACHE_CHARACTERS * sizeof(BitmapFontCacheCharacter));

/**
 * Initialize the bitmap font cache.
 */
bool rasterizer_bitmap_font_cache_initialize(void);

/**
 * Get the font cache.
 */
BitmapFontCache *rasterizer_bitmap_font_get_cache(void);

/**
 * Set up vertex shader constants for bitmap fonts rasterization.
 */
void rasterizer_bitmap_font_set_up_vertex_shader_constants(void);

/**
 * Begin rasterizing bitmap font text draw with the given screen geometry parameters.
 * @param params The screen geometry parameters.
 */
void rasterizer_bitmap_font_begin_text_draw(RasterizerDynamicScreenGeometryParams *params);

/**
 * End rasterizing bitmap font text draw.
 */
void rasterizer_bitmap_font_end_text_draw(void);

/**
 * Draw a character with a drop shadow.
 * @todo Find out what the parameters are.
 */
void rasterizer_bitmap_font_draw_character_with_shadow();

/**
 * Draw a Unicode string within the specified bounds and text rectangle.
 * @param bounds The bounding rectangle where the string will be drawn.
 * @param text_rect The rectangle defining the text area.
 * @param offset_out Pointer to store the output offset after drawing the string.
 * @param flags Flags specifying additional drawing options (e.g., style).
 * @param string The Unicode string to be drawn.
 */
void rasterizer_bitmap_font_draw_unicode_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const wchar_t *string);

/**
 * Draw an ASCII string within the specified bounds and text rectangle.
 * @param bounds The bounding rectangle where the string will be drawn.
 * @param text_rect The rectangle defining the text area.
 * @param offset_out Pointer to store the output offset after drawing the string.
 * @param flags Flags specifying additional drawing options (e.g., style).
 * @param string The ASCII string to be drawn.
 */
void rasterizer_bitmap_font_draw_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const char *string);

/**
 * Draw a unicode string with the given parameters and a drop shadow.
 * @param rect The rectangle to draw the string.
 * @param unk An unknown boolean parameter.
 * @param string The Unicode string to draw.
 */
void rasterizer_bitmap_font_draw_unicode_string_and_hack_in_icons(Rectangle2D *rect, bool unk, const wchar_t *string);

#ifdef __cplusplus
}
#endif

#endif
