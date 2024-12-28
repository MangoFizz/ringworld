#ifndef RINGWORLD__MEMORY__POOL_H
#define RINGWORLD__MEMORY__POOL_H

#ifdef __cplusplus
#define _Static_assert static_assert
extern "C" {
#endif

#include <stdint.h>

#include "memory.h"

typedef struct MemoryPoolBlock {
    uint32_t size;
    uint32_t block_index;
    struct MemoryPoolResourceHeader *previous;
    struct MemoryPoolResourceHeader *next;
} MemoryPoolBlock;
_Static_assert(sizeof(MemoryPoolBlock) == 0x10);

typedef struct MemoryPool {
    char *name;
    void *allocated_memory;
    size_t allocated_size;
    size_t blocks_number;
    uint32_t next_block_index;
    uint32_t bytes_allocated;
    uint32_t peak_bytes_allocated;
    uint32_t blocks_used;
    uint32_t peak_blocks_used;
    uint32_t largest_block_allocated;
    Bool locked;
    MemoryPoolBlock *first_block;
    MemoryPoolBlock *last_block;
    MemoryPoolBlock *blocks[];
} MemoryPool;
_Static_assert(sizeof(MemoryPool) == 0x34);

/**
 * Create a new memory pool block.
 * @param pool pointer to memory pool
 * @param size size of the block
 * @return pointer to the created block
 */
void *memory_pool_new_block(MemoryPool *pool, size_t size);

#ifdef __cplusplus
}
#endif

#endif

