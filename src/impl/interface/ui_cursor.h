#ifndef RINGWORLD__INTERFACE__UI_CURSOR_H
#define RINGWORLD__INTERFACE__UI_CURSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../types/types.h"

/**
 * Displace the cursor by a given amount
 * @param dx the amount to displace the cursor in the x direction
 * @param dy the amount to displace the cursor in the y direction
 */
void ui_cursor_displace(int32_t dx, int32_t dy);

/**
 * Update the cursor position
 */
void ui_cursor_update(void);

/**
 * Get the current menu cursor coordinates
 * @return the current coordinates of the menu cursor
 */
VectorXYInt ui_cursor_get_position(void);

/**
 * Set the bitmap tag handle for the cursor
 * @param tag_handle the tag handle for the cursor bitmap
 */
void ui_cursor_set_bitmap_tag_handle(TagHandle tag_handle);

/**
 * Draw the cursor
 */
void ui_cursor_render(void);

#ifdef __cplusplus
}
#endif

#endif
