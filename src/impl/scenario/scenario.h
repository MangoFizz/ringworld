#ifndef RINGWORLD__SCENARIO__SCENARIO_H
#define RINGWORLD__SCENARIO__SCENARIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/definitions/scenario.h"

/**
 * Get the current loaded scenario.
 * @return The current loaded scenario.
 */
Scenario *scenario_get(void);

#ifdef __cplusplus
}
#endif

#endif
