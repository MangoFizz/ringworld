#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_H

#ifdef __cplusplus
extern "C" {
#endif

#include <d3d9.h>

#include "../tag/definitions/bitmap.h"
#include "../tag/definitions/shader.h"
#include "../tag/tag.h"

typedef enum RasterizerLock {
    RASTERIZER_LOCK_NONE = 0,
    RASTERIZER_LOCK_TEXTURE_CHANGED,
    RASTERIZER_LOCK_VERTEXBUFFER_NEW,
    RASTERIZER_LOCK_DETAIL_OBJECTS,
    RASTERIZER_LOCK_DECAL_UPDATE,
    RASTERIZER_LOCK_DECAL_VERTICES,
    RASTERIZER_LOCK_BINK,
    RASTERIZER_LOCK_UI,
    RASTERIZER_LOCK_CINEMATICS,
    RASTERIZER_LOCK_KOTH,
    RASTERIZER_LOCK_HUD,
    RASTERIZER_LOCK_FLAG,
    RASTERIZER_LOCK_LIGHTNING,
    RASTERIZER_LOCK_DEBUG,
    RASTERIZER_LOCK_TEXT,
    RASTERIZER_LOCK_CONTRAIL,
    RASTERIZER_LOCK_SPRITE,
    RASTERIZER_LOCK_BSP_SWITCH,
    NUMBER_OF_RASTERIZER_LOCKS
} RasterizerLock;

typedef struct RasterizerGlobals {
    Bool initialized;
    int16_t lock_index;
    Rectangle2D screen_bounds;
    Rectangle2D frame_bounds;
    int64_t frame_index;
    uint32_t flip_index;
    uint8_t use_floating_point_zbuffer;
    uint8_t use_rasterizer_frame_rate_throttle;
    uint8_t use_rasterizer_frame_rate_stabilization;
    int16_t refresh_rate;
    float z_near;
    float z_far;
    float z_near_first_person;
    float z_far_first_person;
    IDirect3DBaseTexture9 **default_texture_white;
    IDirect3DBaseTexture9 **default_texture_2d_texture;
    IDirect3DBaseTexture9 **default_texture_3d_texture; // engine actually uses 2d for 3d cases
    IDirect3DBaseTexture9 **default_texture_cubemap;
    int16_t lightmap_mode;
    int16_t maximum_nodes_per_model;
    Bool using_software_vertex_processing;
    int16_t unk2;
    uint32_t fixed_function_ambient; // ambient light value for FF
    Bool use_cheap_active_camo;
    Bool render_targets_disabled;
    Bool alpha_render_targets_disabled;
} RasterizerGlobals; 
_Static_assert(sizeof(RasterizerGlobals) == 0x60);

/**
 * Get the pointer to the rasterizer globals.
 * @return pointer to rasterizer globals
 */
RasterizerGlobals *rasterizer_dx9_get_globals(void);

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

#ifdef __cplusplus
}
#endif

#endif
