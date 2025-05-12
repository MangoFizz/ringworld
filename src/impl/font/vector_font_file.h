#ifndef RINGWORLD__FONT__VECTOR_FONT_FILE_H
#define RINGWORLD__FONT__VECTOR_FONT_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * Retrieves the name of a font from a TrueType or OpenType font file in memory.
 * @param font_data Pointer to the font data in memory.
 * @param font_size Size of the font data in bytes.
 * @param out_name Pointer to a buffer where the font name will be stored.
 * @param out_name_len Length of the output buffer.
 * @return true if the font name was successfully retrieved, false otherwise.
 */
bool font_file_get_name_from_memory(const uint8_t *font_data, size_t font_size, wchar_t *out_name, size_t out_name_len);

#ifdef __cplusplus
}
#endif

#endif
