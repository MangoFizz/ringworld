#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_RENDER_TARGET_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_RENDER_TARGET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <d3d9.h>

#include "../memory/memory.h"

enum {
    NUM_OF_RENDER_TARGETS = 9
};

typedef struct RasterizerDx9RenderTarget {
    uint32_t width;
    uint32_t height;
    D3DFORMAT format;
    IDirect3DSurface9 *surface;
    IDirect3DTexture9 *texture; 
} RasterizerDx9RenderTarget;
_Static_assert(sizeof(RasterizerDx9RenderTarget) == 0x14);

/**
 * Set a render target.
 * @param clear_color The color that the render target will be cleared to.
 * @param index The index of the d3d9 render target to set
 * @param render_target_index The index of the render target.
 */
void rasterizer_dx9_render_target_set(ColorARGBInt clear_color, int16_t index, uint16_t render_target_index);

/**
 * Get the render target surface.
 * @param render_target_index The index of the render target.
 * @return The render target surface.
 */
IDirect3DSurface9 *rasterizer_dx9_render_target_get_surface(uint16_t render_target_index);

/**
 * Get the render target texture.
 * @param render_target_index The index of the render target.
 * @return The render target texture.
 */
IDirect3DTexture9 *rasterizer_dx9_render_target_get_texture(uint16_t render_target_index);

/**
 * Get the render target.
 * @param render_target_index The index of the render target.
 * @return The render target.
 */
RasterizerDx9RenderTarget *rasterizer_dx9_render_target_get(uint16_t render_target_index);

/**
 * Get the current render target index.
 * @return The current render target index.
 */
uint16_t rasterizer_dx9_render_target_current_index(void);

#ifdef __cplusplus
}
#endif

#endif

