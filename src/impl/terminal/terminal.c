#include <stdio.h>
#include <stdarg.h>
#include "../math/color.h"
#include "terminal.h"

extern TerminalGlobals *terminal_globals;

void terminal_printf_in(const ColorARGB *color, const char *fmt, const char *str);

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

void terminal_printf(const ColorARGB *color, const char *fmt, ...) {
    char passed_text[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(passed_text, sizeof(passed_text), fmt, args);
    va_end(args);
    terminal_printf_in(color, "%s", passed_text);
}

void terminal_error_printf(const char *fmt, ...) {
    char passed_text[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(passed_text, sizeof(passed_text), fmt, args);
    va_end(args);
    terminal_printf_in(&COLOR_ARGB_RED_ERROR, "%s", passed_text);
}
