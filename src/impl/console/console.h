#ifndef RINGWORLD__CONSOLE__CONSOLE_H
#define RINGWORLD__CONSOLE__CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../input/input.h"
#include "../memory/table.h"
#include "../types/types.h"

typedef struct ConsoleInputState {
    int16_t keys_count;
    BufferedKey keys[32];
    ColorARGB color;
    char prompt[32];
    char command_result[256];
    char *text_buffer;
    int16_t text_buffer_length;
    int16_t insertion_point;
    int16_t selection_start;
} ConsoleInputState;
_Static_assert(sizeof(ConsoleInputState) == 0x1C0);

typedef struct ConsoleGlobals {
    bool console_is_open;
    bool console_is_enabled;
    ConsoleInputState input_state;
    char history_commands[8][255];
    int16_t history_commands_count;
    int16_t last_history_command_index;
    int16_t selected_history_command_index;
} ConsoleGlobals;

/**
 * Get the console globals.
 * @return pointer to the console globals
 */
ConsoleGlobals *console_get_globals(void);

/**
 * Initialize the console.
 */
void console_initialize(void);

/**
 * Process a console command.
 * @param flags flags to pass to hsc_find_functions
 * @param command the command to process
 * @return true if the command was processed, false otherwise
 */
bool console_process_command(uint32_t flags, const char *command);

#ifdef __cplusplus
}
#endif

#endif
