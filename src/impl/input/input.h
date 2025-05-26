#ifndef RINGWORLD__INPUT__INPUT_H
#define RINGWORLD__INPUT__INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <dinput.h>
#include "../player/player.h"
#include "../types/types.h"

typedef enum BufferedKeyFlags {
    BUFFERED_KEY_FLAGS_SHIFT,
    BUFFERED_KEY_FLAGS_CONTROL,
    BUFFERED_KEY_FLAGS_ALT
} BufferedKeyFlags;

typedef enum PACKED_ENUM BufferedKeyCode {
    BUFFERED_KEY_CODE_ESCAPE,
    BUFFERED_KEY_CODE_F1,
    BUFFERED_KEY_CODE_F2,
    BUFFERED_KEY_CODE_F3,
    BUFFERED_KEY_CODE_F4,
    BUFFERED_KEY_CODE_F5,
    BUFFERED_KEY_CODE_F6,
    BUFFERED_KEY_CODE_F7,
    BUFFERED_KEY_CODE_F8,
    BUFFERED_KEY_CODE_F9,
    BUFFERED_KEY_CODE_F10,
    BUFFERED_KEY_CODE_F11,
    BUFFERED_KEY_CODE_F12,
    BUFFERED_KEY_CODE_PRINT_SCREEN,
    BUFFERED_KEY_CODE_SCROLL_LOCK,
    BUFFERED_KEY_CODE_PAUSE_BREAK,
    BUFFERED_KEY_CODE_TILDE,
    BUFFERED_KEY_CODE_TOP_1,
    BUFFERED_KEY_CODE_TOP_2,
    BUFFERED_KEY_CODE_TOP_3,
    BUFFERED_KEY_CODE_TOP_4,
    BUFFERED_KEY_CODE_TOP_5,
    BUFFERED_KEY_CODE_TOP_6,
    BUFFERED_KEY_CODE_TOP_7,
    BUFFERED_KEY_CODE_TOP_8,
    BUFFERED_KEY_CODE_TOP_9,
    BUFFERED_KEY_CODE_TOP_0,
    BUFFERED_KEY_CODE_TOP_MINUS,
    BUFFERED_KEY_CODE_TOP_EQUALS,
    BUFFERED_KEY_CODE_BACKSPACE,
    BUFFERED_KEY_CODE_TAB,
    BUFFERED_KEY_CODE_Q,
    BUFFERED_KEY_CODE_W,
    BUFFERED_KEY_CODE_E,
    BUFFERED_KEY_CODE_R,
    BUFFERED_KEY_CODE_T,
    BUFFERED_KEY_CODE_Y,
    BUFFERED_KEY_CODE_U,
    BUFFERED_KEY_CODE_I,
    BUFFERED_KEY_CODE_O,
    BUFFERED_KEY_CODE_P,
    BUFFERED_KEY_CODE_LEFT_BRACKET,
    BUFFERED_KEY_CODE_RIGHT_BRACKET,
    BUFFERED_KEY_CODE_BACK_SLASH,
    BUFFERED_KEY_CODE_CAPS_LOCK,
    BUFFERED_KEY_CODE_A,
    BUFFERED_KEY_CODE_S,
    BUFFERED_KEY_CODE_D,
    BUFFERED_KEY_CODE_F,
    BUFFERED_KEY_CODE_G,
    BUFFERED_KEY_CODE_H,
    BUFFERED_KEY_CODE_J,
    BUFFERED_KEY_CODE_K,
    BUFFERED_KEY_CODE_L,
    BUFFERED_KEY_CODE_SEMICOLON,
    BUFFERED_KEY_CODE_APOSTROPHE,
    BUFFERED_KEY_CODE_ENTER,
    BUFFERED_KEY_CODE_LEFT_SHIFT,
    BUFFERED_KEY_CODE_Z,
    BUFFERED_KEY_CODE_X,
    BUFFERED_KEY_CODE_C,
    BUFFERED_KEY_CODE_V,
    BUFFERED_KEY_CODE_B,
    BUFFERED_KEY_CODE_N,
    BUFFERED_KEY_CODE_M,
    BUFFERED_KEY_CODE_COMMA,
    BUFFERED_KEY_CODE_PERIOD,
    BUFFERED_KEY_CODE_FORWARD_SLASH,
    BUFFERED_KEY_CODE_RIGHT_SHIFT,
    BUFFERED_KEY_CODE_LEFT_CONTROL,
    BUFFERED_KEY_CODE_WINDOWS_KEY,
    BUFFERED_KEY_CODE_LEFT_ALT,
    BUFFERED_KEY_CODE_SPACE,
    BUFFERED_KEY_CODE_RIGHT_ALT,
    BUFFERED_KEY_CODE_UNKNOWN,
    BUFFERED_KEY_CODE_MENU,
    BUFFERED_KEY_CODE_RIGHT_CONTROL,
    BUFFERED_KEY_CODE_UP_ARROW,
    BUFFERED_KEY_CODE_DOWN_ARROW,
    BUFFERED_KEY_CODE_LEFT_ARROW,
    BUFFERED_KEY_CODE_RIGHT_ARROW,
    BUFFERED_KEY_CODE_INS,
    BUFFERED_KEY_CODE_HOME,
    BUFFERED_KEY_CODE_PAGE_UP,
    BUFFERED_KEY_CODE_DEL,
    BUFFERED_KEY_CODE_END,
    BUFFERED_KEY_CODE_PAGE_DOWN,
    BUFFERED_KEY_CODE_NUM_LOCK,
    BUFFERED_KEY_CODE_NUM_STAR,
    BUFFERED_KEY_CODE_NUM_FORWARD_SLASH,
    BUFFERED_KEY_CODE_NUM_0,
    BUFFERED_KEY_CODE_NUM_1,
    BUFFERED_KEY_CODE_NUM_2,
    BUFFERED_KEY_CODE_NUM_3,
    BUFFERED_KEY_CODE_NUM_4,
    BUFFERED_KEY_CODE_NUM_5,
    BUFFERED_KEY_CODE_NUM_6,
    BUFFERED_KEY_CODE_NUM_7,
    BUFFERED_KEY_CODE_NUM_8,
    BUFFERED_KEY_CODE_NUM_9,
    BUFFERED_KEY_CODE_NUM_MINUS,
    BUFFERED_KEY_CODE_NUM_PLUS,
    BUFFERED_KEY_CODE_NUM_ENTER,
    BUFFERED_KEY_CODE_NUM_DECIMAL,
    NUMBER_OF_BUFFERED_KEY_CODES,
    BUFFERED_KEY_CODE_NONE = 0xFFFF
} BufferedKeyCode;
_Static_assert(sizeof(BufferedKeyCode) == 2);

typedef struct BufferedKey {
    uint8_t flags;
    uint8_t state;	
    BufferedKeyCode key_code;
} BufferedKey;
_Static_assert(sizeof(BufferedKey) == 4);

typedef struct MouseInputState {
    int32_t horizontal;
    int32_t vertical;
    int32_t wheel;
    uint8_t button_state[8];
    bool button_changed[8];
} MouseInputState; 
_Static_assert(sizeof(MouseInputState) == 0x1C);

typedef struct InputGlobals {
    bool initialized; 
    bool use_virtual_input;
    LPDIRECTINPUT8 direct_input;
    char unk1[32];
    uint8_t key_unknown_data[218];
    int16_t buffered_key_read_index;
    int16_t buffered_key_write_index;
    BufferedKey buffered_keys[64];
    LPDIRECTINPUTDEVICE direct_keyboard;
    LPDIRECTINPUTDEVICE direct_mouse;
    uint32_t direct_mouse_z_granularity;
    MouseInputState direct_mouse_state;
    MouseInputState virtual_mouse_state;
    int16_t next_joystick;
    LPDIRECTINPUTDEVICE direct_joysticks[8]; 
    uint8_t direct_joysticks_states[0x240][8];
} InputGlobals;

/**
 * Get the input globals structure.
 */
InputGlobals *input_get_globals(void);

/**
 * Get the input flags.
 */
uint8_t *input_get_flags(void);

/**
 * Flush the keyboard buffered keys.
 */
void input_flush_keyboard_buffered_keys(void);

#ifdef __cplusplus
}
#endif

#endif 
