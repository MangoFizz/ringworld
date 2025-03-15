#include "pool.h"

void *memory_pool_resize_block_og(size_t new_size, MemoryPool *pool, void *block);

void *memory_pool_resize_block(MemoryPool *pool, void *block, size_t new_size) {
    return memory_pool_resize_block_og(new_size, pool, block);
}
