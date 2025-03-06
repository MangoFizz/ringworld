#ifndef RINGWORLD__MEMORY__DYNAMIC_BUFFER_H
#define RINGWORLD__MEMORY__DYNAMIC_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct DynamicBuffer {
    uint32_t lenght;
    void *data;
} DynamicBuffer;

inline void dynamic_buffer_init(DynamicBuffer *buffer) {
    buffer->lenght = 0;
    buffer->data = NULL;
}

inline void dynamic_buffer_push_data(DynamicBuffer *buffer, const void *data, uint32_t data_size) {
    buffer->data = realloc(buffer->data, buffer->lenght + data_size);
    memcpy(buffer->data + buffer->lenght, data, data_size);
    buffer->lenght += data_size;
}

inline void dynamic_buffer_free(DynamicBuffer *buffer) {
    free(buffer->data);
    buffer->data = NULL;
    buffer->lenght = 0;
}

#ifdef __cplusplus
}
#endif

#endif
