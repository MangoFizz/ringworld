#include <stdio.h>
#include <windows.h>
#include <d3d9.h>

#include "../crypto/md5.h"
#include "../crypto/xtea.h"
#include "../exception/exception.h"
#include "rasterizer_shaders_blob.h"

#ifndef RASTERIZER_SHADER_BLOB_XTEA_KEY
#define RASTERIZER_SHADER_BLOB_XTEA_KEY 0x3FFFFFDD, 0x00007FC3, 0x000000E5, 0x003FFFEF
#endif

bool rasterizer_shaders_blob_decrypt(void *data, size_t data_size) {
    ASSERT(data != NULL);
    
    if(data_size < 34) {
        return false;
    }

    uint32_t key[4] = { RASTERIZER_SHADER_BLOB_XTEA_KEY };
    xtea_decrypt(data_size, data, key);

    char hash_buffer[32];
    generate_md5_hash((char *)data, data_size - 33, hash_buffer);

    char *data_hash = (char *)data + data_size - 33;
    char *expected_hash = hash_buffer;
    for(size_t i = 0; i < 32; i++) {
        if(data_hash[i] != expected_hash[i]) {
            return false;
        }
    }
    return true;
}

void rasterizer_shaders_blob_encrypt(void *data, size_t data_size, void **encrypted_data, size_t *encrypted_data_size) {
    ASSERT(data != NULL);

    char hash_buffer[32];
    generate_md5_hash((char *)data, data_size, hash_buffer);

    *encrypted_data = GlobalAlloc(GPTR, data_size + 33);
    *encrypted_data_size = data_size + 33;

    memcpy(*encrypted_data, data, data_size);
    memcpy((char *)*encrypted_data + data_size, hash_buffer, 32);

    uint32_t key[4] = { RASTERIZER_SHADER_BLOB_XTEA_KEY };
    xtea_encrypt(*encrypted_data_size, *encrypted_data, key);
}

bool rasterizer_shaders_blob_read_file(void **buffer, size_t *data_size, const char *filename) {
    *buffer = NULL;
    *data_size = 0;

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
    size_t bytes_read;
    if(buffer_handle != NULL) {
        bool success = ReadFile(file, buffer_handle, file_size, (LPDWORD)&bytes_read, NULL);
        if(success) {
            CloseHandle(file);
            bool decrypt_success = rasterizer_shaders_blob_decrypt(buffer_handle, bytes_read);
            if(decrypt_success) {
                *buffer = buffer_handle;
                *data_size = bytes_read - 33;
                return true;
            }
        }
        GlobalFree(buffer_handle);
    }

    CloseHandle(file);
    return false;
}
