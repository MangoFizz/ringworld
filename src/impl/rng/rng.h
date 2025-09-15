#ifndef RINGWORLD__RNG__RNG_H
#define RINGWORLD__RNG__RNG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get a pointer to the global RNG seed.
 * @return Pointer to the global RNG seed.
 */
uint32_t *rng_get_global_seed(void);

/**
 * Generate a pseudo-random float in the range [0.0, 1.0) using a linear congruential generator.
 * @param rng_state Pointer to the current state of the RNG.
 * @return A pseudo-random float in the range [0.0, 1.0).
 */
float rng_random_seed_float(uint32_t *rng_state);

#ifdef __cplusplus
}
#endif

#endif
