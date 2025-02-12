#include <stdio.h>
#include <windows.h>
#include <d3d9.h>

#include "../exception/exception.h"
#include "rasterizer_dx9.h"
#include "rasterizer_dx9_shader_blob_file.h"
#include "rasterizer_dx9_shader_effect.h"

extern RasterizerDx9ShaderEffectEntry *shader_effects;
extern RasterizerDx9ShaderEffect **effect_collection_buffer;
extern uint32_t *effect_collection_effect_count;

static uint32_t pop_count(char **data) {
    uint32_t value = *(uint32_t *)*data;
    *data += 4;
    return value;
}

static void pop_string(char **data, char *buffer) {
    uint32_t size = pop_count(data);
    memcpy(buffer, *data, size);
    buffer[size] = '\0';
    *data += size;
}

static unsigned char *pop_function_blob(char **data) {
    uint32_t size = pop_count(data);
    unsigned char *value = *data;
    *data += size * 4;
    return value;
}

/**
 * @note The original function used to check the hardware capabilities to determine 
 * which shader effect collection to load. This implementation assumes the hardware
 * is capable of running the highest shader effect collection available at the time.
 */
bool rasterizer_dx9_shader_effect_load_collection_from_binary(void) {
    void *buffer;
    size_t bytes_read;
    ASSERT(rasterizer_dx9_shader_read_binary_file(&buffer, &bytes_read, "shaders/EffectCollection_ps_2_0.enc"));

    char name[256];
    char *data = buffer;
    uint32_t shaders_count = pop_count(&data);

    IDirect3DDevice9 *device = rasterizer_dx9_device();
    ASSERT(device != NULL);

    *effect_collection_buffer = GlobalAlloc(GPTR, sizeof(RasterizerDx9ShaderEffect) * shaders_count);
    *effect_collection_effect_count = shaders_count;
    if(*effect_collection_buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for shader effects.\n");
        return false;
    }

    for(size_t i = 0; i < shaders_count; i++) {
        RasterizerDx9ShaderEffect *effect = &(*effect_collection_buffer)[i];
        pop_string(&data, effect->name);
        effect->pixel_shader_count = pop_count(&data);
        effect->pixel_shaders = GlobalAlloc(GPTR, sizeof(RasterizerDx9PixelShader) * effect->pixel_shader_count);
        if(effect->pixel_shaders == NULL) {
            fprintf(stderr, "Failed to allocate memory for pixel shaders.\n");
            return false;
        }
        for(size_t j = 0; j < effect->pixel_shader_count; j++) {
            RasterizerDx9PixelShader *shader = &effect->pixel_shaders[j];
            pop_string(&data, shader->name);
            unsigned char *shader_function = pop_function_blob(&data);
            ASSERT(IDirect3DDevice9_CreatePixelShader(device, (DWORD *)shader_function, &shader->pixel_shader) == D3D_OK);
        }
    }

    GlobalFree(buffer);

    return true;
}

void rasterizer_dx9_shader_effects_dispose(void) {
    for(size_t i = 0; i < *effect_collection_effect_count; i++) {
        RasterizerDx9ShaderEffect *effect = &(*effect_collection_buffer)[i];
        for(size_t j = 0; j < effect->pixel_shader_count; j++) {
            RasterizerDx9PixelShader *shader = &effect->pixel_shaders[j];
            if(shader->pixel_shader != NULL) {
                IDirect3DPixelShader9_Release(shader->pixel_shader);
            }
        }
        GlobalFree(effect->pixel_shaders);
    }
    GlobalFree(*effect_collection_buffer);
    *effect_collection_buffer = NULL;
}

RasterizerDx9ShaderEffect *rasterizer_dx9_shader_effect_get(uint16_t index) {
    ASSERT(index < NUM_OF_SHADER_EFFECTS);
    return shader_effects[index].effect;
}
