#ifndef RINGWORLD__LIGHT__LIGHT_H
#define RINGWORLD__LIGHT__LIGHT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize lights.
 */
void lights_initialize(void);

/**
 * Initialize lights for a new map.
 */
void lights_initialize_for_new_map(void);

#ifdef __cplusplus
}
#endif

#endif 

