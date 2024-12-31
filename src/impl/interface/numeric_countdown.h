#ifndef RINGWORLD__INTERFACE__NUMERIC_COUNTDOWN_H
#define RINGWORLD__INTERFACE__NUMERIC_COUNTDOWN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Get the numeric countdown timer for the given units.
 */
uint16_t numeric_countdown_timer_get(uint16_t units);


#ifdef __cplusplus
}
#endif

#endif
