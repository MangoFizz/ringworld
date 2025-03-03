#include <stdint.h>
#include "rng.h"

extern uint32_t *rng_state;

float rng_random_seed_float(void) {
    *rng_state = *rng_state * 0x19660d + 0x3c6ef35f;
    float seed = (*rng_state >> 0x10) * 1.525902e-05;
    return seed;
}
