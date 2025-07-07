#include <math.h>

#include "../types/types.h"
#include "../debug/assertion.h"
#include "../exception/exception.h"
#include "math.h"
#include "wave_functions.h"

extern bool *wave_functions_initialized;
extern uint8_t *wave_functions;

enum {
    PERIODIC_FUNCTIONS_TABLE_SIZE = 1024
};

double wave_function_calculate_value(double period, WaveFunction function) {
    ASSERT(WAVE_FUNCTION_ONE >= 0 && WAVE_FUNCTION_ONE < WAVE_FUNCTION_MAX_VALUE);

    if(*wave_functions_initialized == false) {
        exception_throw_runtime_error("wave functions not initialized yet");
    }

    if(function == WAVE_FUNCTION_ONE) {
        return 1.0;
    }

    float scaled_val = period * 25.6f;
    float fractional_part = fmod(scaled_val, 1.0f);
    int integer_part = round(scaled_val - fractional_part);

    uint8_t *wave_function = &wave_functions[function];
    uint8_t *current = wave_function + integer_part % PERIODIC_FUNCTIONS_TABLE_SIZE;
    float norm_current = normalize_f32(current[0], 0.0f, 255.0f);
    float norm_next = normalize_f32(current[1], 0.0f, 255.0f);

    if(function == WAVE_FUNCTION_SLIDE || function == WAVE_FUNCTION_SLIDE_VARIABLE_PERIOD) {
        if (norm_current > 0.75f && norm_next < 0.25f) {
            norm_next += 1.0f;
        }
    } 
    
    double res = (1.0f - fractional_part) * norm_current + fractional_part * norm_next; 
    clamp_f32(res, 0.0f, 1.0f);

    return res;
}
