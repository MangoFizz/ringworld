#ifndef RINGWORLD__RASTERIZER__RASTERIZER_SHADERS_BLOB_H
#define RINGWORLD__RASTERIZER__RASTERIZER_SHADERS_BLOB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * Decrypts a shaders blob.
 * @param data          Pointer to the shaders blob.
 * @param data_size     Size of the shaders blob.
 * @return              true if successful, false if not.
 */
bool rasterizer_shaders_blob_decrypt(void *data, size_t data_size);

/**
 * Encrypts a shaders blob.
 * @param data                  Pointer to the shaders blob.
 * @param data_size             Size of the shaders blob.
 * @param encrypted_data        Pointer to a buffer that will be allocated and filled with the encrypted shader file data.
 * @param encrypted_data_size   Pointer to a size_t that will be filled with the size of the encrypted shader file data.
 */
void rasterizer_shaders_blob_encrypt(void *data, size_t data_size, void **encrypted_data, size_t *encrypted_data_size);

/**
 * Reads a shaders blob file from disk and decrypts it.
 * @param filename      The filename of the shader file.
 * @param buffer        A pointer to a buffer that will be allocated and filled with the shader file data.
 * @param bytes_read    A pointer to a size_t that will be filled with the size of the shader file data.
 * @return              true if successful, false if not.
 */
bool rasterizer_shaders_blob_read_file(void **buffer, size_t *bytes_read, const char *filename);

#ifdef __cplusplus
}
#endif

#endif
