#ifndef RINGWORLD__GAME__GAME_STATE_H
#define RINGWORLD__GAME__GAME_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Allocate memory in the heap.
 *
 * Take caution that there is not much space in this heap (only 4.25 MiB). If there is not enough space, NULL will be
 * returned, and an error will be logged in stderr.
 *
 * @param size number of bytes to allocate
 *
 * @return     pointer to new data or NULL if there is not enough space
 */
void *game_state_allocate_heap(size_t size);

/**
 * Allocate a table of objects.
 * @param name          name of table
 * @param maximum_count maximum element count
 * @param element_size  size of each element
 * @return table
 */
void *game_state_table_new(const char *name, uint16_t maximum_count, uint16_t element_size);

#ifdef __cplusplus
}
#endif

#endif

