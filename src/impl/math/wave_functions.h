#ifndef RINGWORLD__MATH__WAVE_FUNCTIONS_H
#define RINGWORLD__MATH__WAVE_FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/enum.h"

/**
 * Computes the value of a specified periodic wave function at a given period.
 * @param period The period at which to evaluate the wave function.
 * @param function The wave function to evaluate.
 * @return The evaluated value of the wave function at the given period.
 * 
 * @todo Implement support for 64-bit parameters in the hooks system.
 */
double wave_function_calculate_value(double period, WaveFunction function);
double wave_function_calculate_value_og(double period, WaveFunction function);

#ifdef __cplusplus
}
#endif

#endif
