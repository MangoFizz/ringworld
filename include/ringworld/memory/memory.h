#ifndef RINGWORLD__MEMORY__MEMORY_H
#define RINGWORLD__MEMORY__MEMORY_H

#ifdef __cplusplus
#define _Static_assert static_assert
extern "C" {
#endif

#include <stdint.h>

/**
 * Allocate buffers for holding tag data and sounds.
 */
void memory_map_initialize(void);

/**
 * Allocate memory in the heap.
 *
 * Take caution that there is not much space in this heap (only 4.25 MiB). If there is not enough space, NULL will be
 * returned, and an error will be logged in stderr.
 *
 * @param size number of bytes to allocate
 *
 * @return     pointer to new data or NULL if there is not enough space
 */
void *allocate_heap(size_t size);

/**
 * Calculate the CRC checksum of a buffer.
 * 
 * @param checksum      pointer to the current CRC checksum; the result will be stored here 
 * @param data          pointer to the data to checksum
 * @param data_length   length of the data to checksum
 */
void malloc_crc_checksum_buffer(uint32_t *checksum, void *data, size_t data_length);

#ifdef __cplusplus
}
#endif

#endif
