#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_SCREEN_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Get the width of the screen.
 * @return The width of the screen.
 */
uint16_t rasterizer_screen_get_width(void);

/**
 * Get the height of the screen.
 * @return The height of the screen.
 */
uint16_t rasterizer_screen_get_height(void);

#ifdef __cplusplus
}
#endif

#endif
