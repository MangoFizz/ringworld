#ifndef RINGWORLD__CACHE_FILE__CACHE_FILE_H
#define RINGWORLD__CACHE_FILE__CACHE_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

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
