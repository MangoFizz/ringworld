#ifndef RINGWORLD__MEMORY__CIRCULAR_QUEUE_H
#define RINGWORLD__MEMORY__CIRCULAR_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct CircularQueue {
    char *name;
    uint32_t signature;
    int32_t read_offset;
    int32_t write_offset;
    int32_t buffer_size;
    void *buffer;
} CircularQueue;

#ifdef __cplusplus
}
#endif

#endif
