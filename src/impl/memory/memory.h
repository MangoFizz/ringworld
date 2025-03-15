#ifndef RINGWORLD__MEMORY__MEMORY_H
#define RINGWORLD__MEMORY__MEMORY_H

#include <stdint.h>

#ifdef __cplusplus
#define _Static_assert static_assert
extern "C" {
#endif

#define BASE_HEAP_AMOUNT (size_t)(4352 * 1024) // 4.25 MiB
#define MANAGED_HEAP_ADDRESS_START (void *)(0x40000000)
#define TAG_DATA_LOAD_ADDRESS (void *)(MANAGED_HEAP_ADDRESS_START + BASE_HEAP_AMOUNT) 
#define TEXTURE_CACHE_SIZE (size_t)(16 * 1024) // 16 KiB

#ifndef RINGWORLD_ENABLE_ENHANCEMENTS
#define TAG_DATA_LENGTH (size_t)(23 * 1024 * 1024) // 23 MiB 
#else
#define TAG_DATA_LENGTH (size_t)(64 * 1024 * 1024) // 64 MiB 
#endif


// Helpers
#define PACKED_ENUM __attribute__((__packed__))
#define TEST_BIT(value, bit) (((value) & (1 << (bit))) != 0)
#define SIZEOF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

typedef uint8_t Bool;

/**
 * Allocate buffers for holding tag data and sounds.
 */
void memory_map_initialize(void);

#ifdef __cplusplus
}
#endif

#endif
