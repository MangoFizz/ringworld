#include <stdio.h>

#include "rasterizer_dx9_shader_compiler.h"

bool rasterizer_dx9_shader_compiler_compile_shader_from_blob(const char *source, const char *entry, const char *profile, D3D_SHADER_MACRO *defines, ID3DBlob **compiled_shader) {
    ID3DBlob *error_messages = NULL;
    DWORD flags = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY | D3DCOMPILE_OPTIMIZATION_LEVEL3;
    HRESULT result = D3DCompile(source, strlen(source), NULL, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, profile, flags, 0, compiled_shader, &error_messages);
    if(FAILED(result)) {
        if(error_messages != NULL) {
            fprintf(stderr, "Error compiling pixel shader: %s\n", (char *)ID3D10Blob_GetBufferPointer(error_messages));
            ID3D10Blob_Release(error_messages);
        }
        else {
            fprintf(stderr, "Error compiling pixel shader: unknown error\n");
        }
        return false;
    }
    return true;
}

bool rasterizer_dx9_shader_compiler_compile_shader_from_file(const char *filename, const char *entry, const char *profile, D3D_SHADER_MACRO *defines, ID3DBlob **compiled_shader) {
    HANDLE file = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_SUPPORTS_EXTENDED_ATTRIBUTES, NULL);
    if(file == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    size_t file_size = GetFileSize(file, NULL);
    if(file_size == INVALID_FILE_SIZE) {
        CloseHandle(file);
        return false;
    }

    HGLOBAL buffer_handle = GlobalAlloc(GMEM_FIXED, file_size);
    int bytes_read = 0;
    if(buffer_handle != NULL) {
        bool success = ReadFile(file, buffer_handle, file_size, (LPDWORD)&bytes_read, NULL);
        if(success) {
            CloseHandle(file);
            char buffer[file_size + 1];
            memcpy(buffer, buffer_handle, file_size);
            buffer[file_size] = '\0';
            GlobalFree(buffer_handle);
            return rasterizer_dx9_shader_compiler_compile_shader_from_blob(buffer, entry, profile, defines, compiled_shader);
        }
        GlobalFree(buffer_handle);
    }
    CloseHandle(file);
    return false;
}
