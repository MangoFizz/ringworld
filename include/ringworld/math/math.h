#ifndef DEMON__IMPL_MATH_MATH_H
#define DEMON__IMPL_MATH_MATH_H

#include <stdint.h>

struct VectorXYZ;

/**
 * Calculate base to the power of an exponent.
 *
 * @param base base
 * @param exp  exponent
 *
 * @return base to the power of the exponent
 */
double pow_float(double base, double exp);

/**
 * Calculate the distance squared between two points.
 *
 * Doing distance squared is preferred for speed for doing distance comparisons as it saves calculating a square root.
 *
 * @param a first vector
 * @param b second vector
 *
 * @return distance squared
 */
double vectorxyz_distance_squared(struct VectorXYZ *a, struct VectorXYZ *b);

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
