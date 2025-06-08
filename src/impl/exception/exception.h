#ifndef RINGWORLD__EXCEPTION__EXCEPTION_H
#define RINGWORLD__EXCEPTION__EXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Throw an runtime error exception.
 * This function is used to report assertion failures and other critical errors.
 * @param fmt  The format string for the error message.
 * @param ...  The values to format into the error message.
 */
void exception_throw_runtime_error(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
