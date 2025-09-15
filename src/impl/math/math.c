#include <math.h>
#include "math.h"

double math_pow_float(double base, double exp) {
    return pow(base, exp);
}

int32_t math_float_to_long(float f) {
    return lrintf(f);
}

double math_vector_dot_product(const void *a, const void *b) {
    const VectorIJK *vec_a = a;
    const VectorIJK *vec_b = b;
    return vec_a->i * vec_b->i + vec_a->j * vec_b->j + vec_a->k * vec_b->k;
}

double math_vector_squared_magnitude(const void *v) {
    return math_vector_dot_product(v, v);
}

double math_vector_length(const void *v) {
    return sqrt(math_vector_squared_magnitude(v));
}

void math_vector_2d_scale(const void *v, float scale, void *result) {
    const VectorXY *vector = v;
    VectorXY *result_vector = result;
    result_vector->x = vector->x * scale;
    result_vector->y = vector->y * scale;
}

void math_vector_3d_scale(const void *v, float scale, void *result) {
    const VectorXYZ *vector = v;
    VectorXYZ *result_vector = result;
    result_vector->x = vector->x * scale;
    result_vector->y = vector->y * scale;
    result_vector->z = vector->z * scale;
}

double math_vector_distance(const void *a, const void *b) {
    const VectorXYZ *vec_a = a;
    const VectorXYZ *vec_b = b;
    double dx = vec_a->x - vec_b->x;
    double dy = vec_a->y - vec_b->y;
    double dz = vec_a->z - vec_b->z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

Rectangle2D *math_rectangle_2d_translate(Rectangle2D *rect, int16_t offset_x, int16_t offset_y) {
    rect->left += offset_x;
    rect->right += offset_x;
    rect->top += offset_y;
    rect->bottom += offset_y;
    return rect;
}

bool math_rectangle_2d_contains_point(Rectangle2D *rect, int16_t x, int16_t y) {
    return x >= rect->left && x <= rect->right && y >= rect->top && y <= rect->bottom;
}

void math_matrix_4x3_transform_point(const Matrix4x3 *matrix, const VectorXYZ *point, VectorXYZ *result) {
    VectorXYZ scaled_point;
    math_vector_3d_scale(point, matrix->scale, &scaled_point);
    result->x = scaled_point.x * matrix->forward.i + scaled_point.y * matrix->left.i + scaled_point.z * matrix->up.i + matrix->position.x;
    result->y = scaled_point.x * matrix->forward.j + scaled_point.y * matrix->left.j + scaled_point.z * matrix->up.j + matrix->position.y;
    result->z = scaled_point.x * matrix->forward.k + scaled_point.y * matrix->left.k + scaled_point.z * matrix->up.k + matrix->position.z;
}
