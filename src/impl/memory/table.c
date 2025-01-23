#include <windows.h>
#include <string.h>

#include "../exception/exception.h"
#include "memory.h"
#include "table.h"

#define ITERATOR_FOURCC 0x69746572 // 'iter'
#define INIT_TABLE_NEXT_ID(table) table->next_id = *(uint16_t *)(table->name) | 0x8000

void table_initialize(GenericTable *table, const char *name, uint16_t maximum_count, uint16_t element_size) {
    memset(table, 0, CALCULATE_ALLOCATION_SIZE(maximum_count, element_size));
    strncpy(table->name, name, sizeof(table->name) - 1);
    table->max_elements = maximum_count;
    table->element_size = element_size;
    table->data_fourcc = 0x64407440;
    table->first_element = (void *)(table) + sizeof(*table);
    INIT_TABLE_NEXT_ID(table);
}

void *table_new(const char *name, uint16_t maximum_count, uint16_t element_size) {
    size_t allocation_amount = CALCULATE_ALLOCATION_SIZE(maximum_count, element_size);
    GenericTable *table = GlobalAlloc(GPTR, allocation_amount);
    table_initialize(table, name, maximum_count, element_size);
    return table;
}

void *table_get_element(GenericTable *table, TableResourceHandle handle) {
    if(HANDLE_IS_NULL(handle)) {
        return NULL;
    }

    size_t index = handle.index;
    size_t max_elements = table->max_elements;

    // Out of bounds? (If so then something really needs fixed!)
    if(index >= max_elements) {
        CRASHF_DEBUG("Out-of-bounds access of element %zu / %zu for table 0x%08X (%s) with ID 0x%08X", index, max_elements, (uintptr_t)(table), table->name, handle.value);
        return NULL;
    }

    // Match the salt
    uint16_t salt = handle.salt;
    void *element = table->first_element + index * table->element_size;
    uint16_t element_salt = *(uint16_t *)(element);
    if(element_salt == 0 || (salt != 0 && element_salt != salt)) {
        return NULL;
    }

    // Done!
    return element;
}

void table_init_element(GenericTable *table, void *new_element_location) {
    memset(new_element_location, 0, table->element_size);
    *(uint16_t *)(new_element_location) = (table->next_id)++;
    if(table->next_id == 0) {
        table->next_id = 0x8000;
    }
}

void *table_add_element(GenericTable *table) {
    if(table->next_free_element_index == -1) {
        return NULL;
    }

    void *element = table->first_element + table->next_free_element_index * table->element_size;
    table_init_element(table, element);
    table->count++;

    if(table->current_size == table->next_free_element_index) {
        table->current_size++;
        if(table->current_size < table->max_elements) {
            table->next_free_element_index++;
        }
        else {
            table->next_free_element_index = -1;
        }
    }
    else {
        if(table->count < table->max_elements) {
            for(size_t i = table->next_free_element_index; i < table->max_elements; i++) {
                if(*(uint16_t *)(table->first_element + i * table->element_size) == 0) {
                    table->next_free_element_index = i;
                    break;
                }
            }
        } 
        else {
            table->next_free_element_index = -1;
        }
    }
    
    return element;
}

void table_clear(GenericTable *table) {
    size_t count = table->max_elements;

    for(size_t i = 0; i < count; i++) {
        *(uint16_t *)(table->first_element + i * table->element_size) = 0;
    }

    table->next_free_element_index = 0;
    table->current_size = 0;
    table->count = 0;
    INIT_TABLE_NEXT_ID(table);
}

void table_init_iterator(TableIterator *iterator, const void *table) {
    iterator->table = (GenericTable *)(table);
    iterator->handle = NULL_HANDLE;
    iterator->index = 0;
    iterator->salt = (uint32_t)(table) ^ ITERATOR_FOURCC;
}

void *table_iterate(TableIterator *iterator) {
    const GenericTable *table = iterator->table;
    const size_t current_size = table->current_size;
    const size_t element_size = table->element_size;
    void *element = (void *)(table->first_element + (size_t)(iterator->index) * element_size);

    while(iterator->index < current_size) {
        iterator->index++;

        // First two bytes of every element is a salt value, and if it's non-zero, then it's considered occupied.
        uint16_t salt = *(uint16_t *)(element);
        if(salt != 0) {
            iterator->handle = MAKE_HANDLE(salt, iterator->index - 1);
            return element;
        }

        element += element_size;
    }

    return NULL;
}

void table_iterate_simple(void *table, table_iterator_callback callback, void *user_data) {
    TableIterator iterator;
    table_init_iterator(&iterator, table);

    for(void *value = table_iterate(&iterator); value != NULL; value = table_iterate(&iterator)) {
        if(!callback(&iterator, value, user_data)) {
            return;
        }
    }
}
