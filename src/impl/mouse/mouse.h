#ifndef RINGWORLD__MOUSE__MOUSE_H
#define RINGWORLD__MOUSE__MOUSE_H

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

/**
 * Get the current menu cursor coordinates
 * @return the current coordinates of the menu cursor
 */
VectorXYInt mouse_get_cursor_position(void);

#ifdef __cplusplus
}
#endif

#endif
