#ifndef RINGWORLD__RASTERIZER__FONT_H
#define RINGWORLD__RASTERIZER__FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * Initialize both bitmap and vector font resources.
 */
bool rasterizer_font_initialize(void);

/**
 * Flush the font caches.
 */
void rasterizer_font_cache_flush(void);

#ifdef __cplusplus
}
#endif

#endif
