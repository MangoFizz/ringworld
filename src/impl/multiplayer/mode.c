#include <stdint.h>

#include <ringworld/multiplayer/mode.h>

extern MultiplayerMode *current_multiplayer_mode;

MultiplayerMode get_multiplayer_mode(void) {
    return *current_multiplayer_mode;
}
