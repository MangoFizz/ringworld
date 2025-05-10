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
    unsigned int signature;
    char name[32];
    void *base_address;
    int size;
    int free_size;
    MemoryPoolBlock *first_block;
    MemoryPoolBlock *last_block;
} MemoryPool;

typedef struct StackMemoryPool {
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
} StackMemoryPool;
_Static_assert(sizeof(StackMemoryPool) == 0x34);

/**
 * Create a new memory pool block.
 * @param pool pointer to memory pool
 * @param size size of the block
 * @return pointer to the created block
 */
void *stack_memory_pool_new_block(StackMemoryPool *pool, size_t size);

/**
 * Resize a memory pool block.
 * @param pool pointer to memory pool
 * @param block pointer to the block to resize; NULL to allocate a new block
 * @param new_size new size of the block
 * @return pointer to the resized block
 */
void *stack_memory_pool_resize_block(StackMemoryPool *pool, void *block, size_t new_size);

#ifdef __cplusplus
}
#endif

#endif

