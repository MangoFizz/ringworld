#ifndef RINGWORLD__MEMORY__DYNAMIC_ARRAY_H
#define RINGWORLD__MEMORY__DYNAMIC_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct DynamicArray {
    uint32_t lenght;
    uint32_t capacity;
    uint32_t element_size;
    void *data;
} DynamicArray;

void dynamic_array_init(DynamicArray *buffer, uint32_t element_size);
void *dynamic_array_push(DynamicArray *buffer, const void *data);
void dynamic_array_clear(DynamicArray *buffer);
void dynamic_array_resize(DynamicArray *buffer, uint32_t new_capacity);
void dynamic_array_free(DynamicArray *buffer);
void *dynamic_array_get(DynamicArray *buffer, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif
