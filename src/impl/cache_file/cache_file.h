#ifndef RINGWORLD__CACHE_FILE__CACHE_FILE_H
#define RINGWORLD__CACHE_FILE__CACHE_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../types/types.h"

enum {
    CACHE_FILE_HEAD_LITERAL = 0x68656164,
    CACHE_FILE_FOOT_LITERAL = 0x666F6F74
};

typedef enum PACKED_ENUM CacheFileType {
    CACHE_FILE_TYPE_SINGLE_PLAYER = 0,
    CACHE_FILE_TYPE_MULTIPLAYER,
    CACHE_FILE_TYPE_USER_INTERFACE,
    CACHE_FILE_TYPE_MAX_VALUE,
    CACHE_FILE_TYPE_SIZE = 0xFFFF
} CacheFileType;
_Static_assert(sizeof(CacheFileType) == 2);

typedef struct CacheFileHeader {
    uint32_t head;
    int engine_type;
    uint32_t file_size;
    char pad01[4];
    uint32_t tag_data_offset;
    uint32_t tag_data_size;
    char pad02[8];
    String32 name;
    String32 build;
    CacheFileType game_type;
    char pad03[2];
    uint32_t crc32;
    char pad04[0x2B0];
    char pad05[0x4E4];
    uint32_t foot;
} CacheFileHeader;
_Static_assert(sizeof(CacheFileHeader) == 0x800);

typedef void (*CacheFileRequestFinishedProc)(void *params);

typedef struct CacheFileRequestParams {
    bool *finished_flag; 
    void *finished_proc_params;	// optional
    CacheFileRequestFinishedProc finished_proc; // optional
} CacheFileRequestParams; 
_Static_assert(sizeof(CacheFileRequestParams) == 0xC);

typedef enum CacheFileRequestSource {
    CACHE_FILE_REQUEST_SOURCE_MAP,
    CACHE_FILE_REQUEST_SOURCE_BITMAPS,
    CACHE_FILE_REQUEST_SOURCE_SOUNDS,
    CACHE_FILE_REQUEST_SOURCE_LOC,
} CacheFileRequestSource;

/**
 * Request a read from the cache file.
 * @param data_offset The offset in the cache file to read from.
 * @param data_size The size of the data to read.
 * @param buffer The buffer to read the data into.
 * @param block If true, block until the read is complete.
 * @param source The source of the request.
 * @param params Optional parameters for the request.
 * @return true if the request was successful, false otherwise.
 */
bool cache_file_read_request(uint32_t data_offset, uint32_t data_size, void *buffer, bool block, CacheFileRequestSource source, CacheFileRequestParams *params);

#ifdef __cplusplus
}
#endif

#endif
