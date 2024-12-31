#ifndef DEMON__IMPL_EXCEPTION_EXCEPTION_H
#define DEMON__IMPL_EXCEPTION_EXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Terminate the process with a format.
 *
 * @param fmt format
 */
void crashf(const char *fmt, ...);

/**
 * Terminate the process due to a function being improperly called.
 *
 * @param function_name function name
 * @param from          caller address
 */
void crash_forbidden_function(const char *function_name, void *from);

/**
 * Print a debug message.
 *
 * @param fmt format
 */
void debug_printf(const char *fmt, ...);

#define STR2(s) # s
#define STR(s) STR2(s)

#define CRASHF_DEBUG(...) crashf(__FILE__ ":" STR(__LINE__) ": " __VA_ARGS__)

#define ASSERT(expr) \
    if(!(expr)) { \
        CRASHF_DEBUG("assertion failed: " #expr); \
    }

#ifdef __cplusplus
}
#endif

#endif
