#include "../exception/exception.h"
#include "rasterizer_dx9.h"
#include "rasterizer_dx9_render_target.h"

extern RasterizerDx9RenderTarget *render_targets;
extern uint16_t *current_render_target_index;

IDirect3DSurface9 *rasterizer_dx9_render_target_get_surface(uint16_t render_target_index) {
    ASSERT(render_target_index < NUM_OF_RENDER_TARGETS);
    return render_targets[render_target_index].surface;
}

IDirect3DTexture9 *rasterizer_dx9_render_target_get_texture(uint16_t render_target_index) {
    ASSERT(render_target_index < NUM_OF_RENDER_TARGETS);
    return render_targets[render_target_index].texture;
}

RasterizerDx9RenderTarget *rasterizer_dx9_render_target_get(uint16_t render_target_index) {
    ASSERT(render_target_index < NUM_OF_RENDER_TARGETS);
    return &render_targets[render_target_index];
}

uint16_t rasterizer_dx9_render_target_current_index(void) {
    return *current_render_target_index;
}
