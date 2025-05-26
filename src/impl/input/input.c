#include "input.h"

extern InputGlobals *input_globals;
extern uint8_t *input_flags;

InputGlobals *input_get_globals(void) {
    return input_globals;
}

uint8_t *input_get_flags(void) {
    return input_flags;
}

void input_flush_keyboard_buffered_keys(void) {
    InputGlobals *input_globals = input_get_globals();
    DWORD device_elements = -1;
    IDirectInputDevice_GetDeviceData(input_globals->direct_keyboard, sizeof(DIDEVICEOBJECTDATA), NULL, &device_elements, 0);
    memset(input_globals->key_unknown_data, 0, sizeof(input_globals->key_unknown_data));
}
