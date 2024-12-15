#include <windows.h>
#include <string.h>

#include <ringworld/memory/memory.h>
#include <ringworld/memory/table.h>

#include "../exception/exception.h"

#define ITERATOR_FOURCC 0x69746572 // 'iter'

#define CALCULATE_ALLOCATION_SIZE(maximum_count, element_size) (sizeof(GenericTable) + (size_t)(maximum_count) * (size_t)(element_size))
#define INIT_TABLE_NEXT_ID(table) table->next_id = *(uint16_t *)(table->name) | 0x8000

static void init_table(GenericTable *table, const char *name, uint16_t maximum_count, uint16_t element_size) {
    memset(table, 0, CALCULATE_ALLOCATION_SIZE(maximum_count, element_size));
    strncpy(table->name, name, sizeof(table->name) - 1);
    table->max_elements = maximum_count;
    table->element_size = element_size;
    table->data_fourcc = 0x64407440;
    table->first_element = (void *)(table) + sizeof(*table);
    INIT_TABLE_NEXT_ID(table);
}

extern uint32_t *game_state_checksum;

void *create_table(const char *name, uint16_t maximum_count, uint16_t element_size) {
    size_t allocation_amount = CALCULATE_ALLOCATION_SIZE(maximum_count, element_size);
    GenericTable *table = (GenericTable *)(allocate_heap(allocation_amount));
    init_table(table, name, maximum_count, element_size);
    malloc_crc_checksum_buffer(game_state_checksum, &maximum_count, 4);
    return table;
}
