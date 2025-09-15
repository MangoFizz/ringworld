#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <d3d9.h>
#include "../tag/definitions/shader.h"

/**
 * Get the pointer to the Direct3D 9 device.
* @return pointer to D3D9 device
*/
IDirect3DDevice9 *rasterizer_dx9_device(void);

/**
 * Get the pointer to the Direct3D 9 device capabilities.
 * @return pointer to D3D9 device capabilities
 */
D3DCAPS9 *rasterizer_dx9_device_caps(void);

/**
 * Check if the Direct3D 9 device supports software vertex processing.
 * @return True if the device supports software vertex processing, false otherwise.
 */
bool rasterizer_dx9_device_supports_software_vertex_processing(void);

/**
 * Set the vertex shader.
 * @param vertex_shader The vertex shader to set.
 */
void rasterizer_dx9_set_vertex_shader(IDirect3DVertexShader9 *vertex_shader);

/**
 * Set the vertex declaration.
 * @param vertex_buffer_type The index of the vertex declaration.
 * @todo Move this to the vertex header.
 */
void rasterizer_dx9_set_vertex_declaration(uint16_t vertex_buffer_type);

/**
 * Set the pixel shader.
 * @param pixel_shader The pixel shader to set.
 */
void rasterizer_dx9_set_pixel_shader(IDirect3DPixelShader9 *pixel_shader);

/**
 * Set the render state.
 * @param state The render state to set.
 * @param value The value to set the render state to.
 */
void rasterizer_dx9_set_render_state(D3DRENDERSTATETYPE state, DWORD value);

/**
 * Set the framebuffer blend function
 */
void rasterizer_dx9_set_framebuffer_blend_function(FramebufferBlendFunction blend_function);

/**
 * Set the sampler state
 * @param sampler The sampler to set the state of.
 * @param type The type of the sampler state.
 * @param value The value to set the sampler state to.
 */
void rasterizer_dx9_set_sampler_state(uint16_t sampler, D3DSAMPLERSTATETYPE type, DWORD value);

/**
 * Set the texture
 * @param stage The stage to set the texture to.
 * @param texture The texture to set.
 */
void rasterizer_dx9_set_texture(uint16_t stage, IDirect3DTexture9 *texture);

/**
 * Set the vertex shader constant
 * @param start_register The start register to set the constant to.
 * @param data The data to set the constant to.
 * @param count The number of constants to set.
 */
bool rasterizer_dx9_set_vertex_shader_constant_f(uint16_t start_register, const void *data, uint16_t count);

/**
 * Set the pixel shader constant
 * @param start_register The start register to set the constant to.
 * @param data The data to set the constant to.
 * @param count The number of constants to set.
 */
bool rasterizer_dx9_set_pixel_shader_constant_f(uint16_t start_register, const void *data, uint16_t count);

/**
 * Set the texture stage state
 * @param stage The stage to set the state of.
 * @param type The type of the texture stage state.
 * @param value The value to set the texture stage state to.
 */
void rasterizer_dx9_set_texture_stage_state(uint16_t stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);

/**
 * Set the stencil mode
 * @param stencil_mode The stencil mode to set.
 */
void rasterizer_dx9_set_stencil_mode(uint16_t stencil_mode);

/**
 * Draw a primitive with the given vertex data
 */
bool rasterizer_dx9_draw_primitive_up(D3DPRIMITIVETYPE primitive_type, UINT primitive_count, const void *vertex_data, UINT vertex_stride);

/**
 * Set the render target
 * @param render_target The render target to set.
 * @param render_target_index The index of the render target.
 */
void rasterizer_dx9_set_render_target(DWORD render_target_index, IDirect3DSurface9 *render_target);

/**
 * Get the render target
 * @param render_target The render target to get.
 */
void rasterizer_dx9_set_viewport(const D3DVIEWPORT9 *viewport);

/**
 * Clear the render target
 * @param count The number of rectangles to clear.
 * @param rects The rectangles to clear.
 * @param flags The flags to clear with.
 * @param color The color to clear with.
 * @param z The z value to clear with.
 * @param stencil The stencil value to clear with.
 */
void rasterizer_dx9_clear(uint32_t count, const D3DRECT *rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil);

/**
 * Set the software vertex processing
 * @param software_vertex_processing The software vertex processing to set.
 */
void rasterizer_dx9_set_software_vertex_processing(bool software_vertex_processing);

/**
 * Enable depth biasing if supported by the device
 * @param depth_bias The depth bias to set.
 * @param slope_scale_depth_bias The slope scale depth bias to set.
 */
void rasterizer_dx9_set_depth_bias(float depth_bias, float slope_scale_depth_bias);

#ifdef __cplusplus
}
#endif

#endif
