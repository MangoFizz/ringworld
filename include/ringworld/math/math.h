#ifndef DEMON__IMPL_MATH_MATH_H
#define DEMON__IMPL_MATH_MATH_H

#include <stdint.h>

/**
 * Calculate the greater of the two integers.
 *
 * @return a if a > b, or b otherwise.
 */
static inline int32_t max_i32(int32_t a, int32_t b) {
    return a > b ? a : b;
}

/**
 * Calculate the lower of the two integers.
 *
 * @return a if a < b, or b otherwise.
 */
static inline int32_t min_i32(int32_t a, int32_t b) {
    return a < b ? a : b;
}

#endif
