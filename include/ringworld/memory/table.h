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
#define ID_IS_NULL(handle) (handle.value == NULL_HANDLE.value)

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
    /** Unknown what this does */ \
    uint16_t unknown_2c; \
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

    uint8_t padding[2];

    /** Handle of the object found - not valid if init_iterator returned NULL */
    TableResourceHandle handle;

    /** Unused */
    uint32_t salt;
} TableIterator;

/**
 * Allocate a table of objects.
 *
 * @param name          name of table
 * @param maximum_count maximum element count
 * @param element_size  size of each element
 *
 * @return table
 */
void *create_table(const char *name, uint16_t maximum_count, uint16_t element_size);

#ifdef __cplusplus
}
#endif

#endif
