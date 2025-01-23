#ifndef RINGWORLD__CRYPTO__CRC32_H
#define RINGWORLD__CRYPTO__CRC32_H

#include <stdint.h>

/**
 * Calculate the CRC32 checksum of a buffer.
 * @param checksum      pointer to the current checksum; the result will be stored here 
 * @param data          pointer to the data to checksum
 * @param data_length   length of the data to checksum
 */
void crc32_calculate(uint32_t *checksum, void *data, size_t data_length);

#endif
