#include <stdio.h>
#include <windows.h>
#include <d3d9.h>

#include "../debug/assertion.h"
#include "rasterizer_dx9.h"
#include "rasterizer_shaders_blob.h"
#include "rasterizer_dx9_vertex_shader.h"

extern short *vertex_shader_permutations;
extern VertexShader *vertex_shaders;

bool rasterizer_dx9_shader_load_vertex_shaders_from_binary(void) {
    void *buffer;
    size_t buffer_size;
    ASSERT(rasterizer_shaders_blob_read_file(&buffer, &buffer_size, "shaders/vsh.enc"));

    IDirect3DDevice9 *device = rasterizer_dx9_device();
    ASSERT(device != NULL);

    void *buffer_pointer = buffer;
    size_t i = 0;
    while(i < NUM_OF_VERTEX_SHADERS && buffer_pointer < buffer + buffer_size) {
        size_t shader_size = *(uint32_t *)(buffer_pointer);
        void *current_shader = buffer_pointer + sizeof(uint32_t);
        buffer_pointer = current_shader + shader_size;

        if(buffer_pointer > buffer + buffer_size) {
            break;
        }

        IDirect3DVertexShader9 *shader;
        HRESULT res = IDirect3DDevice9_CreateVertexShader(device, (DWORD *)current_shader, &shader);
        if(res != D3D_OK) {
            break;
        }

        vertex_shaders[i].shader = shader;
        i++;
    }

    GlobalFree(buffer);

    if(i < NUM_OF_VERTEX_SHADERS) {
        for(size_t j = 0; j < i; j++) {
            if(vertex_shaders[j].shader != NULL) {
                IDirect3DVertexShader9_Release(vertex_shaders[j].shader);
            }
        }
        return false;
    }

    return true;
}

IDirect3DVertexShader9 *rasterizer_dx9_shader_get_vertex_shader(uint16_t index) {
    ASSERT(index < NUM_OF_VERTEX_SHADERS);
    return vertex_shaders[index].shader;
}

IDirect3DVertexShader9 *rasterizer_dx9_shader_get_vertex_shader_for_permutation(uint16_t vertex_shader_permutation, VertexBufferType vertex_buffer_type) {
    return rasterizer_dx9_shader_get_vertex_shader(vertex_shader_permutations[vertex_shader_permutation + vertex_buffer_type * 6]);
}
