#include "rasterizer_bitmap_font.h"
#include "rasterizer_vector_font.h"
#include "rasterizer_font.h"

bool rasterizer_font_initialize(void) {
    return rasterizer_bitmap_font_cache_initialize();
}

void rasterizer_font_cache_flush(void) {
    rasterizer_bitmap_font_cache_flush();
    rasterizer_vector_fonts_flush();
}
