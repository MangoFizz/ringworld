#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include "../debug/assertion.h"
#include "../exception/exception.h"
#include "../math/color.h"
#include "../shell/shell.h"
#include "terminal.h"

extern TerminalGlobals *terminal_globals;
extern int32_t *terminal_count;
extern TerminalScreen *terminal_screen;
extern TerminalInput *terminal_input;

static bool mute_output = false;

void terminal_printf_in(const ColorARGB *color, const char *fmt, const char *str);

TerminalScreen *terminal_get_screen(void) {
    return terminal_screen;
}

TerminalGlobals *terminal_get_globals(void) {
    return terminal_globals;
}

TerminalInput *terminal_get_input(void) {
    return terminal_input;
}

void terminal_initialize(void) {
    TerminalGlobals *globals = terminal_get_globals();
    TerminalScreen *screen = terminal_get_screen();
    TerminalInput *input = terminal_get_input();

    globals->initialized = true;
    globals->output_data = table_new("terminal output", TERMINAL_OUTPUT_MAX_DISPLAY_LINES, sizeof(TerminalOutputEntry));
    table_make_valid(globals->output_data);
    globals->first_line = NULL_HANDLE;
    globals->last_line = NULL_HANDLE;
    globals->current_state = NULL;
    globals->receiving_input_start_tick = 0;
    globals->rcon_machine_index = -1;

    if(shell_is_headless_mode()) {

        input->stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
        if(input->stdin_handle == INVALID_HANDLE_VALUE) {
            exception_throw_runtime_error("terminal_initialize(): Failed to get standard input handle");
        }

        input->stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if(input->stdout_handle == INVALID_HANDLE_VALUE) {
            exception_throw_runtime_error("terminal_initialize(): Failed to get standard output handle");
        }

        ASSERT(SetConsoleMode(input->stdin_handle, ENABLE_WINDOW_INPUT) != FALSE);

        ASSERT(SetConsoleCtrlHandler((PHANDLER_ROUTINE)terminal_handler_routine, TRUE) != FALSE);

        char console_title[256];
        snprintf(console_title, sizeof(console_title), "Ringworld Console (%d)", (*terminal_count) + 1);
        ASSERT(SetConsoleTitleA(console_title) != FALSE);

        globals->is_active = true;

        memset(screen, 0, sizeof(TerminalScreen));
    }
}

void terminal_printf(const ColorARGB *color, const char *fmt, ...) {
    if(mute_output) {
        return;
    }
    char passed_text[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(passed_text, sizeof(passed_text), fmt, args);
    va_end(args);
    terminal_printf_in(color, "%s", passed_text);
}

void terminal_info_printf(const char *fmt, ...) {
    if(mute_output) {
        return;
    }
    char passed_text[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(passed_text, sizeof(passed_text), fmt, args);
    va_end(args);
    terminal_printf_in(&COLOR_ARGB_GRAY, "%s", passed_text);
}

void terminal_warning_printf(const char *fmt, ...) {
    if(mute_output) {
        return;
    }
    char passed_text[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(passed_text, sizeof(passed_text), fmt, args);
    va_end(args);
    terminal_printf_in(&COLOR_ARGB_YELLOW, "%s", passed_text);
}

void terminal_error_printf(const char *fmt, ...) {
    if(mute_output) {
        return;
    }
    char passed_text[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(passed_text, sizeof(passed_text), fmt, args);
    va_end(args);
    terminal_printf_in(&COLOR_ARGB_RED, "%s", passed_text);
}

void terminal_mute_output(bool mute) {
    mute_output = mute;
}
