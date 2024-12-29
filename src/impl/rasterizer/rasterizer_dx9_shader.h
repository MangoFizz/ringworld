#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_SHADER_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_SHADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

/**
 * Decrypts a shader file.
 * @param data          A pointer to the shader file data.
 * @param data_size     The size of the shader file data.
 * @return              true if successful, false if not.
 */
bool rasterizer_dx9_shader_decrypt_binary_file(void *data, size_t data_size);

/**
 * Reads a shader file from disk and decrypts it.
 * @param filename      The filename of the shader file.
 * @param buffer        A pointer to a buffer that will be allocated and filled with the shader file data.
 * @param bytes_read    A pointer to a size_t that will be filled with the size of the shader file data.
 * @return              true if successful, false if not.
 */
bool rasterizer_dx9_shader_read_binary_file(char **buffer, size_t *bytes_read, const char *filename);

#ifdef __cplusplus
}
#endif

#endif
