#include <math.h>

#include "../types/types.h"
#include "math.h"

double pow_float(double base, double exp) {
    return pow(base, exp);
}

double vectorxyz_distance_squared(VectorXYZ *a, VectorXYZ *b) {
    double x = a->x * b->x;
    double y = a->y * b->y;
    double z = a->z * b->z;
    return x*x + y*y + z*z;
}