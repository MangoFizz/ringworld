#include "termina.h"

extern TerminalGlobals *terminal_globals;

void terminal_initialize(void) {
    terminal_globals->initialized = true;
    terminal_globals->output_data = table_new("terminal output", TERMINAL_OUTPUT_MAX_DISPLAY_LINES, sizeof(TerminalOutputEntry));
    table_make_valid(terminal_globals->output_data);
    terminal_globals->first_line = NULL_HANDLE;
    terminal_globals->last_line = NULL_HANDLE;
    terminal_globals->current_state = NULL;
    terminal_globals->receiving_input_start_tick = 0;
    terminal_globals->rcon_machine_index = -1;
}
