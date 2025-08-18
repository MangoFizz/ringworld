#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>
#include <shlwapi.h>
#include "../impl/exception/exception.h"
#include "../impl/debug/assertion.h"
#include "../impl/rasterizer/rasterizer_dx9_shader_compiler.h"
#include "../impl/rasterizer/rasterizer_shaders_blob.h"
#include "../impl/memory/dynamic_buffer.h"
#include "../impl/memory/shader_blob.h"
#include "resources.h"
#include "file.h"

void disassemble_shader(const char* shader_file, const char *output_file);

bool unpack_effect_collection_file(const char *effect_collection_file, const char *path, bool disassemble) {
    if(!file_exists(effect_collection_file)) {
        fprintf(stderr, "File \"%s\" does not exist.\n", effect_collection_file);
        return false;
    }

    void *effect_collection_blob;
    size_t effect_collection_blob_size;
    printf("Reading effect collection \"%s\"...\n", effect_collection_file);
    ASSERT(rasterizer_shaders_blob_read_file(&effect_collection_blob, &effect_collection_blob_size, effect_collection_file));

    char current_directory[MAX_PATH];
    get_current_directory(current_directory);
    create_directory(path);
    change_directory(path);

    void *blob_pointer = effect_collection_blob;
    uint32_t shaders_count = shader_blob_read_count(&blob_pointer);
    for(size_t i = 0; i < shaders_count; i++) {
        char effect_name[256];
        shader_blob_read_string(&blob_pointer, effect_name);
        
        create_directory(effect_name);
        change_directory(effect_name);
        
        uint32_t effect_data_count = shader_blob_get_count(blob_pointer);
        if(effect_data_count < 64) {
            uint32_t pixel_shader_count = shader_blob_read_count(&blob_pointer);
            for(size_t j = 0; j < pixel_shader_count; j++) {
                char pixel_shader_name[256];
                shader_blob_read_string(&blob_pointer, pixel_shader_name);
                
                uint32_t pixel_shader_dwords = shader_blob_get_count(blob_pointer);
                uint32_t pixel_shader_bytecode[pixel_shader_dwords];
                shader_blob_read_shader_bytecode(&blob_pointer, pixel_shader_bytecode);
                
                char filename[256];
                sprintf(filename, "#%d_%s.cso", j + 1, pixel_shader_name);
                write_file(filename, pixel_shader_bytecode, pixel_shader_dwords * sizeof(uint32_t));
                
                if(disassemble) {
                    char disassembly_filename[256];
                    sprintf(disassembly_filename, "%s.%d.asm", pixel_shader_name, j + 1);
                    disassemble_shader(filename, disassembly_filename);
                }
            }
        }
        else {
            // treat as a pixel shader source
            uint32_t pixel_shader_source_length = effect_data_count;
            char pixel_shader_source[pixel_shader_source_length];
            shader_blob_read_string(&blob_pointer, pixel_shader_source);
            
            char filename[256];
            sprintf(filename, "%s.hlsl", effect_name);
            write_file(filename, pixel_shader_source, pixel_shader_source_length);
        }

        change_directory("..");
    }
    
    GlobalFree(effect_collection_blob);
    change_directory(current_directory);

    return true;
}

const char *pixel_shader_name_from_filename(const char *filename) {
    const char *underscore = strchr(filename, '_');
    if (!underscore) {
        return filename;
    }
    return underscore + 1;
}

int compare_pixel_shader_filename(const void *a, const void *b) {
    return StrCmpLogicalW(*(const wchar_t **)a, *(const wchar_t **)b);
}

void write_effect_pixel_shader_to_buffer(const wchar_t *filepath, void *userdata) {
    DynamicBuffer *buffer = userdata;
    char ps_filename[MAX_PATH];
    wcstombs(ps_filename, filepath, MAX_PATH);
    const char *extension = get_extension_from_path(ps_filename);
    const char *pixel_shader_filename = pixel_shader_name_from_filename(ps_filename);

    if(strcmp(extension, "hlsl") == 0 && count_files_in_directory(L".") == 1) {
        void *pixel_shader_source;
        size_t pixel_shader_source_size;
        read_file(ps_filename, &pixel_shader_source, &pixel_shader_source_size);

        // Remove pixel shader count from buffer
        buffer->lenght -= sizeof(uint32_t);

        // Push pixel shader source
        uint32_t length = strlen(pixel_shader_source);
        dynamic_buffer_push_data(buffer, &length, sizeof(length));
        dynamic_buffer_push_data(buffer, pixel_shader_source, length);
        
        return;
    }

    if(strcmp(extension, "cso") != 0) {
        return;
    }
    
    void *pixel_shader_bytecode;
    size_t pixel_shader_size;
    read_file(ps_filename, &pixel_shader_bytecode, &pixel_shader_size);
    
    // Push pixel shader name
    char pixel_shader_name[strlen(pixel_shader_filename)];
    get_filename_without_extension_from_path(pixel_shader_filename, pixel_shader_name);
    uint32_t pixel_shader_name_size = strlen(pixel_shader_name);
    dynamic_buffer_push_data(buffer, &pixel_shader_name_size, sizeof(pixel_shader_name_size));
    dynamic_buffer_push_data(buffer, pixel_shader_name, pixel_shader_name_size);

    // Push pixel shader bytecode
    uint32_t pixel_shader_dwords = pixel_shader_size / sizeof(uint32_t);
    dynamic_buffer_push_data(buffer, &pixel_shader_dwords, sizeof(pixel_shader_dwords));
    dynamic_buffer_push_data(buffer, pixel_shader_bytecode, pixel_shader_dwords * 4);

    GlobalFree(pixel_shader_bytecode);
}

void write_effect_to_buffer(const wchar_t *directory, void *userdata) {
    DynamicBuffer *buffer = userdata;
    char effect_name[MAX_PATH];
    wcstombs(effect_name, directory, MAX_PATH);
    
    // Push effect name
    uint32_t effect_name_size = strlen(effect_name);
    dynamic_buffer_push_data(buffer, &effect_name_size, sizeof(effect_name_size));
    dynamic_buffer_push_data(buffer, effect_name, effect_name_size);

    // Push pixel shader count
    uint32_t pixel_shader_count = count_files_in_directory(directory);
    dynamic_buffer_push_data(buffer, &pixel_shader_count, sizeof(pixel_shader_count));

    // Iterate pixel shaders
    change_directory(effect_name);
    iterate_files_by_extension(L".", write_effect_pixel_shader_to_buffer, buffer, compare_pixel_shader_filename);
    change_directory("..");
}

bool pack_effect_collection(const char *path, const char *effect_collection_file) {
    DynamicBuffer effect_collection_blob;
    dynamic_buffer_init(&effect_collection_blob);

    if(!directory_exists(path)) {
        fprintf(stderr, "Directory \"%s\" does not exist.\n", path);
        return false;
    }

    char current_directory[MAX_PATH];
    get_current_directory(current_directory);
    change_directory(path);

    // Push effects count
    uint32_t effects_count = count_directories_in_directory(L".");
    dynamic_buffer_push_data(&effect_collection_blob, &effects_count, sizeof(effects_count));

    // Iterate effects
    iterate_directories(L".", write_effect_to_buffer, &effect_collection_blob, compare_pixel_shader_filename);

    change_directory(current_directory);

    // Encrypt data
    void *encrypted_data;
    size_t encrypted_size;
    rasterizer_shaders_blob_encrypt(effect_collection_blob.data, effect_collection_blob.lenght, &encrypted_data, &encrypted_size);

    // Write to file
    write_file(effect_collection_file, encrypted_data, encrypted_size);
}

static void *get_resource(const wchar_t *resource_id, const wchar_t *resource_type) {
    HMODULE hModule = GetModuleHandle(NULL); 
    if(hModule) {
        HRSRC hRes = FindResourceW(hModule, resource_id, resource_type);
        if(hRes) {
            HGLOBAL hResData = LoadResource(hModule, hRes);
            if(hResData) {
                void *pData = LockResource(hResData);
                return pData;
            }
        }
    }
    return NULL;
}

void write_hud_meters_shader_to_effect_collection_dir(const char *path) {
    const char *hud_meters_effect_name = "hud_meters";
    const char *hud_meters_pixel_shader_name = "#1_PS_HUDMeters_ps_3_0";

    // Compile shader
    char *hud_meters_shader_source = get_resource(MAKEINTRESOURCEW(IDR_SHADER_HUD_METERS), L"HLSL");
    ASSERT(hud_meters_shader_source != NULL);
    ID3DBlob *hud_meters_shader_cso;
    ASSERT(rasterizer_dx9_shader_compiler_compile_shader_from_blob(hud_meters_shader_source, "main", "ps_3_0", NULL, &hud_meters_shader_cso));

    char current_directory[MAX_PATH];
    get_current_directory(current_directory);
    change_directory(path);
    remove_directory(hud_meters_effect_name);
    create_directory(hud_meters_effect_name);
    change_directory(hud_meters_effect_name);

    // Write pixel shader bytecode
    char filename[256];
    sprintf(filename, "%s.cso", hud_meters_pixel_shader_name);
    write_file(filename, ID3D10Blob_GetBufferPointer(hud_meters_shader_cso), ID3D10Blob_GetBufferSize(hud_meters_shader_cso));

    change_directory(current_directory);
}

void write_transparent_generic_shader_to_effect_collection_dir(const char *path) {
    const char *transparent_generic_effect_name = "transparent_generic";
    
    // Get shader source
    char *transparent_generic_shader_source = get_resource(MAKEINTRESOURCEW(IDR_SHADER_TRANSPARENT_GENERIC), L"HLSL");
    ASSERT(transparent_generic_shader_source != NULL);

    char current_directory[MAX_PATH];
    get_current_directory(current_directory);
    change_directory(path);
    remove_directory(transparent_generic_effect_name);
    create_directory(transparent_generic_effect_name);
    change_directory(transparent_generic_effect_name);

    // Write shader source
    write_file("transparent_generic.hlsl", transparent_generic_shader_source, strlen(transparent_generic_shader_source));

    change_directory(current_directory);
}

bool update_effect_collection(const char *effect_collection_file, const char *output_file) {
    char temp_directory[MAX_PATH];
    random_temp_name(temp_directory);
    if(!unpack_effect_collection_file(effect_collection_file, temp_directory, false)) {
        remove_directory(temp_directory);
        return false;
    }
    write_hud_meters_shader_to_effect_collection_dir(temp_directory);
    write_transparent_generic_shader_to_effect_collection_dir(temp_directory);
    if(!pack_effect_collection(temp_directory, output_file)) {
        remove_directory(temp_directory);
        return false;
    }
    remove_directory(temp_directory);
    return true;
}
