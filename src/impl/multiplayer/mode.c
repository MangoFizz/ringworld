#include <stdint.h>

#include "mode.h"

extern MultiplayerMode *current_multiplayer_mode;

MultiplayerMode get_multiplayer_mode(void) {
    return *current_multiplayer_mode;
}
