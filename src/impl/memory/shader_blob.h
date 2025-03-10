#ifndef RINGWORLD__MEMORY__SHADER_BLOB_H
#define RINGWORLD__MEMORY__SHADER_BLOB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct ShaderBlobChunk {
    uint32_t lenght;
    unsigned char data[];
} ShaderBlobChunk;

static inline uint32_t shader_blob_get_count(void *pointer) {
    return *(uint32_t *)pointer;
}

static inline uint32_t shader_blob_read_count(void **pointer) {
    uint32_t value = *(uint32_t *)*pointer;
    *pointer += 4;
    return value;
}

static inline void shader_blob_read_string(void **pointer, char *buffer) {
    uint32_t size = shader_blob_read_count(pointer);
    char *string = *pointer;
    if(buffer) {
        memcpy(buffer, string, size);
        buffer[size] = '\0';
    }
    *pointer += size;
}

static inline void shader_blob_read_shader_bytecode(void **pointer, uint32_t *buffer) {
    uint32_t size = shader_blob_read_count(pointer) * 4;
    char *bytecode = *pointer;
    if(buffer) {
        memcpy(buffer, bytecode, size);
    }
    *pointer += size;
}

#ifdef __cplusplus
}
#endif

#endif
