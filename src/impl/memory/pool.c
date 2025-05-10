#include "pool.h"

void *memory_pool_resize_block_og(size_t new_size, StackMemoryPool *pool, void *block);

void *stack_memory_pool_resize_block(StackMemoryPool *pool, void *block, size_t new_size) {
    return memory_pool_resize_block_og(new_size, pool, block);
}
