#ifndef RINGWORLD__INTERFACE__UI_VIRTUAL_KEYBOARD_H
#define RINGWORLD__INTERFACE__UI_VIRTUAL_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../tag/definitions/virtual_keyboard.h"

enum {
    UI_VIRTUAL_KEYBOARD_BUFFER_LEN = 32, 
    UI_VIRTUAL_KEYBOARD_FONT_SIZE_THRESHOLD = 50,
    UI_VIRTUAL_KEYBOARD_CURSOR_WIDTH = 1,
    UI_VIRTUAL_KEYBOARD_INPUT_PADDING = 2, // pixels
    UI_VIRTUAL_KEYBOARD_CURSOR_BLINK_TIME = 750, // milliseconds
};

typedef enum PACKED_ENUM InputValidationMode {
    INPUT_VALIDATION_PROFILE_NAME = 1,
    INPUT_VALIDATION_GAME_VARIANT_NAME = 2,
    INPUT_VALIDATION_NOT_EMPTY = 3,
    INPUT_VALIDATION_MODE_SIZE = 0xFFFFFFFF
} InputValidationMode;
_Static_assert(sizeof(InputValidationMode) == 4);

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
    bool saved; // true if the buffer wasn't saved yet
    bool text_is_selected;
    wchar_t *text_buffer;
    wchar_t *insertion_point;
    uint32_t cursor_blink_time; 
    TagHandle caret_bitmap_tag;
    wchar_t original_buffer[UI_VIRTUAL_KEYBOARD_BUFFER_LEN];
    InputValidationMode validate_mode;
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
void ui_virtual_keyboard_render_prompt(const Rectangle2D *bounds);

/**
 * Render the virtual keyboard.
 */
void ui_virtual_keyboard_render(void);

/**
 * Validate the input character based on the validation mode.
 * @param character the character to validate
 * @param validation_mode the validation mode to use
 * @return true if the character is valid, false otherwise
 */
bool ui_virtual_keyboard_validate_input_character(unsigned char character, uint32_t validation_mode);

#ifdef __cplusplus
}
#endif

#endif

