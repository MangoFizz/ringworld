#ifndef RINGWORLD__ERROR_ERROR_CODE_H
#define RINGWORLD__ERROR_ERROR_CODE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ErrorCode {
    ERROR_CODE_SUCCESS = 0,
    ERROR_CODE_FAILURE,
    ERROR_CODE_INVALID_ARGUMENT,
    ERROR_CODE_OUT_OF_MEMORY,
    ERROR_CODE_INSUFFICIENT_BUFFER,
} ErrorCode;

#ifdef __cplusplus
}
#endif

#endif
