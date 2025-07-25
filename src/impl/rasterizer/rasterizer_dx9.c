#include <stdint.h>
#include <stdio.h>

#include "../debug/assertion.h"
#include "rasterizer_dx9_vertex.h"
#include "rasterizer_dx9.h"

extern IDirect3DDevice9 **d3d9_device;
extern D3DCAPS9 *d3d9_device_caps;
extern bool *d3d9_device_supports_software_vertex_processing;
extern uint16_t *d3d9_stencil_mode;
extern uint32_t *d3d9_stencil_mode_unk1;
extern uint32_t *d3d9_pixel_shader_version;

IDirect3DDevice9 *rasterizer_dx9_device(void) {
    return *d3d9_device;
}

D3DCAPS9 *rasterizer_dx9_device_caps(void) {
    return d3d9_device_caps;
}

bool rasterizer_dx9_device_supports_software_vertex_processing(void) {
    return *d3d9_device_supports_software_vertex_processing;
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

bool rasterizer_dx9_set_vertex_shader_constant_f(uint16_t start_register, const void *data, uint16_t count) {
    ASSERT(*d3d9_device != NULL);
    return IDirect3DDevice9_SetVertexShaderConstantF(*d3d9_device, start_register, data, count) == D3D_OK;
}

bool rasterizer_dx9_set_pixel_shader_constant_f(uint16_t start_register, const void *data, uint16_t count) {
    ASSERT(*d3d9_device != NULL);
    return IDirect3DDevice9_SetPixelShaderConstantF(*d3d9_device, start_register, data, count) == D3D_OK;
}

void rasterizer_dx9_set_texture_stage_state(uint16_t stage, D3DTEXTURESTAGESTATETYPE type, DWORD value) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetTextureStageState(*d3d9_device, stage, type, value);
}

/**
 * @todo Verify that this function is correct.
 */
void rasterizer_dx9_set_stencil_mode(uint16_t stencil_mode) {
    if(*d3d9_stencil_mode_unk1 == 0) {
        stencil_mode = 0;
    }
    if(*d3d9_stencil_mode == stencil_mode) {
        return;
    }
    switch(stencil_mode) {
        case 0:
            rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, FALSE);
            break;
        case 1:
            rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, TRUE);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
            rasterizer_dx9_set_render_state(D3DRS_STENCILREF, 1);
            rasterizer_dx9_set_render_state(D3DRS_STENCILMASK, 1);
            rasterizer_dx9_set_render_state(D3DRS_STENCILWRITEMASK, 1);
            break;
        case 2:
            rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, TRUE);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
            rasterizer_dx9_set_render_state(D3DRS_STENCILREF, 0);
            rasterizer_dx9_set_render_state(D3DRS_STENCILMASK, 1);
            rasterizer_dx9_set_render_state(D3DRS_STENCILWRITEMASK, 0);
            break;
        case 3:
            rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, TRUE);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
            rasterizer_dx9_set_render_state(D3DRS_STENCILREF, 0);
            rasterizer_dx9_set_render_state(D3DRS_STENCILMASK, 1);
            rasterizer_dx9_set_render_state(D3DRS_STENCILWRITEMASK, 0);
            break;
        case 4:
            rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, TRUE);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
            rasterizer_dx9_set_render_state(D3DRS_STENCILREF, 2);
            rasterizer_dx9_set_render_state(D3DRS_STENCILMASK, 1);
            rasterizer_dx9_set_render_state(D3DRS_STENCILWRITEMASK, 2);
            break;
        case 5:
            rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, TRUE);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
            rasterizer_dx9_set_render_state(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
            rasterizer_dx9_set_render_state(D3DRS_STENCILREF, 0);
            rasterizer_dx9_set_render_state(D3DRS_STENCILMASK, 3);
            rasterizer_dx9_set_render_state(D3DRS_STENCILWRITEMASK, 0);
            break;
    }
    *d3d9_stencil_mode = stencil_mode;
}

bool rasterizer_dx9_draw_primitive_up(D3DPRIMITIVETYPE primitive_type, UINT primitive_count, const void *vertex_data, UINT vertex_stride) {
    ASSERT(*d3d9_device != NULL);
    return IDirect3DDevice9_DrawPrimitiveUP(*d3d9_device, primitive_type, primitive_count, vertex_data, vertex_stride) == D3D_OK;
}

void rasterizer_dx9_set_render_target(DWORD render_target_index, IDirect3DSurface9 *render_target) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetRenderTarget(*d3d9_device, render_target_index, render_target);
}

void rasterizer_dx9_set_viewport(const D3DVIEWPORT9 *viewport) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetViewport(*d3d9_device, viewport);
}

void rasterizer_dx9_clear(uint32_t count, const D3DRECT *rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_Clear(*d3d9_device, count, rects, flags, color, z, stencil);
}

void rasterizer_dx9_set_software_vertex_processing(bool software_vertex_processing) {
    ASSERT(*d3d9_device != NULL);
    IDirect3DDevice9_SetSoftwareVertexProcessing(*d3d9_device, software_vertex_processing);
}
