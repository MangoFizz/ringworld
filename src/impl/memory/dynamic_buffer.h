#ifndef RINGWORLD__MEMORY__DYNAMIC_BUFFER_H
#define RINGWORLD__MEMORY__DYNAMIC_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct DynamicBuffer {
    uint32_t lenght;
    void *data;
} DynamicBuffer;

void dynamic_buffer_init(DynamicBuffer *buffer);
bool dynamic_buffer_push_data(DynamicBuffer *buffer, const void *data, uint32_t data_size);
void dynamic_buffer_free(DynamicBuffer *buffer);

#ifdef __cplusplus
}
#endif

#endif
