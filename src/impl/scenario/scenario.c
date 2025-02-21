#include "scenario.h"

extern Scenario **current_scenario;

Scenario *scenario_get(void) {
    return *current_scenario;
}
