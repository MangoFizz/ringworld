#include <stdint.h>
#include "rasterizer_config.h"

extern uint32_t *linear_texture_addressing_sun;

bool rasterizer_config_linear_texture_addressing_sun(void) {
    return *linear_texture_addressing_sun != 0;
}
