#include "../debug/log.h"
#include "dynamic_buffer.h"

void dynamic_buffer_init(DynamicBuffer *buffer) {
    buffer->lenght = 0;
    buffer->data = NULL;
}

bool dynamic_buffer_push_data(DynamicBuffer *buffer, const void *data, uint32_t data_size) {
    if (data == NULL || data_size == 0) {
        log_error("dynamic_buffer_push_data: invalid arguments");
        return false;
    }

    /* Use size_t for arithmetic and check for overflow */
    size_t old_len = (size_t)buffer->lenght;
    size_t add = (size_t)data_size;
    if (old_len + add < old_len) {
        log_error("dynamic_buffer_push_data: size overflow");
        return false;
    }

    size_t new_len = old_len + add;

    /* Use a temporary pointer so we don't lose the old buffer on OOM */
    unsigned char *new_data = realloc(buffer->data, new_len);
    if (!new_data) {
        /* realloc failed, keep original buffer untouched */
        log_error("dynamic_buffer_push_data: realloc failed");
        return false;
    }

    /* Copy the incoming data into the newly grown buffer */
    memcpy(new_data + old_len, data, add);
    buffer->data = new_data;
    buffer->lenght = (uint32_t)new_len;

    return true;
}

void dynamic_buffer_free(DynamicBuffer *buffer) {
    free(buffer->data);
    buffer->data = NULL;
    buffer->lenght = 0;
}
