#ifndef RINGWORLD__TIME__TIME_H
#define RINGWORLD__TIME__TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Get the current performance counter in milliseconds.
 * @return The current performance counter in milliseconds.
 */
uint32_t time_query_performance_counter_ms(void);

#ifdef __cplusplus
}
#endif

#endif
