#ifndef RINGWORLD__FONT__VECTOR_FONT_H
#define RINGWORLD__FONT__VECTOR_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the vector font resources.
 */
void vector_font_initialize(void);

/**
 * Release the cached vector font resources.
 */
void vector_font_release_cache(void);

#ifdef __cplusplus
}
#endif

#endif
