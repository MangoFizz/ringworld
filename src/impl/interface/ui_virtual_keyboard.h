#ifndef RINGWORLD__INTERFACE__UI_VIRTUAL_KEYBOARD_H
#define RINGWORLD__INTERFACE__UI_VIRTUAL_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../tag/definitions/virtual_keyboard.h"

typedef struct VirtualKeyboardGlobals {
    bool active;
    bool shift_active;
    bool caps_active;
    bool symbols_active;
    VirtualKeyboard *keyboard;
    int16_t row;
    int16_t column;
    uint16_t buffer_size;
    int16_t last_event;
    int16_t last_key;
    uint16_t number_of_event_repeats;
    int16_t caption_index;
    bool unk1;
    bool unk2;
    uint16_t *text_buffer;
    uint16_t *insertion_point;
    uint32_t unk3; // seems like a tick count
    TagHandle caret_bitmap_tag;
    wchar_t original_buffer[32];
    int32_t unk4;
    TagHandle caption_font_tag;
    TagHandle text_font_tag;
} VirtualKeyboardGlobals;

/**
 * Get the virtual keyboard globals.
 * @return pointer to the virtual keyboard globals
 */
VirtualKeyboardGlobals *ui_virtual_keyboard_get_globals(void);

/**
 * Render the virtual keyboard prompt within the specified bounds.
 * @param bounds the bounds within which to render the prompt
 */
void ui_virtual_keyboard_render_prompt(Rectangle2D *bounds);

/**
 * Render the virtual keyboard.
 */
void ui_virtual_keyboard_render(void);

#ifdef __cplusplus
}
#endif

#endif

