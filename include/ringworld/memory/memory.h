#ifndef RINGWORLD__MEMORY__MEMORY_H
#define RINGWORLD__MEMORY__MEMORY_H

#define PACKED_ENUM __attribute__((__packed__))

#ifdef __cplusplus
#define _Static_assert static_assert
extern "C" {
#endif

#define BASE_HEAP_AMOUNT (size_t)(4352 * 1024) // 4.25 MiB

#ifndef RINGWORLD_ENABLE_ENHANCEMENTS
#define TAG_DATA_LENGTH (size_t)(23 * 1024 * 1024) // 23 MiB 
#else
#define TAG_DATA_LENGTH (size_t)(64 * 1024 * 1024) // 64 MiB 
#endif

#define MANAGED_HEAP_ADDRESS_START (void *)(0x40000000)
#define TAG_DATA_LOAD_ADDRESS (void *)(MANAGED_HEAP_ADDRESS_START + BASE_HEAP_AMOUNT) 

#define TEXTURE_CACHE_UNK1_SIZE (size_t)(16 * 1024) // 16 KiB

#include <stdint.h>

typedef uint8_t Bool;

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
