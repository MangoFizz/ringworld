#include <math.h>
#include "math.h"

double math_pow_float(double base, double exp) {
    return pow(base, exp);
}

uint32_t math_float_to_long(float f) {
    return lrintf(f);
}

double math_vector_dot_product(VectorIJK *a, VectorIJK *b) {
    return a->i * b->i + a->j * b->j + a->k * b->k;
}

double math_vector_squared_magnitude(VectorIJK *v) {
    return pow(v->i, 2) + pow(v->j, 2) + pow(v->k, 2);
}

double math_vector_normalize(VectorIJK *v) {
    return sqrt(math_vector_squared_magnitude(v));
}

void math_vector_2d_scale(void *v, float scale, void *result) {
    VectorIJ *vector = v;
    VectorIJ *result_vector = result;
    result_vector->i = vector->i * scale;
    result_vector->j = vector->j * scale;
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
