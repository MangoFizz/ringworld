#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_H

#ifdef __cplusplus
extern "C" {
#endif

#include <d3d9.h>

#include "../tag/definitions/bitmap.h"
#include "../tag/definitions/shader.h"
#include "../tag/tag.h"

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
bool rasterizer_dx9_set_vertex_shader_constant_f(uint16_t start_register, const float *data, uint16_t count);

/**
 * Set the pixel shader constant
 * @param start_register The start register to set the constant to.
 * @param data The data to set the constant to.
 * @param count The number of constants to set.
 */
bool rasterizer_dx9_set_pixel_shader_constant_f(uint16_t start_register, const float *data, uint16_t count);

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

#ifdef __cplusplus
}
#endif

#endif
