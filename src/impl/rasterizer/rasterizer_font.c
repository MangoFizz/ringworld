#include "rasterizer_bitmap_font.h"
#include "rasterizer_vector_font.h"
#include "rasterizer_font.h"

bool rasterizer_font_initialize(void) {
    rasterizer_vector_fonts_initialize();
    return rasterizer_bitmap_font_cache_initialize();
}
