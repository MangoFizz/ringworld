#ifndef DEMON__IMPL_EXCEPTION_EXCEPTION_H
#define DEMON__IMPL_EXCEPTION_EXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Terminate the process with a format.
 * @param fmt format
 */
void crashf(const char *fmt, ...);

/**
 * Terminate the process due to a function being improperly called.
 * @param function_name function name
 * @param from          caller address
 */
void crash_forbidden_function(const char *function_name, void *from);

/**
 * Print a debug message.
 * @param fmt format
 */
void debug_printf(const char *fmt, ...);

/**
 * Print a message box.
 * @param fmt format
 */
void message_box(const char *fmt, ...);

/**
 * Print an error box.
 * @param fmt format
 */
void error_box(const char *fmt, ...);

#define STR2(s) # s
#define STR(s) STR2(s)

#define CRASHF_DEBUG(...) crashf(__FILE__ ":" STR(__LINE__) ": " __VA_ARGS__)
#define ASSERT(expr) if(!(expr)) CRASHF_DEBUG("assertion failed: " #expr); 
#define ASSERT_OR_RETURN(expr) if(!(expr)) return;
#define ASSERT_OR_RETURN_VALUE(expr, ret) if(!(expr)) return ret; 

#ifdef __cplusplus
}
#endif

#endif
