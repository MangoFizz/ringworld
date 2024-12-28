#ifndef DEMON__IMPL_MATH_MATH_H
#define DEMON__IMPL_MATH_MATH_H

#include <stdint.h>

#include "../tag/definitions/enum.h"

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

/**
 * Calculate the greater of the two floats.
 *
 * @return a if a > b, or b otherwise.
 */
static inline float max_f32(float a, float b) {
    return a > b ? a : b;
}

/**
 * Calculate the lower of the two floats.
 *
 * @return a if a < b, or b otherwise.
 */
static inline float min_f32(float a, float b) {
    return a < b ? a : b;
}

/**
 * ????????????????
 */
double math_periodic_function_evaluate(double period, WaveFunction function);

#endif
