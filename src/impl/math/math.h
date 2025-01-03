#ifndef RINGWORLD__MATH__MATH_H
#define RINGWORLD__MATH__MATH_H

#include "../tag/definitions/enum.h"

/**
 * Calculate the greater of the two integers.
 * @return a if a > b, or b otherwise.
 */
static inline int32_t max_i32(int32_t a, int32_t b) {
    return a > b ? a : b;
}

/**
 * Calculate the lower of the two integers.
 * @return a if a < b, or b otherwise.
 */
static inline int32_t min_i32(int32_t a, int32_t b) {
    return a < b ? a : b;
}

/**
 * Calculate the greater of the two floats.
 * @return a if a > b, or b otherwise.
 */
static inline float max_f32(float a, float b) {
    return a > b ? a : b;
}

/**
 * Calculate the lower of the two floats.
 * @return a if a < b, or b otherwise.
 */
static inline float min_f32(float a, float b) {
    return a < b ? a : b;
}

/**
 * Clamp a value between a minimum and maximum.
 * @return min if x < min, max if x > max, or x otherwise.
 */
static inline int32_t clamp_i32(int32_t x, int32_t min, int32_t max) {
    return x < min ? min : (x > max ? max : x);
}

/**
 * Clamp a value between a minimum and maximum.
 * @return min if x < min, max if x > max, or x otherwise.
 */
static inline float clamp_f32(float x, float min, float max) {
    return x < min ? min : (x > max ? max : x);
}

/**
 * Normalize a value between a minimum and maximum.
 * @return (x - min) / (max - min).
 */
static inline float normalize_f32(float x, float min, float max) {
    return (x - min) / (max - min);
}

#endif
