#include "../debug/assertion.h"
#include "../exception/exception.h"
#include "rasterizer_dx9.h"
#include "rasterizer_dx9_render_target.h"

extern RasterizerDx9RenderTarget *render_targets;
extern uint16_t *current_render_target_index;
extern bool *d3d9_render_target_unk_flag;
extern bool *rasterizer_render_targets_disabled;
extern IDirect3DIndexBuffer9 **d3d9_render_target_index_buffer;
extern IDirect3DVertexBuffer9 **d3d9_render_target_vertex_buffer;

uint32_t d3d_get_fvf_vertex_size(uint32_t fvf_flags) {
    uint32_t position_flags;
    uint32_t texcoord_flags;
    int vertex_size = 0;
  
    position_flags = fvf_flags & (D3DFVF_POSITION_MASK | D3DFVF_XYZB5);
  
    switch(position_flags) {
        case D3DFVF_XYZ:
            vertex_size = 12; // 3 floats
            break;
        case D3DFVF_XYZRHW:
            vertex_size = 16; // 4 floats
            break;
        case D3DFVF_XYZB1:
            vertex_size = 20; // 4 floats + 1 blend weight
            break;
        case D3DFVF_XYZB2:
            vertex_size = 24;
            break;
        case D3DFVF_XYZB3:
            vertex_size = 28;
            break;
        case D3DFVF_XYZB4:
            vertex_size = 32;
            break;
        default:
            vertex_size = 0;
            break;
    }
  
    if((fvf_flags & D3DFVF_NORMAL) != 0) {
        vertex_size += 12; // 3 floats
    }
    if((fvf_flags & D3DFVF_PSIZE) != 0) {
        vertex_size += 4; // float
    }
    if((fvf_flags & D3DFVF_DIFFUSE) != 0) {
        vertex_size += 4; // DWORD color
    }
    if((fvf_flags & D3DFVF_SPECULAR) != 0) {
        vertex_size += 4; // DWORD color
    }
  
    uint32_t texcoord_count = (fvf_flags & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    texcoord_flags = fvf_flags >> 16;
  
    if(texcoord_flags == 0) {
        vertex_size += texcoord_count * 8; 
    } 
    else {
        while(texcoord_count != 0) {
            uint32_t tex_dim = texcoord_flags & 3;
            switch (tex_dim) {
                case 0: vertex_size += 8;  break; // 2D
                case 1: vertex_size += 12; break; // 3D
                case 2: vertex_size += 16; break; // 4D
                case 3: vertex_size += 4;  break; // 1D
            }
            texcoord_flags >>= 2;
            texcoord_count--;
        }
    }
  
    return vertex_size;
}

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

bool rasterizer_dx9_render_targets_initialize(void) {
    IDirect3DDevice9 *device = rasterizer_dx9_device();
    bool result = true;
    if(*d3d9_render_target_unk_flag == false) {
        HRESULT r1 = IDirect3DDevice9_GetRenderTarget(device, 0, &render_targets[0].surface);
        if(FAILED(r1)) {
            result = false;
            exception_throw_runtime_error("failed to get render target 0");
        }

        D3DSURFACE_DESC desc;
        HRESULT r2 = IDirect3DSurface9_GetDesc(render_targets[0].surface, &desc) == D3D_OK;
        if(FAILED(r2)) {
            result = false;
            exception_throw_runtime_error("failed to get render target 0 desc");
        }

        render_targets[0].width = desc.Width;
        render_targets[0].height = desc.Height;
        render_targets[0].format = desc.Format;
        render_targets[1].width = desc.Width;
        render_targets[1].height = desc.Height;
        render_targets[1].format = D3DFMT_A8R8G8B8;
        render_targets[2].width = desc.Width >> 1;
        render_targets[2].height = desc.Height >> 1;
    }
    else {
        if(FAILED(IDirect3DDevice9_GetRenderTarget(device, 0, &render_targets[1].surface))) {
            result = false;
            exception_throw_runtime_error("failed to get render target 1");
        }

        D3DSURFACE_DESC desc;
        if(FAILED(IDirect3DSurface9_GetDesc(render_targets[1].surface, &desc))) {
            result = false;
            exception_throw_runtime_error("failed to get render target 1 desc");
        }

        render_targets[1].width = desc.Width;
        render_targets[1].height = desc.Height;
        render_targets[1].format = desc.Format;
    }

    if(*rasterizer_render_targets_disabled == false && result) {
        for(uint16_t i = 1; i < NUM_OF_RENDER_TARGETS; i++) {
            if(render_targets[i].texture == NULL && *d3d9_render_target_unk_flag == false || render_targets[1].format != D3DFMT_A8R8G8B8) {
                int mipmap_levels = RENDER_TARGET_DEFAULT_MIPMAP_LEVELS;
#ifdef RINGWORLD_ENABLE_ENHANCEMENTS
                if(i == RENDER_TARGET_WATER) {
                    mipmap_levels = RENDER_TARGET_WATER_MIPMAP_LEVELS;
                }
#endif
                HRESULT r1 = IDirect3DDevice9_CreateTexture(device, render_targets[i].width, render_targets[i].height, mipmap_levels, D3DUSAGE_RENDERTARGET,
                                                            render_targets[i].format, D3DPOOL_DEFAULT, &render_targets[i].texture, NULL);
                if(FAILED(r1)) {
                    result = false;
                    exception_throw_runtime_error("failed to create render target %d", i);
                }

                if(render_targets[i].texture == NULL) {
                    exception_throw_runtime_error("failed to create render target %d texture", i);
                }
                
                if(FAILED(IDirect3DTexture9_GetSurfaceLevel(render_targets[i].texture, 0, &render_targets[i].surface))) {
                    result = false;
                    exception_throw_runtime_error("failed to get render target %d surface", i);
                }
            }
        }
    }

    if(FAILED(IDirect3DDevice9_CreateIndexBuffer(device, 8, 8, D3DFMT_INDEX16, D3DPOOL_DEFAULT, d3d9_render_target_index_buffer, NULL))) {
        result = false;
        exception_throw_runtime_error("failed to create index buffer");
    }

    if(*d3d9_render_target_index_buffer == NULL) {
        return false;
    }

    uint16_t *indices = NULL;
    if(FAILED(IDirect3DIndexBuffer9_Lock(*d3d9_render_target_index_buffer, 0, 4, (void **)&indices, 0))) {
        result = false;
        exception_throw_runtime_error("failed to lock index buffer");
    }

    if(indices != NULL) {
        for(size_t i = 0; i < 4; i++) {
            indices[i] = i;
        }

        if(FAILED(IDirect3DIndexBuffer9_Unlock(*d3d9_render_target_index_buffer))) {
            result = false;
            exception_throw_runtime_error("failed to unlock index buffer");
        }
    }
    
    if(result) {
        uint32_t fvf = D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE;
        uint32_t fvf_size = d3d_get_fvf_vertex_size(fvf);
        uint32_t usage_flags = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
        if(FAILED(IDirect3DDevice9_CreateVertexBuffer(device, 4 * fvf_size, usage_flags, fvf,
                                                        D3DPOOL_DEFAULT, d3d9_render_target_vertex_buffer, NULL))) {
            result = false;
            exception_throw_runtime_error("failed to create vertex buffer");
        }
    }

    return result;
}
