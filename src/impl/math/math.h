#ifndef RINGWORLD__MATH__MATH_H
#define RINGWORLD__MATH__MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/enum.h"

typedef struct ProjectionMatrix {
    float x[4];
    float y[4];
    float z[4];
    float w[4];
} ProjectionMatrix;
_Static_assert(sizeof(ProjectionMatrix) == sizeof(float) * 4 * 4);

/**
 * Check if a float is NaN.
 * @return true if the float is NaN, or false otherwise.
 */
static inline bool nan_f32(float f) {
    return f != f;
}

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

/**
 * Calculate the dot product of two vectors
 * @param a The first vector 
 * @param b The second vector 
 * @return a.i * b.i + a.j * b.j + a.k * b.k.
 */
double math_vector_dot_product(VectorIJK *a, VectorIJK *b);

/**
 * Calculate the squared magnitude of a vector 
 * @param v The vector
 * @return vector.i^2 + vector.j^2 + vector.k^2.
 */
double math_vector_squared_magnitude(VectorIJK *v);

/**
 * Calculate the length of a vector 
 * @param v The vector
 * @return sqrt(vector.i^2 + vector.j^2 + vector.k^2).
 */
double math_vector_normalize(VectorIJK *v);

#ifdef __cplusplus
}
#endif

#endif
