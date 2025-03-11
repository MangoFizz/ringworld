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
static inline int32_t clamp_i8(int8_t x, int8_t min, int8_t max) {
    return x < min ? min : (x > max ? max : x);
}

/**
 * Clamp a value between a minimum and maximum.
 * @return min if x < min, max if x > max, or x otherwise.
 */
static inline int32_t clamp_u8(uint8_t x, uint8_t min, uint8_t max) {
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
 * Calculate the power of a float.
 * @param base The base value.
 * @param exp The exponent value.
 * @return The result of base raised to the power of exp.
 */
double math_pow_float(double base, double exp);

/**
 * Convert a float to a long integer.
 * @param f The float value to convert.
 * @return The converted long integer.
 */
int32_t math_float_to_long(float f);

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

/**
 * Scale a 2D vector by a scalar
 * @param v The vector to scale
 * @param scale The scalar to scale by
 * @param result The result of the scaling
 */
void math_vector_2d_scale(const void *v, float scale, void *result);

/**
 * Scale a 3D vector by a scalar.
 * @param v The vector to scale.
 * @param scale The scalar to scale by.
 * @param result The result of the scaling.
 */
void math_vector_3d_scale(const void *v, float scale, void *result);

/**
 * Translate a 2D rectangle by a given offset.
 * @param rect Pointer to the rectangle to be translated.
 * @param offset_x The offset along the x-axis.
 * @param offset_y The offset along the y-axis.
 * @return Pointer to the translated rectangle.
 */
Rectangle2D *math_rectangle_2d_translate(Rectangle2D *rect, int16_t offset_x, int16_t offset_y);

/**
 * Check if a 2D rectangle contains a point.
 * @param rect Pointer to the rectangle to check.
 * @param x The x-coordinate of the point.
 * @param y The y-coordinate of the point.
 * @return true if the point is inside the rectangle, or false otherwise.
 */
bool math_rectangle_2d_contains_point(Rectangle2D *rect, int16_t x, int16_t y);

/**
 * Transform a 3D point by a 4x3 matrix.
 * @param matrix Pointer to the matrix to transform by.
 * @param point Pointer to the point to transform.
 * @param result Pointer to the result of the transformation.
 */
void math_matrix_4x3_transform_point(const Matrix4x3 *matrix, const VectorXYZ *point, VectorXYZ *result);

#ifdef __cplusplus
}
#endif

#endif
