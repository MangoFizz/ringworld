#ifndef RINGWORLD__MEMORY__TABLE_H
#define RINGWORLD__MEMORY__TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

typedef union TableResourceHandle {
    uint32_t value;
    struct {
        uint16_t index;
        uint16_t salt;
    };
} TableResourceHandle;

/** Null handle that refers to no object */
#define NULL_HANDLE ((TableResourceHandle)(0xFFFFFFFF))

/**
 * Check if the handle is null
 *
 * @return true if the handle is NULL, false if not
 */
#define HANDLE_IS_NULL(handle) ((handle).value == NULL_HANDLE.value)

/**
 * Make a handle from a salt and index
 *
 * @param salt  salt to use
 * @param index index to use
 *
 * @return handle
 */
#define MAKE_HANDLE(salt, index) (TableResourceHandle)((uint16_t)(index) | ((uint32_t)(salt) << 16))

/**
 * Generate a table struct
 *
 * @param table_struct_name name of the struct to generate
 * @param object_type       element type to use
 *
 * @return table struct definition
 */
#define MAKE_TABLE_STRUCT(table_struct_name, object_type) typedef struct table_struct_name { \
    /** Name - unused */ \
    char name[32]; \
    \
    /** Capacity of the table */ \
    uint16_t max_elements; \
    \
    /** Size of each element in the table */ \
    uint16_t element_size; \
    \
    /** Unknown */ \
    uint8_t unknown_24; \
    \
    /** Unknown? */ \
    uint8_t unknown1[3]; \
    \
    /** d@t@ fourcc - unused */ \
    uint32_t data_fourcc; \
    \
    /** Index of the next not-active element of the table */ \
    int16_t next_free_element_index; \
    \
    /** Current size of the table, even including elements not active */ \
    uint16_t current_size; \
    \
    /** Number of elements that are active */ \
    uint16_t count; \
    \
    /** Salt of the next element to be occupied (probably) */ \
    uint16_t next_id; \
    \
    /** Pointer to first element in the table */ \
    object_type *first_element; \
} table_struct_name; \
_Static_assert(sizeof(table_struct_name) == 0x38);

/**
 * Generic table struct that can be used to refer to most tables when knowing the element type isn't necessary
 */
MAKE_TABLE_STRUCT(GenericTable, void);

/**
 * Iterator for iterating through tables.
 *
 * Should be initialized with init_table_iterator()
 */
typedef struct TableIterator {
    /** Reference to the table being iterated */
    GenericTable *table;

    /** Current index the iterator is on */
    uint16_t index;

    /** Handle of the object found - not valid if init_iterator returned NULL */
    TableResourceHandle handle;

    /** Unused */
    uint32_t salt;
} TableIterator;
_Static_assert(sizeof(TableIterator) == 0x10);

/**
 * Allocate a table of objects.
 * @param name          name of table
 * @param maximum_count maximum element count
 * @param element_size  size of each element
 * @return table
 */
void *table_new(const char *name, uint16_t maximum_count, uint16_t element_size);

/**
 * Get the element of a table by its handle.
 * If the input handle has a salt of 0x0000, then the salt will not be checked.
 * @param table     table pointer
 * @param handle    Handle of the element
 * @return pointer if the handle corresponds to a valid table index, NULL if not
 */
void *table_get_element(GenericTable *table, TableResourceHandle handle);

/**
 * Initialize a table element.
 * @param table                table pointer
 * @param new_element_location pointer to the new element
 */
void table_init_element(GenericTable *table, void *new_element_location);

/**
 * Add a new element to a table.
 * @param table table pointer
 * @return pointer to the new element, or NULL if the table is full
 */
void *table_add_element(GenericTable *table);

/**
 * Clear the contents of a table.
 * @param table table pointer
 */
void table_clear(GenericTable *table);

/**
 * Prepare a TableIterator.
 */
void table_init_iterator(TableIterator *iterator, const void *table);

/**
 * Iterate through a table to get the next valid instance.
 * @param iterator to use (must be initialized with init_iterator)
 * @return next valid instance, or NULL if none
 */
void *table_iterate(TableIterator *iterator);

/**
 * Iterator callback for the iterate_table_simple function.
 * @param iterator  current iterator value
 * @param item      pointer to item in table
 * @param user_data user data passed in iterate function
 * @return true if the iterator should continue, false if not
 */
typedef bool (*table_iterator_callback)(const TableIterator *iterator, void *item, void *user_data);

/**
 * Iterate a table from start to the end.
 * @param table     table to iterate
 * @param callback  callback to call
 * @param user_data user data to pass into the callback
 */
void table_iterate_simple(void *table, table_iterator_callback callback, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
