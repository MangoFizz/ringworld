#ifndef RINGWORLD__RASTERIZER__RASTERIZER_CONFIG_H
#define RINGWORLD__RASTERIZER__RASTERIZER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * Check if the linear texture addressing extension is enabled.
 */
bool rasterizer_config_linear_texture_addressing_sun(void);

#ifdef __cplusplus
}
#endif

#endif
