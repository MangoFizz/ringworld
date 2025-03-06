#include <stdio.h>
#include <windows.h>
#include <d3d9.h>

#include "../exception/exception.h"
#include "../memory/shader_blob.h"
#include "rasterizer_dx9.h"
#include "rasterizer_shaders_blob.h"
#include "rasterizer_dx9_shader_effect.h"

enum {
    RESTORED_SHADER_EFFECT_HUD_METERS,
    NUM_OF_RESTORED_SHADER_EFFECTS
};

extern RasterizerDx9ShaderEffectEntry *shader_effects;
extern RasterizerDx9ShaderEffect **effect_collection_buffer;
extern uint32_t *effect_collection_effect_count;
extern char *shader_transparent_generic_source; 
RasterizerDx9ShaderEffect *restored_shader_effects[NUM_OF_RESTORED_SHADER_EFFECTS];

RasterizerDx9ShaderEffect *rasterizer_dx9_shader_effect_find(const char *name) {
    for(size_t i = 0; i < *effect_collection_effect_count; i++) {
        RasterizerDx9ShaderEffect *effect = &(*effect_collection_buffer)[i];
        if(strcmp(effect->name, name) == 0) {
            return effect;
        }
    }
    return NULL;
}

/**
 * @note The original function used to check the hardware capabilities to determine 
 * which shader effect collection to load. This implementation assumes the hardware
 * is capable of running the highest shader effect collection available at the time.
 */
bool rasterizer_dx9_shader_effect_load_collection_from_binary(void) {
    void *effect_collection_blob;
    size_t effect_collection_blob_size;
    ASSERT(rasterizer_shaders_blob_read_file(&effect_collection_blob, &effect_collection_blob_size, "shaders/EffectCollection_ps_3_0.enc"));

    IDirect3DDevice9 *device = rasterizer_dx9_device();
    ASSERT(device != NULL);

    void *blob_pointer = effect_collection_blob;
    uint32_t shaders_count = shader_blob_read_count(&blob_pointer);

    *effect_collection_buffer = GlobalAlloc(GPTR, sizeof(RasterizerDx9ShaderEffect) * shaders_count);
    *effect_collection_effect_count = shaders_count;
    if(*effect_collection_buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for shader effects.\n");
        return false;
    }

    for(size_t i = 0; i < shaders_count; i++) {
        RasterizerDx9ShaderEffect *effect = &(*effect_collection_buffer)[i];
        shader_blob_read_string(&blob_pointer, effect->name);

        printf("Loaded shader effect: %s\n", effect->name);
        
        uint32_t effect_data_count = shader_blob_get_count(blob_pointer);
        if(effect_data_count < 64) {
            effect->pixel_shader_count = shader_blob_read_count(&blob_pointer);
            effect->pixel_shaders = GlobalAlloc(GPTR, sizeof(RasterizerDx9PixelShader) * effect->pixel_shader_count);
            for(size_t j = 0; j < effect->pixel_shader_count; j++) {
                RasterizerDx9PixelShader *shader = &effect->pixel_shaders[j];
                shader_blob_read_string(&blob_pointer, shader->name);
                
                uint32_t pixel_shader_dwords = shader_blob_get_count(blob_pointer);
                uint32_t pixel_shader_bytecode[pixel_shader_dwords];
                shader_blob_read_shader_bytecode(&blob_pointer, pixel_shader_bytecode);
                
                ASSERT(IDirect3DDevice9_CreatePixelShader(device, (void *)pixel_shader_bytecode, &shader->pixel_shader) == D3D_OK);
            }
        }
        else {
            // treat as a pixel shader source
            uint32_t pixel_shader_source_length = effect_data_count;
            char pixel_shader_source[pixel_shader_source_length];
            shader_blob_read_string(&blob_pointer, pixel_shader_source);

            effect->pixel_shader_count = 0;
            effect->pixel_shaders = NULL;
            
            if(strcmp(effect->name, "transparent_generic") == 0) {
                shader_transparent_generic_source = GlobalAlloc(GPTR, pixel_shader_source_length + 1);
                strcpy(shader_transparent_generic_source, pixel_shader_source);
            }
        }
    }

    restored_shader_effects[RESTORED_SHADER_EFFECT_HUD_METERS] = rasterizer_dx9_shader_effect_find("hud_meters");

    GlobalFree(effect_collection_blob);

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
    if(index < NUM_OF_STOCK_SHADER_EFFECTS) {
        return shader_effects[index].effect;
    }
    else {
        switch(index) {
            case SHADER_EFFECT_HUD_METERS:
                return restored_shader_effects[RESTORED_SHADER_EFFECT_HUD_METERS];
            default:
                return NULL;
        }
    }
}
