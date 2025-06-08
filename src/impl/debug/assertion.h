#ifndef RINGWORLD__DEBUG__ASSERTION_H
#define RINGWORLD__DEBUG__ASSERTION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Throw an exception when an assertion fails.
 * @param expr The string representation of the expression that failed.
 * @param file The file where the assertion failed.
 * @param line The line number where the assertion failed.
 */
void assertion_failed_throw_exception(const char *expr, const char *file, int line);

/**
 * Assert that an expression is true.
 * If the expression is false, it will throw an exception with the expression, file, and line number.
 * @param expr The expression to assert.
 */
#define ASSERT(expr) if(!(expr)) assertion_failed_throw_exception(#expr, __FILE__, __LINE__);

/**
 * Assert that an expression is true, and return from the current function if it is false.
 * @param expr The expression to assert.
 */
#define ASSERT_OR_RETURN(expr) if(!(expr)) return;

/**
 * Assert that an expression is true, and return a specific value from the current function if it is false.
 * @param expr The expression to assert.
 * @param ret The value to return if the assertion fails.
 */
#define ASSERT_OR_RETURN_VALUE(expr, ret) if(!(expr)) return ret; 

#ifdef __cplusplus
}
#endif

#endif
