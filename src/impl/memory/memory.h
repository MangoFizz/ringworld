#ifndef RINGWORLD__MEMORY__MEMORY_H
#define RINGWORLD__MEMORY__MEMORY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#define _Static_assert static_assert
extern "C" {
#endif

#define TEXTURE_CACHE_SIZE (size_t)(16 * 1024) // 16 KiB

#ifndef RINGWORLD_ENABLE_ENHANCEMENTS
#define MANAGED_HEAP_BASE_ADDRESS (void *)(0x40000000)
#define GAME_STATE_SIZE (size_t)(0x440000) // 4.25 MiB
#define TAG_DATA_LENGTH (size_t)(64 * 1024 * 1024) // 64 MiB 
#define TAG_DATA_LOAD_ADDRESS (void *)(MANAGED_HEAP_BASE_ADDRESS + GAME_STATE_SIZE) 
#define MANAGED_HEAP_SIZE (size_t)(GAME_STATE_SIZE + TAG_DATA_LENGTH) // 4.25 MiB + 23 MiB

#else
#define GAME_STATE_EXTRA_MEMORY (size_t)(16 * 1024 * 1024) // 16 MiB
#define GAME_STATE_SIZE (size_t)(0x440000 + GAME_STATE_EXTRA_MEMORY) // 4.25 MiB + 16 MiB
#define MANAGED_HEAP_BASE_ADDRESS (void *)(0x40000000 - GAME_STATE_EXTRA_MEMORY) // We need to work around the tag data base address
#define TAG_DATA_LENGTH (size_t)(64 * 1024 * 1024) // 64 MiB 
#define TAG_DATA_LOAD_ADDRESS (void *)(MANAGED_HEAP_BASE_ADDRESS + GAME_STATE_SIZE) 
#define MANAGED_HEAP_SIZE (size_t)(GAME_STATE_SIZE + TAG_DATA_LENGTH) // 4.25 MiB + 16 MiB + 64 MiB

#endif


// Helpers
#define PACKED_ENUM __attribute__((__packed__))
#define TEST_BIT(value, bit) (((value) & (1 << (bit))) != 0)
#define SIZEOF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

typedef uint8_t Bool;

typedef struct ManagedHeapMap {
    void *game_state_base_address;
    void *tag_data_base_address;
} ManagedHeapMap;

/**
 * Allocate buffers for holding tag data and sounds.
 */
void memory_map_initialize(void);

/**
 * Get the managed heap map containing base addresses for game state and tag data.
 * @return pointer to the managed heap map
 */
ManagedHeapMap *memory_map_get(void);

#ifdef __cplusplus
}
#endif

#endif
