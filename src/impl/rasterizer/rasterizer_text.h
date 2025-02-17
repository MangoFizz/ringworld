#ifndef RINGWORLD__RASTERIZER__RASTERIZER_TEXT_H
#define RINGWORLD__RASTERIZER__RASTERIZER_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/font.h"
#include "../tag/definitions/bitmap.h"
#include "rasterizer_screen_geometry.h"

typedef struct FontCacheCharacter {
    FontCharacter *character;
    VectorXYInt position;
} FontCacheCharacter;
_Static_assert(sizeof(FontCacheCharacter) == 0x8);

typedef struct RasterizerFontCache {
    bool initialized;
    int16_t read_index;
    int16_t write_index;
    VectorXYInt position;
    int16_t maximum_character_height;
    BitmapData *bitmap;
    FontCacheCharacter characters[512];
} RasterizerFontCache;
_Static_assert(sizeof(RasterizerFontCache) == 0x1010);

/**
 * Get the font cache.
 */
RasterizerFontCache *rasterizer_text_get_font_cache(void);

/**
 * Set up vertex shader constants for text rasterization.
 */
void rasterizer_text_set_up_vertex_shader_constants(void);

/**
 * Begin rasterizing text with the given screen geometry parameters.
 * @param params The screen geometry parameters.
 */
void rasterizer_text_begin(RasterizerDynamicScreenGeometryParams *params);

/**
 * End rasterizing text.
 */
void rasterizer_text_end(void);

/**
 * Draw a character with a drop shadow.
 * @todo Find out what the parameters are.
 */
void rasterizer_text_draw_character_with_shadow();

#ifdef __cplusplus
}
#endif

#endif

