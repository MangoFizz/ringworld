#ifndef RINGWORLD__INTERFACE__UI_ERROR_MODAL_H
#define RINGWORLD__INTERFACE__UI_ERROR_MODAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../types/types.h"
#include "../player/player.h"

typedef enum UIErrorModalIndex {
    UI_ERROR_MODAL_SAVE_FILE_ALREADY_EXISTS = 0x1B, 
    UI_ERROR_MODAL_EMPTY_NAME = 0x1D 
} UIErrorModalIndex;

/**
 * Display an error modal.
 * @param error_string the error string to display
 * @param local_player the local player handle
 * @param display_modal true if the modal should be displayed, false otherwise
 * @param display_paused true if the modal should be displayed while paused, false otherwise
 */
void ui_error_modal_show(int16_t error_string, PlayerHandle local_player, bool display_modal, bool display_paused);

#ifdef __cplusplus
}
#endif

#endif
