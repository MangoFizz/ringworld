#include "exception/exception.h"
#include "assertion.h"

void assertion_failed_throw_exception(const char *expr, const char *file, int line) {
    exception_throw_runtime_error("Assertion failed: %s, file %s, line %d", expr, file, line);
}
