#ifndef DEMON__IMPL_INPUT_MOUSE_H
#define DEMON__IMPL_INPUT_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Scale the mouse input by an amount for aiming
 *
 * @param sensitivity sensitivity scaler
 * @param raw_input   raw integer input
 */
double scale_mouse_input(float sensitivity, uint32_t raw_input);

#ifdef __cplusplus
}
#endif

#endif
