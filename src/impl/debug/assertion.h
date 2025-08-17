#ifndef RINGWORLD__DEBUG__ASSERTION_H
#define RINGWORLD__DEBUG__ASSERTION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Throw an exception when an assertion fails.
 *
 * If assertion enforcing is disabled, the function will just log the
 * failure instead.
 *
 * @param expr The string representation of the expression that failed.
 * @param file The file where the assertion failed.
 * @param line The line number where the assertion failed.
 * @param assertion_context_fmt The format for the context in which the assertion failed.
 * @param ... The values to format into the assertion context.
 */
void assertion_failed_throw_exception(const char *expr, const char *file, int line, const char *assertion_context_fmt, ...);

/**
 * Assert that an expression is true.
 * If the expression is false, it will throw an exception with the expression, file, and line number.
 * @param expr The expression to assert.
 */
#define ASSERT(expr) \
    if(!(expr)) { \
        assertion_failed_throw_exception(#expr, __FILE__, __LINE__, NULL); \
    }

/**
 * Assert that an expression is true.
 * If the expression is false, it will throw an exception with the expression, file, and line number.
 * @param expr The expression to assert.
 * @param fmt String literal for formatting the assertion context.
 * @param ... The values to format into the assertion context.
 */
#define ASSERT_WITH_CONTEXT(expr, ...) \
    if(!(expr)) { \
        assertion_failed_throw_exception(#expr, __FILE__, __LINE__, __VA_ARGS__); \
    }

/**
 * Assert that an expression is true, and return from the current function if it is false.
 * @param expr The expression to assert.
 */
#define ASSERT_OR_RETURN(expr) \
    if(!(expr)) { \
        assertion_failed_throw_exception(#expr, __FILE__, __LINE__, NULL); \
        return; \
    }

/**
 * Assert that an expression is true, and return a specific value from the current function if it is false.
 * @param expr The expression to assert.
 * @param ret The value to return if the assertion fails.
 */
#define ASSERT_OR_RETURN_VALUE(expr, ret) \
    if(!(expr)) { \
        assertion_failed_throw_exception(#expr, __FILE__, __LINE__, NULL); \
        return ret; \
    }

#ifdef __cplusplus
}
#endif

#endif
