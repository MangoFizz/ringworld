#include <math.h>

#include "math.h"

double pow_float(double base, double exp) {
    return pow(base, exp);
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
