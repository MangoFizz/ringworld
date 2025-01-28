#include "../memory/table.h"
#include "../crypto/crc32.h"
#include "../exception/exception.h"
#include "game_state.h"

extern void **game_state_base_address;
extern uint32_t *game_state_size;
extern uint32_t *game_state_checksum;

void *game_state_allocate_heap(size_t size) {
    size_t remaining_bytes = BASE_HEAP_AMOUNT - *game_state_size;
    if(remaining_bytes < size) {
        crashf("allocate_heap(): Can't allocate 0x%08zX bytes (only 0x%08zX bytes remaining)", size, remaining_bytes);
    }

    void *allocated = *game_state_base_address + *game_state_size;
    *game_state_size += size;

    crc32_calculate(game_state_checksum, &size, 4);

    return allocated;
}

void *game_state_table_new(const char *name, uint16_t maximum_count, uint16_t element_size) {
    size_t allocation_amount = CALCULATE_ALLOCATION_SIZE(maximum_count, element_size);
    GenericTable *table = game_state_allocate_heap(allocation_amount);
    table_initialize(table, name, maximum_count, element_size);
    return table;
}
