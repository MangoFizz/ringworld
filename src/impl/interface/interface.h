#ifndef RINGWORLD__INTERFACE__INTERFACE_H
#define RINGWORLD__INTERFACE__INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize interface resources
 */
void interface_initialize(void);

/**
 * Initialize interface resources for a new map
 */
void interface_initialize_for_new_map(void);

#ifdef __cplusplus
}
#endif

#endif
