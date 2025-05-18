#include "dynamic_buffer.h"

void dynamic_buffer_init(DynamicBuffer *buffer) {
    buffer->lenght = 0;
    buffer->data = NULL;
}

void dynamic_buffer_push_data(DynamicBuffer *buffer, const void *data, uint32_t data_size) {
    buffer->data = realloc(buffer->data, buffer->lenght + data_size);
    memcpy(buffer->data + buffer->lenght, data, data_size);
    buffer->lenght += data_size;
}

void dynamic_buffer_free(DynamicBuffer *buffer) {
    free(buffer->data);
    buffer->data = NULL;
    buffer->lenght = 0;
}
