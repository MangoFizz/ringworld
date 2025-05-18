#include "dynamic_array.h"

void dynamic_array_init(DynamicArray *buffer, uint32_t element_size) {
    buffer->lenght = 0;
    buffer->capacity = 0;
    buffer->element_size = element_size;
    buffer->data = NULL;
}

void *dynamic_array_push(DynamicArray *buffer, const void *data) {
    if(buffer->lenght == buffer->capacity) {
        buffer->capacity = (buffer->capacity == 0) ? 1 : buffer->capacity * 2;
        buffer->data = realloc(buffer->data, buffer->capacity * buffer->element_size);
    }
    void *new_element = (char *)buffer->data + (buffer->lenght * buffer->element_size);
    if(data) {
        memcpy(new_element, data, buffer->element_size);
    }
    else {
        memset(new_element, 0, buffer->element_size);
    }
    buffer->lenght++;
    return new_element;
}

void dynamic_array_clear(DynamicArray *buffer) {
    buffer->lenght = 0;
}

void dynamic_array_resize(DynamicArray *buffer, uint32_t new_capacity) {
    buffer->capacity = new_capacity;
    buffer->data = realloc(buffer->data, buffer->capacity * buffer->element_size);
}

void dynamic_array_free(DynamicArray *buffer) {
    if(buffer->data) {
        free(buffer->data);
        buffer->data = NULL;
    }
    buffer->lenght = 0;
    buffer->capacity = 0;
}

void *dynamic_array_get(DynamicArray *buffer, uint32_t index) {
    if(index < buffer->lenght) {
        return (char *)buffer->data + (index * buffer->element_size);
    }
    return NULL;
}
