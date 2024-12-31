#include <stdint.h>
#include <stdio.h>
#include <d3d9.h>

#include "../exception/exception.h"
#include "rasterizer_dx9_vertex.h"
#include "rasterizer_dx9.h"

extern IDirect3DDevice9 **d3d9_device;
extern D3DCAPS9 *d3d9_device_caps;
extern RenderGlobals *render_globals;
extern FrameParameters *frame_parameters;

RenderGlobals *get_render_globals(void) {
    return render_globals;
}

FrameParameters *get_frame_parameters(void) {
    return frame_parameters;
}

IDirect3DDevice9 *rasterizer_dx9_device(void) {
    return *d3d9_device;
}

D3DCAPS9 *rasterizer_dx9_device_caps(void) {
    return d3d9_device_caps;
}

void rasterizer_dx9_set_vertex_shader(IDirect3DVertexShader9 *vertex_shader) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetVertexShader(*d3d9_device, vertex_shader);
}

void rasterizer_dx9_set_vertex_declaration(uint16_t vertex_buffer_type) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetVertexDeclaration(*d3d9_device, rasterizer_dx9_vertex_get_declaration(vertex_buffer_type));
}

void rasterizer_dx9_set_pixel_shader(IDirect3DPixelShader9 *pixel_shader) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetPixelShader(*d3d9_device, pixel_shader);
}

void rasterizer_dx9_set_render_state(D3DRENDERSTATETYPE state, DWORD value) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetRenderState(*d3d9_device, state, value);
}

void rasterizer_dx9_set_sampler_state(uint16_t sampler, D3DSAMPLERSTATETYPE type, DWORD value) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetSamplerState(*d3d9_device, sampler, type, value);
}

void rasterizer_dx9_set_texture(uint16_t stage, IDirect3DTexture9 *texture) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetTexture(*d3d9_device, stage, (IDirect3DBaseTexture9 *)texture);
}

bool rasterizer_dx9_set_vertex_shader_constant_f(uint16_t start_register, const float *data, uint16_t count) {
    ASSERT(*d3d9_device != NULL);
    return IDirect3DDevice9_SetVertexShaderConstantF(*d3d9_device, start_register, data, count) == D3D_OK;
}

bool rasterizer_dx9_set_pixel_shader_constant_f(uint16_t start_register, const float *data, uint16_t count) {
    ASSERT(*d3d9_device != NULL);
    return IDirect3DDevice9_SetPixelShaderConstantF(*d3d9_device, start_register, data, count) == D3D_OK;
}

void rasterizer_dx9_set_texture_stage_state(uint16_t stage, D3DTEXTURESTAGESTATETYPE type, DWORD value) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetTextureStageState(*d3d9_device, stage, type, value);
}
