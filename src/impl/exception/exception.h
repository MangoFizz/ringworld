#ifndef RINGWORLD__EXCEPTION__EXCEPTION_H
#define RINGWORLD__EXCEPTION__EXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <windows.h>

/**
 * Print the stack trace of the current thread.
 * 
 * This function is used to display the call stack at the point of an exception.
 * It will display the address of each function, its symbol, and, if libbacktrace 
 * is available, its source code location.
 * 
 * @param context The context of the thread to print the stack trace for.
 */
void exception_print_stack_trace(CONTEXT *context);

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
