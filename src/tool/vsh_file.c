#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "../impl/rasterizer/rasterizer_shaders_blob.h"

typedef struct VariableLenghtChunk {
    uint32_t lenght;
    unsigned char data[];
} VariableLenghtChunk;

const char *vertex_shaders_files[] = {
    "convolution",
    "debug",
    "decal",
    "detail_object_type0",
    "detail_object_type1",
    "effect",
    "effect_multitexture",
    "effect_multitexture_screenspace",
    "effect_zsprite",
    "environment_diffuse_light",
    "environment_diffuse_light_ff",
    "environment_fog",
    "environment_fog_screen",
    "environment_lightmap",
    "environment_reflection_bumped",
    "environment_reflection_flat",
    "environment_reflection_lightmap_mask",
    "environment_reflection_mirror",
    "environment_reflection_radiosity",
    "environment_shadow",
    "environment_specular_light",
    "environment_specular_spot_light",
    "environment_specular_lightmap",
    "environment_texture",
    "lens_flare",
    "model_fogged",
    "model",
    "model_ff",
    "model_fast",
    "model_scenery",
    "model_active_camouflage",
    "model_active_camouflage_ff",
    "model_fog_screen",
    "model_shadow",
    "model_zbuffer",
    "screen",
    "screen2",
    "transparent_generic",
    "transparent_generic_lit_m",
    "transparent_generic_m",
    "transparent_generic_object_centered",
    "transparent_generic_object_centered_m",
    "transparent_generic_reflection",
    "transparent_generic_reflection_m",
    "transparent_generic_screenspace",
    "transparent_generic_screenspace_m",
    "transparent_generic_viewer_centered",
    "transparent_generic_viewer_centered_m",
    "transparent_glass_diffuse_light",
    "transparent_glass_diffuse_light_m",
    "transparent_glass_reflection_bumped",
    "transparent_glass_reflection_bumped_m",
    "transparent_glass_reflection_flat",
    "transparent_glass_reflection_flat_m",
    "transparent_glass_reflection_mirror",
    "transparent_glass_tint",
    "transparent_glass_tint_m",
    "transparent_meter",
    "transparent_meter_m",
    "transparent_plasma_m",
    "transparent_water_opacity",
    "transparent_water_opacity_m",
    "transparent_water_reflection",
    "transparent_water_reflection_m",
#ifdef RINGWORLD_TOOL_ENABLE_DEBUG_SHADERS
    "model_fast_prepass",
    "model_scenery_prepass",
    "environment_prepass"
#endif
};

#define VERTEX_SHADER_FILES_COUNT (sizeof(vertex_shaders_files) / sizeof(vertex_shaders_files[0]))

bool unpack_vsh_file(const char *vsh_file, const char *path) {
    void *data;
    size_t data_size;
    bool success = rasterizer_shaders_blob_read_file(&data, &data_size, vsh_file);
    if(!success) {
        return NULL;
    }

    DWORD attributes = GetFileAttributesA(path);
    if(attributes == INVALID_FILE_ATTRIBUTES) {
        CreateDirectoryA(path, NULL);
    }

    void *ptr = data;
    for(size_t i = 0; i < VERTEX_SHADER_FILES_COUNT; i++) {
        VariableLenghtChunk *chunk = ptr;
        size_t size = sizeof(VariableLenghtChunk) + chunk->lenght;

        char filepath[MAX_PATH];
        snprintf(filepath, MAX_PATH, "%s/%s.cso", path, vertex_shaders_files[i]);
        HANDLE file = CreateFileA(filepath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(file == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Could not open file \"%s\" for writing.\n", filepath);
            return false;
        }

        DWORD bytes_written;
        WriteFile(file, chunk->data, chunk->lenght, &bytes_written, NULL);
        CloseHandle(file);

        ptr += size;
    }

    return true;
}

bool pack_vsh_file(const char *path, const char *vsh_file) {
    VariableLenghtChunk *shaders[VERTEX_SHADER_FILES_COUNT];

    for(size_t i = 0; i < VERTEX_SHADER_FILES_COUNT; i++) {
        char file_path[MAX_PATH];
        snprintf(file_path, MAX_PATH, "%s/%s.cso", path, vertex_shaders_files[i]);

        HANDLE shader_file = CreateFileA(file_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(shader_file == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Could not open file \"%s\" for reading.\n", file_path);
            return false;
        }

        DWORD file_size = GetFileSize(shader_file, NULL);
        if(file_size == INVALID_FILE_SIZE) {
            fprintf(stderr, "Could not get file size for \"%s\".\n", file_path);
            return false;
        }

        void *buffer = GlobalAlloc(GPTR, file_size);
        if(buffer == NULL) {
            fprintf(stderr, "Could not allocate memory for file buffer.\n");
            return false;
        }

        DWORD bytes_read;
        ReadFile(shader_file, buffer, file_size, &bytes_read, NULL);
        CloseHandle(shader_file);

        shaders[i] = GlobalAlloc(GPTR, sizeof(VariableLenghtChunk) + bytes_read);
        shaders[i]->lenght = bytes_read;
        memcpy(shaders[i]->data, buffer, bytes_read);

        GlobalFree(buffer);
    }

    size_t total_size = 0;
    for(size_t i = 0; i < VERTEX_SHADER_FILES_COUNT; i++) {
        total_size += sizeof(VariableLenghtChunk) + shaders[i]->lenght;
    }

    void *data = GlobalAlloc(GPTR, total_size);
    void *ptr = data;
    for(size_t i = 0; i < VERTEX_SHADER_FILES_COUNT; i++) {
        VariableLenghtChunk *chunk = ptr;
        size_t size = sizeof(VariableLenghtChunk) + shaders[i]->lenght;
        memcpy(chunk, shaders[i], size);
        ptr += size;
    }

    void *encrypted_data = NULL;
    size_t encrypted_data_size = 0;
    rasterizer_shaders_blob_encrypt(data, total_size, &encrypted_data, &encrypted_data_size);

    HANDLE file = CreateFileA(vsh_file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Could not open file \"%s\" for writing.\n", vsh_file);
        return false;
    }

    DWORD bytes_written;
    WriteFile(file, encrypted_data, encrypted_data_size, &bytes_written, NULL);
    CloseHandle(file);
    
    return true;
}
